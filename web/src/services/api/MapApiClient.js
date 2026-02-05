// Map API Client - Handles communication with the C++ map server

import axios from 'axios';
import { getApiUrl, getQualitySettings } from '../../config/settings.js';
import { buildRequest } from '../../config/endpoints.js';
import { RequestError, ValidationError, TimeoutError } from './errorHandler.js';

export class MapApiClient {
    constructor(baseURL) {
        this.baseURL = baseURL || getApiUrl('');
        this.client = axios.create({
            baseURL: this.baseURL,
            timeout: 30000,
            headers: {
                'Content-Type': 'application/json',
                Accept: 'image/png, application/json',
                'X-Client-Version': '1.0.0',
            },
            responseType: 'json',
        });

        // Add request interceptor
        this.client.interceptors.request.use(this.requestInterceptor.bind(this), (error) =>
            Promise.reject(error)
        );

        // Add response interceptor
        this.client.interceptors.response.use(
            this.responseInterceptor.bind(this),
            this.errorInterceptor.bind(this)
        );

        this.activeRequests = new Map();
        this.requestCounter = 0;
    }

    // ========== Public API Methods ==========

    /**
     * Generate a map image
     * @param {Object} params - Map generation parameters
     * @returns {Promise<ArrayBuffer>} - Image data
     */
    async generateMap(params) {
        try {
            // Validate and normalize parameters
            const validatedParams = this.validateGenerateParams(params);

            // Build the request
            const request = buildRequest('GENERATE_MAP', {
                body: validatedParams,
            });

            // Generate request ID for cancellation
            const requestId = `generate_${Date.now()}_${++this.requestCounter}`;

            // Execute request
            const response = await this.executeRequest(request, requestId, { responseType: 'arraybuffer' });

            // Extract metadata from headers if available
            const metadata = this.extractMetadata(response.headers);

            return {
                image: response.data,
                metadata: {
                    ...metadata,
                    bbox: validatedParams.bbox,
                    width: validatedParams.width,
                    height: validatedParams.height,
                },
            };
        } catch (error) {
            // Re-throw with additional context
            throw this.enhanceError(error, 'generateMap', params);
        }
    }

    /**
     * Get a map tile
     * @param {number} z - Zoom level
     * @param {number} x - Tile X coordinate
     * @param {number} y - Tile Y coordinate
     * @param {Object} options - Additional options
     * @returns {Promise<ArrayBuffer>} - Tile image data
     */
    async getTile(z, x, y, options = {}) {
        try {
            const request = buildRequest('GET_TILE', {
                path: { z, x, y },
                query: options,
            });

            const requestId = `tile_${z}_${x}_${y}_${Date.now()}`;
            const response = await this.executeRequest(request, requestId, { responseType: 'arraybuffer' });

            return {
                image: response.data,
                metadata: {
                    z,
                    x,
                    y,
                    timestamp: new Date().toISOString(),
                    ...this.extractMetadata(response.headers),
                },
            };
        } catch (error) {
            throw this.enhanceError(error, 'getTile', { z, x, y, options });
        }
    }

    /**
     * List available layers
     * @returns {Promise<Array>} - List of layer objects
     */
    async listLayers() {
        try {
            const request = buildRequest('LIST_LAYERS');
            const response = await this.executeRequest(request, 'list_layers', { responseType: 'json' });
            return response.data;
        } catch (error) {
            throw this.enhanceError(error, 'listLayers');
        }
    }

    /**
     * Get layer information
     * @param {string} layerId - Layer identifier
     * @returns {Promise<Object>} - Layer details
     */
    async getLayerInfo(layerId) {
        try {
            const request = buildRequest('GET_LAYER_INFO', {
                path: { layerId },
            });

            const response = await this.executeRequest(request, `layer_info_${layerId}`, { responseType: 'json' });
            return response.data;
        } catch (error) {
            throw this.enhanceError(error, 'getLayerInfo', { layerId });
        }
    }

    /**
     * Get service capabilities
     * @returns {Promise<Object>} - Service capabilities
     */
    async getCapabilities() {
        try {
            const request = buildRequest('GET_CAPABILITIES');
            const response = await this.executeRequest(request, 'capabilities', { responseType: 'json' });
            return response.data;
        } catch (error) {
            throw this.enhanceError(error, 'getCapabilities');
        }
    }

    /**
     * Check service health
     * @returns {Promise<Object>} - Health status
     */
    async getHealth() {
        try {
            const request = buildRequest('GET_HEALTH');

            // Use shorter timeout for health checks
            const response = await this.executeRequest(
                {
                    ...request,
                    timeout: 5000,
                },
                'health',
                { responseType: 'json' }
            );

            return response.data;
        } catch (error) {
            throw this.enhanceError(error, 'getHealth');
        }
    }

    /**
     * Submit metrics
     * @param {Array} metrics - Metrics data
     * @returns {Promise<void>}
     */
    async submitMetrics(metrics) {
        try {
            const request = buildRequest('POST_METRICS', {
                body: { metrics },
            });

            await this.executeRequest(request, 'metrics', { responseType: 'json' });
        } catch (error) {
            // Don't throw for metric submission errors
            console.warn('Failed to submit metrics:', error);
        }
    }

    /**
     * Cancel a specific request
     * @param {string} requestId - Request identifier
     */
    cancelRequest(requestId) {
        const controller = this.activeRequests.get(requestId);
        if (controller) {
            controller.abort();
            this.activeRequests.delete(requestId);
        }
    }

    /**
     * Cancel all pending requests
     */
    cancelAllRequests() {
        for (const [requestId, controller] of this.activeRequests) {
            controller.abort();
        }
        this.activeRequests.clear();
    }

    // ========== Private Methods ==========

    requestInterceptor(config) {
        // Generate request ID if not provided
        if (!config.requestId) {
            config.requestId = `req_${Date.now()}_${++this.requestCounter}`;
        }

        // Create AbortController for this request
        const controller = new AbortController();
        config.signal = controller.signal;
        config.timeout = config.timeout || 30000;

        // Store controller for potential cancellation
        this.activeRequests.set(config.requestId, controller);

        // Add timestamp for performance tracking
        config.metadata = {
            startTime: performance.now(),
            requestId: config.requestId,
        };

        return config;
    }

    responseInterceptor(response) {
        const { requestId } = response.config.metadata;
        const endTime = performance.now();

        // Calculate request duration
        const duration = endTime - response.config.metadata.startTime;

        // Clean up active requests
        this.activeRequests.delete(requestId);

        // Add performance metadata
        response.metadata = {
            requestId,
            duration,
            timestamp: new Date().toISOString(),
            status: response.status,
            statusText: response.statusText,
        };

        return response;
    }

    errorInterceptor(error) {
        // Clean up active requests
        if (error.config?.metadata?.requestId) {
            this.activeRequests.delete(error.config.metadata.requestId);
        }

        // Enhance error with additional context
        const enhancedError = this.classifyError(error);

        // Log error (could be sent to monitoring service)
        this.logError(enhancedError, error.config);

        return Promise.reject(enhancedError);
    }

    async executeRequest(request, requestId, options = {}) {
        try {
            const config = {
                method: request.method,
                url: request.url,
                data: request.body,
                timeout: request.timeout,
                requestId,
                ...options,
            };

            const response = await this.client(config);

            return response;
        } catch (error) {
            // Handle timeout specifically
            if (error.code === 'ECONNABORTED' || error.message.includes('timeout')) {
                throw new TimeoutError(`Request timeout after ${request.timeout}ms`, {
                    requestId,
                    url: request.url,
                    method: request.method,
                });
            }

            throw error;
        }
    }

    validateGenerateParams(params) {
        const errors = [];
        const result = { ...params };

        // Validate bbox
        if (!Array.isArray(result.bbox) || result.bbox.length !== 4) {
            errors.push('bbox must be an array of 4 numbers [west, south, east, north]');
        } else {
            const [west, south, east, north] = result.bbox;

            if (west < -180 || east > 180 || south < -90 || north > 90) {
                errors.push('bbox coordinates out of valid range [-180, -90, 180, 90]');
            }

            if (west >= east || south >= north) {
                errors.push('bbox coordinates invalid: west < east and south < north required');
            }

            // Calculate area and warn if too large
            const area = (east - west) * (north - south);
            if (area > 10000) {
                console.warn(`Requested bbox area (${area} sq degrees) is very large`);
            }
        }

        // Validate width and height
        result.width = Math.min(Math.max(parseInt(result.width) || 1024, 1), 4096);
        result.height = Math.min(Math.max(parseInt(result.height) || 768, 1), 4096);

        // Validate layers
        if (!Array.isArray(result.layers)) {
            result.layers = [];
        }

        // Validate styles
        if (!result.styles || typeof result.styles !== 'object') {
            result.styles = {};
        }

        // Validate background color
        if (!result.backgroundColor || !/^#[0-9a-fA-F]{6}$/.test(result.backgroundColor)) {
            result.backgroundColor = '#ffffff';
        }

        // Validate format
        const validFormats = ['png', 'webp', 'jpeg'];
        if (!validFormats.includes(result.format)) {
            result.format = 'png';
        }

        // Validate quality
        result.quality = Math.min(Math.max(parseInt(result.quality) || 90, 0), 100);

        // Apply quality settings
        const qualitySettings = getQualitySettings(result.quality);
        if (qualitySettings) {
            // Scale dimensions if needed
            if (qualitySettings.scale && qualitySettings.scale !== 1) {
                result.width = Math.floor(result.width * qualitySettings.scale);
                result.height = Math.floor(result.height * qualitySettings.scale);
            }
        }

        if (errors.length > 0) {
            throw new ValidationError('Invalid map generation parameters', errors);
        }

        return result;
    }

    extractMetadata(headers) {
        const metadata = {
            renderTime: parseInt(headers['x-render-time']) || 0,
            cacheHit: headers['x-cache-hit'] === 'true',
            featuresCount: parseInt(headers['x-features-count']) || 0,
            timestamp: headers['x-timestamp'] || new Date().toISOString(),
        };

        // Try to extract bbox from headers if available
        const bboxHeader = headers['x-bbox'];
        if (bboxHeader) {
            try {
                metadata.bbox = JSON.parse(bboxHeader);
            } catch (e) {
                // Ignore parsing errors
            }
        }

        return metadata;
    }

    classifyError(error) {
        if (
            error instanceof ValidationError ||
            error instanceof RequestError ||
            error instanceof TimeoutError
        ) {
            return error;
        }

        if (error.response) {
            // Server responded with error status
            let message = error.message;

            try {
                // Handle different response types
                let data;
                const responseType = error.config?.responseType;
                if (responseType === 'json' && typeof error.response.data === 'object') {
                    // Already parsed JSON object
                    data = error.response.data;
                } else if (error.response.data instanceof ArrayBuffer) {
                    // ArrayBuffer needs decoding
                    data = JSON.parse(new TextDecoder().decode(error.response.data));
                } else if (typeof error.response.data === 'string') {
                    // String that might be JSON
                    data = JSON.parse(error.response.data);
                } else {
                    // Fallback: try to parse as string
                    const text = String(error.response.data);
                    data = JSON.parse(text);
                }
                
                if (data.error?.message) {
                    message = data.error.message;
                }
            } catch (e) {
                // Ignore parsing errors
            }

            return new RequestError(message, {
                status: error.response.status,
                statusText: error.response.statusText,
                data: error.response.data,
            });
        }

        if (error.request) {
            // Request made but no response
            return new RequestError('No response from server', {
                originalError: error.message,
            });
        }

        // Network or other error
        return new RequestError('Network error', {
            originalError: error.message,
        });
    }

    enhanceError(error, method, params) {
        // Add context to the error
        error.context = {
            method,
            params,
            timestamp: new Date().toISOString(),
            clientId: this.clientId,
        };

        return error;
    }

    logError(error, config) {
        const logEntry = {
            timestamp: new Date().toISOString(),
            level: 'error',
            error: {
                name: error.name,
                message: error.message,
                stack: error.stack,
            },
            request: config
                ? {
                      url: config.url,
                      method: config.method,
                      requestId: config.metadata?.requestId,
                  }
                : null,
        };

        // Log to console in development
        if (process.env.NODE_ENV === 'development') {
            console.error('API Error:', logEntry);
        }

        // Could send to monitoring service here
        // this.monitoringService.logError(logEntry);
    }
}

// Default export
export default MapApiClient;
