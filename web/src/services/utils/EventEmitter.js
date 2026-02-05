// EventEmitter - Simple event emitter implementation

export class EventEmitter {
    constructor() {
        this.events = new Map();
        this.maxListeners = 10;
        this.warnings = true;
    }

    /**
     * Add an event listener
     * @param {string} event - Event name
     * @param {Function} listener - Event listener function
     * @param {Object} options - Listener options
     * @returns {EventEmitter} - This instance for chaining
     */
    on(event, listener, options = {}) {
        if (typeof listener !== 'function') {
            throw new TypeError('Listener must be a function');
        }

        if (!this.events.has(event)) {
            this.events.set(event, new Set());
        }

        const listeners = this.events.get(event);

        // Check max listeners
        if (listeners.size >= this.maxListeners && this.warnings) {
            console.warn(
                `EventEmitter: Possible memory leak detected. '${event}' event ` +
                    `has ${listeners.size} listeners. Max is ${this.maxListeners}.`
            );
        }

        // Add listener with metadata
        const listenerWrapper = {
            fn: listener,
            once: options.once || false,
            context: options.context || null,
            priority: options.priority || 0,
        };

        listeners.add(listenerWrapper);

        return this;
    }

    /**
     * Add a one-time event listener
     * @param {string} event - Event name
     * @param {Function} listener - Event listener function
     * @param {Object} options - Listener options
     * @returns {EventEmitter} - This instance for chaining
     */
    once(event, listener, options = {}) {
        return this.on(event, listener, { ...options, once: true });
    }

    /**
     * Remove an event listener
     * @param {string} event - Event name
     * @param {Function} listener - Event listener function to remove
     * @returns {EventEmitter} - This instance for chaining
     */
    off(event, listener) {
        if (!this.events.has(event)) {
            return this;
        }

        const listeners = this.events.get(event);

        // Find and remove matching listener
        for (const wrapper of listeners) {
            if (wrapper.fn === listener) {
                listeners.delete(wrapper);
                break;
            }
        }

        // Clean up if no listeners left
        if (listeners.size === 0) {
            this.events.delete(event);
        }

        return this;
    }

    /**
     * Emit an event
     * @param {string} event - Event name
     * @param {...any} args - Arguments to pass to listeners
     * @returns {boolean} - True if event had listeners
     */
    emit(event, ...args) {
        if (!this.events.has(event)) {
            return false;
        }

        const listeners = this.events.get(event);

        // Sort by priority (highest first)
        const sortedListeners = Array.from(listeners).sort((a, b) => b.priority - a.priority);

        // Execute listeners
        const listenersToRemove = [];

        for (const wrapper of sortedListeners) {
            try {
                // Call listener with appropriate context
                const context = wrapper.context || this;
                wrapper.fn.apply(context, args);

                // Mark for removal if it's a once listener
                if (wrapper.once) {
                    listenersToRemove.push(wrapper);
                }
            } catch (error) {
                // Prevent one listener's error from breaking others
                console.error(`EventEmitter: Error in listener for event '${event}':`, error);
            }
        }

        // Remove once listeners
        if (listenersToRemove.length > 0) {
            for (const wrapper of listenersToRemove) {
                listeners.delete(wrapper);
            }
        }

        // Clean up if no listeners left
        if (listeners.size === 0) {
            this.events.delete(event);
        }

        return true;
    }

    /**
     * Get all listeners for an event
     * @param {string} event - Event name
     * @returns {Array<Function>} - Array of listener functions
     */
    listeners(event) {
        if (!this.events.has(event)) {
            return [];
        }

        const wrappers = this.events.get(event);
        return Array.from(wrappers).map((wrapper) => wrapper.fn);
    }

    /**
     * Get the number of listeners for an event
     * @param {string} event - Event name
     * @returns {number} - Number of listeners
     */
    listenerCount(event) {
        if (!this.events.has(event)) {
            return 0;
        }

        return this.events.get(event).size;
    }

    /**
     * Remove all listeners for an event
     * @param {string} event - Event name (optional)
     * @returns {EventEmitter} - This instance for chaining
     */
    removeAllListeners(event) {
        if (event) {
            this.events.delete(event);
        } else {
            this.events.clear();
        }

        return this;
    }

    /**
     * Get the event names that have listeners
     * @returns {Array<string>} - Array of event names
     */
    eventNames() {
        return Array.from(this.events.keys());
    }

    /**
     * Set the maximum number of listeners per event
     * @param {number} n - Maximum number of listeners
     * @returns {EventEmitter} - This instance for chaining
     */
    setMaxListeners(n) {
        if (typeof n !== 'number' || n < 0 || !Number.isInteger(n)) {
            throw new TypeError('setMaxListeners: argument must be a positive integer');
        }

        this.maxListeners = n;
        return this;
    }

    /**
     * Get the maximum number of listeners per event
     * @returns {number} - Maximum number of listeners
     */
    getMaxListeners() {
        return this.maxListeners;
    }

    /**
     * Enable or disable memory leak warnings
     * @param {boolean} enabled - Whether warnings are enabled
     * @returns {EventEmitter} - This instance for chaining
     */
    setWarnings(enabled) {
        this.warnings = enabled;
        return this;
    }

    /**
     * Add listener with high priority
     * @param {string} event - Event name
     * @param {Function} listener - Event listener function
     * @param {Object} options - Listener options
     * @returns {EventEmitter} - This instance for chaining
     */
    onPriority(event, listener, options = {}) {
        return this.on(event, listener, { ...options, priority: options.priority || 1 });
    }

    /**
     * Wait for an event to occur
     * @param {string} event - Event name
     * @param {Object} options - Wait options
     * @returns {Promise<any>} - Promise that resolves with event arguments
     */
    waitFor(event, options = {}) {
        return new Promise((resolve, reject) => {
            const timeout = options.timeout || 0;
            let timeoutId = null;

            const listener = (...args) => {
                if (timeoutId) {
                    clearTimeout(timeoutId);
                }
                resolve(args.length === 1 ? args[0] : args);
            };

            // Add timeout if specified
            if (timeout > 0) {
                timeoutId = setTimeout(() => {
                    this.off(event, listener);
                    reject(new Error(`Event '${event}' did not occur within ${timeout}ms`));
                }, timeout);
            }

            this.once(event, listener);
        });
    }

    /**
     * Emit an event asynchronously
     * @param {string} event - Event name
     * @param {...any} args - Arguments to pass to listeners
     * @returns {Promise<boolean>} - Promise that resolves when all listeners have been called
     */
    async emitAsync(event, ...args) {
        if (!this.events.has(event)) {
            return false;
        }

        const listeners = this.events.get(event);
        const sortedListeners = Array.from(listeners).sort((a, b) => b.priority - a.priority);

        const listenersToRemove = [];
        const promises = [];

        for (const wrapper of sortedListeners) {
            try {
                const context = wrapper.context || this;
                const result = wrapper.fn.apply(context, args);

                // Handle async listeners
                if (result && typeof result.then === 'function') {
                    promises.push(
                        result.catch((error) => {
                            console.error(
                                `EventEmitter: Async listener error for event '${event}':`,
                                error
                            );
                        })
                    );
                }

                if (wrapper.once) {
                    listenersToRemove.push(wrapper);
                }
            } catch (error) {
                console.error(`EventEmitter: Error in listener for event '${event}':`, error);
            }
        }

        // Wait for all async listeners to complete
        if (promises.length > 0) {
            await Promise.allSettled(promises);
        }

        // Remove once listeners
        if (listenersToRemove.length > 0) {
            for (const wrapper of listenersToRemove) {
                listeners.delete(wrapper);
            }
        }

        // Clean up if no listeners left
        if (listeners.size === 0) {
            this.events.delete(event);
        }

        return true;
    }

    /**
     * Check if an event has listeners
     * @param {string} event - Event name
     * @returns {boolean} - True if event has listeners
     */
    hasListeners(event) {
        return this.events.has(event) && this.events.get(event).size > 0;
    }

    /**
     * Get the number of registered events
     * @returns {number} - Number of registered events
     */
    eventCount() {
        return this.events.size;
    }

    /**
     * Get total number of listeners across all events
     * @returns {number} - Total number of listeners
     */
    totalListenerCount() {
        let total = 0;

        for (const listeners of this.events.values()) {
            total += listeners.size;
        }

        return total;
    }

    /**
     * Safely emit event with error handling
     * @param {string} event - Event name
     * @param {...any} args - Arguments to pass to listeners
     * @returns {Promise<boolean>} - Promise that resolves with success status
     */
    async safeEmit(event, ...args) {
        try {
            return await this.emitAsync(event, ...args);
        } catch (error) {
            console.error(`EventEmitter: Error emitting event '${event}':`, error);
            return false;
        }
    }

    /**
     * Clear all events and listeners
     */
    clear() {
        this.events.clear();
    }

    /**
     * Check if instance is an EventEmitter
     * @static
     * @param {any} obj - Object to check
     * @returns {boolean} - True if object is an EventEmitter
     */
    static isEventEmitter(obj) {
        return obj instanceof EventEmitter;
    }

    /**
     * Create an EventEmitter from an object
     * @static
     * @param {Object} obj - Object to augment
     * @returns {Object} - Augmented object
     */
    static augment(obj) {
        const emitter = new EventEmitter();

        // Copy methods to object
        const methods = [
            'on',
            'once',
            'off',
            'emit',
            'listeners',
            'listenerCount',
            'removeAllListeners',
            'eventNames',
            'setMaxListeners',
            'getMaxListeners',
            'setWarnings',
            'onPriority',
            'waitFor',
            'emitAsync',
            'hasListeners',
            'eventCount',
            'totalListenerCount',
            'safeEmit',
            'clear',
        ];

        methods.forEach((method) => {
            obj[method] = emitter[method].bind(emitter);
        });

        // Copy events property
        obj.events = emitter.events;

        return obj;
    }
}

// Default export
export default EventEmitter;
