// Debounce utility functions

/**
 * Creates a debounced function that delays invoking `func` until after `wait` milliseconds
 * have elapsed since the last time the debounced function was invoked.
 *
 * @param {Function} func - The function to debounce
 * @param {number} wait - The number of milliseconds to delay
 * @param {Object} options - Options object
 * @param {boolean} options.leading - Specify invoking on the leading edge of the timeout
 * @param {number} options.maxWait - The maximum time `func` is allowed to be delayed before it's invoked
 * @param {boolean} options.trailing - Specify invoking on the trailing edge of the timeout
 * @returns {Function} - Returns the new debounced function
 */
export function debounce(func, wait, options = {}) {
    let lastArgs,
        lastThis,
        result,
        timerId,
        lastCallTime = 0,
        lastInvokeTime = 0;

    const { leading = false, maxWait = 0, trailing = true } = options;

    const useMaxWait = maxWait > 0;

    // Bypass requestAnimationFrame optimizations
    const useRAF = !wait && wait !== 0 && typeof requestAnimationFrame === 'function';

    // Timer function
    function startTimer(pendingFunc, delay) {
        if (useRAF) {
            return requestAnimationFrame(pendingFunc);
        }
        return setTimeout(pendingFunc, delay);
    }

    // Cancel timer function
    function cancelTimer(id) {
        if (useRAF) {
            return cancelAnimationFrame(id);
        }
        clearTimeout(id);
    }

    // Invoke the function
    function invokeFunc(time) {
        const args = lastArgs;
        const thisArg = lastThis;

        lastArgs = lastThis = undefined;
        lastInvokeTime = time;
        result = func.apply(thisArg, args);
        return result;
    }

    // Leading edge invocation
    function leadingEdge(time) {
        lastInvokeTime = time;

        if (leading) {
            return invokeFunc(time);
        }

        // Reset timer for trailing edge
        timerId = startTimer(timerExpired, wait);
        return result;
    }

    // Calculate remaining wait time
    function remainingWait(time) {
        const timeSinceLastCall = time - lastCallTime;
        const timeSinceLastInvoke = time - lastInvokeTime;
        const timeWaiting = wait - timeSinceLastCall;

        return useMaxWait ? Math.min(timeWaiting, maxWait - timeSinceLastInvoke) : timeWaiting;
    }

    // Check if we should invoke the function
    function shouldInvoke(time) {
        const timeSinceLastCall = time - lastCallTime;
        const timeSinceLastInvoke = time - lastInvokeTime;

        // First call
        if (lastCallTime === 0) {
            return true;
        }

        // Time since last call is greater than wait time
        if (timeSinceLastCall >= wait) {
            return true;
        }

        // Time since last call is negative (system time adjusted)
        if (timeSinceLastCall < 0) {
            return true;
        }

        // Max wait time exceeded
        if (useMaxWait && timeSinceLastInvoke >= maxWait) {
            return true;
        }

        return false;
    }

    // Timer expired handler
    function timerExpired() {
        const time = Date.now();

        if (shouldInvoke(time)) {
            return trailingEdge(time);
        }

        // Restart timer
        timerId = startTimer(timerExpired, remainingWait(time));
    }

    // Trailing edge invocation
    function trailingEdge(time) {
        timerId = undefined;

        // Only invoke if we have recent arguments and trailing is enabled
        if (trailing && lastArgs) {
            return invokeFunc(time);
        }

        lastArgs = lastThis = undefined;
        return result;
    }

    // Cancel the debounced function
    function cancel() {
        if (timerId !== undefined) {
            cancelTimer(timerId);
        }

        lastInvokeTime = 0;
        lastArgs = lastCallTime = lastThis = timerId = undefined;
    }

    // Flush the debounced function
    function flush() {
        return timerId === undefined ? result : trailingEdge(Date.now());
    }

    // Check if debounced function is pending
    function pending() {
        return timerId !== undefined;
    }

    // The debounced function
    function debounced(...args) {
        const time = Date.now();
        const isInvoking = shouldInvoke(time);

        lastArgs = args;
        lastThis = this;
        lastCallTime = time;

        if (isInvoking) {
            if (timerId === undefined) {
                return leadingEdge(lastCallTime);
            }

            if (useMaxWait) {
                // Handle invocations in a tight loop
                timerId = startTimer(timerExpired, wait);
                return invokeFunc(lastCallTime);
            }
        }

        if (timerId === undefined) {
            timerId = startTimer(timerExpired, wait);
        }

        return result;
    }

    // Attach cancel, flush, and pending methods
    debounced.cancel = cancel;
    debounced.flush = flush;
    debounced.pending = pending;

    return debounced;
}

/**
 * Creates a throttled function that only invokes `func` at most once per every `wait` milliseconds.
 *
 * @param {Function} func - The function to throttle
 * @param {number} wait - The number of milliseconds to throttle invocations to
 * @param {Object} options - Options object
 * @param {boolean} options.leading - Specify invoking on the leading edge of the timeout
 * @param {boolean} options.trailing - Specify invoking on the trailing edge of the timeout
 * @returns {Function} - Returns the new throttled function
 */
export function throttle(func, wait, options = {}) {
    const { leading = true, trailing = true } = options;

    return debounce(func, wait, {
        leading,
        maxWait: wait,
        trailing,
    });
}

/**
 * Creates a debounced function that invokes `func` on the leading edge.
 *
 * @param {Function} func - The function to debounce
 * @param {number} wait - The number of milliseconds to delay
 * @returns {Function} - Returns the new debounced function
 */
export function debounceLeading(func, wait) {
    return debounce(func, wait, { leading: true, trailing: false });
}

/**
 * Creates a debounced function that invokes `func` on the trailing edge.
 *
 * @param {Function} func - The function to debounce
 * @param {number} wait - The number of milliseconds to delay
 * @returns {Function} - Returns the new debounced function
 */
export function debounceTrailing(func, wait) {
    return debounce(func, wait, { leading: false, trailing: true });
}

/**
 * Creates a debounced function that invokes `func` on both leading and trailing edges.
 *
 * @param {Function} func - The function to debounce
 * @param {number} wait - The number of milliseconds to delay
 * @returns {Function} - Returns the new debounced function
 */
export function debounceBoth(func, wait) {
    return debounce(func, wait, { leading: true, trailing: true });
}

/**
 * Creates a debounced version of a method on an object.
 *
 * @param {Object} obj - The object containing the method
 * @param {string} methodName - The name of the method to debounce
 * @param {number} wait - The number of milliseconds to delay
 * @param {Object} options - Options object
 * @returns {Object} - The original object with the debounced method
 */
export function debounceMethod(obj, methodName, wait, options = {}) {
    const originalMethod = obj[methodName];

    if (typeof originalMethod !== 'function') {
        throw new TypeError(`Property '${methodName}' is not a function`);
    }

    const debouncedFunc = debounce(originalMethod, wait, options);

    // Replace the method with the debounced version
    obj[methodName] = function (...args) {
        return debouncedFunc.apply(this, args);
    };

    // Attach control methods
    obj[methodName].cancel = debouncedFunc.cancel;
    obj[methodName].flush = debouncedFunc.flush;
    obj[methodName].pending = debouncedFunc.pending;

    return obj;
}

/**
 * Creates a throttled version of a method on an object.
 *
 * @param {Object} obj - The object containing the method
 * @param {string} methodName - The name of the method to throttle
 * @param {number} wait - The number of milliseconds to throttle invocations to
 * @param {Object} options - Options object
 * @returns {Object} - The original object with the throttled method
 */
export function throttleMethod(obj, methodName, wait, options = {}) {
    return debounceMethod(obj, methodName, wait, {
        ...options,
        maxWait: wait,
    });
}

/**
 * Debounce manager for handling multiple debounced functions.
 */
export class DebounceManager {
    constructor() {
        this.debouncedFunctions = new Map();
    }

    /**
     * Create a debounced function with a unique key.
     *
     * @param {string} key - Unique key for the debounced function
     * @param {Function} func - The function to debounce
     * @param {number} wait - The number of milliseconds to delay
     * @param {Object} options - Options object
     * @returns {Function} - The debounced function
     */
    create(key, func, wait, options = {}) {
        const debouncedFunc = debounce(func, wait, options);

        this.debouncedFunctions.set(key, {
            func: debouncedFunc,
            original: func,
            wait,
            options,
        });

        return debouncedFunc;
    }

    /**
     * Check if a debounced function exists for the given key.
     *
     * @param {string} key - The key to check
     * @returns {boolean} - True if the debounced function exists
     */
    has(key) {
        return this.debouncedFunctions.has(key);
    }

    /**
     * Get a debounced function by key.
     *
     * @param {string} key - The key of the debounced function
     * @returns {Function|null} - The debounced function or null if not found
     */
    get(key) {
        const entry = this.debouncedFunctions.get(key);
        return entry ? entry.func : null;
    }

    /**
     * Cancel a debounced function by key.
     *
     * @param {string} key - The key of the debounced function to cancel
     * @returns {boolean} - True if the function was found and cancelled
     */
    cancel(key) {
        const entry = this.debouncedFunctions.get(key);

        if (entry) {
            entry.func.cancel();
            return true;
        }

        return false;
    }

    /**
     * Flush a debounced function by key.
     *
     * @param {string} key - The key of the debounced function to flush
     * @returns {any} - The result of the function or undefined if not found
     */
    flush(key) {
        const entry = this.debouncedFunctions.get(key);

        if (entry) {
            return entry.func.flush();
        }

        return undefined;
    }

    /**
     * Check if a debounced function is pending by key.
     *
     * @param {string} key - The key of the debounced function
     * @returns {boolean} - True if the function is pending
     */
    pending(key) {
        const entry = this.debouncedFunctions.get(key);

        if (entry) {
            return entry.func.pending();
        }

        return false;
    }

    /**
     * Remove a debounced function by key.
     *
     * @param {string} key - The key of the debounced function to remove
     * @returns {boolean} - True if the function was found and removed
     */
    remove(key) {
        const entry = this.debouncedFunctions.get(key);

        if (entry) {
            entry.func.cancel();
            this.debouncedFunctions.delete(key);
            return true;
        }

        return false;
    }

    /**
     * Update the wait time for a debounced function.
     *
     * @param {string} key - The key of the debounced function
     * @param {number} wait - New wait time in milliseconds
     * @returns {boolean} - True if the function was found and updated
     */
    updateWait(key, wait) {
        const entry = this.debouncedFunctions.get(key);

        if (entry) {
            entry.func.cancel();

            const newDebouncedFunc = debounce(entry.original, wait, entry.options);

            this.debouncedFunctions.set(key, {
                ...entry,
                func: newDebouncedFunc,
                wait,
            });

            return true;
        }

        return false;
    }

    /**
     * Clear all debounced functions.
     */
    clear() {
        for (const entry of this.debouncedFunctions.values()) {
            entry.func.cancel();
        }

        this.debouncedFunctions.clear();
    }

    /**
     * Get statistics about all debounced functions.
     *
     * @returns {Object} - Statistics object
     */
    getStats() {
        const stats = {
            total: this.debouncedFunctions.size,
            byWait: {},
            byOptions: {},
        };

        for (const entry of this.debouncedFunctions.values()) {
            // Group by wait time
            const waitKey = `${entry.wait}ms`;
            stats.byWait[waitKey] = (stats.byWait[waitKey] || 0) + 1;

            // Group by options
            const optionsKey = JSON.stringify(entry.options);
            stats.byOptions[optionsKey] = (stats.byOptions[optionsKey] || 0) + 1;
        }

        return stats;
    }
}

// Default exports
export default {
    debounce,
    throttle,
    debounceLeading,
    debounceTrailing,
    debounceBoth,
    debounceMethod,
    throttleMethod,
    DebounceManager,
};
