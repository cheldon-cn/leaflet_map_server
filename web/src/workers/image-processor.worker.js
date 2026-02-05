// Image Processor Web Worker - Processes images in a separate thread

/**
 * Image Processor Worker
 * 
 * This worker handles image processing tasks to avoid blocking the main thread.
 * Supported operations:
 * - Image decoding and format conversion
 * - Resizing and scaling
 * - Compression and quality adjustment
 * - Color space conversion
 * - Canvas operations
 */

// Worker state
let workerState = {
    id: 'image-processor-worker',
    version: '1.0.0',
    isBusy: false,
    processedCount: 0,
    errorCount: 0,
    startupTime: Date.now()
};

// Image processing capabilities
const capabilities = {
    // Format support
    formats: {
        jpeg: true,
        png: true,
        webp: true,
        gif: false,
        bmp: false,
        tiff: false
    },
    
    // Operations support
    operations: {
        decode: true,
        encode: true,
        resize: true,
        scale: true,
        compress: true,
        convertFormat: true,
        adjustQuality: true,
        extractThumbnail: true
    },
    
    // Performance characteristics
    performance: {
        maxImageSize: 8192, // Maximum image dimension (width or height)
        maxFileSize: 50 * 1024 * 1024, // 50MB
        supportedColorSpaces: ['RGB', 'RGBA', 'Grayscale']
    }
};

// Message handler
self.onmessage = async function(event) {
    const { id, type, data, options } = event.data;
    
    try {
        workerState.isBusy = true;
        
        // Route to appropriate handler
        let result;
        switch (type) {
            case 'decode':
                result = await decodeImage(data, options);
                break;
                
            case 'encode':
                result = await encodeImage(data, options);
                break;
                
            case 'resize':
                result = await resizeImage(data, options);
                break;
                
            case 'scale':
                result = await scaleImage(data, options);
                break;
                
            case 'compress':
                result = await compressImage(data, options);
                break;
                
            case 'convert':
                result = await convertImage(data, options);
                break;
                
            case 'thumbnail':
                result = await createThumbnail(data, options);
                break;
                
            case 'analyze':
                result = await analyzeImage(data, options);
                break;
                
            case 'ping':
                result = await handlePing();
                break;
                
            case 'capabilities':
                result = await getCapabilities();
                break;
                
            default:
                throw new Error(`Unknown operation type: ${type}`);
        }
        
        // Send success response
        self.postMessage({
            id,
            type,
            success: true,
            result,
            workerState: {
                ...workerState,
                processedCount: ++workerState.processedCount
            }
        });
        
    } catch (error) {
        // Send error response
        workerState.errorCount++;
        
        self.postMessage({
            id,
            type,
            success: false,
            error: {
                message: error.message,
                stack: error.stack,
                code: error.code || 'PROCESSING_ERROR'
            },
            workerState: {
                ...workerState,
                errorCount: workerState.errorCount
            }
        });
        
    } finally {
        workerState.isBusy = false;
    }
};

// ========== Image Processing Functions ==========

/**
 * Decode image data
 */
async function decodeImage(data, options = {}) {
    const { format, width, height } = options;
    
    // Create blob from data
    let blob;
    if (data instanceof ArrayBuffer) {
        const mimeType = getMimeType(format);
        blob = new Blob([data], { type: mimeType });
    } else if (data instanceof Blob) {
        blob = data;
    } else {
        throw new Error('Unsupported data type for decoding');
    }
    
    // Create image bitmap
    const imageBitmap = await createImageBitmap(blob, {
        resizeWidth: width,
        resizeHeight: height,
        resizeQuality: options.quality || 'high',
        premultiplyAlpha: options.premultiplyAlpha || 'premultiply',
        colorSpaceConversion: options.colorSpaceConversion || 'default'
    });
    
    // Extract image data
    const canvas = new OffscreenCanvas(imageBitmap.width, imageBitmap.height);
    const ctx = canvas.getContext('2d');
    
    if (!ctx) {
        throw new Error('Failed to get canvas context');
    }
    
    // Draw image to canvas
    ctx.drawImage(imageBitmap, 0, 0);
    
    // Get image data
    const imageData = ctx.getImageData(0, 0, imageBitmap.width, imageBitmap.height);
    
    // Clean up
    imageBitmap.close();
    
    return {
        width: imageBitmap.width,
        height: imageBitmap.height,
        data: imageData.data.buffer,
        format: format || 'rgba',
        channels: 4,
        hasAlpha: true
    };
}

/**
 * Encode image to specified format
 */
async function encodeImage(data, options = {}) {
    const { format = 'png', quality = 0.92 } = options;
    
    if (!capabilities.formats[format]) {
        throw new Error(`Unsupported output format: ${format}`);
    }
    
    // Create canvas from data
    const { width, height, imageData } = data;
    const canvas = new OffscreenCanvas(width, height);
    const ctx = canvas.getContext('2d');
    
    if (!ctx) {
        throw new Error('Failed to get canvas context');
    }
    
    // Put image data on canvas
    const imgData = new ImageData(
        new Uint8ClampedArray(imageData),
        width,
        height
    );
    ctx.putImageData(imgData, 0, 0);
    
    // Convert to blob
    let blob;
    switch (format) {
        case 'jpeg':
            blob = await canvas.convertToBlob({
                type: 'image/jpeg',
                quality: quality
            });
            break;
            
        case 'png':
            blob = await canvas.convertToBlob({
                type: 'image/png'
            });
            break;
            
        case 'webp':
            blob = await canvas.convertToBlob({
                type: 'image/webp',
                quality: quality
            });
            break;
            
        default:
            throw new Error(`Encoding not implemented for format: ${format}`);
    }
    
    // Convert blob to array buffer
    const arrayBuffer = await blob.arrayBuffer();
    
    return {
        data: arrayBuffer,
        format: format,
        size: arrayBuffer.byteLength,
        width: width,
        height: height,
        mimeType: getMimeType(format)
    };
}

/**
 * Resize image to specific dimensions
 */
async function resizeImage(data, options = {}) {
    const { width, height, maintainAspectRatio = true, method = 'lanczos' } = options;
    
    if (!width || !height) {
        throw new Error('Width and height are required for resizing');
    }
    
    // Decode image if needed
    let imageData;
    if (data.imageData) {
        imageData = data;
    } else {
        imageData = await decodeImage(data, options);
    }
    
    // Calculate new dimensions
    let newWidth = width;
    let newHeight = height;
    
    if (maintainAspectRatio) {
        const aspectRatio = imageData.width / imageData.height;
        
        if (width / height > aspectRatio) {
            newWidth = Math.round(height * aspectRatio);
        } else {
            newHeight = Math.round(width / aspectRatio);
        }
    }
    
    // Check size limits
    if (newWidth > capabilities.performance.maxImageSize || 
        newHeight > capabilities.performance.maxImageSize) {
        throw new Error(`Image dimensions too large: ${newWidth}x${newHeight}`);
    }
    
    // Create canvas for resizing
    const sourceCanvas = new OffscreenCanvas(imageData.width, imageData.height);
    const sourceCtx = sourceCanvas.getContext('2d');
    
    const destCanvas = new OffscreenCanvas(newWidth, newHeight);
    const destCtx = destCanvas.getContext('2d');
    
    if (!sourceCtx || !destCtx) {
        throw new Error('Failed to get canvas contexts');
    }
    
    // Put source image data
    const imgData = new ImageData(
        new Uint8ClampedArray(imageData.data),
        imageData.width,
        imageData.height
    );
    sourceCtx.putImageData(imgData, 0, 0);
    
    // Draw resized image
    destCtx.drawImage(sourceCanvas, 0, 0, newWidth, newHeight);
    
    // Get resized image data
    const resizedImageData = destCtx.getImageData(0, 0, newWidth, newHeight);
    
    return {
        width: newWidth,
        height: newHeight,
        data: resizedImageData.data.buffer,
        format: imageData.format,
        channels: imageData.channels,
        hasAlpha: imageData.hasAlpha,
        originalWidth: imageData.width,
        originalHeight: imageData.height
    };
}

/**
 * Scale image by a factor
 */
async function scaleImage(data, options = {}) {
    const { scale = 1.0 } = options;
    
    if (scale <= 0) {
        throw new Error('Scale factor must be positive');
    }
    
    // Decode image if needed
    let imageData;
    if (data.imageData) {
        imageData = data;
    } else {
        imageData = await decodeImage(data, options);
    }
    
    const newWidth = Math.round(imageData.width * scale);
    const newHeight = Math.round(imageData.height * scale);
    
    // Use resize function
    return resizeImage(imageData, {
        width: newWidth,
        height: newHeight,
        maintainAspectRatio: true,
        ...options
    });
}

/**
 * Compress image with quality adjustment
 */
async function compressImage(data, options = {}) {
    const { quality = 0.75, format = 'jpeg' } = options;
    
    if (quality < 0 || quality > 1) {
        throw new Error('Quality must be between 0 and 1');
    }
    
    // First encode with specified quality
    const encoded = await encodeImage(data, { format, quality });
    
    return {
        ...encoded,
        originalSize: data.size || 0,
        compressionRatio: data.size ? 
            (data.size - encoded.size) / data.size : 0
    };
}

/**
 * Convert image format
 */
async function convertImage(data, options = {}) {
    const { format: targetFormat, quality = 0.92 } = options;
    
    if (!targetFormat) {
        throw new Error('Target format is required');
    }
    
    // Decode first
    const decoded = await decodeImage(data, options);
    
    // Encode to target format
    return encodeImage(decoded, { format: targetFormat, quality });
}

/**
 * Create thumbnail image
 */
async function createThumbnail(data, options = {}) {
    const { maxSize = 200, quality = 0.85 } = options;
    
    // Decode image if needed
    let imageData;
    if (data.imageData) {
        imageData = data;
    } else {
        imageData = await decodeImage(data, options);
    }
    
    // Calculate thumbnail dimensions
    let thumbWidth, thumbHeight;
    
    if (imageData.width > imageData.height) {
        thumbWidth = maxSize;
        thumbHeight = Math.round((imageData.height / imageData.width) * maxSize);
    } else {
        thumbHeight = maxSize;
        thumbWidth = Math.round((imageData.width / imageData.height) * maxSize);
    }
    
    // Resize
    const resized = await resizeImage(imageData, {
        width: thumbWidth,
        height: thumbHeight,
        maintainAspectRatio: true
    });
    
    // Encode as JPEG for smaller size
    const encoded = await encodeImage(resized, {
        format: 'jpeg',
        quality: quality
    });
    
    return {
        ...encoded,
        originalWidth: imageData.width,
        originalHeight: imageData.height,
        thumbnail: true
    };
}

/**
 * Analyze image characteristics
 */
async function analyzeImage(data, options = {}) {
    // Decode image if needed
    let imageData;
    if (data.imageData) {
        imageData = data;
    } else {
        imageData = await decodeImage(data, options);
    }
    
    // Create canvas for analysis
    const canvas = new OffscreenCanvas(imageData.width, imageData.height);
    const ctx = canvas.getContext('2d');
    
    if (!ctx) {
        throw new Error('Failed to get canvas context');
    }
    
    // Put image data
    const imgData = new ImageData(
        new Uint8ClampedArray(imageData.data),
        imageData.width,
        imageData.height
    );
    ctx.putImageData(imgData, 0, 0);
    
    // Get image data for analysis
    const analysisData = ctx.getImageData(0, 0, imageData.width, imageData.height);
    const pixels = analysisData.data;
    
    // Calculate brightness histogram
    const histogram = new Array(256).fill(0);
    let sumBrightness = 0;
    let sumRed = 0;
    let sumGreen = 0;
    let sumBlue = 0;
    let totalPixels = 0;
    
    for (let i = 0; i < pixels.length; i += 4) {
        const r = pixels[i];
        const g = pixels[i + 1];
        const b = pixels[i + 2];
        
        // Calculate brightness (using weighted average)
        const brightness = Math.round((r * 0.299 + g * 0.587 + b * 0.114));
        
        histogram[brightness]++;
        sumBrightness += brightness;
        sumRed += r;
        sumGreen += g;
        sumBlue += b;
        totalPixels++;
    }
    
    const avgBrightness = sumBrightness / totalPixels;
    const avgRed = sumRed / totalPixels;
    const avgGreen = sumGreen / totalPixels;
    const avgBlue = sumBlue / totalPixels;
    
    // Calculate contrast (standard deviation of brightness)
    let brightnessVariance = 0;
    for (let i = 0; i < pixels.length; i += 4) {
        const r = pixels[i];
        const g = pixels[i + 1];
        const b = pixels[i + 2];
        const brightness = (r * 0.299 + g * 0.587 + b * 0.114);
        
        brightnessVariance += Math.pow(brightness - avgBrightness, 2);
    }
    
    const contrast = Math.sqrt(brightnessVariance / totalPixels);
    
    // Detect if image is mostly dark or light
    const isDark = avgBrightness < 128;
    const isLight = avgBrightness >= 128;
    
    // Estimate dominant color
    const dominantColor = {
        r: Math.round(avgRed),
        g: Math.round(avgGreen),
        b: Math.round(avgBlue),
        hex: rgbToHex(Math.round(avgRed), Math.round(avgGreen), Math.round(avgBlue))
    };
    
    return {
        dimensions: {
            width: imageData.width,
            height: imageData.height,
            aspectRatio: imageData.width / imageData.height,
            megapixels: (imageData.width * imageData.height) / 1000000
        },
        color: {
            average: dominantColor,
            brightness: {
                average: Math.round(avgBrightness),
                histogram: histogram,
                isDark: isDark,
                isLight: isLight
            },
            contrast: Math.round(contrast),
            saturation: estimateSaturation(pixels)
        },
        metadata: {
            format: imageData.format,
            channels: imageData.channels,
            hasAlpha: imageData.hasAlpha,
            fileSize: data.size || 0
        },
        features: {
            hasTransparency: detectTransparency(pixels),
            isGrayscale: detectGrayscale(pixels),
            colorCount: estimateColorCount(pixels)
        }
    };
}

// ========== Utility Functions ==========

/**
 * Get MIME type for image format
 */
function getMimeType(format) {
    const mimeTypes = {
        jpeg: 'image/jpeg',
        jpg: 'image/jpeg',
        png: 'image/png',
        webp: 'image/webp',
        gif: 'image/gif',
        bmp: 'image/bmp',
        tiff: 'image/tiff'
    };
    
    return mimeTypes[format.toLowerCase()] || 'application/octet-stream';
}

/**
 * Convert RGB to hex color
 */
function rgbToHex(r, g, b) {
    return '#' + [r, g, b]
        .map(x => {
            const hex = x.toString(16);
            return hex.length === 1 ? '0' + hex : hex;
        })
        .join('');
}

/**
 * Estimate image saturation
 */
function estimateSaturation(pixels) {
    let sumSaturation = 0;
    let count = 0;
    
    for (let i = 0; i < pixels.length; i += 4) {
        const r = pixels[i];
        const g = pixels[i + 1];
        const b = pixels[i + 2];
        
        // Simple saturation estimation
        const max = Math.max(r, g, b);
        const min = Math.min(r, g, b);
        
        if (max > 0) {
            sumSaturation += (max - min) / max;
            count++;
        }
    }
    
    return count > 0 ? sumSaturation / count : 0;
}

/**
 * Detect transparency in image
 */
function detectTransparency(pixels) {
    for (let i = 3; i < pixels.length; i += 4) {
        if (pixels[i] < 255) {
            return true;
        }
    }
    
    return false;
}

/**
 * Detect if image is grayscale
 */
function detectGrayscale(pixels) {
    const sampleSize = Math.min(1000, pixels.length / 4);
    const step = Math.floor(pixels.length / 4 / sampleSize);
    
    let grayscalePixels = 0;
    let totalSampled = 0;
    
    for (let i = 0; i < pixels.length; i += step * 4) {
        const r = pixels[i];
        const g = pixels[i + 1];
        const b = pixels[i + 2];
        
        // Consider pixel grayscale if RGB values are very close
        const maxDiff = Math.max(
            Math.abs(r - g),
            Math.abs(r - b),
            Math.abs(g - b)
        );
        
        if (maxDiff <= 10) { // Tolerance threshold
            grayscalePixels++;
        }
        
        totalSampled++;
        
        if (totalSampled >= sampleSize) {
            break;
        }
    }
    
    // Consider image grayscale if > 90% of sampled pixels are grayscale
    return totalSampled > 0 && (grayscalePixels / totalSampled) > 0.9;
}

/**
 * Estimate number of distinct colors
 */
function estimateColorCount(pixels) {
    // Sample pixels to estimate color count
    const sampleSize = Math.min(5000, pixels.length / 4);
    const step = Math.floor(pixels.length / 4 / sampleSize);
    
    const colorSet = new Set();
    
    for (let i = 0; i < pixels.length; i += step * 4) {
        const r = pixels[i];
        const g = pixels[i + 1];
        const b = pixels[i + 2];
        
        // Quantize colors to reduce distinct count
        const quantized = quantizeColor(r, g, b, 16);
        colorSet.add(quantized);
        
        if (colorSet.size > 1000) {
            // Too many colors, return estimated count
            return Math.round(pixels.length / 4 / step);
        }
    }
    
    return colorSet.size;
}

/**
 * Quantize color to reduce distinct colors
 */
function quantizeColor(r, g, b, levels = 16) {
    const step = 256 / levels;
    const qr = Math.floor(r / step) * step;
    const qg = Math.floor(g / step) * step;
    const qb = Math.floor(b / step) * step;
    
    return (qr << 16) | (qg << 8) | qb;
}

/**
 * Handle ping request
 */
function handlePing() {
    return {
        status: 'ok',
        timestamp: Date.now(),
        uptime: Date.now() - workerState.startupTime,
        state: workerState
    };
}

/**
 * Get worker capabilities
 */
function getCapabilities() {
    return {
        ...capabilities,
        state: workerState
    };
}

// Send startup message
self.postMessage({
    type: 'startup',
    workerState,
    capabilities,
    timestamp: Date.now()
});

console.log('Image Processor Worker started');