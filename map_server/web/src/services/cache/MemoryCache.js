// Memory cache implementation using LRU strategy

export class MemoryCache {
    constructor(options = {}) {
        this.options = {
            maxSize: options.maxSize || 50 * 1024 * 1024, // 50MB default
            maxItems: options.maxItems || 100,
            ttl: options.ttl || 180000, // 3 minutes default
            cleanupInterval: options.cleanupInterval || 60000, // 1 minute
        };

        this.cache = new Map();
        this.size = 0;
        this.hits = 0;
        this.misses = 0;
        this.lastCleanup = Date.now();

        // Start cleanup interval
        this.startCleanup();

        // Performance monitoring
        this.stats = {
            totalOperations: 0,
            evictionCount: 0,
            hitRate: 0,
        };
    }

    /**
     * Set a value in cache
     * @param {string} key - Cache key
     * @param {any} value - Value to cache
     * @param {Object} options - Cache options
     * @returns {boolean} - Success status
     */
    set(key, value, options = {}) {
        try {
            // Validate inputs
            if (!key || typeof key !== 'string') {
                throw new Error('Invalid cache key');
            }

            // Calculate entry size
            const entrySize = this.calculateEntrySize(key, value);

            // Check if entry exceeds max size
            if (entrySize > this.options.maxSize) {
                console.warn(`Entry too large for cache: ${entrySize} bytes`);
                return false;
            }

            // Evict entries if needed
            while (
                this.size + entrySize > this.options.maxSize ||
                this.cache.size >= this.options.maxItems
            ) {
                this.evictOldest();
            }

            // Create cache entry
            const entry = {
                value,
                size: entrySize,
                timestamp: Date.now(),
                ttl: options.ttl || this.options.ttl,
                accessCount: 0,
                lastAccess: Date.now(),
            };

            // Store in cache
            this.cache.set(key, entry);
            this.size += entrySize;

            // Update stats
            this.stats.totalOperations++;

            return true;
        } catch (error) {
            console.error('Cache set error:', error);
            return false;
        }
    }

    /**
     * Get a value from cache
     * @param {string} key - Cache key
     * @returns {any|null} - Cached value or null
     */
    get(key) {
        try {
            // Validate key
            if (!key || typeof key !== 'string') {
                return null;
            }

            const entry = this.cache.get(key);

            // Check if entry exists and is not expired
            if (!entry || this.isExpired(entry)) {
                this.misses++;
                this.remove(key); // Clean up expired entry
                return null;
            }

            // Update entry access stats
            entry.accessCount++;
            entry.lastAccess = Date.now();

            // Move to "most recently used" position
            this.cache.delete(key);
            this.cache.set(key, entry);

            // Update stats
            this.hits++;
            this.stats.totalOperations++;
            this.updateHitRate();

            return entry.value;
        } catch (error) {
            console.error('Cache get error:', error);
            this.misses++;
            return null;
        }
    }

    /**
     * Check if a key exists in cache
     * @param {string} key - Cache key
     * @returns {boolean} - Existence status
     */
    has(key) {
        try {
            const entry = this.cache.get(key);
            if (!entry) {
                return false;
            }

            if (this.isExpired(entry)) {
                this.remove(key);
                return false;
            }

            return true;
        } catch (error) {
            console.error('Cache has error:', error);
            return false;
        }
    }

    /**
     * Remove a key from cache
     * @param {string} key - Cache key
     * @returns {boolean} - Success status
     */
    remove(key) {
        try {
            const entry = this.cache.get(key);
            if (!entry) {
                return false;
            }

            this.cache.delete(key);
            this.size -= entry.size;

            return true;
        } catch (error) {
            console.error('Cache remove error:', error);
            return false;
        }
    }

    /**
     * Clear all entries from cache
     */
    clear() {
        this.cache.clear();
        this.size = 0;
        this.hits = 0;
        this.misses = 0;
        this.lastCleanup = Date.now();
    }

    /**
     * Get cache statistics
     * @returns {Object} - Cache statistics
     */
    getStats() {
        const hitRate =
            this.hits + this.misses > 0 ? (this.hits / (this.hits + this.misses)) * 100 : 0;

        return {
            size: this.size,
            items: this.cache.size,
            hits: this.hits,
            misses: this.misses,
            hitRate: parseFloat(hitRate.toFixed(2)),
            maxSize: this.options.maxSize,
            maxItems: this.options.maxItems,
            ttl: this.options.ttl,
            ...this.stats,
        };
    }

    /**
     * Get all cache keys
     * @returns {Array<string>} - Array of cache keys
     */
    keys() {
        return Array.from(this.cache.keys());
    }

    /**
     * Get cache size for a specific key
     * @param {string} key - Cache key
     * @returns {number} - Entry size in bytes
     */
    getSize(key) {
        const entry = this.cache.get(key);
        return entry ? entry.size : 0;
    }

    /**
     * Clean up expired entries
     */
    cleanup() {
        const now = Date.now();
        const removedKeys = [];

        for (const [key, entry] of this.cache) {
            if (this.isExpired(entry)) {
                removedKeys.push(key);
            }
        }

        // Remove expired entries
        removedKeys.forEach((key) => this.remove(key));

        // Update cleanup timestamp
        this.lastCleanup = now;

        if (removedKeys.length > 0) {
            console.debug(`Cache cleanup removed ${removedKeys.length} expired entries`);
        }
    }

    /**
     * Start automatic cleanup interval
     */
    startCleanup() {
        if (this.cleanupInterval) {
            clearInterval(this.cleanupInterval);
        }

        this.cleanupInterval = setInterval(() => {
            this.cleanup();
        }, this.options.cleanupInterval);
    }

    /**
     * Stop automatic cleanup
     */
    stopCleanup() {
        if (this.cleanupInterval) {
            clearInterval(this.cleanupInterval);
            this.cleanupInterval = null;
        }
    }

    /**
     * Update cache hit rate
     */
    updateHitRate() {
        const total = this.hits + this.misses;
        this.stats.hitRate = total > 0 ? (this.hits / total) * 100 : 0;
    }

    // ========== Private Methods ==========

    /**
     * Calculate entry size in bytes
     * @param {string} key - Cache key
     * @param {any} value - Value to cache
     * @returns {number} - Entry size in bytes
     */
    calculateEntrySize(key, value) {
        try {
            // For ArrayBuffer, use byteLength
            if (value instanceof ArrayBuffer) {
                return value.byteLength;
            }

            // For Blob, use size
            if (value instanceof Blob) {
                return value.size;
            }

            // For other objects, approximate size by stringifying
            const keySize = new TextEncoder().encode(key).length;
            const valueSize = new TextEncoder().encode(JSON.stringify(value)).length;

            return keySize + valueSize + 50; // Add overhead for metadata
        } catch (error) {
            // Fallback to a safe default
            console.warn('Error calculating cache entry size:', error);
            return 1024; // 1KB default
        }
    }

    /**
     * Check if entry is expired
     * @param {Object} entry - Cache entry
     * @returns {boolean} - Expiration status
     */
    isExpired(entry) {
        const now = Date.now();
        return now - entry.timestamp > entry.ttl;
    }

    /**
     * Evict oldest entry from cache
     */
    evictOldest() {
        if (this.cache.size === 0) {
            return;
        }

        // Find least recently used entry
        let oldestKey = null;
        let oldestAccess = Infinity;

        for (const [key, entry] of this.cache) {
            if (entry.lastAccess < oldestAccess) {
                oldestAccess = entry.lastAccess;
                oldestKey = key;
            }
        }

        if (oldestKey) {
            this.remove(oldestKey);
            this.stats.evictionCount++;
            console.debug(`Cache evicted key: ${oldestKey}`);
        }
    }

    /**
     * Get cache entry metadata
     * @param {string} key - Cache key
     * @returns {Object|null} - Entry metadata
     */
    getEntryMetadata(key) {
        const entry = this.cache.get(key);
        if (!entry) {
            return null;
        }

        return {
            timestamp: entry.timestamp,
            ttl: entry.ttl,
            size: entry.size,
            accessCount: entry.accessCount,
            lastAccess: entry.lastAccess,
            expiresAt: entry.timestamp + entry.ttl,
            age: Date.now() - entry.timestamp,
        };
    }

    /**
     * Prune cache to specified size
     * @param {number} targetSize - Target cache size in bytes
     */
    prune(targetSize) {
        if (this.size <= targetSize) {
            return;
        }

        // Sort entries by access time (oldest first)
        const entries = Array.from(this.cache.entries())
            .map(([key, entry]) => ({ key, ...entry }))
            .sort((a, b) => a.lastAccess - b.lastAccess);

        let removedSize = 0;
        const removedKeys = [];

        for (const entry of entries) {
            if (this.size - removedSize <= targetSize) {
                break;
            }

            removedSize += entry.size;
            removedKeys.push(entry.key);
        }

        // Remove selected keys
        removedKeys.forEach((key) => this.remove(key));

        console.debug(
            `Cache pruned: removed ${removedKeys.length} entries, freed ${removedSize} bytes`
        );
    }
}

// Default export
export default MemoryCache;
