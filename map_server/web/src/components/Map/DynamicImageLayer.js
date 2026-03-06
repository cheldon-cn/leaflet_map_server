// Dynamic Image Layer - Manages dynamic map image overlay

import { EventEmitter } from '../../services/utils/EventEmitter.js';
import { debounce } from '../../services/utils/debounce.js';

export class DynamicImageLayer extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            map: options.map,
            apiClient: options.apiClient,
            store: options.store,
            cacheManager: options.cacheManager,
            workerManager: options.workerManager,
            onLoadingStart: options.onLoadingStart || (() => {}),
            onLoadingEnd: options.onLoadingEnd || (() => {}),
            onError: options.onError || (() => {}),
            debounceDelay: options.debounceDelay || 200,
            maxImageSize: options.maxImageSize || 4096,
            defaultBackgroundColor: options.defaultBackgroundColor || '#ffffff',
            ...options,
        };

        if (!this.options.map) {
            throw new Error('Map instance is required');
        }

        if (!this.options.apiClient) {
            throw new Error('API client is required');
        }

        this.map = this.options.map;
        this.apiClient = this.options.apiClient;
        this.store = this.options.store;
        this.cacheManager = this.options.cacheManager;
        this.workerManager = this.options.workerManager;

        // Layer state
        this.imageOverlay = null;
        this.currentImage = null;
        this.currentRequest = null;
        this.isLoading = false;
        this.lastRequestTime = 0;
        this.errorCount = 0;
        this.maxErrorCount = 3;

        // Cache
        this.cache = new Map();
        this.pendingRequests = new Map();

        // Configuration
        this.layers = options.layers || [];
        this.styles = options.styles || {};
        this.backgroundColor = options.backgroundColor || this.options.defaultBackgroundColor;
        this.quality = options.quality || 'medium';
        this.format = options.format || 'png';

        // Stats
        this.stats = {
            totalRequests: 0,
            successfulRequests: 0,
            failedRequests: 0,
            cacheHits: 0,
            cacheMisses: 0,
            averageLoadTime: 0,
            lastLoadTime: 0,
        };

        // Setup
        this.setupEventListeners();
        this.initDebouncedUpdate();

        console.debug('DynamicImageLayer initialized');
    }

    /**
     * Initialize the layer
     */
    async init() {
        try {
            // Load initial layers from store
            await this.loadLayersFromStore();

            // Generate initial image
            await this.updateViewport();

            this.emit('initialized');
        } catch (error) {
            console.error('Failed to initialize DynamicImageLayer:', error);
            this.handleError(error, 'init');
        }
    }

    /**
     * Setup event listeners
     */
    setupEventListeners() {
        // Map move events
        this.map.on('moveend', () => this.onMapMove());
        this.map.on('zoomend', () => this.onMapZoom());
        this.map.on('resize', () => this.onMapResize());

        // Store subscriptions
        if (this.store) {
            this.store.subscribe(() => this.onStoreUpdate());
        }
    }

    /**
     * Initialize debounced update function
     */
    initDebouncedUpdate() {
        this.debouncedUpdateViewport = debounce(
            () => this.updateViewport(),
            this.options.debounceDelay
        );
    }

    /**
     * Handle map move event
     */
    onMapMove() {
        this.debouncedUpdateViewport();
    }

    /**
     * Handle map zoom event
     */
    onMapZoom() {
        this.debouncedUpdateViewport();
    }

    /**
     * Handle map resize event
     */
    onMapResize() {
        // Update image overlay bounds
        if (this.imageOverlay && this.map) {
            const bounds = this.map.getBounds();
            if (bounds && typeof bounds.getSouthWest === 'function') {
                this.imageOverlay.setBounds(bounds);
            }
        }
    }

    /**
     * Handle store update
     */
    onStoreUpdate() {
        const state = this.store.getState();

        // Update layer state if changed
        if (state.layers !== this.layers) {
            this.layers = state.layers;
            this.updateViewport();
        }

        // Update styles if changed
        if (state.styles !== this.styles) {
            this.styles = state.styles;
            this.updateViewport();
        }

        // Update background color if changed
        if (state.backgroundColor !== this.backgroundColor) {
            this.backgroundColor = state.backgroundColor;
            this.updateViewport();
        }
    }

    /**
     * Update viewport and generate new image
     */
    async updateViewport() {
        // Cancel previous request if still pending
        this.cancelCurrentRequest();

        // Skip if already loading
        if (this.isLoading) {
            return;
        }

        try {
            this.setLoading(true);

            // Generate request parameters
            const params = this.generateRequestParams();
            const cacheKey = this.generateCacheKey(params);

            // Check cache first
            const cachedResult = this.getFromCache(cacheKey);
            if (cachedResult) {
                console.debug('Cache hit for key:', cacheKey);
                this.applyImage(cachedResult.image, cachedResult.metadata);
                this.stats.cacheHits++;
                return;
            }

            this.stats.cacheMisses++;

            // Create request ID for cancellation
            const requestId = `req_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
            this.currentRequest = { id: requestId, params };

            // Execute API request
            const startTime = performance.now();

            try {
                const result = await this.apiClient.generateMap(params);

                const loadTime = performance.now() - startTime;
                this.stats.lastLoadTime = loadTime;
                this.updateAverageLoadTime(loadTime);

                // Store in cache
                this.storeInCache(cacheKey, result);

                // Apply image to map
                this.applyImage(result.image, result.metadata);

                this.stats.successfulRequests++;
                this.errorCount = 0; // Reset error count on success

                this.emit('imageLoaded', {
                    metadata: result.metadata,
                    loadTime,
                    cacheHit: false,
                });
            } catch (error) {
                const loadTime = performance.now() - startTime;

                this.stats.failedRequests++;
                this.errorCount++;

                this.handleError(error, 'generateMap', {
                    params,
                    loadTime,
                    requestId,
                });

                throw error;
            }
        } finally {
            this.setLoading(false);
            this.currentRequest = null;
        }
    }

    /**
     * Generate request parameters from current map state
     */
    generateRequestParams() {
        const bounds = this.map.getBounds();
        const size = this.map.getSize();

        // Validate bounds
        if (!bounds || typeof bounds.getWest !== 'function') {
            console.warn('Invalid bounds object, using default bounds');
            return {
                bbox: [-180, -90, 180, 90], // Default world bounds
                width: 1024,
                height: 768,
                layers: this.layers.filter((layer) => layer.visible).map((layer) => layer.id),
                styles: this.styles,
                backgroundColor: this.backgroundColor,
                format: this.format,
                quality: this.mapQualityToNumber(this.quality),
            };
        }

        // Ensure minimum size
        const width = Math.max(Math.min(size.x, this.options.maxImageSize), 1);
        const height = Math.max(Math.min(size.y, this.options.maxImageSize), 1);

        return {
            bbox: [bounds.getWest(), bounds.getSouth(), bounds.getEast(), bounds.getNorth()],
            width: width,
            height: height,
            layers: this.layers.filter((layer) => layer.visible).map((layer) => layer.id),
            styles: this.styles,
            backgroundColor: this.backgroundColor,
            format: this.format,
            quality: this.mapQualityToNumber(this.quality),
        };
    }

    /**
     * Apply image to map
     */
    applyImage(imageData, metadata) {
        // Convert ArrayBuffer to Blob URL for Leaflet
        const blob = new Blob([imageData], { type: `image/${this.format}` });
        const imageUrl = URL.createObjectURL(blob);

        // Clean up previous image overlay
        if (this.imageOverlay) {
            this.map.removeLayer(this.imageOverlay);
            // Revoke previous blob URL
            if (this.currentImage) {
                URL.revokeObjectURL(this.currentImage);
            }
        }

        // Create new image overlay
        if (!this.map) {
            console.error('Map instance not available for image overlay');
            return;
        }
        
        let bounds = this.map.getBounds();
        if (!bounds || typeof bounds.getSouthWest !== 'function') {
            console.warn('Invalid map bounds, using default bounds for image overlay');
            // Use default world bounds as fallback
            bounds = L.latLngBounds([-90, -180], [90, 180]);
        }
        
        this.imageOverlay = L.imageOverlay(imageUrl, bounds, {
            opacity: 0.9,
            interactive: false,
            className: 'dynamic-image-layer',
        }).addTo(this.map);

        // Store current image URL for cleanup
        this.currentImage = imageUrl;

        // Update store with image metadata
        if (this.store) {
            this.store.updateImageState({
                url: imageUrl,
                size: {
                    width: metadata?.width || 0,
                    height: metadata?.height || 0,
                },
                loadTime: metadata?.renderTime || 0,
                timestamp: metadata?.timestamp || new Date().toISOString(),
            });
        }

        // Emit event
        this.emit('imageApplied', {
            metadata,
            imageUrl,
            timestamp: new Date().toISOString(),
        });
    }

    /**
     * Cancel current request
     */
    cancelCurrentRequest() {
        if (this.currentRequest && this.apiClient.cancelRequest) {
            this.apiClient.cancelRequest(this.currentRequest.id);
        }

        this.currentRequest = null;
    }

    /**
     * Set loading state
     */
    setLoading(isLoading) {
        this.isLoading = isLoading;

        if (isLoading) {
            this.options.onLoadingStart();
            this.emit('loadingStart');
        } else {
            this.options.onLoadingEnd();
            this.emit('loadingEnd');
        }
    }

    /**
     * Update layers
     */
    updateLayers(layers) {
        this.layers = layers;
        this.updateViewport();
    }

    /**
     * Update quality setting
     */
    updateQuality(quality) {
        this.quality = quality;
        this.updateViewport();
    }

    /**
     * Update cache strategy
     */
    updateCacheStrategy(strategy) {
        if (this.cacheManager) {
            this.cacheManager.updateStrategy(strategy);
        }
        console.debug('Cache strategy updated:', strategy);
    }

    /**
     * Retry failed requests
     */
    retryFailedRequests() {
        if (this.errorCount > 0) {
            console.debug('Retrying failed requests, error count:', this.errorCount);
            this.updateViewport();
        }
    }

    /**
     * Load layers from store
     */
    async loadLayersFromStore() {
        if (!this.store) {
            return;
        }

        try {
            const state = this.store.getState();
            this.layers = state.layers || [];
            this.styles = state.styles || {};
            this.backgroundColor = state.backgroundColor || this.options.defaultBackgroundColor;
        } catch (error) {
            console.error('Failed to load layers from store:', error);
        }
    }

    // ========== Cache Management ==========

    /**
     * Generate cache key from parameters
     */
    generateCacheKey(params) {
        // Create a stable string representation
        const keyParts = [];

        keyParts.push(`bbox:${params.bbox.map((v) => v.toFixed(6)).join(',')}`);
        keyParts.push(`w:${params.width}`);
        keyParts.push(`h:${params.height}`);

        if (params.layers && params.layers.length > 0) {
            keyParts.push(`layers:${params.layers.sort().join(',')}`);
        }

        if (params.styles && Object.keys(params.styles).length > 0) {
            // Simple hash of styles object
            const stylesStr = JSON.stringify(params.styles);
            keyParts.push(`styles:${this.hashString(stylesStr)}`);
        }

        keyParts.push(`bg:${params.backgroundColor}`);
        keyParts.push(`fmt:${params.format}`);
        keyParts.push(`q:${params.quality}`);

        const keyString = keyParts.join('|');
        return `img_${this.hashString(keyString)}`;
    }

    /**
     * Get image from cache
     */
    async getFromCache(cacheKey) {
        // First check internal cache
        const entry = this.cache.get(cacheKey);
        if (entry) {
            // Check if entry is expired
            if (Date.now() - entry.timestamp > entry.ttl) {
                this.cache.delete(cacheKey);
            } else {
                return entry.data;
            }
        }

        // Then check cache manager if available
        if (this.cacheManager) {
            try {
                const result = await this.cacheManager.get(cacheKey, () => null); // fetchFn returns null, we only want cached
                if (result) {
                    // Also store in internal cache for faster access
                    this.storeInCache(cacheKey, result);
                    return result;
                }
            } catch (error) {
                console.debug('Cache manager error:', error);
            }
        }

        return null;
    }

    /**
     * Store image in cache
     */
    async storeInCache(cacheKey, data) {
        // Store in internal cache
        const entry = {
            data,
            timestamp: Date.now(),
            ttl: 300000, // 5 minutes default TTL
        };

        this.cache.set(cacheKey, entry);

        // Limit cache size
        if (this.cache.size > 100) {
            // Remove oldest entry
            const oldestKey = this.cache.keys().next().value;
            this.cache.delete(oldestKey);
        }

        // Also store in cache manager if available
        if (this.cacheManager) {
            try {
                await this.cacheManager.storeInCaches(cacheKey, data, {
                    ttl: 300000,
                });
            } catch (error) {
                console.debug('Failed to store in cache manager:', error);
            }
        }
    }

    /**
     * Clear cache
     */
    clearCache() {
        this.cache.clear();
        console.debug('Image cache cleared');
    }

    // ========== Error Handling ==========

    /**
     * Handle error
     */
    handleError(error, context, details = {}) {
        console.error(`DynamicImageLayer error (${context}):`, error);

        // Call error handler
        this.options.onError(error, { context, ...details });

        // Emit error event
        this.emit('error', {
            error,
            context,
            details,
            timestamp: new Date().toISOString(),
        });

        // Update error stats
        this.stats.totalRequests++;
    }

    // ========== Utility Methods ==========

    /**
     * Map quality string to number
     */
    mapQualityToNumber(quality) {
        const qualityMap = {
            low: 50,
            medium: 75,
            high: 90,
        };

        return qualityMap[quality] || 75;
    }

    /**
     * Simple string hash function
     */
    hashString(str) {
        let hash = 0;
        for (let i = 0; i < str.length; i++) {
            const char = str.charCodeAt(i);
            hash = (hash << 5) - hash + char;
            hash = hash & hash; // Convert to 32-bit integer
        }
        return Math.abs(hash).toString(36);
    }

    /**
     * Update average load time
     */
    updateAverageLoadTime(newTime) {
        const totalTime = this.stats.averageLoadTime * this.stats.successfulRequests + newTime;
        this.stats.successfulRequests++;
        this.stats.averageLoadTime = totalTime / this.stats.successfulRequests;
    }

    /**
     * Get layer statistics
     */
    getStats() {
        return {
            ...this.stats,
            cacheSize: this.cache.size,
            isLoading: this.isLoading,
            errorCount: this.errorCount,
            currentLayers: this.layers.length,
            activeLayers: this.layers.filter((l) => l.visible).length,
        };
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Remove event listeners
        this.map.off('moveend', () => this.onMapMove());
        this.map.off('zoomend', () => this.onMapZoom());
        this.map.off('resize', () => this.onMapResize());

        // Remove image overlay
        if (this.imageOverlay) {
            this.map.removeLayer(this.imageOverlay);
        }

        // Revoke blob URL
        if (this.currentImage) {
            URL.revokeObjectURL(this.currentImage);
        }

        // Clear cache
        this.clearCache();

        console.debug('DynamicImageLayer destroyed');
    }
}

// Default export
export default DynamicImageLayer;
