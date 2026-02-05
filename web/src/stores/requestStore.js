// Request store - manages API request state and statistics

import { EventEmitter } from '../services/utils/EventEmitter.js';

class RequestStore extends EventEmitter {
    constructor() {
        super();

        // Initial state
        this.state = {
            // Active requests
            activeRequests: new Map(),

            // Request statistics
            totalRequests: 0,
            successfulRequests: 0,
            failedRequests: 0,

            // Cache statistics
            cacheHits: 0,
            cacheMisses: 0,

            // Performance metrics
            lastRenderTime: 0,
            averageRenderTime: 0,
            totalRenderTime: 0,
            minRenderTime: Infinity,
            maxRenderTime: 0,

            // Error tracking
            lastError: null,
            errorCount: 0,
            recentErrors: [],

            // Network status
            isOnline: navigator.onLine,
            lastOnlineCheck: new Date().toISOString(),

            // Throttling/limiting
            requestQueue: [],
            concurrentLimit: 3,
            isThrottled: false,
            throttleUntil: null,

            // History (for debugging)
            requestHistory: [],
            maxHistorySize: 100,
        };

        // Request timeout tracking
        this.timeouts = new Map();

        console.debug('RequestStore initialized');
    }

    /**
     * Initialize store
     */
    async init() {
        // Load saved state from localStorage
        await this.loadFromStorage();

        // Setup network monitoring
        this.setupNetworkMonitoring();

        // Setup auto-save
        this.setupAutoSave();
    }

    /**
     * Get current state
     */
    getState() {
        return {
            ...this.state,
            activeRequests: new Map(this.state.activeRequests),
            requestQueue: [...this.state.requestQueue],
            recentErrors: [...this.state.recentErrors],
            requestHistory: [...this.state.requestHistory],
        };
    }

    /**
     * Subscribe to state changes
     * @param {Function} callback - Function to call when state changes
     * @returns {Function} Unsubscribe function
     */
    subscribe(callback) {
        this.on('stateChanged', callback);
        return () => this.off('stateChanged', callback);
    }

    /**
     * Start tracking a new request
     */
    startRequest(requestId, config = {}) {
        const requestInfo = {
            id: requestId,
            url: config.url,
            method: config.method || 'GET',
            startTime: performance.now(),
            config,
            status: 'pending',
        };

        const updatedActive = new Map(this.state.activeRequests);
        updatedActive.set(requestId, requestInfo);

        this.updateState({
            activeRequests: updatedActive,
            totalRequests: this.state.totalRequests + 1,
        });

        this.emit('requestStarted', requestInfo);

        return requestInfo;
    }

    /**
     * Update request status
     */
    updateRequest(requestId, updates) {
        const request = this.state.activeRequests.get(requestId);

        if (!request) {
            console.warn(`Request ${requestId} not found`);
            return null;
        }

        const updatedRequest = { ...request, ...updates };
        const updatedActive = new Map(this.state.activeRequests);
        updatedActive.set(requestId, updatedRequest);

        this.updateState({
            activeRequests: updatedActive,
        });

        this.emit('requestUpdated', { requestId, updates });

        return updatedRequest;
    }

    /**
     * Complete a successful request
     */
    completeRequest(requestId, responseData) {
        const request = this.state.activeRequests.get(requestId);

        if (!request) {
            console.warn(`Request ${requestId} not found`);
            return;
        }

        const endTime = performance.now();
        const duration = endTime - request.startTime;

        // Update request info
        const completedRequest = {
            ...request,
            endTime,
            duration,
            status: 'completed',
            response: responseData,
            error: null,
        };

        // Remove from active requests
        const updatedActive = new Map(this.state.activeRequests);
        updatedActive.delete(requestId);

        // Update statistics
        const updates = {
            activeRequests: updatedActive,
            successfulRequests: this.state.successfulRequests + 1,
            lastRenderTime: responseData.renderTime || duration,
            totalRenderTime: this.state.totalRenderTime + duration,
        };

        // Update min/max render times
        if (duration < this.state.minRenderTime) {
            updates.minRenderTime = duration;
        }

        if (duration > this.state.maxRenderTime) {
            updates.maxRenderTime = duration;
        }

        // Update average render time
        updates.averageRenderTime = updates.totalRenderTime / updates.successfulRequests;

        // Add to history
        const historyEntry = {
            ...completedRequest,
            timestamp: new Date().toISOString(),
        };

        const updatedHistory = [
            ...this.state.requestHistory.slice(-(this.state.maxHistorySize - 1)),
            historyEntry,
        ];

        updates.requestHistory = updatedHistory;

        this.updateState(updates);

        this.emit('requestCompleted', completedRequest);

        // Clear any timeout
        this.clearTimeout(requestId);

        return completedRequest;
    }

    /**
     * Fail a request
     */
    failRequest(requestId, error) {
        const request = this.state.activeRequests.get(requestId);

        if (!request) {
            console.warn(`Request ${requestId} not found`);
            return;
        }

        const endTime = performance.now();
        const duration = endTime - request.startTime;

        // Update request info
        const failedRequest = {
            ...request,
            endTime,
            duration,
            status: 'failed',
            response: null,
            error: {
                message: error.message,
                code: error.code,
                timestamp: new Date().toISOString(),
            },
        };

        // Remove from active requests
        const updatedActive = new Map(this.state.activeRequests);
        updatedActive.delete(requestId);

        // Update statistics
        const errorEntry = {
            requestId,
            error: failedRequest.error,
            timestamp: new Date().toISOString(),
            config: request.config,
        };

        const recentErrors = [...this.state.recentErrors.slice(-9), errorEntry];

        const updates = {
            activeRequests: updatedActive,
            failedRequests: this.state.failedRequests + 1,
            lastError: errorEntry,
            errorCount: this.state.errorCount + 1,
            recentErrors,
        };

        // Add to history
        const historyEntry = {
            ...failedRequest,
            timestamp: new Date().toISOString(),
        };

        const updatedHistory = [
            ...this.state.requestHistory.slice(-(this.state.maxHistorySize - 1)),
            historyEntry,
        ];

        updates.requestHistory = updatedHistory;

        this.updateState(updates);

        this.emit('requestFailed', failedRequest);

        // Clear any timeout
        this.clearTimeout(requestId);

        return failedRequest;
    }

    /**
     * Cancel a request
     */
    cancelRequest(requestId, reason = 'user_cancelled') {
        const request = this.state.activeRequests.get(requestId);

        if (!request) {
            console.warn(`Request ${requestId} not found`);
            return;
        }

        const endTime = performance.now();
        const duration = endTime - request.startTime;

        // Update request info
        const cancelledRequest = {
            ...request,
            endTime,
            duration,
            status: 'cancelled',
            response: null,
            error: { message: reason, timestamp: new Date().toISOString() },
        };

        // Remove from active requests
        const updatedActive = new Map(this.state.activeRequests);
        updatedActive.delete(requestId);

        // Add to history
        const historyEntry = {
            ...cancelledRequest,
            timestamp: new Date().toISOString(),
        };

        const updatedHistory = [
            ...this.state.requestHistory.slice(-(this.state.maxHistorySize - 1)),
            historyEntry,
        ];

        this.updateState({
            activeRequests: updatedActive,
            requestHistory: updatedHistory,
        });

        this.emit('requestCancelled', cancelledRequest);

        // Clear any timeout
        this.clearTimeout(requestId);

        return cancelledRequest;
    }

    /**
     * Record cache hit
     */
    recordCacheHit() {
        const updates = {
            cacheHits: this.state.cacheHits + 1,
        };

        this.updateState(updates);
        this.emit('cacheHit');
    }

    /**
     * Record cache miss
     */
    recordCacheMiss() {
        const updates = {
            cacheMisses: this.state.cacheMisses + 1,
        };

        this.updateState(updates);
        this.emit('cacheMiss');
    }

    /**
     * Get cache hit rate
     */
    getCacheHitRate() {
        const total = this.state.cacheHits + this.state.cacheMisses;
        return total > 0 ? this.state.cacheHits / total : 0;
    }

    /**
     * Get active request count
     */
    getActiveRequestCount() {
        return this.state.activeRequests.size;
    }

    /**
     * Get request statistics
     */
    getRequestStats() {
        const totalRequests = this.state.totalRequests;
        const successRate =
            totalRequests > 0 ? (this.state.successfulRequests / totalRequests) * 100 : 0;

        return {
            totalRequests,
            successfulRequests: this.state.successfulRequests,
            failedRequests: this.state.failedRequests,
            successRate: parseFloat(successRate.toFixed(2)),
            activeRequests: this.state.activeRequests.size,
            averageRenderTime: parseFloat(this.state.averageRenderTime.toFixed(2)),
            minRenderTime:
                this.state.minRenderTime === Infinity
                    ? 0
                    : parseFloat(this.state.minRenderTime.toFixed(2)),
            maxRenderTime: parseFloat(this.state.maxRenderTime.toFixed(2)),
            cacheHitRate: parseFloat((this.getCacheHitRate() * 100).toFixed(2)),
            errorCount: this.state.errorCount,
        };
    }

    /**
     * Queue a request for later execution
     */
    queueRequest(requestId, executeFn, priority = 'normal') {
        const queueEntry = {
            id: requestId,
            executeFn,
            priority,
            queuedAt: new Date().toISOString(),
        };

        const updatedQueue = [...this.state.requestQueue, queueEntry];

        // Sort by priority (high > normal > low)
        updatedQueue.sort((a, b) => {
            const priorityOrder = { high: 3, normal: 2, low: 1 };
            return priorityOrder[b.priority] - priorityOrder[a.priority];
        });

        this.updateState({ requestQueue: updatedQueue });
        this.emit('requestQueued', queueEntry);

        // Process queue
        this.processQueue();

        return queueEntry;
    }

    /**
     * Process queued requests
     */
    processQueue() {
        if (this.state.isThrottled) {
            const now = Date.now();
            if (this.state.throttleUntil && now < this.state.throttleUntil) {
                // Still throttled
                return;
            }
            // Throttle period ended
            this.updateState({ isThrottled: false, throttleUntil: null });
        }

        const availableSlots = this.state.concurrentLimit - this.getActiveRequestCount();

        if (availableSlots <= 0 || this.state.requestQueue.length === 0) {
            return;
        }

        // Take up to availableSlots from queue
        const toProcess = this.state.requestQueue.slice(0, availableSlots);
        const remainingQueue = this.state.requestQueue.slice(availableSlots);

        this.updateState({ requestQueue: remainingQueue });

        // Execute queued requests
        toProcess.forEach((entry) => {
            this.executeQueuedRequest(entry);
        });
    }

    /**
     * Execute a queued request
     */
    async executeQueuedRequest(queueEntry) {
        const { id, executeFn } = queueEntry;

        try {
            this.startRequest(id);

            const result = await executeFn();

            this.completeRequest(id, result);
        } catch (error) {
            this.failRequest(id, error);
        }

        // Continue processing queue
        setTimeout(() => this.processQueue(), 100);
    }

    /**
     * Throttle requests
     */
    throttleRequests(durationMs = 5000, reason = 'rate_limit') {
        const throttleUntil = Date.now() + durationMs;

        this.updateState({
            isThrottled: true,
            throttleUntil,
        });

        console.warn(`Requests throttled for ${durationMs}ms: ${reason}`);

        // Schedule un-throttle
        setTimeout(() => {
            this.updateState({ isThrottled: false, throttleUntil: null });
            this.processQueue();
        }, durationMs);

        this.emit('requestsThrottled', { durationMs, reason });
    }

    /**
     * Set request timeout
     */
    setTimeout(requestId, timeoutMs, onTimeout) {
        const timeoutId = setTimeout(() => {
            this.failRequest(requestId, {
                message: `Request timeout after ${timeoutMs}ms`,
                code: 'timeout',
            });

            if (onTimeout) {
                onTimeout();
            }
        }, timeoutMs);

        this.timeouts.set(requestId, timeoutId);
    }

    /**
     * Clear request timeout
     */
    clearTimeout(requestId) {
        const timeoutId = this.timeouts.get(requestId);

        if (timeoutId) {
            clearTimeout(timeoutId);
            this.timeouts.delete(requestId);
        }
    }

    /**
     * Reset store to initial state
     */
    reset() {
        const initialState = {
            activeRequests: new Map(),
            totalRequests: 0,
            successfulRequests: 0,
            failedRequests: 0,
            cacheHits: 0,
            cacheMisses: 0,
            lastRenderTime: 0,
            averageRenderTime: 0,
            totalRenderTime: 0,
            minRenderTime: Infinity,
            maxRenderTime: 0,
            lastError: null,
            errorCount: 0,
            recentErrors: [],
            isOnline: navigator.onLine,
            lastOnlineCheck: new Date().toISOString(),
            requestQueue: [],
            concurrentLimit: 3,
            isThrottled: false,
            throttleUntil: null,
            requestHistory: [],
            maxHistorySize: 100,
        };

        this.state = initialState;
        this.emit('storeReset', initialState);

        // Clear all timeouts
        for (const timeoutId of this.timeouts.values()) {
            clearTimeout(timeoutId);
        }
        this.timeouts.clear();
    }

    // ========== Private Methods ==========

    /**
     * Update state with changes
     */
    updateState(updates) {
        const oldState = { ...this.state };
        const newState = { ...this.state, ...updates };

        // Ensure Map objects are properly handled
        if (updates.activeRequests && !(updates.activeRequests instanceof Map)) {
            newState.activeRequests = new Map(updates.activeRequests);
        }

        this.state = newState;
        this.emit('stateChanged', newState, updates);
    }

    /**
     * Setup network monitoring
     */
    setupNetworkMonitoring() {
        window.addEventListener('online', () => {
            this.updateState({
                isOnline: true,
                lastOnlineCheck: new Date().toISOString(),
            });
            this.emit('networkOnline');
        });

        window.addEventListener('offline', () => {
            this.updateState({
                isOnline: false,
                lastOnlineCheck: new Date().toISOString(),
            });
            this.emit('networkOffline');
        });

        // Periodic online check
        setInterval(() => {
            this.updateState({
                lastOnlineCheck: new Date().toISOString(),
            });
        }, 30000);
    }

    /**
     * Setup auto-save to localStorage
     */
    setupAutoSave() {
        let saveTimeout = null;

        // Debounced save on state changes
        this.on('stateChanged', () => {
            if (saveTimeout) {
                clearTimeout(saveTimeout);
            }

            saveTimeout = setTimeout(() => {
                this.saveToStorage();
            }, 5000); // Save every 5 seconds if there are changes
        });

        // Save on page unload
        window.addEventListener('beforeunload', () => {
            this.saveToStorage();
        });
    }

    /**
     * Save state to localStorage
     */
    saveToStorage() {
        try {
            const storageState = {
                totalRequests: this.state.totalRequests,
                successfulRequests: this.state.successfulRequests,
                failedRequests: this.state.failedRequests,
                cacheHits: this.state.cacheHits,
                cacheMisses: this.state.cacheMisses,
                lastRenderTime: this.state.lastRenderTime,
                averageRenderTime: this.state.averageRenderTime,
                errorCount: this.state.errorCount,
                isOnline: this.state.isOnline,
                lastOnlineCheck: this.state.lastOnlineCheck,
                recentErrors: this.state.recentErrors.slice(-10), // Keep only recent errors
            };

            localStorage.setItem('requestStore_state', JSON.stringify(storageState));

            console.debug('RequestStore state saved to localStorage');
        } catch (error) {
            console.error('Failed to save RequestStore state:', error);
        }
    }

    /**
     * Load state from localStorage
     */
    async loadFromStorage() {
        try {
            const saved = localStorage.getItem('requestStore_state');

            if (saved) {
                const parsed = JSON.parse(saved);
                this.updateState(parsed);

                console.debug('RequestStore state loaded from localStorage');
            }
        } catch (error) {
            console.error('Failed to load RequestStore state:', error);
        }
    }

    // ========== Performance Methods ==========

    /**
     * Get performance summary
     */
    getPerformanceSummary() {
        const stats = this.getRequestStats();

        return {
            requests: {
                total: stats.totalRequests,
                successRate: stats.successRate,
                active: stats.activeRequests,
            },
            cache: {
                hits: this.state.cacheHits,
                misses: this.state.cacheMisses,
                hitRate: stats.cacheHitRate,
            },
            renderTimes: {
                average: stats.averageRenderTime,
                min: stats.minRenderTime,
                max: stats.maxRenderTime,
                last: this.state.lastRenderTime,
            },
            errors: {
                total: stats.errorCount,
                recent: this.state.recentErrors.length,
            },
            network: {
                isOnline: this.state.isOnline,
                lastCheck: this.state.lastOnlineCheck,
            },
        };
    }

    /**
     * Clear request history
     */
    clearHistory() {
        this.updateState({
            requestHistory: [],
        });

        this.emit('historyCleared');
    }

    /**
     * Get recent errors
     */
    getRecentErrors(limit = 10) {
        return this.state.recentErrors.slice(-limit);
    }

    /**
     * Clear recent errors
     */
    clearRecentErrors() {
        this.updateState({
            recentErrors: [],
            lastError: null,
        });

        this.emit('errorsCleared');
    }

    /**
     * Update concurrent request limit
     */
    updateConcurrentLimit(limit) {
        if (limit < 1 || limit > 10) {
            throw new Error('Concurrent limit must be between 1 and 10');
        }

        this.updateState({
            concurrentLimit: limit,
        });

        this.emit('concurrentLimitUpdated', limit);
    }
}

// Create and export singleton instance
export const requestStore = new RequestStore();

// Default export
export default requestStore;
