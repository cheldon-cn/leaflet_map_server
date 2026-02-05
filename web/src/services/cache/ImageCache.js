// Image cache manager - coordinates between memory and localStorage caches

import { MemoryCache } from './MemoryCache.js';
import { LocalStorageCache } from './LocalStorageCache.js';

export class ImageCache {
    constructor(options = {}) {
        this.options = {
            strategy: options.strategy || 'balanced',
            skipNetwork: options.skipNetwork || false,
            ...options,
        };

        // Initialize caches based on strategy
        this.initCaches();

        // Statistics
        this.stats = {
            memoryHits: 0,
            localStorageHits: 0,
            networkRequests: 0,
            cacheMisses: 0,
            errors: 0,
        };

        // Request tracking for deduplication
        this.pendingRequests = new Map();

        console.debug(`ImageCache initialized with strategy: ${this.options.strategy}`);
    }

    /**
     * Get an image from cache or network
     * @param {string} cacheKey - Cache key
     * @param {Function} fetchFn - Function to fetch from network
     * @param {Object} options - Cache options
     * @returns {Promise<any>} - Cached or fetched image
     */
    async get(cacheKey, fetchFn, options = {}) {
        const requestId =
            options.requestId || `req_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;

        try {
            // Check for pending request with same key (deduplication)
            if (this.pendingRequests.has(cacheKey)) {
                console.debug(`Deduplicating request for key: ${cacheKey}`);
                return await this.pendingRequests.get(cacheKey);
            }

            // Create the promise for this request
            const requestPromise = this.executeGet(cacheKey, fetchFn, options);

            // Store promise for deduplication
            this.pendingRequests.set(cacheKey, requestPromise);

            // Execute and clean up
            const result = await requestPromise;
            this.pendingRequests.delete(cacheKey);

            return result;
        } catch (error) {
            // Clean up on error
            this.pendingRequests.delete(cacheKey);
            this.stats.errors++;
            throw error;
        }
    }

    /**
     * Execute cache get with strategy
     */
    async executeGet(cacheKey, fetchFn, options) {
        const { skipNetwork } = this.options;
        const now = Date.now();

        // Step 1: Check memory cache (fastest)
        if (this.memoryCache) {
            const memoryResult = this.memoryCache.get(cacheKey);
            if (memoryResult !== null) {
                this.stats.memoryHits++;
                console.debug(`Memory cache hit for key: ${cacheKey}`);
                return memoryResult;
            }
        }

        // Step 2: Check localStorage cache
        if (this.localStorageCache && !skipNetwork) {
            const localStorageResult = this.localStorageCache.get(cacheKey);
            if (localStorageResult !== null) {
                this.stats.localStorageHits++;
                console.debug(`LocalStorage cache hit for key: ${cacheKey}`);

                // Update memory cache with result
                if (this.memoryCache) {
                    this.memoryCache.set(cacheKey, localStorageResult, options);
                }

                return localStorageResult;
            }
        }

        // Step 3: Fetch from network (if not skipping)
        if (!skipNetwork) {
            this.stats.networkRequests++;
            console.debug(`Cache miss for key: ${cacheKey}, fetching from network`);

            try {
                const result = await fetchFn();

                // Store in both caches if successful
                if (result) {
                    await this.storeInCaches(cacheKey, result, options);
                }

                return result;
            } catch (error) {
                this.stats.cacheMisses++;
                throw error;
            }
        }

        // Step 4: All caches missed and network skipped
        this.stats.cacheMisses++;
        console.debug(`Cache miss for key: ${cacheKey}, network requests skipped`);
        return null;
    }

    /**
     * Prefetch an image
     * @param {string} cacheKey - Cache key
     * @param {Function} fetchFn - Function to fetch from network
     * @param {Object} options - Cache options
     * @returns {Promise<void>}
     */
    async prefetch(cacheKey, fetchFn, options = {}) {
        // Check if already cached
        if (this.memoryCache?.has(cacheKey) || this.localStorageCache?.has(cacheKey)) {
            return;
        }

        // Low priority fetch
        try {
            const result = await fetchFn();
            if (result) {
                await this.storeInCaches(cacheKey, result, {
                    ...options,
                    lowPriority: true,
                });
            }
        } catch (error) {
            // Silent fail for prefetch
            console.debug(`Prefetch failed for key: ${cacheKey}:`, error.message);
        }
    }

    /**
     * Store an image in all available caches
     * @param {string} cacheKey - Cache key
     * @param {any} value - Value to cache
     * @param {Object} options - Cache options
     */
    async storeInCaches(cacheKey, value, options = {}) {
        const storePromises = [];

        // Store in memory cache
        if (this.memoryCache) {
            const memorySuccess = this.memoryCache.set(cacheKey, value, options);
            if (!memorySuccess) {
                console.warn(`Failed to store in memory cache: ${cacheKey}`);
            }
        }

        // Store in localStorage cache (async)
        if (this.localStorageCache) {
            // Use requestIdleCallback if available for non-critical storage
            if ('requestIdleCallback' in window && options.lowPriority) {
                window.requestIdleCallback(() => {
                    this.localStorageCache.set(cacheKey, value, options);
                });
            } else {
                storePromises.push(
                    Promise.resolve().then(() => {
                        this.localStorageCache.set(cacheKey, value, options);
                    })
                );
            }
        }

        // Wait for storage operations (if any)
        if (storePromises.length > 0) {
            await Promise.allSettled(storePromises);
        }
    }

    /**
     * Remove an image from all caches
     * @param {string} cacheKey - Cache key
     */
    remove(cacheKey) {
        let removed = false;

        if (this.memoryCache) {
            removed = this.memoryCache.remove(cacheKey) || removed;
        }

        if (this.localStorageCache) {
            removed = this.localStorageCache.remove(cacheKey) || removed;
        }

        return removed;
    }

    /**
     * Clear all caches
     */
    clear() {
        if (this.memoryCache) {
            this.memoryCache.clear();
        }

        if (this.localStorageCache) {
            this.localStorageCache.clear();
        }

        this.stats = {
            memoryHits: 0,
            localStorageHits: 0,
            networkRequests: 0,
            cacheMisses: 0,
            errors: 0,
        };
    }

    /**
     * Get cache statistics
     * @returns {Object} - Cache statistics
     */
    getStats() {
        const memoryStats = this.memoryCache ? this.memoryCache.getStats() : null;
        const localStorageStats = this.localStorageCache ? this.localStorageCache.getStats() : null;

        const totalHits = this.stats.memoryHits + this.stats.localStorageHits;
        const totalRequests = totalHits + this.stats.networkRequests + this.stats.cacheMisses;
        const hitRate = totalRequests > 0 ? (totalHits / totalRequests) * 100 : 0;

        return {
            strategy: this.options.strategy,
            total: {
                hits: totalHits,
                networkRequests: this.stats.networkRequests,
                misses: this.stats.cacheMisses,
                errors: this.stats.errors,
                hitRate: parseFloat(hitRate.toFixed(2)),
            },
            memory: memoryStats,
            localStorage: localStorageStats,
        };
    }

    /**
     * Update cache strategy
     * @param {string} strategy - New cache strategy
     */
    updateStrategy(strategy) {
        if (this.options.strategy === strategy) {
            return;
        }

        console.debug(`Updating cache strategy from ${this.options.strategy} to ${strategy}`);
        this.options.strategy = strategy;

        // Reinitialize caches with new strategy
        this.initCaches();
    }

    /**
     * Estimate cache size for a value
     * @param {any} value - Value to estimate
     * @returns {number} - Estimated size in bytes
     */
    estimateSize(value) {
        let size = 0;

        // Estimate based on type
        if (value instanceof ArrayBuffer) {
            size = value.byteLength;
        } else if (value instanceof Blob) {
            size = value.size;
        } else if (typeof value === 'string') {
            size = new TextEncoder().encode(value).length;
        } else if (typeof value === 'object') {
            size = new TextEncoder().encode(JSON.stringify(value)).length;
        }

        // Add metadata overhead
        return size + 100; // Approximate overhead for cache metadata
    }

    /**
     * Check if a key exists in any cache
     * @param {string} cacheKey - Cache key
     * @returns {boolean} - Existence status
     */
    has(cacheKey) {
        if (this.memoryCache?.has(cacheKey)) {
            return true;
        }

        if (this.localStorageCache?.has(cacheKey)) {
            return true;
        }

        return false;
    }

    /**
     * Get cache keys from all caches
     * @returns {Array<string>} - Array of cache keys
     */
    keys() {
        const keys = new Set();

        if (this.memoryCache) {
            this.memoryCache.keys().forEach((key) => keys.add(key));
        }

        if (this.localStorageCache) {
            this.localStorageCache.keys().forEach((key) => keys.add(key));
        }

        return Array.from(keys);
    }

    /**
     * Clean up expired entries in all caches
     */
    cleanup() {
        if (this.memoryCache) {
            this.memoryCache.cleanup();
        }

        if (this.localStorageCache) {
            this.localStorageCache.cleanup();
        }
    }

    // ========== Private Methods ==========

    /**
     * Initialize caches based on strategy
     */
    initCaches() {
        const { strategy } = this.options;

        // Clear existing caches
        if (this.memoryCache) {
            this.memoryCache.stopCleanup();
        }

        // Configure caches based on strategy
        switch (strategy) {
            case 'aggressive':
                // Large memory cache, aggressive localStorage, skip network when possible
                this.memoryCache = new MemoryCache({
                    maxSize: 100 * 1024 * 1024, // 100MB
                    maxItems: 200,
                    ttl: 300000, // 5 minutes
                });

                this.localStorageCache = new LocalStorageCache({
                    maxSize: 50 * 1024 * 1024, // 50MB
                    ttl: 3600000, // 1 hour
                });

                this.options.skipNetwork = true;
                break;

            case 'balanced':
                // Moderate memory cache, moderate localStorage, use network as needed
                this.memoryCache = new MemoryCache({
                    maxSize: 50 * 1024 * 1024, // 50MB
                    maxItems: 100,
                    ttl: 180000, // 3 minutes
                });

                this.localStorageCache = new LocalStorageCache({
                    maxSize: 25 * 1024 * 1024, // 25MB
                    ttl: 1800000, // 30 minutes
                });

                this.options.skipNetwork = false;
                break;

            case 'none':
                // No caching
                this.memoryCache = null;
                this.localStorageCache = null;
                this.options.skipNetwork = false;
                break;

            default:
                console.warn(`Unknown cache strategy: ${strategy}, using balanced`);
                this.initCaches(); // Recursively call with default
                return;
        }

        console.debug(`Caches initialized for strategy: ${strategy}`);
    }

    /**
     * Generate cache key from request parameters
     * @param {Object} params - Request parameters
     * @returns {string} - Cache key
     */
    generateCacheKey(params) {
        // Create a stable string representation of parameters
        const keyParts = [];

        // Include bbox (normalized to 6 decimal places)
        if (params.bbox && Array.isArray(params.bbox)) {
            keyParts.push('bbox:' + params.bbox.map((v) => v.toFixed(6)).join(','));
        }

        // Include dimensions
        if (params.width) keyParts.push(`w:${params.width}`);
        if (params.height) keyParts.push(`h:${params.height}`);

        // Include layers (sorted for consistency)
        if (params.layers && Array.isArray(params.layers)) {
            keyParts.push('layers:' + params.layers.sort().join(','));
        }

        // Include format and quality
        if (params.format) keyParts.push(`format:${params.format}`);
        if (params.quality) keyParts.push(`q:${params.quality}`);

        // Create a hash of the key parts
        const keyString = keyParts.join('|');

        // Simple hash function (could use more robust hashing in production)
        let hash = 0;
        for (let i = 0; i < keyString.length; i++) {
            const char = keyString.charCodeAt(i);
            hash = (hash << 5) - hash + char;
            hash = hash & hash; // Convert to 32-bit integer
        }

        return `img_${Math.abs(hash).toString(36)}`;
    }

    /**
     * Get cache entry metadata
     * @param {string} cacheKey - Cache key
     * @returns {Object|null} - Entry metadata from all caches
     */
    getEntryMetadata(cacheKey) {
        const metadata = {};

        if (this.memoryCache) {
            metadata.memory = this.memoryCache.getEntryMetadata(cacheKey);
        }

        if (this.localStorageCache) {
            // LocalStorageCache doesn't have getEntryMetadata yet
            // We'll implement a simple version
            metadata.localStorage = this.getLocalStorageMetadata(cacheKey);
        }

        return metadata;
    }

    /**
     * Get localStorage cache metadata
     */
    getLocalStorageMetadata(cacheKey) {
        try {
            const indexEntry = this.localStorageCache.index?.get(cacheKey);
            if (!indexEntry) {
                return null;
            }

            return {
                size: indexEntry.size,
                timestamp: indexEntry.timestamp,
                ttl: indexEntry.ttl,
                lastAccess: indexEntry.lastAccess,
                expiresAt: indexEntry.timestamp + indexEntry.ttl,
                age: Date.now() - indexEntry.timestamp,
            };
        } catch (error) {
            console.debug('Failed to get localStorage metadata:', error);
            return null;
        }
    }

    /**
     * Get cache storage information
     */
    getStorageInfo() {
        const info = {
            memory: null,
            localStorage: null,
        };

        if (this.memoryCache) {
            const stats = this.memoryCache.getStats();
            info.memory = {
                size: stats.size,
                items: stats.items,
                maxSize: stats.maxSize,
                usagePercent: (stats.size / stats.maxSize) * 100,
            };
        }

        if (this.localStorageCache) {
            const stats = this.localStorageCache.getStats();
            info.localStorage = {
                size: stats.size,
                items: stats.items,
                maxSize: stats.maxSize,
                usagePercent: stats.usagePercent,
            };
        }

        return info;
    }
}

// Default export
export default ImageCache;
