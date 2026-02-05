// Cache Manager - Coordinates multiple cache layers and strategies

import { EventEmitter } from '../utils/EventEmitter.js';
import { MemoryCache } from './MemoryCache.js';
import { LocalStorageCache } from './LocalStorageCache.js';

export class CacheManager extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            strategy: options.strategy || 'balanced', // 'aggressive', 'balanced', 'conservative', 'none'
            enableMemoryCache: options.enableMemoryCache !== false,
            enableLocalStorageCache: options.enableLocalStorageCache !== false,
            enableIndexedDBCache: options.enableIndexedDBCache || false,
            enableServiceWorkerCache: options.enableServiceWorkerCache || false,
            skipNetwork: options.skipNetwork || false,
            offlineFirst: options.offlineFirst || false,
            ...options,
        };

        // Cache instances
        this.memoryCache = null;
        this.localStorageCache = null;
        this.indexedDBCache = null;
        this.serviceWorkerCache = null;

        // Request tracking for deduplication
        this.pendingRequests = new Map();

        // Statistics
        this.stats = {
            totalRequests: 0,
            memoryHits: 0,
            localStorageHits: 0,
            indexedDBHits: 0,
            serviceWorkerHits: 0,
            networkRequests: 0,
            cacheMisses: 0,
            errors: 0,
            bytesSaved: 0,
            averageResponseTime: 0,
        };

        // Cache hierarchy
        this.cacheHierarchy = [];

        // Initialize caches based on strategy
        this.initCaches();

        // Setup cleanup interval
        this.setupCleanupInterval();

        console.debug(`CacheManager initialized with strategy: ${this.options.strategy}`);
    }

    /**
     * Initialize caches based on strategy
     */
    initCaches() {
        // Clear existing caches
        this.destroyCaches();

        // Configure based on strategy
        switch (this.options.strategy) {
            case 'aggressive':
                this.initAggressiveCaching();
                break;

            case 'balanced':
                this.initBalancedCaching();
                break;

            case 'conservative':
                this.initConservativeCaching();
                break;

            case 'none':
                // No caching
                break;

            default:
                console.warn(`Unknown cache strategy: ${this.options.strategy}, using balanced`);
                this.options.strategy = 'balanced';
                this.initBalancedCaching();
                break;
        }

        // Build cache hierarchy
        this.buildCacheHierarchy();

        this.emit('cachesInitialized', { strategy: this.options.strategy });
    }

    /**
     * Initialize aggressive caching
     */
    initAggressiveCaching() {
        // Memory cache: large, short TTL
        if (this.options.enableMemoryCache) {
            this.memoryCache = new MemoryCache({
                maxSize: 200 * 1024 * 1024, // 200MB
                maxItems: 500,
                ttl: 300000, // 5 minutes
                cleanupInterval: 60000, // 1 minute
            });
        }

        // LocalStorage cache: large, medium TTL
        if (this.options.enableLocalStorageCache) {
            this.localStorageCache = new LocalStorageCache({
                maxSize: 100 * 1024 * 1024, // 100MB
                ttl: 3600000, // 1 hour
                prefix: 'cache_aggressive_',
            });
        }

        // Skip network when possible
        this.options.skipNetwork = true;
        this.options.offlineFirst = true;
    }

    /**
     * Initialize balanced caching
     */
    initBalancedCaching() {
        // Memory cache: moderate, medium TTL
        if (this.options.enableMemoryCache) {
            this.memoryCache = new MemoryCache({
                maxSize: 100 * 1024 * 1024, // 100MB
                maxItems: 200,
                ttl: 600000, // 10 minutes
                cleanupInterval: 120000, // 2 minutes
            });
        }

        // LocalStorage cache: moderate, long TTL
        if (this.options.enableLocalStorageCache) {
            this.localStorageCache = new LocalStorageCache({
                maxSize: 50 * 1024 * 1024, // 50MB
                ttl: 7200000, // 2 hours
                prefix: 'cache_balanced_',
            });
        }

        // Use network when needed
        this.options.skipNetwork = false;
        this.options.offlineFirst = false;
    }

    /**
     * Initialize conservative caching
     */
    initConservativeCaching() {
        // Memory cache: small, short TTL
        if (this.options.enableMemoryCache) {
            this.memoryCache = new MemoryCache({
                maxSize: 50 * 1024 * 1024, // 50MB
                maxItems: 100,
                ttl: 300000, // 5 minutes
                cleanupInterval: 300000, // 5 minutes
            });
        }

        // LocalStorage cache: small, short TTL
        if (this.options.enableLocalStorageCache) {
            this.localStorageCache = new LocalStorageCache({
                maxSize: 25 * 1024 * 1024, // 25MB
                ttl: 1800000, // 30 minutes
                prefix: 'cache_conservative_',
            });
        }

        // Prefer network
        this.options.skipNetwork = false;
        this.options.offlineFirst = false;
    }

    /**
     * Build cache hierarchy
     */
    buildCacheHierarchy() {
        this.cacheHierarchy = [];

        // Add caches in order of speed (fastest first)
        if (this.memoryCache) {
            this.cacheHierarchy.push({
                type: 'memory',
                instance: this.memoryCache,
                priority: 1,
            });
        }

        if (this.localStorageCache) {
            this.cacheHierarchy.push({
                type: 'localStorage',
                instance: this.localStorageCache,
                priority: 2,
            });
        }

        // IndexedDB cache (if implemented)
        if (this.indexedDBCache) {
            this.cacheHierarchy.push({
                type: 'indexedDB',
                instance: this.indexedDBCache,
                priority: 3,
            });
        }

        // Service Worker cache (if implemented)
        if (this.serviceWorkerCache) {
            this.cacheHierarchy.push({
                type: 'serviceWorker',
                instance: this.serviceWorkerCache,
                priority: 4,
            });
        }
    }

    /**
     * Get data from cache or network
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
     * Execute cache get with hierarchy
     */
    async executeGet(cacheKey, fetchFn, options) {
        const startTime = performance.now();
        const { skipNetwork, offlineFirst } = this.options;
        const requestOptions = { ...this.options, ...options };

        // Step 1: Check cache hierarchy (fastest first)
        for (const cacheInfo of this.cacheHierarchy) {
            try {
                const cachedResult = await this.getFromCache(cacheInfo.type, cacheKey);
                if (cachedResult !== null && cachedResult !== undefined) {
                    // Update statistics
                    this.updateHitStats(cacheInfo.type);

                    // Calculate response time
                    const responseTime = performance.now() - startTime;
                    this.updateAverageResponseTime(responseTime);

                    // Update cache if needed (e.g., update last access time)
                    await this.touchCache(cacheInfo.type, cacheKey);

                    // Promote to faster cache if applicable
                    await this.promoteToFasterCache(cacheKey, cachedResult, cacheInfo.type);

                    this.emit('cacheHit', {
                        cacheKey,
                        cacheType: cacheInfo.type,
                        responseTime,
                        fromNetwork: false,
                    });

                    return cachedResult;
                }
            } catch (error) {
                console.debug(`Error reading from ${cacheInfo.type} cache:`, error);
            }
        }

        // Step 2: All caches missed
        this.stats.cacheMisses++;

        // Step 3: Check if we should skip network
        if (skipNetwork && offlineFirst) {
            console.debug(
                `Cache miss for key: ${cacheKey}, network requests skipped (offline first)`
            );
            this.emit('cacheMiss', {
                cacheKey,
                reason: 'offline_first',
                fromNetwork: false,
            });
            return null;
        }

        // Step 4: Fetch from network
        this.stats.networkRequests++;
        console.debug(`Cache miss for key: ${cacheKey}, fetching from network`);

        try {
            const result = await fetchFn();

            // Calculate response time
            const responseTime = performance.now() - startTime;
            this.updateAverageResponseTime(responseTime);

            // Store in caches if successful
            if (result !== null && result !== undefined) {
                await this.storeInCaches(cacheKey, result, requestOptions);

                // Calculate bytes saved (estimate)
                const size = this.estimateSize(result);
                this.stats.bytesSaved += size;
            }

            this.emit('cacheMiss', {
                cacheKey,
                reason: 'network_fetch',
                fromNetwork: true,
                responseTime,
            });

            this.emit('networkFetch', {
                cacheKey,
                responseTime,
                resultSize: this.estimateSize(result),
            });

            return result;
        } catch (error) {
            this.stats.errors++;

            // Check if we have stale data in any cache
            if (offlineFirst) {
                const staleResult = await this.getStaleFromCaches(cacheKey);
                if (staleResult !== null) {
                    console.debug(`Using stale data from cache for key: ${cacheKey}`);
                    this.emit('staleDataUsed', {
                        cacheKey,
                        error: error.message,
                    });
                    return staleResult;
                }
            }

            throw error;
        }
    }

    /**
     * Get data from specific cache type
     */
    async getFromCache(cacheType, cacheKey) {
        switch (cacheType) {
            case 'memory':
                return this.memoryCache ? this.memoryCache.get(cacheKey) : null;

            case 'localStorage':
                return this.localStorageCache ? this.localStorageCache.get(cacheKey) : null;

            case 'indexedDB':
                return this.indexedDBCache ? await this.indexedDBCache.get(cacheKey) : null;

            case 'serviceWorker':
                return this.serviceWorkerCache ? await this.serviceWorkerCache.get(cacheKey) : null;

            default:
                return null;
        }
    }

    /**
     * Store data in specific cache type
     */
    async storeInCache(cacheType, cacheKey, value, options = {}) {
        switch (cacheType) {
            case 'memory':
                if (this.memoryCache) {
                    return this.memoryCache.set(cacheKey, value, options);
                }
                break;

            case 'localStorage':
                if (this.localStorageCache) {
                    return this.localStorageCache.set(cacheKey, value, options);
                }
                break;

            case 'indexedDB':
                if (this.indexedDBCache) {
                    return await this.indexedDBCache.set(cacheKey, value, options);
                }
                break;

            case 'serviceWorker':
                if (this.serviceWorkerCache) {
                    return await this.serviceWorkerCache.set(cacheKey, value, options);
                }
                break;
        }

        return false;
    }

    /**
     * Store data in all caches
     */
    async storeInCaches(cacheKey, value, options = {}) {
        const storePromises = [];

        for (const cacheInfo of this.cacheHierarchy) {
            storePromises.push(this.storeInCache(cacheInfo.type, cacheKey, value, options));
        }

        // Wait for all storage operations to complete
        await Promise.allSettled(storePromises);

        this.emit('dataCached', {
            cacheKey,
            caches: this.cacheHierarchy.map((c) => c.type),
            size: this.estimateSize(value),
        });
    }

    /**
     * Touch cache entry (update last access time)
     */
    async touchCache(cacheType, cacheKey) {
        // Only implemented for some cache types
        if (cacheType === 'memory' && this.memoryCache) {
            this.memoryCache.touch(cacheKey);
        }
        // Add similar methods for other cache types if needed
    }

    /**
     * Promote data to faster cache
     */
    async promoteToFasterCache(cacheKey, value, fromCacheType) {
        // Find faster caches in hierarchy
        const fromCacheIndex = this.cacheHierarchy.findIndex((c) => c.type === fromCacheType);
        if (fromCacheIndex === -1 || fromCacheIndex === 0) {
            return; // Already in fastest cache or not found
        }

        // Promote to all faster caches
        for (let i = 0; i < fromCacheIndex; i++) {
            const fasterCache = this.cacheHierarchy[i];
            await this.storeInCache(fasterCache.type, cacheKey, value, {
                ttl: 300000, // 5 minutes for promoted items
            });
        }
    }

    /**
     * Get stale data from caches (ignoring TTL)
     */
    async getStaleFromCaches(cacheKey) {
        for (const cacheInfo of this.cacheHierarchy) {
            try {
                // For memory and localStorage, we can bypass TTL check
                if (cacheInfo.type === 'memory' && this.memoryCache) {
                    const entry = this.memoryCache.getEntry(cacheKey);
                    if (entry) return entry.value;
                }

                if (cacheInfo.type === 'localStorage' && this.localStorageCache) {
                    const entry = this.localStorageCache.getEntry(cacheKey);
                    if (entry) return entry.value;
                }

                // For other cache types, we'd need specific implementations
            } catch (error) {
                console.debug(`Error getting stale data from ${cacheInfo.type}:`, error);
            }
        }

        return null;
    }

    /**
     * Prefetch data
     */
    async prefetch(cacheKey, fetchFn, options = {}) {
        // Check if already cached
        for (const cacheInfo of this.cacheHierarchy) {
            const cached = await this.getFromCache(cacheInfo.type, cacheKey);
            if (cached !== null) {
                return; // Already cached
            }
        }

        // Low priority fetch
        try {
            const result = await fetchFn();
            if (result) {
                await this.storeInCaches(cacheKey, result, {
                    ...options,
                    lowPriority: true,
                    prefetch: true,
                });
            }
        } catch (error) {
            // Silent fail for prefetch
            console.debug(`Prefetch failed for key: ${cacheKey}:`, error.message);
        }
    }

    /**
     * Remove data from all caches
     */
    async remove(cacheKey) {
        const removePromises = [];

        for (const cacheInfo of this.cacheHierarchy) {
            removePromises.push(
                Promise.resolve().then(() => {
                    switch (cacheInfo.type) {
                        case 'memory':
                            return this.memoryCache?.remove(cacheKey) || false;
                        case 'localStorage':
                            return this.localStorageCache?.remove(cacheKey) || false;
                        case 'indexedDB':
                            return this.indexedDBCache?.remove(cacheKey) || false;
                        case 'serviceWorker':
                            return this.serviceWorkerCache?.remove(cacheKey) || false;
                    }
                })
            );
        }

        const results = await Promise.allSettled(removePromises);
        const removed = results.some((r) => r.value === true);

        if (removed) {
            this.emit('dataRemoved', { cacheKey });
        }

        return removed;
    }

    /**
     * Clear all caches
     */
    async clear() {
        const clearPromises = [];

        for (const cacheInfo of this.cacheHierarchy) {
            clearPromises.push(
                Promise.resolve().then(() => {
                    switch (cacheInfo.type) {
                        case 'memory':
                            return this.memoryCache?.clear();
                        case 'localStorage':
                            return this.localStorageCache?.clear();
                        case 'indexedDB':
                            return this.indexedDBCache?.clear();
                        case 'serviceWorker':
                            return this.serviceWorkerCache?.clear();
                    }
                })
            );
        }

        await Promise.allSettled(clearPromises);

        // Reset statistics
        this.stats = {
            totalRequests: 0,
            memoryHits: 0,
            localStorageHits: 0,
            indexedDBHits: 0,
            serviceWorkerHits: 0,
            networkRequests: 0,
            cacheMisses: 0,
            errors: 0,
            bytesSaved: 0,
            averageResponseTime: 0,
        };

        this.emit('cachesCleared');
    }

    /**
     * Update cache hit statistics
     */
    updateHitStats(cacheType) {
        this.stats.totalRequests++;

        switch (cacheType) {
            case 'memory':
                this.stats.memoryHits++;
                break;
            case 'localStorage':
                this.stats.localStorageHits++;
                break;
            case 'indexedDB':
                this.stats.indexedDBHits++;
                break;
            case 'serviceWorker':
                this.stats.serviceWorkerHits++;
                break;
        }
    }

    /**
     * Update average response time
     */
    updateAverageResponseTime(newTime) {
        const totalRequests = this.stats.totalRequests + this.stats.networkRequests;
        if (totalRequests === 0) {
            this.stats.averageResponseTime = newTime;
        } else {
            const totalTime = this.stats.averageResponseTime * totalRequests + newTime;
            this.stats.averageResponseTime = totalTime / (totalRequests + 1);
        }
    }

    /**
     * Estimate size of value in bytes
     */
    estimateSize(value) {
        if (value === null || value === undefined) {
            return 0;
        }

        if (value instanceof ArrayBuffer) {
            return value.byteLength;
        }

        if (value instanceof Blob) {
            return value.size;
        }

        if (typeof value === 'string') {
            return new TextEncoder().encode(value).length;
        }

        if (typeof value === 'object') {
            try {
                return new TextEncoder().encode(JSON.stringify(value)).length;
            } catch (error) {
                return 0;
            }
        }

        return 0;
    }

    /**
     * Setup cleanup interval
     */
    setupCleanupInterval() {
        // Clean up every 5 minutes
        this.cleanupInterval = setInterval(() => {
            this.cleanup();
        }, 300000);
    }

    /**
     * Clean up expired entries
     */
    async cleanup() {
        const cleanupPromises = [];

        for (const cacheInfo of this.cacheHierarchy) {
            cleanupPromises.push(
                Promise.resolve().then(() => {
                    switch (cacheInfo.type) {
                        case 'memory':
                            return this.memoryCache?.cleanup();
                        case 'localStorage':
                            return this.localStorageCache?.cleanup();
                        case 'indexedDB':
                            return this.indexedDBCache?.cleanup();
                        case 'serviceWorker':
                            return this.serviceWorkerCache?.cleanup();
                    }
                })
            );
        }

        await Promise.allSettled(cleanupPromises);

        this.emit('cleanupCompleted');
    }

    /**
     * Update cache strategy
     */
    updateStrategy(strategy) {
        if (this.options.strategy === strategy) {
            return;
        }

        console.debug(`Updating cache strategy from ${this.options.strategy} to ${strategy}`);
        this.options.strategy = strategy;

        // Reinitialize caches with new strategy
        this.initCaches();

        this.emit('strategyChanged', { strategy });
    }

    /**
     * Get cache statistics
     */
    getStats() {
        const totalHits =
            this.stats.memoryHits +
            this.stats.localStorageHits +
            this.stats.indexedDBHits +
            this.stats.serviceWorkerHits;
        const totalRequests = totalHits + this.stats.networkRequests + this.stats.cacheMisses;

        const hitRate = totalRequests > 0 ? (totalHits / totalRequests) * 100 : 0;
        const memoryHitRate =
            this.stats.memoryHits > 0 ? (this.stats.memoryHits / totalHits) * 100 : 0;

        const cacheInfo = {};

        if (this.memoryCache) {
            const memoryStats = this.memoryCache.getStats();
            cacheInfo.memory = {
                size: memoryStats.size,
                items: memoryStats.items,
                maxSize: memoryStats.maxSize,
                usagePercent: memoryStats.usagePercent,
                hitRate: memoryHitRate,
            };
        }

        if (this.localStorageCache) {
            const localStorageStats = this.localStorageCache.getStats();
            cacheInfo.localStorage = {
                size: localStorageStats.size,
                items: localStorageStats.items,
                maxSize: localStorageStats.maxSize,
                usagePercent: localStorageStats.usagePercent,
            };
        }

        return {
            strategy: this.options.strategy,
            total: {
                requests: totalRequests,
                hits: totalHits,
                networkRequests: this.stats.networkRequests,
                misses: this.stats.cacheMisses,
                errors: this.stats.errors,
                hitRate: parseFloat(hitRate.toFixed(2)),
                averageResponseTime: parseFloat(this.stats.averageResponseTime.toFixed(2)),
                bytesSaved: this.stats.bytesSaved,
            },
            breakdown: {
                memoryHits: this.stats.memoryHits,
                localStorageHits: this.stats.localStorageHits,
                indexedDBHits: this.stats.indexedDBHits,
                serviceWorkerHits: this.stats.serviceWorkerHits,
            },
            caches: cacheInfo,
            hierarchy: this.cacheHierarchy.map((c) => c.type),
            pendingRequests: this.pendingRequests.size,
        };
    }

    /**
     * Get cache hierarchy information
     */
    getHierarchy() {
        return this.cacheHierarchy.map((cacheInfo) => ({
            type: cacheInfo.type,
            priority: cacheInfo.priority,
            enabled: cacheInfo.instance !== null,
            stats: this.getCacheStats(cacheInfo.type),
        }));
    }

    /**
     * Get cache-specific statistics
     */
    getCacheStats(cacheType) {
        switch (cacheType) {
            case 'memory':
                return this.memoryCache?.getStats() || null;
            case 'localStorage':
                return this.localStorageCache?.getStats() || null;
            case 'indexedDB':
                return this.indexedDBCache?.getStats() || null;
            case 'serviceWorker':
                return this.serviceWorkerCache?.getStats() || null;
            default:
                return null;
        }
    }

    /**
     * Check if key exists in any cache
     */
    async has(cacheKey) {
        for (const cacheInfo of this.cacheHierarchy) {
            const exists = await this.cacheHas(cacheInfo.type, cacheKey);
            if (exists) {
                return true;
            }
        }

        return false;
    }

    /**
     * Check if key exists in specific cache
     */
    async cacheHas(cacheType, cacheKey) {
        switch (cacheType) {
            case 'memory':
                return this.memoryCache?.has(cacheKey) || false;
            case 'localStorage':
                return this.localStorageCache?.has(cacheKey) || false;
            case 'indexedDB':
                return this.indexedDBCache?.has(cacheKey) || false;
            case 'serviceWorker':
                return this.serviceWorkerCache?.has(cacheKey) || false;
            default:
                return false;
        }
    }

    /**
     * Get all cache keys
     */
    async keys() {
        const allKeys = new Set();

        for (const cacheInfo of this.cacheHierarchy) {
            const keys = await this.getCacheKeys(cacheInfo.type);
            keys.forEach((key) => allKeys.add(key));
        }

        return Array.from(allKeys);
    }

    /**
     * Get keys from specific cache
     */
    async getCacheKeys(cacheType) {
        switch (cacheType) {
            case 'memory':
                return this.memoryCache?.keys() || [];
            case 'localStorage':
                return this.localStorageCache?.keys() || [];
            case 'indexedDB':
                return this.indexedDBCache?.keys() || [];
            case 'serviceWorker':
                return this.serviceWorkerCache?.keys() || [];
            default:
                return [];
        }
    }

    /**
     * Destroy all cache instances
     */
    destroyCaches() {
        if (this.memoryCache) {
            this.memoryCache.stopCleanup();
            this.memoryCache = null;
        }

        if (this.localStorageCache) {
            this.localStorageCache = null;
        }

        if (this.indexedDBCache) {
            this.indexedDBCache = null;
        }

        if (this.serviceWorkerCache) {
            this.serviceWorkerCache = null;
        }

        this.cacheHierarchy = [];
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Clear cleanup interval
        if (this.cleanupInterval) {
            clearInterval(this.cleanupInterval);
        }

        // Clear pending requests
        this.pendingRequests.clear();

        // Destroy caches
        this.destroyCaches();

        this.clear();

        console.debug('CacheManager destroyed');
    }
}

// Default export
export default CacheManager;
