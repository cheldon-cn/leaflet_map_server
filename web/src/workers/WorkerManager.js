// Worker Manager - Manages Web Workers for parallel processing

import { EventEmitter } from '../services/utils/EventEmitter.js';

export class WorkerManager extends EventEmitter {
    constructor(options = {}) {
        super();

        this.options = {
            maxWorkers: options.maxWorkers || 4,
            workerTimeout: options.workerTimeout || 30000,
            enableWorkerPool: options.enableWorkerPool !== false,
            workerTypes: options.workerTypes || {
                imageProcessor: {
                    url: './src/workers/image-processor.worker.js',
                    maxInstances: 2,
                    enabled: true,
                },
                cacheWorker: {
                    url: './src/workers/cache-worker.worker.js',
                    maxInstances: 2,
                    enabled: true,
                },
            },
            ...options,
        };

        // Worker instances
        this.workers = new Map();
        this.workerPools = new Map();

        // Task queue
        this.taskQueue = [];
        this.activeTasks = new Map();

        // Statistics
        this.stats = {
            totalTasks: 0,
            completedTasks: 0,
            failedTasks: 0,
            activeWorkers: 0,
            totalWorkers: 0,
            workerErrors: 0,
            averageTaskTime: 0,
            queueLength: 0,
        };

        // Worker readiness
        this.workerReady = new Map();

        // Initialize worker pools
        this.initWorkerPools();

        console.debug('WorkerManager initialized');
    }

    /**
     * Initialize worker pools
     */
    initWorkerPools() {
        for (const [workerType, config] of Object.entries(this.options.workerTypes)) {
            if (config.enabled) {
                this.initWorkerPool(workerType, config);
            }
        }
    }

    /**
     * Initialize a specific worker pool
     */
    initWorkerPool(workerType, config) {
        const pool = {
            type: workerType,
            config,
            workers: [],
            idleWorkers: [],
            busyWorkers: new Set(),
            taskQueue: [],
            stats: {
                created: 0,
                destroyed: 0,
                maxConcurrent: 0,
                totalTasks: 0,
                currentTasks: 0,
            },
        };

        this.workerPools.set(workerType, pool);

        // Pre-initialize workers if pool is enabled
        if (this.options.enableWorkerPool) {
            this.preInitializeWorkers(workerType);
        }

        this.emit('workerPoolInitialized', { workerType, config });
    }

    /**
     * Pre-initialize workers
     */
    preInitializeWorkers(workerType) {
        const pool = this.workerPools.get(workerType);
        if (!pool) return;

        const { maxInstances = 2 } = pool.config;

        // Create initial workers (up to half of max instances)
        const initialCount = Math.min(2, maxInstances);

        for (let i = 0; i < initialCount; i++) {
            this.createWorker(workerType);
        }
    }

    /**
     * Create a new worker instance
     */
    createWorker(workerType, options = {}) {
        const pool = this.workerPools.get(workerType);
        if (!pool) {
            throw new Error(`Unknown worker type: ${workerType}`);
        }

        const { url, maxInstances = 2 } = pool.config;
        const { onMessage, onError, onTerminate } = options;

        // Check if we've reached the maximum instances
        if (pool.workers.length >= maxInstances) {
            console.warn(`Maximum worker instances reached for ${workerType}: ${maxInstances}`);
            return null;
        }

        // Create worker instance
        const workerId = `${workerType}_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;
        const worker = {
            id: workerId,
            type: workerType,
            instance: new Worker(url, { type: 'module' }),
            busy: false,
            ready: false,
            lastUsed: Date.now(),
            taskCount: 0,
            errorCount: 0,
            createdAt: Date.now(),
            timeout: null,
        };

        // Setup message handler
        worker.instance.onmessage = (event) => {
            this.handleWorkerMessage(worker, event, onMessage);
        };

        // Setup error handler
        worker.instance.onerror = (error) => {
            this.handleWorkerError(worker, error, onError);
        };

        // Setup termination handler
        worker.instance.onmessageerror = (error) => {
            console.error(`Worker ${workerId} message error:`, error);
        };

        // Add to pool
        pool.workers.push(worker);
        pool.idleWorkers.push(worker);
        pool.stats.created++;

        // Update global stats
        this.stats.totalWorkers++;

        // Send initialization message
        this.sendToWorker(worker, {
            type: 'init',
            data: {
                workerId,
                workerType,
                config: pool.config,
                timestamp: Date.now(),
            },
        });

        this.emit('workerCreated', {
            workerId,
            workerType,
            poolSize: pool.workers.length,
            idleWorkers: pool.idleWorkers.length,
        });

        return worker;
    }

    /**
     * Handle worker message
     */
    handleWorkerMessage(worker, event, customHandler) {
        const { data } = event;
        const { type, id, result, error, state } = data;

        // Update worker state
        worker.lastUsed = Date.now();

        // Clear timeout
        if (worker.timeout) {
            clearTimeout(worker.timeout);
            worker.timeout = null;
        }

        // Handle different message types
        switch (type) {
            case 'ready':
                worker.ready = true;
                this.markWorkerIdle(worker);
                this.emit('workerReady', {
                    workerId: worker.id,
                    workerType: worker.type,
                    state,
                });
                break;

            case 'result':
                worker.taskCount++;
                this.handleTaskResult(id, result, error);
                this.markWorkerIdle(worker);
                break;

            case 'error':
                worker.errorCount++;
                this.emit('workerError', {
                    workerId: worker.id,
                    error,
                    state,
                });
                break;

            default:
                // Pass to custom handler if provided
                if (customHandler) {
                    customHandler(data);
                }
                break;
        }

        // Update pool stats
        const pool = this.workerPools.get(worker.type);
        if (pool) {
            pool.stats.currentTasks = pool.busyWorkers.size;
            pool.stats.maxConcurrent = Math.max(pool.stats.maxConcurrent, pool.busyWorkers.size);
        }
    }

    /**
     * Handle worker error
     */
    handleWorkerError(worker, error, customHandler) {
        console.error(`Worker ${worker.id} error:`, error);

        worker.errorCount++;
        this.stats.workerErrors++;

        // Call custom handler if provided
        if (customHandler) {
            customHandler(error, worker);
        }

        this.emit('workerError', {
            workerId: worker.id,
            error,
            workerState: worker,
        });
    }

    /**
     * Mark worker as idle
     */
    markWorkerIdle(worker) {
        worker.busy = false;

        const pool = this.workerPools.get(worker.type);
        if (pool) {
            pool.busyWorkers.delete(worker.id);

            // Add to idle workers if not already there
            if (!pool.idleWorkers.includes(worker)) {
                pool.idleWorkers.push(worker);
            }

            // Process queued tasks
            this.processTaskQueue(worker.type);
        }

        this.stats.activeWorkers--;

        this.emit('workerIdle', {
            workerId: worker.id,
            workerType: worker.type,
            taskCount: worker.taskCount,
            errorCount: worker.errorCount,
        });
    }

    /**
     * Mark worker as busy
     */
    markWorkerBusy(worker) {
        worker.busy = true;

        const pool = this.workerPools.get(worker.type);
        if (pool) {
            pool.busyWorkers.add(worker.id);

            // Remove from idle workers
            const index = pool.idleWorkers.indexOf(worker);
            if (index > -1) {
                pool.idleWorkers.splice(index, 1);
            }
        }

        this.stats.activeWorkers++;

        this.emit('workerBusy', {
            workerId: worker.id,
            workerType: worker.type,
        });
    }

    /**
     * Execute a task on a worker
     */
    async executeTask(workerType, taskData, options = {}) {
        const {
            timeout = this.options.workerTimeout,
            priority = 0,
            onProgress,
            onResult,
            onError,
        } = options;

        // Generate task ID
        const taskId = `task_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`;

        // Create task object
        const task = {
            id: taskId,
            workerType,
            data: taskData,
            options,
            status: 'queued',
            created: Date.now(),
            started: null,
            completed: null,
            result: null,
            error: null,
            priority,
        };

        // Add to task queue
        this.taskQueue.push(task);
        this.stats.queueLength++;
        this.stats.totalTasks++;

        // Sort by priority (higher priority first)
        this.taskQueue.sort((a, b) => b.priority - a.priority);

        // Try to execute immediately
        const executed = await this.tryExecuteTask(task);

        if (!executed) {
            // Task is queued, wait for execution
            return new Promise((resolve, reject) => {
                const taskCompletionHandler = (completedTask) => {
                    if (completedTask.id === taskId) {
                        this.off('taskCompleted', taskCompletionHandler);
                        this.off('taskFailed', taskCompletionHandler);

                        if (completedTask.status === 'completed') {
                            resolve(completedTask.result);
                        } else {
                            reject(completedTask.error);
                        }
                    }
                };

                this.on('taskCompleted', taskCompletionHandler);
                this.on('taskFailed', taskCompletionHandler);

                // Set timeout for task execution
                if (timeout > 0) {
                    setTimeout(() => {
                        if (task.status === 'queued' || task.status === 'executing') {
                            task.status = 'failed';
                            task.error = new Error(`Task timeout after ${timeout}ms`);
                            this.emit('taskFailed', task);
                        }
                    }, timeout);
                }
            });
        }

        // Task was executed immediately, return result
        if (task.status === 'completed') {
            return task.result;
        } else {
            throw task.error;
        }
    }

    /**
     * Try to execute a task
     */
    async tryExecuteTask(task) {
        const { workerType } = task;

        // Get an available worker
        const worker = await this.getAvailableWorker(workerType);

        if (!worker) {
            return false; // No worker available
        }

        // Mark task as executing
        task.status = 'executing';
        task.started = Date.now();

        // Add to active tasks
        this.activeTasks.set(task.id, { task, worker });

        // Mark worker as busy
        this.markWorkerBusy(worker);

        // Set timeout for task execution
        const timeout = task.options.timeout || this.options.workerTimeout;
        if (timeout > 0) {
            worker.timeout = setTimeout(() => {
                this.handleTaskTimeout(task, worker);
            }, timeout);
        }

        // Send task to worker
        this.sendToWorker(worker, {
            id: task.id,
            type: 'task',
            data: task.data,
            options: task.options,
        });

        return true;
    }

    /**
     * Get an available worker
     */
    async getAvailableWorker(workerType) {
        const pool = this.workerPools.get(workerType);
        if (!pool) {
            throw new Error(`Unknown worker type: ${workerType}`);
        }

        // Check for idle workers
        if (pool.idleWorkers.length > 0) {
            return pool.idleWorkers.shift();
        }

        // Check if we can create a new worker
        if (pool.workers.length < pool.config.maxInstances) {
            return this.createWorker(workerType);
        }

        // No workers available
        return null;
    }

    /**
     * Handle task timeout
     */
    handleTaskTimeout(task, worker) {
        console.warn(`Task ${task.id} timeout, terminating worker ${worker.id}`);

        // Terminate the worker
        this.terminateWorker(worker.id);

        // Mark task as failed
        task.status = 'failed';
        task.error = new Error(`Task execution timeout after ${worker.timeout}ms`);
        task.completed = Date.now();

        // Remove from active tasks
        this.activeTasks.delete(task.id);

        // Update stats
        this.stats.failedTasks++;

        // Emit event
        this.emit('taskFailed', task);
    }

    /**
     * Handle task result
     */
    handleTaskResult(taskId, result, error) {
        const taskInfo = this.activeTasks.get(taskId);
        if (!taskInfo) {
            console.warn(`No active task found for ID: ${taskId}`);
            return;
        }

        const { task, worker } = taskInfo;

        // Update task state
        task.completed = Date.now();
        task.duration = task.completed - task.started;

        if (error) {
            task.status = 'failed';
            task.error = error;
            this.stats.failedTasks++;
            this.emit('taskFailed', task);
        } else {
            task.status = 'completed';
            task.result = result;
            this.stats.completedTasks++;
            this.emit('taskCompleted', task);
        }

        // Update average task time
        this.updateAverageTaskTime(task.duration);

        // Remove from active tasks
        this.activeTasks.delete(taskId);

        // Update pool stats
        const pool = this.workerPools.get(worker.type);
        if (pool) {
            pool.stats.totalTasks++;
        }
    }

    /**
     * Update average task time
     */
    updateAverageTaskTime(newTime) {
        const totalTasks = this.stats.completedTasks + this.stats.failedTasks;
        if (totalTasks === 1) {
            this.stats.averageTaskTime = newTime;
        } else {
            const totalTime = this.stats.averageTaskTime * (totalTasks - 1) + newTime;
            this.stats.averageTaskTime = totalTime / totalTasks;
        }
    }

    /**
     * Send message to worker
     */
    sendToWorker(worker, message) {
        worker.instance.postMessage(message);
    }

    /**
     * Terminate a worker
     */
    terminateWorker(workerId) {
        // Find worker
        for (const pool of this.workerPools.values()) {
            const index = pool.workers.findIndex((w) => w.id === workerId);
            if (index > -1) {
                const worker = pool.workers[index];

                // Terminate worker instance
                worker.instance.terminate();

                // Remove from collections
                pool.workers.splice(index, 1);

                const idleIndex = pool.idleWorkers.indexOf(worker);
                if (idleIndex > -1) {
                    pool.idleWorkers.splice(idleIndex, 1);
                }

                pool.busyWorkers.delete(workerId);
                pool.stats.destroyed++;

                // Update global stats
                this.stats.totalWorkers--;
                if (worker.busy) {
                    this.stats.activeWorkers--;
                }

                // Clean up active tasks for this worker
                for (const [taskId, info] of this.activeTasks.entries()) {
                    if (info.worker.id === workerId) {
                        this.activeTasks.delete(taskId);
                    }
                }

                this.emit('workerTerminated', {
                    workerId,
                    workerType: worker.type,
                    taskCount: worker.taskCount,
                    errorCount: worker.errorCount,
                    lifetime: Date.now() - worker.createdAt,
                });

                return true;
            }
        }

        return false;
    }

    /**
     * Get worker status
     */
    getWorkerStatus(workerId) {
        for (const pool of this.workerPools.values()) {
            const worker = pool.workers.find((w) => w.id === workerId);
            if (worker) {
                return {
                    id: worker.id,
                    type: worker.type,
                    busy: worker.busy,
                    ready: worker.ready,
                    taskCount: worker.taskCount,
                    errorCount: worker.errorCount,
                    createdAt: worker.createdAt,
                    lastUsed: worker.lastUsed,
                    isAlive: true,
                };
            }
        }

        return null;
    }

    /**
     * Get worker pool status
     */
    getWorkerPoolStatus(workerType) {
        const pool = this.workerPools.get(workerType);
        if (!pool) {
            return null;
        }

        const idleCount = pool.idleWorkers.length;
        const busyCount = pool.busyWorkers.size;
        const totalCount = pool.workers.length;

        return {
            type: workerType,
            config: { ...pool.config },
            workers: {
                total: totalCount,
                idle: idleCount,
                busy: busyCount,
                available: idleCount,
                maxInstances: pool.config.maxInstances,
            },
            tasks: {
                total: pool.stats.totalTasks,
                current: pool.stats.currentTasks,
                maxConcurrent: pool.stats.maxConcurrent,
                queued: pool.taskQueue.length,
            },
            stats: { ...pool.stats },
        };
    }

    /**
     * Get all worker pool statuses
     */
    getAllWorkerPoolStatuses() {
        const statuses = {};

        for (const [workerType] of Object.entries(this.options.workerTypes)) {
            const status = this.getWorkerPoolStatus(workerType);
            if (status) {
                statuses[workerType] = status;
            }
        }

        return statuses;
    }

    /**
     * Get manager statistics
     */
    getStats() {
        const totalQueued = this.taskQueue.length;
        const totalActive = this.activeTasks.size;
        const totalWorkers = this.stats.totalWorkers;
        const activeWorkers = this.stats.activeWorkers;

        const poolStatuses = this.getAllWorkerPoolStatuses();
        const totalPoolTasks = Object.values(poolStatuses).reduce(
            (sum, pool) => sum + pool.tasks.total,
            0
        );

        const successRate =
            this.stats.totalTasks > 0
                ? (this.stats.completedTasks / this.stats.totalTasks) * 100
                : 0;

        return {
            tasks: {
                total: this.stats.totalTasks,
                completed: this.stats.completedTasks,
                failed: this.stats.failedTasks,
                queued: totalQueued,
                active: totalActive,
                successRate: parseFloat(successRate.toFixed(2)),
                averageTime: parseFloat(this.stats.averageTaskTime.toFixed(2)),
            },
            workers: {
                total: totalWorkers,
                active: activeWorkers,
                idle: totalWorkers - activeWorkers,
                errors: this.stats.workerErrors,
            },
            pools: {
                total: Object.keys(this.options.workerTypes).length,
                enabled: Object.values(this.options.workerTypes).filter((c) => c.enabled).length,
                statuses: poolStatuses,
            },
            performance: {
                averageTaskTime: this.stats.averageTaskTime,
                totalPoolTasks,
            },
        };
    }

    /**
     * Clean up idle workers
     */
    cleanupIdleWorkers(maxIdleTime = 60000) {
        const now = Date.now();
        const terminated = [];

        for (const pool of this.workerPools.values()) {
            for (let i = pool.idleWorkers.length - 1; i >= 0; i--) {
                const worker = pool.idleWorkers[i];

                if (now - worker.lastUsed > maxIdleTime) {
                    // Terminate idle worker
                    if (this.terminateWorker(worker.id)) {
                        terminated.push({
                            workerId: worker.id,
                            workerType: worker.type,
                            idleTime: now - worker.lastUsed,
                        });

                        // Remove from idle workers list
                        pool.idleWorkers.splice(i, 1);
                    }
                }
            }
        }

        if (terminated.length > 0) {
            this.emit('idleWorkersCleaned', {
                terminatedCount: terminated.length,
                terminatedWorkers: terminated,
                maxIdleTime,
            });
        }

        return terminated;
    }

    /**
     * Reset worker manager
     */
    async reset() {
        // Terminate all workers
        for (const pool of this.workerPools.values()) {
            for (const worker of pool.workers) {
                worker.instance.terminate();
            }

            pool.workers = [];
            pool.idleWorkers = [];
            pool.busyWorkers.clear();
            pool.taskQueue = [];
            pool.stats = {
                created: 0,
                destroyed: 0,
                maxConcurrent: 0,
                totalTasks: 0,
                currentTasks: 0,
            };
        }

        // Clear task queue
        this.taskQueue = [];
        this.activeTasks.clear();

        // Reset statistics
        this.stats = {
            totalTasks: 0,
            completedTasks: 0,
            failedTasks: 0,
            activeWorkers: 0,
            totalWorkers: 0,
            workerErrors: 0,
            averageTaskTime: 0,
            queueLength: 0,
        };

        // Reinitialize worker pools
        this.initWorkerPools();

        this.emit('managerReset');
    }

    /**
     * Clean up resources
     */
    destroy() {
        // Terminate all workers

        for (const pool of this.workerPools.values()) {
            for (const worker of pool.workers) {
                worker.instance.terminate();
            }
        }

        // Clear all collections
        this.workers.clear();
        this.workerPools.clear();
        this.taskQueue = [];
        this.activeTasks.clear();
        this.workerReady.clear();

        this.clear();

        console.debug('WorkerManager destroyed');
    }
}

// Default export
export default WorkerManager;
