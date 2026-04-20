package cn.cycle.echart.workspace;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Supplier;

/**
 * 错误边界。
 * 
 * <p>为关键操作提供错误隔离和恢复机制。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ErrorBoundary {

    private final ErrorHandler errorHandler;
    private final List<BoundaryListener> listeners;
    private int maxRetries = 3;
    private long retryDelayMs = 1000;

    public ErrorBoundary() {
        this.errorHandler = ErrorHandler.getInstance();
        this.listeners = new ArrayList<>();
    }

    public <T> T execute(Supplier<T> operation, T fallbackValue, String context) {
        Exception lastException = null;
        
        for (int attempt = 0; attempt <= maxRetries; attempt++) {
            try {
                T result = operation.get();
                if (attempt > 0) {
                    notifyRecovered(context, attempt);
                }
                return result;
            } catch (Exception e) {
                lastException = e;
                
                if (attempt < maxRetries) {
                    notifyRetry(context, attempt + 1);
                    try {
                        Thread.sleep(retryDelayMs);
                    } catch (InterruptedException ie) {
                        Thread.currentThread().interrupt();
                        break;
                    }
                }
            }
        }

        errorHandler.handleError(ErrorCode.UNKNOWN_ERROR, context, lastException);
        notifyFailed(context, lastException);
        
        return fallbackValue;
    }

    public void execute(Runnable operation, Runnable fallback, String context) {
        Exception lastException = null;
        
        for (int attempt = 0; attempt <= maxRetries; attempt++) {
            try {
                operation.run();
                if (attempt > 0) {
                    notifyRecovered(context, attempt);
                }
                return;
            } catch (Exception e) {
                lastException = e;
                
                if (attempt < maxRetries) {
                    notifyRetry(context, attempt + 1);
                    try {
                        Thread.sleep(retryDelayMs);
                    } catch (InterruptedException ie) {
                        Thread.currentThread().interrupt();
                        break;
                    }
                }
            }
        }

        errorHandler.handleError(ErrorCode.UNKNOWN_ERROR, context, lastException);
        notifyFailed(context, lastException);
        
        if (fallback != null) {
            try {
                fallback.run();
            } catch (Exception e) {
                errorHandler.handleError(ErrorCode.UNKNOWN_ERROR, 
                        "Fallback operation failed", e);
            }
        }
    }

    public <T> T executeWithFallback(Supplier<T> primary, Supplier<T> fallback, String context) {
        try {
            return primary.get();
        } catch (Exception e) {
            errorHandler.handleError(ErrorCode.UNKNOWN_ERROR, context, e);
            notifyFallback(context, e);
            
            try {
                return fallback.get();
            } catch (Exception fe) {
                errorHandler.handleError(ErrorCode.UNKNOWN_ERROR, 
                        "Fallback operation failed", fe);
                return null;
            }
        }
    }

    public void setMaxRetries(int maxRetries) {
        this.maxRetries = maxRetries;
    }

    public int getMaxRetries() {
        return maxRetries;
    }

    public void setRetryDelayMs(long retryDelayMs) {
        this.retryDelayMs = retryDelayMs;
    }

    public long getRetryDelayMs() {
        return retryDelayMs;
    }

    public void addListener(BoundaryListener listener) {
        listeners.add(listener);
    }

    public void removeListener(BoundaryListener listener) {
        listeners.remove(listener);
    }

    private void notifyRetry(String context, int attempt) {
        for (BoundaryListener listener : listeners) {
            listener.onRetry(context, attempt);
        }
    }

    private void notifyRecovered(String context, int attempts) {
        for (BoundaryListener listener : listeners) {
            listener.onRecovered(context, attempts);
        }
    }

    private void notifyFailed(String context, Exception e) {
        for (BoundaryListener listener : listeners) {
            listener.onFailed(context, e);
        }
    }

    private void notifyFallback(String context, Exception e) {
        for (BoundaryListener listener : listeners) {
            listener.onFallback(context, e);
        }
    }

    public interface BoundaryListener {
        void onRetry(String context, int attempt);
        void onRecovered(String context, int attempts);
        void onFailed(String context, Exception e);
        void onFallback(String context, Exception e);
    }
}
