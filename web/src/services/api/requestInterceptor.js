// Request Interceptor - Handles HTTP request interception and transformation

import { EventEmitter } from '../utils/EventEmitter.js';

export class RequestInterceptor extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            baseURL: options.baseURL || '',
            timeout: options.timeout || 30000,
            retryCount: options.retryCount || 3,
            retryDelay: options.retryDelay || 1000,
            enableOfflineQueue: options.enableOfflineQueue || true,
            enableCache: options.enableCache || true,
            ...options,
        };

        // Request queue for offline mode
        this.offlineQueue = [];
        this.isOnline = navigator.onLine;
        this.isProcessingQueue = false;

        // Request tracking
        this.activeRequests = new Map();
        this.requestCount = 0;

        // Statistics
        this.stats = {
            totalRequests: 0,
            successfulRequests: 0,
            failedRequests: 0,
            retriedRequests: 0,
            queuedRequests: 0,
            cacheHits: 0,
            cacheMisses: 0,
            averageResponseTime: 0,
        };

        // Interceptors
        this.requestInterceptors = [];
        this.responseInterceptors = [];
        this.errorInterceptors = [];

        // Setup network monitoring
        this.setupNetworkMonitoring();

        console.debug('RequestInterceptor initialized');
    }

    /**
     * Setup network monitoring
     */
    setupNetworkMonitoring() {
        window.addEventListener('online', () => this.onNetworkStatusChange(true));
        window.addEventListener('offline', () => this.onNetworkStatusChange(false));

        // Initial status
        this.onNetworkStatusChange(this.isOnline);
    }

    /**
     * Handle network status change
     */
    onNetworkStatusChange(isOnline) {
        const wasOnline = this.isOnline;
        this.isOnline = isOnline;

        if (isOnline && !wasOnline) {
            // Came back online, process queued requests
            this.processOfflineQueue();

            this.emit('networkOnline');
        } else if (!isOnline && wasOnline) {
            // Went offline
            this.emit('networkOffline');
        }

        this.emit('networkStatusChanged', { isOnline });
    }

    /**
     * Add request interceptor
     */
    addRequestInterceptor(interceptor) {
        if (typeof interceptor !== 'function') {
            throw new TypeError('Request interceptor must be a function');
        }

        this.requestInterceptors.push(interceptor);
        return () => {
            const index = this.requestInterceptors.indexOf(interceptor);
            if (index > -1) {
                this.requestInterceptors.splice(index, 1);
            }
        };
    }

    /**
     * Add response interceptor
     */
    addResponseInterceptor(interceptor) {
        if (typeof interceptor !== 'function') {
            throw new TypeError('Response interceptor must be a function');
        }

        this.responseInterceptors.push(interceptor);
        return () => {
            const index = this.responseInterceptors.indexOf(interceptor);
            if (index > -1) {
                this.responseInterceptors.splice(index, 1);
            }
        };
    }

    /**
     * Add error interceptor
     */
    addErrorInterceptor(interceptor) {
        if (typeof interceptor !== 'function') {
            throw new TypeError('Error interceptor must be a function');
        }

        this.errorInterceptors.push(interceptor);
        return () => {
            const index = this.errorInterceptors.indexOf(interceptor);
            if (index > -1) {
                this.errorInterceptors.splice(index, 1);
            }
        };
    }

    /**
     * Execute request with interceptors
     */
    async executeRequest(requestFn, requestConfig = {}) {
        const requestId = `req_${Date.now()}_${this.requestCount++}`;
        const startTime = performance.now();

        // Create request context
        const context = {
            id: requestId,
            config: requestConfig,
            timestamp: new Date().toISOString(),
            startTime,
            retryCount: 0,
        };

        // Track active request
        this.activeRequests.set(requestId, context);
        this.stats.totalRequests++;

        // Apply request interceptors
        let modifiedConfig = { ...requestConfig };
        for (const interceptor of this.requestInterceptors) {
            try {
                const result = interceptor(modifiedConfig, context);
                if (result && typeof result === 'object') {
                    modifiedConfig = result;
                }
            } catch (error) {
                console.warn('Request interceptor error:', error);
            }
        }

        // Check if we should queue the request (offline mode)
        if (!this.isOnline && this.options.enableOfflineQueue) {
            return this.queueOfflineRequest(requestFn, modifiedConfig, context);
        }

        // Execute the request
        try {
            const response = await this.executeWithRetry(
                () => requestFn(modifiedConfig),
                context,
                modifiedConfig
            );

            const endTime = performance.now();
            const responseTime = endTime - startTime;

            // Update stats
            this.stats.successfulRequests++;
            this.updateAverageResponseTime(responseTime);

            // Apply response interceptors
            let modifiedResponse = response;
            for (const interceptor of this.responseInterceptors) {
                try {
                    const result = interceptor(modifiedResponse, context);
                    if (result !== undefined) {
                        modifiedResponse = result;
                    }
                } catch (error) {
                    console.warn('Response interceptor error:', error);
                }
            }

            // Clean up
            this.activeRequests.delete(requestId);

            this.emit('requestSuccess', {
                requestId,
                config: modifiedConfig,
                response: modifiedResponse,
                responseTime,
                context,
            });

            return modifiedResponse;
        } catch (error) {
            const endTime = performance.now();
            const responseTime = endTime - startTime;

            // Update stats
            this.stats.failedRequests++;

            // Apply error interceptors
            let modifiedError = error;
            for (const interceptor of this.errorInterceptors) {
                try {
                    const result = interceptor(modifiedError, context);
                    if (result !== undefined) {
                        modifiedError = result;
                    }
                } catch (error) {
                    console.warn('Error interceptor error:', error);
                }
            }

            // Clean up
            this.activeRequests.delete(requestId);

            this.emit('requestError', {
                requestId,
                config: modifiedConfig,
                error: modifiedError,
                responseTime,
                context,
            });

            throw modifiedError;
        }
    }

    /**
     * Execute request with retry logic
     */
    async executeWithRetry(requestFn, context, config) {
        const maxRetries = config.retryCount || this.options.retryCount;
        const retryDelay = config.retryDelay || this.options.retryDelay;

        let lastError;

        for (let attempt = 0; attempt <= maxRetries; attempt++) {
            try {
                if (attempt > 0) {
                    // Not first attempt, so this is a retry
                    context.retryCount = attempt;
                    this.stats.retriedRequests++;

                    // Apply retry delay (exponential backoff)
                    const delay = retryDelay * Math.pow(2, attempt - 1);
                    await this.sleep(delay);

                    this.emit('requestRetry', {
                        requestId: context.id,
                        attempt,
                        delay,
                        config,
                    });
                }

                return await requestFn();
            } catch (error) {
                lastError = error;

                // Check if error is retryable
                if (!this.isErrorRetryable(error) || attempt === maxRetries) {
                    break;
                }

                // Log retry attempt
                console.debug(
                    `Request ${context.id} failed, retrying (${attempt + 1}/${maxRetries}):`,
                    error.message
                );
            }
        }

        throw lastError;
    }

    /**
     * Queue request for offline mode
     */
    async queueOfflineRequest(requestFn, config, context) {
        const queueEntry = {
            id: context.id,
            requestFn,
            config,
            context,
            timestamp: Date.now(),
            priority: config.priority || 0,
        };

        this.offlineQueue.push(queueEntry);
        this.stats.queuedRequests++;

        // Sort queue by priority (higher priority first)
        this.offlineQueue.sort((a, b) => b.priority - a.priority);

        this.emit('requestQueued', {
            requestId: context.id,
            config,
            context,
            queueSize: this.offlineQueue.length,
        });

        // Return a promise that will resolve when the request is processed
        return new Promise((resolve, reject) => {
            queueEntry.resolve = resolve;
            queueEntry.reject = reject;
        });
    }

    /**
     * Process offline queue
     */
    async processOfflineQueue() {
        if (this.isProcessingQueue || this.offlineQueue.length === 0) {
            return;
        }

        this.isProcessingQueue = true;

        try {
            this.emit('queueProcessingStart', {
                queueSize: this.offlineQueue.length,
            });

            // Process queued requests
            while (this.offlineQueue.length > 0 && this.isOnline) {
                const queueEntry = this.offlineQueue.shift();

                try {
                    const response = await this.executeWithRetry(
                        () => queueEntry.requestFn(queueEntry.config),
                        queueEntry.context,
                        queueEntry.config
                    );

                    // Resolve the original promise
                    if (queueEntry.resolve) {
                        queueEntry.resolve(response);
                    }
                } catch (error) {
                    // Reject the original promise
                    if (queueEntry.reject) {
                        queueEntry.reject(error);
                    }
                }

                // Small delay between requests to avoid overwhelming the server
                await this.sleep(100);
            }

            this.emit('queueProcessingComplete', {
                processedCount: this.offlineQueue.length,
                remaining: this.offlineQueue.length,
            });
        } finally {
            this.isProcessingQueue = false;
        }
    }

    /**
     * Check if error is retryable
     */
    isErrorRetryable(error) {
        // Network errors are usually retryable
        if (error.name === 'NetworkError' || error.name === 'TypeError') {
            return true;
        }

        // Timeout errors are retryable
        if (error.name === 'TimeoutError' || error.code === 'ECONNABORTED') {
            return true;
        }

        // 5xx server errors are retryable
        if (error.response && error.response.status >= 500 && error.response.status < 600) {
            return true;
        }

        // 429 Too Many Requests (with Retry-After header)
        if (error.response && error.response.status === 429) {
            return true;
        }

        return false;
    }

    /**
     * Sleep helper
     */
    sleep(ms) {
        return new Promise((resolve) => setTimeout(resolve, ms));
    }

    /**
     * Update average response time
     */
    updateAverageResponseTime(newTime) {
        const totalTime = this.stats.averageResponseTime * this.stats.successfulRequests + newTime;
        this.stats.averageResponseTime = totalTime / this.stats.successfulRequests;
    }

    /**
     * Cancel a request
     */
    cancelRequest(requestId) {
        const context = this.activeRequests.get(requestId);
        if (!context) {
            return false;
        }

        // Check if request has an abort controller
        if (context.abortController) {
            context.abortController.abort();
        }

        // Remove from active requests
        this.activeRequests.delete(requestId);

        this.emit('requestCancelled', { requestId, context });

        return true;
    }

    /**
     * Cancel all active requests
     */
    cancelAllRequests() {
        const cancelled = [];

        for (const [requestId, context] of this.activeRequests) {
            if (this.cancelRequest(requestId)) {
                cancelled.push(requestId);
            }
        }

        return cancelled;
    }

    /**
     * Clear offline queue
     */
    clearOfflineQueue() {
        const cleared = this.offlineQueue.length;
        this.offlineQueue = [];

        this.emit('queueCleared', { clearedCount: cleared });

        return cleared;
    }

    /**
     * Get active requests
     */
    getActiveRequests() {
        const requests = [];

        for (const [requestId, context] of this.activeRequests) {
            requests.push({
                id: requestId,
                config: context.config,
                timestamp: context.timestamp,
                elapsed: performance.now() - context.startTime,
                retryCount: context.retryCount,
            });
        }

        return requests;
    }

    /**
     * Get offline queue status
     */
    getOfflineQueueStatus() {
        return {
            size: this.offlineQueue.length,
            isProcessing: this.isProcessingQueue,
            items: this.offlineQueue.map((entry) => ({
                id: entry.id,
                timestamp: entry.timestamp,
                priority: entry.priority,
                config: entry.config,
            })),
        };
    }

    /**
     * Get statistics
     */
    getStats() {
        const successRate =
            this.stats.totalRequests > 0
                ? (this.stats.successfulRequests / this.stats.totalRequests) * 100
                : 0;

        const retryRate =
            this.stats.totalRequests > 0
                ? (this.stats.retriedRequests / this.stats.totalRequests) * 100
                : 0;

        const cacheHitRate =
            this.stats.cacheHits + this.stats.cacheMisses > 0
                ? (this.stats.cacheHits / (this.stats.cacheHits + this.stats.cacheMisses)) * 100
                : 0;

        return {
            ...this.stats,
            successRate: parseFloat(successRate.toFixed(2)),
            retryRate: parseFloat(retryRate.toFixed(2)),
            cacheHitRate: parseFloat(cacheHitRate.toFixed(2)),
            activeRequests: this.activeRequests.size,
            offlineQueueSize: this.offlineQueue.length,
            isOnline: this.isOnline,
        };
    }

    /**
     * Create abort controller for request
     */
    createAbortController() {
        const abortController = new AbortController();

        // Add timeout if specified
        if (this.options.timeout > 0) {
            setTimeout(() => {
                abortController.abort();
            }, this.options.timeout);
        }

        return abortController;
    }

    /**
     * Create fetch request with interceptors
     */
    async fetch(url, options = {}) {
        const abortController = this.createAbortController();

        const fetchConfig = {
            ...options,
            signal: abortController.signal,
        };

        const context = {
            abortController,
            url,
            options: fetchConfig,
        };

        return this.executeRequest(() => fetch(url, fetchConfig), { ...fetchConfig, context });
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Cancel all active requests
        this.cancelAllRequests();

        // Clear offline queue
        this.clearOfflineQueue();

        // Remove event listeners
        window.removeEventListener('online', () => this.onNetworkStatusChange(true));
        window.removeEventListener('offline', () => this.onNetworkStatusChange(false));

        // Clear interceptors
        this.requestInterceptors = [];
        this.responseInterceptors = [];
        this.errorInterceptors = [];

        // Clear active requests
        this.activeRequests.clear();

        this.clear();

        console.debug('RequestInterceptor destroyed');
    }
}

// Default export
export default RequestInterceptor;
