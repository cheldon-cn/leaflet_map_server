package cn.cycle.echart.workspace;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 错误处理器。
 * 
 * <p>集中处理工作区模块中的所有错误。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ErrorHandler {

    private static ErrorHandler instance;
    
    private final List<ErrorRecord> errorHistory;
    private final Map<ErrorCode, ErrorStrategy> strategies;
    private final List<ErrorListener> listeners;
    private int maxHistorySize = 100;
    private boolean loggingEnabled = true;

    private ErrorHandler() {
        this.errorHistory = new CopyOnWriteArrayList<>();
        this.strategies = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        
        registerDefaultStrategies();
    }

    public static synchronized ErrorHandler getInstance() {
        if (instance == null) {
            instance = new ErrorHandler();
        }
        return instance;
    }

    private void registerDefaultStrategies() {
        strategies.put(ErrorCode.WORKSPACE_LOAD_FAILED, ErrorStrategy.RETRY);
        strategies.put(ErrorCode.WORKSPACE_SAVE_FAILED, ErrorStrategy.RETRY);
        strategies.put(ErrorCode.WORKSPACE_RECOVERY_FAILED, ErrorStrategy.FALLBACK);
        strategies.put(ErrorCode.FILE_NOT_FOUND, ErrorStrategy.NOTIFY);
        strategies.put(ErrorCode.FILE_ACCESS_DENIED, ErrorStrategy.NOTIFY);
        strategies.put(ErrorCode.UNKNOWN_ERROR, ErrorStrategy.LOG);
    }

    public void handleError(ErrorCode errorCode, String context) {
        handleError(errorCode, context, null);
    }

    public void handleError(ErrorCode errorCode, String context, Throwable cause) {
        ErrorRecord record = new ErrorRecord(
                errorCode,
                context,
                cause,
                Instant.now()
        );

        errorHistory.add(record);
        trimHistory();

        ErrorStrategy strategy = strategies.getOrDefault(errorCode, ErrorStrategy.LOG);
        executeStrategy(strategy, record);

        notifyErrorOccurred(record);
    }

    public void handleError(Exception exception) {
        ErrorCode errorCode = determineErrorCode(exception);
        handleError(errorCode, exception.getMessage(), exception);
    }

    private ErrorCode determineErrorCode(Exception exception) {
        String message = exception.getMessage();
        if (message == null) {
            message = "";
        }

        if (message.contains("not found") || message.contains("未找到")) {
            return ErrorCode.FILE_NOT_FOUND;
        }
        if (message.contains("access denied") || message.contains("访问被拒绝")) {
            return ErrorCode.FILE_ACCESS_DENIED;
        }
        if (message.contains("corrupted") || message.contains("损坏")) {
            return ErrorCode.FILE_CORRUPTED;
        }

        return ErrorCode.UNKNOWN_ERROR;
    }

    private void executeStrategy(ErrorStrategy strategy, ErrorRecord record) {
        switch (strategy) {
            case LOG:
                logError(record);
                break;
            case NOTIFY:
                logError(record);
                break;
            case RETRY:
                logError(record);
                break;
            case FALLBACK:
                logError(record);
                break;
            case IGNORE:
                break;
        }
    }

    private void logError(ErrorRecord record) {
        if (!loggingEnabled) {
            return;
        }

        System.err.println("[ERROR] " + record.getErrorCode().getFullMessage());
        System.err.println("  Context: " + record.getContext());
        System.err.println("  Time: " + record.getTimestamp());
        
        if (record.getCause() != null) {
            StringWriter sw = new StringWriter();
            record.getCause().printStackTrace(new PrintWriter(sw));
            System.err.println("  Cause: " + sw.toString());
        }
    }

    private void trimHistory() {
        while (errorHistory.size() > maxHistorySize) {
            errorHistory.remove(0);
        }
    }

    public void registerStrategy(ErrorCode errorCode, ErrorStrategy strategy) {
        strategies.put(errorCode, strategy);
    }

    public void unregisterStrategy(ErrorCode errorCode) {
        strategies.remove(errorCode);
    }

    public List<ErrorRecord> getErrorHistory() {
        return new ArrayList<>(errorHistory);
    }

    public List<ErrorRecord> getErrorsByCode(ErrorCode errorCode) {
        List<ErrorRecord> result = new ArrayList<>();
        for (ErrorRecord record : errorHistory) {
            if (record.getErrorCode() == errorCode) {
                result.add(record);
            }
        }
        return result;
    }

    public void clearHistory() {
        errorHistory.clear();
    }

    public void setMaxHistorySize(int size) {
        this.maxHistorySize = size;
        trimHistory();
    }

    public int getMaxHistorySize() {
        return maxHistorySize;
    }

    public void setLoggingEnabled(boolean enabled) {
        this.loggingEnabled = enabled;
    }

    public boolean isLoggingEnabled() {
        return loggingEnabled;
    }

    public void addListener(ErrorListener listener) {
        listeners.add(listener);
    }

    public void removeListener(ErrorListener listener) {
        listeners.remove(listener);
    }

    private void notifyErrorOccurred(ErrorRecord record) {
        for (ErrorListener listener : listeners) {
            listener.onErrorOccurred(record);
        }
    }

    public enum ErrorStrategy {
        LOG,
        NOTIFY,
        RETRY,
        FALLBACK,
        IGNORE
    }

    public static class ErrorRecord {
        private final ErrorCode errorCode;
        private final String context;
        private final Throwable cause;
        private final Instant timestamp;
        private boolean handled;

        public ErrorRecord(ErrorCode errorCode, String context, Throwable cause, Instant timestamp) {
            this.errorCode = errorCode;
            this.context = context;
            this.cause = cause;
            this.timestamp = timestamp;
            this.handled = false;
        }

        public ErrorCode getErrorCode() {
            return errorCode;
        }

        public String getContext() {
            return context;
        }

        public Throwable getCause() {
            return cause;
        }

        public Instant getTimestamp() {
            return timestamp;
        }

        public boolean isHandled() {
            return handled;
        }

        public void setHandled(boolean handled) {
            this.handled = handled;
        }
    }

    public interface ErrorListener {
        void onErrorOccurred(ErrorRecord record);
    }
}
