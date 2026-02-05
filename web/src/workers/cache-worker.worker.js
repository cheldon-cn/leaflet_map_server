// Cache Web Worker - Handles cache operations in a separate thread

/**
 * Cache Worker
 * 
 * This worker handles cache operations to avoid blocking the main thread.
 * Supported operations:
 * - LocalStorage read/write operations
 * - IndexedDB operations (if available)
 * - Cache statistics and monitoring
 * - Cache cleanup and maintenance
 */

// Worker state
let workerState = {
    id: 'cache-worker',
    version: '1.0.0',
    isBusy: false,
    operationsCount: 0,
    errorCount: 0,
    startupTime: Date.now(),
    supportedStorage: []
};

// Cache configuration
const config = {
    // Cache types and their priorities
    cacheTypes: {
        memory: { priority: 1, enabled: false },
        localStorage: { priority: 2, enabled: true },
        indexedDB: { priority: 3, enabled: false },
        serviceWorker: { priority: 4, enabled: false }
    },
    
    // Default TTLs (in milliseconds)
    defaultTTL: {
        memory: 300000,      // 5 minutes
        localStorage: 3600000, // 1 hour
        indexedDB: 86400000,   // 24 hours
        serviceWorker: 604800000 // 7 days
    },
    
    // Size limits (in bytes)
    sizeLimits: {
        memory: 100 * 1024 * 1024,      // 100MB
        localStorage: 5 * 1024 * 1024,   // 5MB (per domain)
        indexedDB: 50 * 1024 * 1024,     // 50MB
        serviceWorker: 100 * 1024 * 1024 // 100MB
    },
    
    // Performance settings
    performance: {
        batchSize: 100,
        timeout: 30000,
        retryAttempts: 3,
        retryDelay: 1000
    }
};

// Storage instances
let storageInstances = {
    localStorage: null,
    indexedDB: null
};

// Statistics
let stats = {
    totalOperations: 0,
    readOperations: 0,
    writeOperations: 0,
    deleteOperations: 0,
    hitCount: 0,
    missCount: 0,
    errorCount: 0,
    totalSize: 0,
    averageOperationTime: 0
};

// Cache index for quick lookups
let cacheIndex = new Map();

// Message handler
self.onmessage = async function(event) {
    const { id, type, data, options } = event.data;
    
    try {
        workerState.isBusy = true;
        const startTime = performance.now();
        
        // Route to appropriate handler
        let result;
        switch (type) {
            case 'get':
                result = await handleGet(data, options);
                break;
                
            case 'set':
                result = await handleSet(data, options);
                break;
                
            case 'delete':
                result = await handleDelete(data, options);
                break;
                
            case 'has':
                result = await handleHas(data, options);
                break;
                
            case 'keys':
                result = await handleKeys(data, options);
                break;
                
            case 'clear':
                result = await handleClear(data, options);
                break;
                
            case 'cleanup':
                result = await handleCleanup(data, options);
                break;
                
            case 'stats':
                result = await handleStats(data, options);
                break;
                
            case 'ping':
                result = await handlePing();
                break;
                
            case 'capabilities':
                result = await handleCapabilities();
                break;
                
            case 'configure':
                result = await handleConfigure(data, options);
                break;
                
            case 'batch':
                result = await handleBatch(data, options);
                break;
                
            case 'optimize':
                result = await handleOptimize(data, options);
                break;
                
            default:
                throw new Error(`Unknown operation type: ${type}`);
        }
        
        // Calculate operation time
        const operationTime = performance.now() - startTime;
        
        // Update statistics
        updateStats(type, operationTime, result);
        
        // Send success response
        self.postMessage({
            id,
            type,
            success: true,
            result,
            stats: getCurrentStats(),
            operationTime
        });
        
    } catch (error) {
        // Update error statistics
        stats.errorCount++;
        workerState.errorCount++;
        
        // Send error response
        self.postMessage({
            id,
            type,
            success: false,
            error: {
                message: error.message,
                stack: error.stack,
                code: error.code || 'CACHE_ERROR'
            },
            stats: getCurrentStats()
        });
        
    } finally {
        workerState.isBusy = false;
        workerState.operationsCount++;
    }
};

// ========== Operation Handlers ==========

/**
 * Handle get operation
 */
async function handleGet(key, options = {}) {
    const { cacheType = 'localStorage', checkExpiry = true } = options;
    
    // Validate key
    validateKey(key);
    
    // Route to appropriate storage
    let value;
    switch (cacheType) {
        case 'localStorage':
            value = await getFromLocalStorage(key, options);
            break;
            
        case 'indexedDB':
            value = await getFromIndexedDB(key, options);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    // Check if value is expired
    if (checkExpiry && value !== null && isExpired(value)) {
        // Auto-delete expired entry
        await handleDelete(key, { cacheType });
        return null;
    }
    
    // Update cache index
    if (value !== null) {
        updateIndex(key, {
            lastAccess: Date.now(),
            accessCount: (cacheIndex.get(key)?.accessCount || 0) + 1
        });
    }
    
    return value ? value.data : null;
}

/**
 * Handle set operation
 */
async function handleSet(data, options = {}) {
    const { key, value, ttl, cacheType = 'localStorage' } = data;
    
    // Validate key and value
    validateKey(key);
    validateValue(value);
    
    // Calculate size
    const size = estimateSize(value);
    
    // Check size limits
    checkSizeLimits(cacheType, size);
    
    // Create cache entry
    const entry = createCacheEntry(key, value, ttl, size);
    
    // Route to appropriate storage
    let result;
    switch (cacheType) {
        case 'localStorage':
            result = await setInLocalStorage(entry, options);
            break;
            
        case 'indexedDB':
            result = await setInIndexedDB(entry, options);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    // Update cache index
    updateIndex(key, {
        size: entry.size,
        ttl: entry.ttl,
        expiresAt: entry.expiresAt,
        lastModified: Date.now(),
        lastAccess: Date.now(),
        accessCount: 1
    });
    
    // Update total size
    stats.totalSize += size;
    
    return {
        key,
        size: entry.size,
        expiresAt: entry.expiresAt,
        cacheType,
        success: true
    };
}

/**
 * Handle delete operation
 */
async function handleDelete(key, options = {}) {
    const { cacheType = 'localStorage' } = options;
    
    // Validate key
    validateKey(key);
    
    // Get current size for stats update
    const indexEntry = cacheIndex.get(key);
    if (indexEntry) {
        stats.totalSize -= indexEntry.size || 0;
    }
    
    // Route to appropriate storage
    let result;
    switch (cacheType) {
        case 'localStorage':
            result = await deleteFromLocalStorage(key, options);
            break;
            
        case 'indexedDB':
            result = await deleteFromIndexedDB(key, options);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    // Remove from cache index
    cacheIndex.delete(key);
    
    return {
        key,
        cacheType,
        success: true,
        existed: result
    };
}

/**
 * Handle has operation
 */
async function handleHas(key, options = {}) {
    const { cacheType = 'localStorage', checkExpiry = true } = options;
    
    // Validate key
    validateKey(key);
    
    // Check cache index first
    if (cacheIndex.has(key)) {
        if (checkExpiry) {
            const entry = cacheIndex.get(key);
            if (isExpired({ expiresAt: entry.expiresAt })) {
                // Auto-delete expired entry
                await handleDelete(key, { cacheType });
                return false;
            }
        }
        return true;
    }
    
    // Check storage
    let exists;
    switch (cacheType) {
        case 'localStorage':
            exists = await hasInLocalStorage(key, options);
            break;
            
        case 'indexedDB':
            exists = await hasInIndexedDB(key, options);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    return exists;
}

/**
 * Handle keys operation
 */
async function handleKeys(options = {}) {
    const { cacheType = 'localStorage', pattern, limit = 1000 } = options;
    
    let keys;
    switch (cacheType) {
        case 'localStorage':
            keys = await getKeysFromLocalStorage(options);
            break;
            
        case 'indexedDB':
            keys = await getKeysFromIndexedDB(options);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    // Apply filters
    if (pattern) {
        const regex = new RegExp(pattern);
        keys = keys.filter(key => regex.test(key));
    }
    
    // Apply limit
    if (limit && keys.length > limit) {
        keys = keys.slice(0, limit);
    }
    
    return keys;
}

/**
 * Handle clear operation
 */
async function handleClear(options = {}) {
    const { cacheType = 'localStorage' } = options;
    
    let result;
    switch (cacheType) {
        case 'localStorage':
            result = await clearLocalStorage(options);
            break;
            
        case 'indexedDB':
            result = await clearIndexedDB(options);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    // Clear cache index for this cache type
    for (const [key, entry] of cacheIndex.entries()) {
        if (entry.cacheType === cacheType) {
            cacheIndex.delete(key);
        }
    }
    
    // Reset size stats for this cache type
    stats.totalSize = 0; // Simplified - in production, track per cache type
    
    return {
        cacheType,
        success: true,
        clearedCount: result
    };
}

/**
 * Handle cleanup operation
 */
async function handleCleanup(options = {}) {
    const { cacheType = 'localStorage', maxAge } = options;
    
    let cleanedCount = 0;
    const now = Date.now();
    
    switch (cacheType) {
        case 'localStorage':
            cleanedCount = await cleanupLocalStorage(maxAge);
            break;
            
        case 'indexedDB':
            cleanedCount = await cleanupIndexedDB(maxAge);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    // Update cache index
    for (const [key, entry] of cacheIndex.entries()) {
        if (entry.cacheType === cacheType) {
            if (maxAge && (now - entry.lastModified) > maxAge) {
                cacheIndex.delete(key);
            } else if (isExpired({ expiresAt: entry.expiresAt })) {
                cacheIndex.delete(key);
            }
        }
    }
    
    return {
        cacheType,
        success: true,
        cleanedCount
    };
}

/**
 * Handle stats operation
 */
async function handleStats(options = {}) {
    const { cacheType, detailed = false } = options;
    
    // Get all stats or filtered by cache type
    const allStats = getCurrentStats();
    
    if (!cacheType) {
        return detailed ? {
            ...allStats,
            indexSize: cacheIndex.size,
            cacheIndex: detailed ? Array.from(cacheIndex.entries()) : undefined
        } : allStats;
    }
    
    // Filter by cache type
    const filteredStats = {
        operations: {
            read: stats.readOperations,
            write: stats.writeOperations,
            delete: stats.deleteOperations
        },
        performance: {
            hitRate: allStats.performance.hitRate,
            averageOperationTime: allStats.performance.averageOperationTime
        },
        size: {
            total: stats.totalSize,
            limit: config.sizeLimits[cacheType]
        }
    };
    
    // Add detailed info if requested
    if (detailed) {
        filteredStats.details = {
            cacheType,
            enabled: config.cacheTypes[cacheType].enabled,
            ttl: config.defaultTTL[cacheType]
        };
    }
    
    return filteredStats;
}

/**
 * Handle batch operations
 */
async function handleBatch(operations, options = {}) {
    const { cacheType = 'localStorage', parallel = true } = options;
    
    if (!Array.isArray(operations) || operations.length === 0) {
        throw new Error('Batch operations must be a non-empty array');
    }
    
    const results = [];
    const errors = [];
    
    if (parallel) {
        // Execute operations in parallel
        const promises = operations.map(async (op, index) => {
            try {
                const result = await executeOperation(op.type, {
                    ...op,
                    cacheType
                });
                return { index, result, success: true };
            } catch (error) {
                return { index, error, success: false };
            }
        });
        
        const settled = await Promise.allSettled(promises);
        
        settled.forEach((item, i) => {
            if (item.status === 'fulfilled') {
                const { index, result, success } = item.value;
                if (success) {
                    results[index] = result;
                } else {
                    errors[index] = item.value.error;
                }
            } else {
                errors[i] = { message: 'Promise rejected', reason: item.reason };
            }
        });
    } else {
        // Execute operations sequentially
        for (let i = 0; i < operations.length; i++) {
            const op = operations[i];
            try {
                const result = await executeOperation(op.type, {
                    ...op,
                    cacheType
                });
                results[i] = result;
            } catch (error) {
                errors[i] = { index: i, error };
            }
        }
    }
    
    return {
        total: operations.length,
        successful: results.length,
        failed: errors.length,
        results,
        errors: errors.length > 0 ? errors : undefined
    };
}

/**
 * Handle optimization
 */
async function handleOptimize(options = {}) {
    const { cacheType = 'localStorage', strategy = 'default' } = options;
    
    let optimizationResult;
    switch (cacheType) {
        case 'localStorage':
            optimizationResult = await optimizeLocalStorage(strategy);
            break;
            
        case 'indexedDB':
            optimizationResult = await optimizeIndexedDB(strategy);
            break;
            
        default:
            throw new Error(`Unsupported cache type: ${cacheType}`);
    }
    
    // Rebuild cache index
    await rebuildCacheIndex(cacheType);
    
    return {
        cacheType,
        strategy,
        success: true,
        ...optimizationResult
    };
}

/**
 * Handle ping request
 */
function handlePing() {
    return {
        status: 'ok',
        timestamp: Date.now(),
        uptime: Date.now() - workerState.startupTime,
        state: { ...workerState, isBusy: false },
        stats: getCurrentStats()
    };
}

/**
 * Handle capabilities request
 */
function handleCapabilities() {
    return {
        config,
        state: workerState,
        stats: getCurrentStats()
    };
}

/**
 * Handle configuration update
 */
async function handleConfigure(newConfig, options = {}) {
    // Validate and merge configuration
    const validatedConfig = validateConfiguration(newConfig);
    
    // Update configuration
    Object.assign(config, validatedConfig);
    
    // Update worker state
    if (newConfig.supportedStorage) {
        workerState.supportedStorage = newConfig.supportedStorage;
    }
    
    // Reinitialize storage if needed
    if (validatedConfig.cacheTypes) {
        await initializeStorage();
    }
    
    return {
        success: true,
        updatedConfig: config,
        state: workerState
    };
}

// ========== Storage Implementations ==========

/**
 * Get from LocalStorage
 */
async function getFromLocalStorage(key, options) {
    try {
        const item = localStorage.getItem(key);
        if (!item) {
            return null;
        }
        
        // Parse cache entry
        const entry = JSON.parse(item);
        
        // Check if entry is valid
        if (!validateCacheEntry(entry)) {
            // Invalid entry, delete it
            localStorage.removeItem(key);
            return null;
        }
        
        return entry;
        
    } catch (error) {
        console.warn(`Error reading from localStorage for key "${key}":`, error);
        return null;
    }
}

/**
 * Set in LocalStorage
 */
async function setInLocalStorage(entry, options = {}) {
    try {
        const { lowPriority = false } = options;
        
        // Convert entry to string
        const entryString = JSON.stringify(entry);
        
        // Check size limit
        if (entryString.length > config.sizeLimits.localStorage * 0.8) {
            throw new Error('Entry too large for localStorage');
        }
        
        // Check available space
        await ensureLocalStorageSpace(entryString.length);
        
        // Store entry
        localStorage.setItem(entry.key, entryString);
        
        return true;
        
    } catch (error) {
        console.warn(`Error writing to localStorage for key "${entry.key}":`, error);
        throw error;
    }
}

/**
 * Delete from LocalStorage
 */
async function deleteFromLocalStorage(key, options) {
    try {
        const existed = localStorage.getItem(key) !== null;
        localStorage.removeItem(key);
        return existed;
    } catch (error) {
        console.warn(`Error deleting from localStorage for key "${key}":`, error);
        return false;
    }
}

/**
 * Check if key exists in LocalStorage
 */
async function hasInLocalStorage(key, options) {
    return localStorage.getItem(key) !== null;
}

/**
 * Get keys from LocalStorage
 */
async function getKeysFromLocalStorage(options) {
    const keys = [];
    for (let i = 0; i < localStorage.length; i++) {
        keys.push(localStorage.key(i));
    }
    return keys;
}

/**
 * Clear LocalStorage
 */
async function clearLocalStorage(options) {
    const prefix = options.prefix;
    let clearedCount = 0;
    
    if (prefix) {
        // Clear only keys with prefix
        const keys = await getKeysFromLocalStorage();
        for (const key of keys) {
            if (key.startsWith(prefix)) {
                localStorage.removeItem(key);
                clearedCount++;
            }
        }
    } else {
        // Clear everything
        clearedCount = localStorage.length;
        localStorage.clear();
    }
    
    return clearedCount;
}

/**
 * Cleanup LocalStorage
 */
async function cleanupLocalStorage(maxAge) {
    const now = Date.now();
    let cleanedCount = 0;
    
    for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        try {
            const item = localStorage.getItem(key);
            if (item) {
                const entry = JSON.parse(item);
                
                // Check if expired or older than maxAge
                const isExpiredEntry = entry.expiresAt && entry.expiresAt < now;
                const isTooOld = maxAge && entry.timestamp && (now - entry.timestamp) > maxAge;
                
                if (isExpiredEntry || isTooOld) {
                    localStorage.removeItem(key);
                    cleanedCount++;
                }
            }
        } catch (error) {
            // Invalid entry, remove it
            localStorage.removeItem(key);
            cleanedCount++;
        }
    }
    
    return cleanedCount;
}

/**
 * Optimize LocalStorage
 */
async function optimizeLocalStorage(strategy) {
    switch (strategy) {
        case 'aggressive':
            // Remove least recently used items
            return await cleanupLocalStorage(24 * 60 * 60 * 1000); // 24 hours
            
        case 'conservative':
            // Remove only expired items
            return await cleanupLocalStorage();
            
        case 'default':
        default:
            // Balance between space and performance
            const currentSize = await getLocalStorageSize();
            const maxSize = config.sizeLimits.localStorage;
            
            if (currentSize > maxSize * 0.9) {
                // Remove oldest items until under 80% capacity
                return await cleanupLocalStorage(7 * 24 * 60 * 60 * 1000); // 1 week
            }
            
            return { optimized: false, reason: 'Storage within limits' };
    }
}

// ========== Utility Functions ==========

/**
 * Validate key
 */
function validateKey(key) {
    if (typeof key !== 'string' || key.length === 0) {
        throw new Error('Cache key must be a non-empty string');
    }
    
    if (key.length > 256) {
        throw new Error('Cache key is too long (max 256 characters)');
    }
}

/**
 * Validate value
 */
function validateValue(value) {
    if (value === undefined) {
        throw new Error('Cache value cannot be undefined');
    }
}

/**
 * Estimate size of value in bytes
 */
function estimateSize(value) {
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
 * Check size limits
 */
function checkSizeLimits(cacheType, size) {
    const limit = config.sizeLimits[cacheType];
    if (size > limit) {
        throw new Error(`Value size (${size} bytes) exceeds ${cacheType} limit (${limit} bytes)`);
    }
}

/**
 * Create cache entry
 */
function createCacheEntry(key, data, ttl, size) {
    const now = Date.now();
    const defaultTTL = config.defaultTTL.localStorage;
    
    return {
        key,
        data,
        size: size,
        timestamp: now,
        ttl: ttl || defaultTTL,
        expiresAt: now + (ttl || defaultTTL),
        metadata: {
            created: now,
            version: workerState.version
        }
    };
}

/**
 * Validate cache entry
 */
function validateCacheEntry(entry) {
    if (!entry || typeof entry !== 'object') {
        return false;
    }
    
    if (!entry.key || !entry.data || !entry.timestamp) {
        return false;
    }
    
    if (typeof entry.expiresAt !== 'number') {
        return false;
    }
    
    return true;
}

/**
 * Check if entry is expired
 */
function isExpired(entry) {
    return entry.expiresAt && entry.expiresAt < Date.now();
}

/**
 * Update cache index
 */
function updateIndex(key, updates) {
    const existing = cacheIndex.get(key) || {};
    cacheIndex.set(key, { ...existing, ...updates });
}

/**
 * Update statistics
 */
function updateStats(operationType, operationTime, result) {
    stats.totalOperations++;
    
    switch (operationType) {
        case 'get':
            stats.readOperations++;
            if (result !== null) {
                stats.hitCount++;
            } else {
                stats.missCount++;
            }
            break;
            
        case 'set':
            stats.writeOperations++;
            break;
            
        case 'delete':
            stats.deleteOperations++;
            break;
    }
    
    // Update average operation time
    const totalTime = stats.averageOperationTime * (stats.totalOperations - 1) + operationTime;
    stats.averageOperationTime = totalTime / stats.totalOperations;
}

/**
 * Get current statistics
 */
function getCurrentStats() {
    const hitRate = (stats.hitCount + stats.missCount) > 0
        ? (stats.hitCount / (stats.hitCount + stats.missCount)) * 100
        : 0;
    
    const missRate = 100 - hitRate;
    
    return {
        operations: {
            total: stats.totalOperations,
            read: stats.readOperations,
            write: stats.writeOperations,
            delete: stats.deleteOperations
        },
        performance: {
            hitRate: parseFloat(hitRate.toFixed(2)),
            missRate: parseFloat(missRate.toFixed(2)),
            averageOperationTime: parseFloat(stats.averageOperationTime.toFixed(2))
        },
        size: {
            total: stats.totalSize,
            limit: config.sizeLimits.localStorage,
            usagePercent: (stats.totalSize / config.sizeLimits.localStorage) * 100
        },
        errors: stats.errorCount
    };
}

/**
 * Execute operation
 */
async function executeOperation(operationType, data) {
    switch (operationType) {
        case 'get':
            return await handleGet(data.key, data);
        case 'set':
            return await handleSet(data, data);
        case 'delete':
            return await handleDelete(data.key, data);
        case 'has':
            return await handleHas(data.key, data);
        default:
            throw new Error(`Unknown operation type: ${operationType}`);
    }
}

/**
 * Validate configuration
 */
function validateConfiguration(newConfig) {
    const validated = {};
    
    if (newConfig.cacheTypes) {
        validated.cacheTypes = {};
        for (const [type, config] of Object.entries(newConfig.cacheTypes)) {
            if (typeof config === 'object' && config !== null) {
                validated.cacheTypes[type] = {
                    priority: config.priority || 1,
                    enabled: config.enabled !== false
                };
            }
        }
    }
    
    if (newConfig.defaultTTL) {
        validated.defaultTTL = {};
        for (const [type, ttl] of Object.entries(newConfig.defaultTTL)) {
            if (typeof ttl === 'number' && ttl > 0) {
                validated.defaultTTL[type] = ttl;
            }
        }
    }
    
    if (newConfig.sizeLimits) {
        validated.sizeLimits = {};
        for (const [type, limit] of Object.entries(newConfig.sizeLimits)) {
            if (typeof limit === 'number' && limit > 0) {
                validated.sizeLimits[type] = limit;
            }
        }
    }
    
    if (newConfig.performance) {
        validated.performance = {};
        for (const [key, value] of Object.entries(newConfig.performance)) {
            if (typeof value === 'number' && value > 0) {
                validated.performance[key] = value;
            }
        }
    }
    
    return validated;
}

/**
 * Initialize storage
 */
async function initializeStorage() {
    // Detect supported storage types
    workerState.supportedStorage = [];
    
    // Check LocalStorage
    try {
        localStorage.setItem('test', 'test');
        localStorage.removeItem('test');
        workerState.supportedStorage.push('localStorage');
    } catch (error) {
        console.warn('LocalStorage not available:', error);
    }
    
    // Check IndexedDB
    if ('indexedDB' in self) {
        try {
            const db = await new Promise((resolve, reject) => {
                const request = indexedDB.open('test', 1);
                request.onerror = () => reject(request.error);
                request.onsuccess = () => {
                    request.result.close();
                    resolve(true);
                };
                request.onblocked = () => reject(new Error('IndexedDB blocked'));
            });
            
            if (db) {
                workerState.supportedStorage.push('indexedDB');
            }
        } catch (error) {
            console.warn('IndexedDB not available:', error);
        }
    }
    
    console.log(`Supported storage types: ${workerState.supportedStorage.join(', ')}`);
}

/**
 * Ensure LocalStorage space
 */
async function ensureLocalStorageSpace(requiredSize) {
    const currentSize = await getLocalStorageSize();
    const maxSize = config.sizeLimits.localStorage;
    
    if (currentSize + requiredSize <= maxSize * 0.9) {
        return; // Enough space available
    }
    
    // Need to free up space
    console.debug(`LocalStorage needs cleanup: current=${currentSize}, required=${requiredSize}`);
    
    // Clean up expired entries first
    await cleanupLocalStorage();
    
    // Check again
    const newSize = await getLocalStorageSize();
    if (newSize + requiredSize <= maxSize * 0.9) {
        return; // Enough space after cleanup
    }
    
    // Still not enough space, remove oldest entries
    await cleanupOldestEntries(requiredSize);
}

/**
 * Get LocalStorage size
 */
async function getLocalStorageSize() {
    let totalSize = 0;
    
    for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        const item = localStorage.getItem(key);
        totalSize += key.length + (item ? item.length : 0);
    }
    
    return totalSize;
}

/**
 * Cleanup oldest entries
 */
async function cleanupOldestEntries(requiredSize) {
    const entries = [];
    
    // Collect all entries with metadata
    for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        const item = localStorage.getItem(key);
        
        try {
            const entry = JSON.parse(item);
            if (entry && entry.timestamp) {
                entries.push({
                    key,
                    timestamp: entry.timestamp,
                    size: key.length + item.length
                });
            }
        } catch (error) {
            // Skip invalid entries
        }
    }
    
    // Sort by timestamp (oldest first)
    entries.sort((a, b) => a.timestamp - b.timestamp);
    
    // Remove oldest entries until we have enough space
    let freedSize = 0;
    const removedKeys = [];
    
    for (const entry of entries) {
        localStorage.removeItem(entry.key);
        freedSize += entry.size;
        removedKeys.push(entry.key);
        
        if (freedSize >= requiredSize) {
            break;
        }
    }
    
    console.debug(`Freed ${freedSize} bytes by removing ${removedKeys.length} entries`);
    
    // Update cache index
    for (const key of removedKeys) {
        cacheIndex.delete(key);
    }
}

/**
 * Rebuild cache index
 */
async function rebuildCacheIndex(cacheType) {
    // Clear existing index entries for this cache type
    for (const [key, entry] of cacheIndex.entries()) {
        if (entry.cacheType === cacheType) {
            cacheIndex.delete(key);
        }
    }
    
    // Rebuild index based on storage content
    switch (cacheType) {
        case 'localStorage':
            await rebuildLocalStorageIndex();
            break;
            
        case 'indexedDB':
            await rebuildIndexedDBIndex();
            break;
    }
}

/**
 * Rebuild LocalStorage index
 */
async function rebuildLocalStorageIndex() {
    for (let i = 0; i < localStorage.length; i++) {
        const key = localStorage.key(i);
        const item = localStorage.getItem(key);
        
        try {
            const entry = JSON.parse(item);
            if (validateCacheEntry(entry)) {
                updateIndex(key, {
                    size: entry.size,
                    ttl: entry.ttl,
                    expiresAt: entry.expiresAt,
                    lastModified: entry.timestamp,
                    lastAccess: entry.timestamp,
                    accessCount: 1,
                    cacheType: 'localStorage'
                });
            }
        } catch (error) {
            // Skip invalid entries
        }
    }
}

// Placeholder functions for IndexedDB (to be implemented if needed)
async function getFromIndexedDB(key, options) { return null; }
async function setInIndexedDB(entry, options) { return false; }
async function deleteFromIndexedDB(key, options) { return false; }
async function hasInIndexedDB(key, options) { return false; }
async function getKeysFromIndexedDB(options) { return []; }
async function clearIndexedDB(options) { return 0; }
async function cleanupIndexedDB(maxAge) { return 0; }
async function optimizeIndexedDB(strategy) { return { optimized: false }; }
async function rebuildIndexedDBIndex() {}

// Initialize on startup
(async function init() {
    await initializeStorage();
    
    // Send startup message
    self.postMessage({
        type: 'startup',
        workerState,
        capabilities: await handleCapabilities(),
        timestamp: Date.now()
    });
    
    console.log('Cache Worker started');
})();