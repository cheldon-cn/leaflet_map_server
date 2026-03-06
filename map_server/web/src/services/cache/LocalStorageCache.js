// LocalStorage cache implementation with size limits and TTL

export class LocalStorageCache {
    constructor(options = {}) {
        this.options = {
            prefix: options.prefix || 'map_cache_',
            maxSize: options.maxSize || 25 * 1024 * 1024, // 25MB default
            ttl: options.ttl || 1800000, // 30 minutes default
            cleanupThreshold: options.cleanupThreshold || 0.8, // 80% usage
            compression: options.compression !== false, // Enable compression by default
        };

        this.storage = window.localStorage;
        this.indexKey = `${this.options.prefix}index`;
        this.sizeKey = `${this.options.prefix}size`;

        // Initialize index and size tracking
        this.initialize();

        // Performance monitoring
        this.stats = {
            hits: 0,
            misses: 0,
            writes: 0,
            evictions: 0,
            errors: 0,
        };

        // Auto-cleanup on initialization if needed
        this.autoCleanup();
    }

    /**
     * Initialize cache index and size tracking
     */
    initialize() {
        try {
            // Load or create index
            const indexData = this.storage.getItem(this.indexKey);
            if (indexData) {
                this.index = JSON.parse(indexData);
            } else {
                this.index = new Map();
                this.saveIndex();
            }

            // Load or initialize size tracking
            const sizeData = this.storage.getItem(this.sizeKey);
            if (sizeData) {
                this.currentSize = parseInt(sizeData) || 0;
            } else {
                this.currentSize = 0;
                this.saveSize();
            }
        } catch (error) {
            console.error('LocalStorageCache initialization error:', error);
            this.index = new Map();
            this.currentSize = 0;
            this.stats.errors++;
        }
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
            const fullKey = this.getFullKey(key);

            // Calculate entry size
            const entry = this.createEntry(value, options);
            const entrySize = this.calculateEntrySize(entry);

            // Check if entry exceeds max size
            if (entrySize > this.options.maxSize) {
                console.warn(`Entry too large for LocalStorage cache: ${entrySize} bytes`);
                this.stats.errors++;
                return false;
            }

            // Check if we need to make space
            this.makeSpace(entrySize);

            // Store the entry
            const serialized = JSON.stringify(entry);
            this.storage.setItem(fullKey, serialized);

            // Update index
            this.index.set(key, {
                size: entrySize,
                timestamp: Date.now(),
                ttl: options.ttl || this.options.ttl,
                lastAccess: Date.now(),
            });

            // Update size tracking
            this.currentSize += entrySize;
            this.saveIndex();
            this.saveSize();

            // Update stats
            this.stats.writes++;

            return true;
        } catch (error) {
            console.error('LocalStorageCache set error:', error);
            this.stats.errors++;
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
            const fullKey = this.getFullKey(key);

            // Check index first
            const indexEntry = this.index.get(key);
            if (!indexEntry) {
                this.stats.misses++;
                return null;
            }

            // Check if entry is expired
            if (this.isExpired(indexEntry)) {
                this.remove(key);
                this.stats.misses++;
                return null;
            }

            // Retrieve from storage
            const serialized = this.storage.getItem(fullKey);
            if (!serialized) {
                // Data missing from storage but present in index - clean up
                this.remove(key);
                this.stats.misses++;
                return null;
            }

            // Parse the entry
            const entry = JSON.parse(serialized);

            // Update last access time in index
            indexEntry.lastAccess = Date.now();
            this.index.set(key, indexEntry);
            this.saveIndex();

            // Extract and decompress value if needed
            const value = this.extractValue(entry);

            // Update stats
            this.stats.hits++;

            return value;
        } catch (error) {
            console.error('LocalStorageCache get error:', error);
            this.stats.misses++;
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
            const indexEntry = this.index.get(key);
            if (!indexEntry) {
                return false;
            }

            if (this.isExpired(indexEntry)) {
                this.remove(key);
                return false;
            }

            return true;
        } catch (error) {
            console.error('LocalStorageCache has error:', error);
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
            const fullKey = this.getFullKey(key);
            const indexEntry = this.index.get(key);

            if (indexEntry) {
                // Update size tracking
                this.currentSize -= indexEntry.size;

                // Remove from index
                this.index.delete(key);

                // Remove from storage
                this.storage.removeItem(fullKey);

                // Save updated state
                this.saveIndex();
                this.saveSize();

                return true;
            }

            return false;
        } catch (error) {
            console.error('LocalStorageCache remove error:', error);
            this.stats.errors++;
            return false;
        }
    }

    /**
     * Clear all cache entries
     */
    clear() {
        try {
            // Remove all keys with our prefix
            const keysToRemove = [];
            for (let i = 0; i < this.storage.length; i++) {
                const key = this.storage.key(i);
                if (key.startsWith(this.options.prefix)) {
                    keysToRemove.push(key);
                }
            }

            keysToRemove.forEach((key) => this.storage.removeItem(key));

            // Reset index and size
            this.index.clear();
            this.currentSize = 0;
            this.saveIndex();
            this.saveSize();

            // Reset stats
            this.stats = {
                hits: 0,
                misses: 0,
                writes: 0,
                evictions: 0,
                errors: 0,
            };
        } catch (error) {
            console.error('LocalStorageCache clear error:', error);
            this.stats.errors++;
        }
    }

    /**
     * Get cache statistics
     * @returns {Object} - Cache statistics
     */
    getStats() {
        const total = this.stats.hits + this.stats.misses;
        const hitRate = total > 0 ? (this.stats.hits / total) * 100 : 0;

        return {
            size: this.currentSize,
            items: this.index.size,
            hits: this.stats.hits,
            misses: this.stats.misses,
            writes: this.stats.writes,
            evictions: this.stats.evictions,
            errors: this.stats.errors,
            hitRate: parseFloat(hitRate.toFixed(2)),
            maxSize: this.options.maxSize,
            ttl: this.options.ttl,
            usagePercent: (this.currentSize / this.options.maxSize) * 100,
        };
    }

    /**
     * Get all cache keys
     * @returns {Array<string>} - Array of cache keys
     */
    keys() {
        return Array.from(this.index.keys());
    }

    /**
     * Get cache size for a specific key
     * @param {string} key - Cache key
     * @returns {number} - Entry size in bytes
     */
    getSize(key) {
        const indexEntry = this.index.get(key);
        return indexEntry ? indexEntry.size : 0;
    }

    /**
     * Clean up expired entries
     */
    cleanup() {
        try {
            const expiredKeys = [];
            const now = Date.now();

            for (const [key, indexEntry] of this.index) {
                if (now - indexEntry.timestamp > indexEntry.ttl) {
                    expiredKeys.push(key);
                }
            }

            // Remove expired entries
            expiredKeys.forEach((key) => this.remove(key));

            console.debug(
                `LocalStorageCache cleanup removed ${expiredKeys.length} expired entries`
            );
        } catch (error) {
            console.error('LocalStorageCache cleanup error:', error);
            this.stats.errors++;
        }
    }

    /**
     * Auto-cleanup if storage usage exceeds threshold
     */
    autoCleanup() {
        try {
            // Check if we need to clean up
            const usage = this.currentSize / this.options.maxSize;
            if (usage > this.options.cleanupThreshold) {
                console.debug(
                    `LocalStorage usage ${(usage * 100).toFixed(1)}% exceeds threshold, performing cleanup`
                );
                this.cleanup();

                // If still over threshold, remove oldest entries
                if (this.currentSize / this.options.maxSize > this.options.cleanupThreshold) {
                    this.removeOldestEntries();
                }
            }
        } catch (error) {
            console.error('LocalStorageCache autoCleanup error:', error);
            this.stats.errors++;
        }
    }

    // ========== Private Methods ==========

    /**
     * Get full storage key
     */
    getFullKey(key) {
        return `${this.options.prefix}${key}`;
    }

    /**
     * Create cache entry
     */
    createEntry(value, options) {
        const now = Date.now();

        // Compress value if enabled
        const processedValue = this.compressValue(value);

        return {
            value: processedValue,
            compressed: this.options.compression && this.isCompressible(value),
            timestamp: now,
            ttl: options.ttl || this.options.ttl,
            originalType: this.getType(value),
        };
    }

    /**
     * Calculate entry size
     */
    calculateEntrySize(entry) {
        try {
            const serialized = JSON.stringify(entry);
            return new TextEncoder().encode(serialized).length;
        } catch (error) {
            // Fallback to estimated size
            return 1024;
        }
    }

    /**
     * Make space for new entry
     */
    makeSpace(requiredSize) {
        try {
            // Check if we have enough space
            const availableSpace = this.options.maxSize - this.currentSize;

            if (availableSpace >= requiredSize) {
                return; // Enough space available
            }

            // Need to make space - remove oldest entries
            const entriesToRemove = [];
            let spaceFreed = 0;
            const spaceNeeded = requiredSize - availableSpace;

            // Sort entries by last access time
            const sortedEntries = Array.from(this.index.entries())
                .map(([key, indexEntry]) => ({ key, ...indexEntry }))
                .sort((a, b) => a.lastAccess - b.lastAccess);

            // Remove oldest entries until we have enough space
            for (const entry of sortedEntries) {
                entriesToRemove.push(entry.key);
                spaceFreed += entry.size;

                if (spaceFreed >= spaceNeeded) {
                    break;
                }
            }

            // Remove selected entries
            entriesToRemove.forEach((key) => this.remove(key));

            this.stats.evictions += entriesToRemove.length;
            console.debug(
                `Made space: removed ${entriesToRemove.length} entries, freed ${spaceFreed} bytes`
            );
        } catch (error) {
            console.error('LocalStorageCache makeSpace error:', error);
            this.stats.errors++;
        }
    }

    /**
     * Remove oldest entries
     */
    removeOldestEntries() {
        try {
            // Sort entries by last access time
            const sortedEntries = Array.from(this.index.entries())
                .map(([key, indexEntry]) => ({ key, ...indexEntry }))
                .sort((a, b) => a.lastAccess - b.lastAccess);

            // Remove the oldest 20% of entries
            const entriesToRemove = sortedEntries
                .slice(0, Math.ceil(sortedEntries.length * 0.2))
                .map((entry) => entry.key);

            entriesToRemove.forEach((key) => this.remove(key));

            console.debug(`Removed ${entriesToRemove.length} oldest entries`);
        } catch (error) {
            console.error('LocalStorageCache removeOldestEntries error:', error);
            this.stats.errors++;
        }
    }

    /**
     * Check if entry is expired
     */
    isExpired(indexEntry) {
        const now = Date.now();
        return now - indexEntry.timestamp > indexEntry.ttl;
    }

    /**
     * Extract value from entry
     */
    extractValue(entry) {
        let value = entry.value;

        // Decompress if needed
        if (entry.compressed) {
            value = this.decompressValue(value);
        }

        // Convert back to original type if needed
        return this.convertToType(value, entry.originalType);
    }

    /**
     * Compress value
     */
    compressValue(value) {
        if (!this.options.compression || !this.isCompressible(value)) {
            return value;
        }

        try {
            // Simple compression: base64 for binary data, string compression for large strings
            if (value instanceof ArrayBuffer) {
                // Convert ArrayBuffer to base64 string for storage
                const bytes = new Uint8Array(value);
                let binary = '';
                for (let i = 0; i < bytes.length; i++) {
                    binary += String.fromCharCode(bytes[i]);
                }
                return btoa(binary);
            }

            if (typeof value === 'string' && value.length > 1000) {
                // For large strings, use simple run-length encoding approximation
                return this.simpleStringCompression(value);
            }

            return value;
        } catch (error) {
            console.warn('Compression failed, storing original value:', error);
            return value;
        }
    }

    /**
     * Decompress value
     */
    decompressValue(value) {
        try {
            // Check if value is base64 encoded binary
            if (
                typeof value === 'string' &&
                value.length > 100 &&
                /^[A-Za-z0-9+/]+=*$/.test(value)
            ) {
                try {
                    const binary = atob(value);
                    const bytes = new Uint8Array(binary.length);
                    for (let i = 0; i < binary.length; i++) {
                        bytes[i] = binary.charCodeAt(i);
                    }
                    return bytes.buffer;
                } catch (e) {
                    // Not binary data, continue
                }
            }

            // Check if value is compressed string
            if (typeof value === 'string' && value.startsWith('c:')) {
                return this.simpleStringDecompression(value.substring(2));
            }

            return value;
        } catch (error) {
            console.warn('Decompression failed:', error);
            return value;
        }
    }

    /**
     * Simple string compression
     */
    simpleStringCompression(str) {
        // Very basic compression for demonstration
        // In production, consider using a proper compression library
        return 'c:' + str; // Placeholder
    }

    /**
     * Simple string decompression
     */
    simpleStringDecompression(str) {
        // Very basic decompression for demonstration
        return str; // Placeholder
    }

    /**
     * Check if value is compressible
     */
    isCompressible(value) {
        return value instanceof ArrayBuffer || (typeof value === 'string' && value.length > 1000);
    }

    /**
     * Get type of value
     */
    getType(value) {
        if (value instanceof ArrayBuffer) return 'arraybuffer';
        if (value instanceof Blob) return 'blob';
        if (typeof value === 'string') return 'string';
        if (typeof value === 'object') return 'object';
        return typeof value;
    }

    /**
     * Convert value to original type
     */
    convertToType(value, originalType) {
        if (originalType === 'arraybuffer' && !(value instanceof ArrayBuffer)) {
            // Convert base64 string back to ArrayBuffer
            if (typeof value === 'string') {
                const binary = atob(value);
                const bytes = new Uint8Array(binary.length);
                for (let i = 0; i < binary.length; i++) {
                    bytes[i] = binary.charCodeAt(i);
                }
                return bytes.buffer;
            }
        }

        return value;
    }

    /**
     * Save index to storage
     */
    saveIndex() {
        try {
            const serialized = JSON.stringify(Array.from(this.index.entries()));
            this.storage.setItem(this.indexKey, serialized);
        } catch (error) {
            console.error('Failed to save index:', error);
            this.stats.errors++;
        }
    }

    /**
     * Save size to storage
     */
    saveSize() {
        try {
            this.storage.setItem(this.sizeKey, this.currentSize.toString());
        } catch (error) {
            console.error('Failed to save size:', error);
            this.stats.errors++;
        }
    }

    /**
     * Get storage usage statistics
     */
    getStorageInfo() {
        try {
            let totalSize = 0;
            let itemCount = 0;

            for (let i = 0; i < this.storage.length; i++) {
                const key = this.storage.key(i);
                if (key.startsWith(this.options.prefix)) {
                    const item = this.storage.getItem(key);
                    totalSize += key.length + (item ? item.length : 0);
                    itemCount++;
                }
            }

            return {
                totalSize,
                itemCount,
                cacheSize: this.currentSize,
                cacheItems: this.index.size,
                storageLimit: this.getStorageLimit(),
            };
        } catch (error) {
            console.error('Failed to get storage info:', error);
            return null;
        }
    }

    /**
     * Get browser storage limit
     */
    getStorageLimit() {
        // Estimate based on browser typical limits
        if ('storage' in navigator && 'estimate' in navigator.storage) {
            return navigator.storage.estimate().then((estimate) => estimate.quota);
        }

        // Typical limits: 5MB per domain
        return Promise.resolve(5 * 1024 * 1024);
    }
}

// Default export
export default LocalStorageCache;
