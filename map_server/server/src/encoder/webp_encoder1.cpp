#include "webp_encoder.h"
#include "../utils/logger.h"
#include <algorithm>
#include <cstring>

namespace cycle {
namespace encoder {

WebPEncoder::WebPEncoder()
    : quality_(80)
    , compressionLevel_(6)
    , initialized_(false) {
    
    LOG_INFO("WebP encoder created");
}

WebPEncoder::~WebPEncoder() {
    LOG_INFO("WebP encoder destroyed");
}

bool WebPEncoder::Initialize() {
    if (initialized_) {
        LOG_WARN("WebP encoder is already initialized");
        return true;
    }
    
    int version = WebPGetEncoderVersion();
    LOG_INFO("WebP encoder version: " + std::to_string(version));
    
    initialized_ = true;
    LOG_INFO("WebP encoder initialized successfully");
    
    return true;
}

bool WebPEncoder::IsInitialized() const {
    return initialized_;
}
EncodedImage WebPEncoder::Encode(const RawImage& image, const EncodeOptions& options) {
    if (!initialized_) {
        lastError_ = "WebP encoder not initialized";
        LOG_ERROR(lastError_);
        return {};
    }
    
    if (!ValidateImageData(image)) {
        lastError_ = "Invalid image data";
        LOG_ERROR(lastError_);
        return {};
    }
    
    if (quality_ >= 100) {
        return EncodeLossless(image);
    } else {
        return EncodeLossy(image);
    }
}

ImageData WebPEncoder::Decode(const std::vector<uint8_t>& data) {
    if (!initialized_) {
        lastError_ = "WebP encoder not initialized";
        LOG_ERROR(lastError_);
        return ImageData{};
    }
    
    if (data.empty()) {
        lastError_ = "Empty data provided for decoding";
        LOG_ERROR(lastError_);
        return ImageData{};
    }
    
    int width = 0, height = 0;
    if (!WebPGetInfo(data.data(), data.size(), &width, &height)) {
        lastError_ = "Invalid WebP data";
        LOG_ERROR(lastError_);
        return ImageData{};
    }
    
    WebPDecoderConfig config;
    if (!WebPInitDecoderConfig(&config)) {
        lastError_ = "Failed to initialize WebP decoder config";
        LOG_ERROR(lastError_);
        return ImageData{};
    }
    
    VP8StatusCode status = WebPGetFeatures(data.data(), data.size(), &config.input);
    if (status != VP8_STATUS_OK) {
        lastError_ = "Failed to get WebP features: " + std::to_string(status);
        LOG_ERROR(lastError_);
        return ImageData{};
    }
    
    bool hasAlpha = config.input.has_alpha;
    ImageData result;
    result.width = width;
    result.height = height;
    result.format = hasAlpha ? ImageFormat::RGBA : ImageFormat::RGB;
    
    size_t pixelCount = width * height;
    size_t bytesPerPixel = hasAlpha ? 4 : 3;
    result.data.resize(pixelCount * bytesPerPixel);
    
    config.output.colorspace = hasAlpha ? MODE_RGBA : MODE_RGB;
    config.output.u.RGBA.rgba = result.data.data();
    config.output.u.RGBA.stride = width * bytesPerPixel;
    config.output.u.RGBA.size = result.data.size();
    config.output.is_external_memory = 1;
    
    status = WebPDecode(data.data(), data.size(), &config);
    if (status != VP8_STATUS_OK) {
        lastError_ = "WebP decoding failed: " + std::to_string(status);
        LOG_ERROR(lastError_);
        return ImageData{};
    }
    
    LOG_DEBUG("WebP image decoded: " + std::to_string(width) + "x" + std::to_string(height) + 
              " (" + (hasAlpha ? "RGBA" : "RGB") + ")");
    
    return result;
}

bool WebPEncoder::SupportsFormat(ImageFormat format) const {
    return format == ImageFormat::RGB || 
           format == ImageFormat::RGBA ||
           format == ImageFormat::WEBP;
}

std::string WebPEncoder::GetName() const {
    return "WebP";
}

void WebPEncoder::SetQuality(int quality) {
    quality_ = std::max(0, std::min(100, quality));
    LOG_DEBUG("WebP quality set to: " + std::to_string(quality_));
}

int WebPEncoder::GetQuality() const {
    return quality_;
}

void WebPEncoder::SetCompressionLevel(int level) {
    compressionLevel_ = std::max(0, std::min(9, level));
    LOG_DEBUG("WebP compression level set to: " + std::to_string(compressionLevel_));
}

int WebPEncoder::GetCompressionLevel() const {
    return compressionLevel_;
}

std::string WebPEncoder::GetLastError() const {
    return lastError_;
}

std::vector<uint8_t> WebPEncoder::EncodeLossless(const ImageData& image) {
    std::vector<uint8_t> output;
    
    if (image.format == ImageFormat::RGBA) {
        std::vector<uint8_t> rgbaData;
        if (!ConvertToRGBA(image, rgbaData)) {
            return {};
        }
        
        size_t size = WebPEncodeLosslessRGBA(rgbaData.data(), image.width, image.height, 
                                            image.width * 4, &output);
        
        if (size == 0) {
            lastError_ = "WebP lossless RGBA encoding failed";
            LOG_ERROR(lastError_);
            return {};
        }
    } else {
        std::vector<uint8_t> rgbData;
        if (!ConvertToRGB(image, rgbData)) {
            return {};
        }
        
        size_t size = WebPEncodeLosslessRGB(rgbData.data(), image.width, image.height, 
                                           image.width * 3, &output);
        
        if (size == 0) {
            lastError_ = "WebP lossless RGB encoding failed";
            LOG_ERROR(lastError_);
            return {};
        }
    }
    
    LOG_DEBUG("WebP lossless encoding completed: " + std::to_string(output.size()) + " bytes");
    return output;
}

std::vector<uint8_t> WebPEncoder::EncodeLossy(const ImageData& image) {
    WebPConfig config;
    if (!WebPConfigInit(&config)) {
        lastError_ = "Failed to initialize WebP config";
        LOG_ERROR(lastError_);
        return {};
    }
    
    config.quality = quality_;
    config.method = compressionLevel_;
    config.lossless = 0;
    
    if (!WebPValidateConfig(&config)) {
        lastError_ = "Invalid WebP config";
        LOG_ERROR(lastError_);
        return {};
    }
    
    WebPPicture picture;
    if (!WebPPictureInit(&picture)) {
        lastError_ = "Failed to initialize WebP picture";
        LOG_ERROR(lastError_);
        return {};
    }
    
    picture.width = image.width;
    picture.height = image.height;
    
    WebPMemoryWriter writer;
    WebPMemoryWriterInit(&writer);
    picture.writer = WebPMemoryWrite;
    picture.custom_ptr = &writer;
    
    std::vector<uint8_t> output;
    
    if (image.format == ImageFormat::RGBA) {
        std::vector<uint8_t> rgbaData;
        if (!ConvertToRGBA(image, rgbaData)) {
            return {};
        }
        
        if (WebPPictureImportRGBA(&picture, rgbaData.data(), image.width * 4) == 0) {
            lastError_ = "Failed to import RGBA data to WebP picture";
            LOG_ERROR(lastError_);
            WebPPictureFree(&picture);
            return {};
        }
    } else {
        std::vector<uint8_t> rgbData;
        if (!ConvertToRGB(image, rgbData)) {
            return {};
        }
        
        if (WebPPictureImportRGB(&picture, rgbData.data(), image.width * 3) == 0) {
            lastError_ = "Failed to import RGB data to WebP picture";
            LOG_ERROR(lastError_);
            WebPPictureFree(&picture);
            return {};
        }
    }
    
    if (WebPEncode(&config, &picture) == 0) {
        lastError_ = "WebP encoding failed: " + std::string(picture.error_code);
        LOG_ERROR(lastError_);
        WebPPictureFree(&picture);
        return {};
    }
    
    output.assign(writer.mem, writer.mem + writer.size);
    WebPMemoryWriterClear(&writer);
    WebPPictureFree(&picture);
    
    LOG_DEBUG("WebP lossy encoding completed: " + std::to_string(output.size()) + " bytes");
    return output;
}

bool WebPEncoder::ValidateImageData(const ImageData& image) const {
    if (image.width <= 0 || image.height <= 0) {
        return false;
    }
    
    if (image.data.empty()) {
        return false;
    }
    
    size_t expectedSize = 0;
    switch (image.format) {
        case ImageFormat::RGB:
            expectedSize = image.width * image.height * 3;
            break;
        case ImageFormat::RGBA:
            expectedSize = image.width * image.height * 4;
            break;
        case ImageFormat::GRAYSCALE:
            expectedSize = image.width * image.height;
            break;
        default:
            return false;
    }
    
    return image.data.size() == expectedSize;
}

bool WebPEncoder::ConvertToRGB(const ImageData& image, std::vector<uint8_t>& rgbData) const {
    if (image.format == ImageFormat::RGB) {
        rgbData = image.data;
        return true;
    }
    
    if (image.format == ImageFormat::RGBA) {
        rgbData.resize(image.width * image.height * 3);
        
        for (size_t i = 0; i < image.width * image.height; ++i) {
            size_t rgbaIndex = i * 4;
            size_t rgbIndex = i * 3;
            
            rgbData[rgbIndex] = image.data[rgbaIndex];
            rgbData[rgbIndex + 1] = image.data[rgbaIndex + 1];
            rgbData[rgbIndex + 2] = image.data[rgbaIndex + 2];
        }
        
        return true;
    }
    
    if (image.format == ImageFormat::GRAYSCALE) {
        rgbData.resize(image.width * image.height * 3);
        
        for (size_t i = 0; i < image.width * image.height; ++i) {
            size_t rgbIndex = i * 3;
            uint8_t gray = image.data[i];
            
            rgbData[rgbIndex] = gray;
            rgbData[rgbIndex + 1] = gray;
            rgbData[rgbIndex + 2] = gray;
        }
        
        return true;
    }
    
    return false;
}

bool WebPEncoder::ConvertToRGBA(const ImageData& image, std::vector<uint8_t>& rgbaData) const {
    if (image.format == ImageFormat::RGBA) {
        rgbaData = image.data;
        return true;
    }
    
    if (image.format == ImageFormat::RGB) {
        rgbaData.resize(image.width * image.height * 4);
        
        for (size_t i = 0; i < image.width * image.height; ++i) {
            size_t rgbIndex = i * 3;
            size_t rgbaIndex = i * 4;
            
            rgbaData[rgbaIndex] = image.data[rgbIndex];
            rgbaData[rgbaIndex + 1] = image.data[rgbIndex + 1];
            rgbaData[rgbaIndex + 2] = image.data[rgbIndex + 2];
            rgbaData[rgbaIndex + 3] = 255;
        }
        
        return true;
    }
    
    if (image.format == ImageFormat::GRAYSCALE) {
        rgbaData.resize(image.width * image.height * 4);
        
        for (size_t i = 0; i < image.width * image.height; ++i) {
            size_t rgbaIndex = i * 4;
            uint8_t gray = image.data[i];
            
            rgbaData[rgbaIndex] = gray;
            rgbaData[rgbaIndex + 1] = gray;
            rgbaData[rgbaIndex + 2] = gray;
            rgbaData[rgbaIndex + 3] = 255;
        }
        
        return true;
    }
    
    return false;
}

} // namespace encoder
} // namespace cycle