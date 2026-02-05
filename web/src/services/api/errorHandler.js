// Error handling utilities for API client

/**
 * Base API Error class
 */
export class ApiError extends Error {
    constructor(message, details = {}) {
        super(message);
        this.name = this.constructor.name;
        this.details = details;
        this.timestamp = new Date().toISOString();

        // Capture stack trace
        if (Error.captureStackTrace) {
            Error.captureStackTrace(this, this.constructor);
        }
    }
}

/**
 * Validation error - invalid request parameters
 */
export class ValidationError extends ApiError {
    constructor(message, errors = []) {
        super(message, { errors });
        this.name = 'ValidationError';
        this.statusCode = 400;
    }
}

/**
 * Request error - server responded with error status
 */
export class RequestError extends ApiError {
    constructor(message, details = {}) {
        super(message, details);
        this.name = 'RequestError';
        this.statusCode = details.status || 500;
    }
}

/**
 * Timeout error - request took too long
 */
export class TimeoutError extends ApiError {
    constructor(message, details = {}) {
        super(message, details);
        this.name = 'TimeoutError';
        this.statusCode = 408;
    }
}

/**
 * Network error - connection issues
 */
export class NetworkError extends ApiError {
    constructor(message, details = {}) {
        super(message, details);
        this.name = 'NetworkError';
        this.statusCode = 0; // Network errors have no HTTP status
    }
}

/**
 * CORS error - cross-origin request blocked
 */
export class CorsError extends ApiError {
    constructor(message, details = {}) {
        super(message, details);
        this.name = 'CorsError';
        this.statusCode = 0;
    }
}

/**
 * Error classification utility
 */
export function classifyError(error) {
    // If already an ApiError, return as-is
    if (error instanceof ApiError) {
        return error;
    }

    // Classify based on error properties
    if (error.response) {
        // Axios response error
        const { status, statusText, data } = error.response;

        let message = error.message;
        let details = { status, statusText };

        // Try to extract error details from response body
        if (data) {
            try {
                const jsonData = typeof data === 'string' ? JSON.parse(data) : data;
                if (jsonData.error?.message) {
                    message = jsonData.error.message;
                    details = { ...details, ...jsonData.error };
                }
            } catch (e) {
                // Not JSON, use raw data
                details.data = data;
            }
        }

        return new RequestError(message, details);
    }

    if (error.request) {
        // Request made but no response
        return new NetworkError('No response from server', {
            originalError: error.message,
        });
    }

    if (error.code === 'ECONNABORTED' || error.message.includes('timeout')) {
        return new TimeoutError('Request timeout', {
            originalError: error.message,
        });
    }

    if (error.message && error.message.includes('Network Error')) {
        return new NetworkError('Network error', {
            originalError: error.message,
        });
    }

    if (error.message && error.message.includes('CORS')) {
        return new CorsError('CORS error', {
            originalError: error.message,
        });
    }

    // Generic error
    return new ApiError(error.message || 'Unknown error', {
        originalError: {
            message: error.message,
            name: error.name,
            stack: error.stack,
        },
    });
}

/**
 * Error code mapping to user-friendly messages
 */
export const ERROR_MESSAGES = {
    // Client errors (4xx)
    INVALID_BBOX: {
        message: 'The requested map area is invalid.',
        suggestion: 'Please check the coordinates and try again.',
    },
    IMAGE_TOO_LARGE: {
        message: 'The requested image size is too large.',
        suggestion: 'Please reduce the width or height and try again.',
    },
    LAYER_NOT_FOUND: {
        message: 'One or more requested layers were not found.',
        suggestion: 'Please check the layer names and try again.',
    },
    INVALID_PARAMETERS: {
        message: 'The request contains invalid parameters.',
        suggestion: 'Please review your request and try again.',
    },

    // Server errors (5xx)
    RENDER_TIMEOUT: {
        message: 'The map rendering took too long.',
        suggestion: 'Please try a smaller area or lower quality settings.',
    },
    DATABASE_ERROR: {
        message: 'There was an error accessing the map data.',
        suggestion: 'Please try again later.',
    },
    PNG_ENCODE_ERROR: {
        message: 'There was an error creating the map image.',
        suggestion: 'Please try again with different settings.',
    },
    INTERNAL_SERVER_ERROR: {
        message: 'An internal server error occurred.',
        suggestion: 'Please try again later.',
    },

    // Network errors
    NETWORK_ERROR: {
        message: 'Unable to connect to the map server.',
        suggestion: 'Please check your internet connection and try again.',
    },
    CORS_ERROR: {
        message: 'Cross-origin request blocked.',
        suggestion: 'Please contact the administrator to configure CORS.',
    },
    TIMEOUT_ERROR: {
        message: 'The request timed out.',
        suggestion: 'Please try again or contact support if the issue persists.',
    },
};

/**
 * Get user-friendly error message
 */
export function getUserFriendlyError(error) {
    const apiError = classifyError(error);
    const errorCode = apiError.details?.code || apiError.name;

    const defaultMessage = {
        message: apiError.message,
        suggestion: 'Please try again later.',
    };

    return ERROR_MESSAGES[errorCode] || defaultMessage;
}

/**
 * Error reporting utility
 */
export class ErrorReporter {
    constructor(options = {}) {
        this.options = {
            enabled: true,
            endpoint: '/api/errors',
            sampleRate: 1.0,
            ...options,
        };

        this.queue = [];
        this.flushing = false;
    }

    report(error, context = {}) {
        if (!this.options.enabled || Math.random() > this.options.sampleRate) {
            return;
        }

        // Sanitize error details to avoid circular references
        let sanitizedDetails = error.details;
        if (sanitizedDetails && typeof sanitizedDetails === 'object') {
            sanitizedDetails = { ...sanitizedDetails };
            // If originalError is an object (not string), convert to serializable form
            if (
                sanitizedDetails.originalError &&
                typeof sanitizedDetails.originalError === 'object'
            ) {
                const orig = sanitizedDetails.originalError;
                sanitizedDetails.originalError = {
                    message: orig.message,
                    name: orig.name,
                    stack: orig.stack,
                };
            }
        }

        const report = {
            id: `err_${Date.now()}_${Math.random().toString(36).substr(2, 9)}`,
            timestamp: new Date().toISOString(),
            error: {
                name: error.name,
                message: error.message,
                stack: error.stack,
                details: sanitizedDetails,
            },
            context: {
                userAgent: navigator.userAgent,
                url: window.location.href,
                timestamp: Date.now(),
                ...context,
            },
        };

        this.queue.push(report);

        // Debounce flushing
        if (!this.flushing) {
            this.flushing = true;
            setTimeout(() => this.flush(), 1000);
        }
    }

    async flush() {
        if (this.queue.length === 0) {
            this.flushing = false;
            return;
        }

        const reports = [...this.queue];
        this.queue = [];

        try {
            await fetch(this.options.endpoint, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ reports }),
            });
        } catch (error) {
            // Log to console if reporting fails
            console.error('Failed to report errors:', error);

            // Re-queue failed reports (with limit)
            this.queue.unshift(...reports.slice(0, 10));
        }

        this.flushing = false;
    }
}

/**
 * Global error handler setup
 */
export function setupGlobalErrorHandling(reporter) {
    // Window error events
    window.addEventListener('error', (event) => {
        const error = event.error || new Error(event.message);
        reporter.report(error, {
            type: 'window_error',
            filename: event.filename,
            lineno: event.lineno,
            colno: event.colno,
        });
    });

    // Unhandled promise rejections
    window.addEventListener('unhandledrejection', (event) => {
        const error =
            event.reason instanceof Error ? event.reason : new Error(String(event.reason));

        reporter.report(error, {
            type: 'unhandled_rejection',
        });
    });

    // Log to console in development
    if (process.env.NODE_ENV === 'development') {
        window.addEventListener('error', (event) => {
            console.error('Global error:', event.error);
        });
    }
}

// Default exports
export default {
    ApiError,
    ValidationError,
    RequestError,
    TimeoutError,
    NetworkError,
    CorsError,
    classifyError,
    getUserFriendlyError,
    ERROR_MESSAGES,
    ErrorReporter,
    setupGlobalErrorHandling,
};
