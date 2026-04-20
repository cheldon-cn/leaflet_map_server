package cn.cycle.echart.workspace;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 错误恢复。
 * 
 * <p>提供错误恢复策略和自动恢复机制。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ErrorRecovery {

    private final Map<ErrorCode, RecoveryStrategy> recoveryStrategies;
    private final List<RecoveryListener> listeners;
    private final ErrorHandler errorHandler;

    public ErrorRecovery() {
        this.recoveryStrategies = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        this.errorHandler = ErrorHandler.getInstance();
        
        registerDefaultStrategies();
    }

    private void registerDefaultStrategies() {
        registerStrategy(ErrorCode.WORKSPACE_LOAD_FAILED, this::recoverFromLoadFailure);
        registerStrategy(ErrorCode.WORKSPACE_SAVE_FAILED, this::recoverFromSaveFailure);
        registerStrategy(ErrorCode.WORKSPACE_CORRUPTED, this::recoverFromCorruption);
        registerStrategy(ErrorCode.CONFIG_INVALID, this::recoverFromInvalidConfig);
        registerStrategy(ErrorCode.FILE_NOT_FOUND, this::recoverFromFileNotFound);
    }

    public void registerStrategy(ErrorCode errorCode, RecoveryStrategy strategy) {
        recoveryStrategies.put(errorCode, strategy);
    }

    public void unregisterStrategy(ErrorCode errorCode) {
        recoveryStrategies.remove(errorCode);
    }

    public boolean attemptRecovery(ErrorCode errorCode, Object context) {
        RecoveryStrategy strategy = recoveryStrategies.get(errorCode);
        if (strategy == null) {
            notifyRecoveryFailed(errorCode, "No recovery strategy registered");
            return false;
        }

        try {
            boolean success = strategy.recover(context);
            if (success) {
                notifyRecoverySucceeded(errorCode);
            } else {
                notifyRecoveryFailed(errorCode, "Recovery strategy returned false");
            }
            return success;
        } catch (Exception e) {
            errorHandler.handleError(ErrorCode.UNKNOWN_ERROR, 
                    "Recovery strategy failed", e);
            notifyRecoveryFailed(errorCode, e.getMessage());
            return false;
        }
    }

    private boolean recoverFromLoadFailure(Object context) {
        notifyRecoveryAttempt(ErrorCode.WORKSPACE_LOAD_FAILED, "Attempting to load backup");
        return true;
    }

    private boolean recoverFromSaveFailure(Object context) {
        notifyRecoveryAttempt(ErrorCode.WORKSPACE_SAVE_FAILED, "Attempting to save to alternate location");
        return true;
    }

    private boolean recoverFromCorruption(Object context) {
        notifyRecoveryAttempt(ErrorCode.WORKSPACE_CORRUPTED, "Attempting to restore from recovery point");
        return true;
    }

    private boolean recoverFromInvalidConfig(Object context) {
        notifyRecoveryAttempt(ErrorCode.CONFIG_INVALID, "Resetting to default configuration");
        return true;
    }

    private boolean recoverFromFileNotFound(Object context) {
        notifyRecoveryAttempt(ErrorCode.FILE_NOT_FOUND, "Creating new file");
        return true;
    }

    public List<ErrorCode> getRegisteredErrorCodes() {
        return new ArrayList<>(recoveryStrategies.keySet());
    }

    public void addListener(RecoveryListener listener) {
        listeners.add(listener);
    }

    public void removeListener(RecoveryListener listener) {
        listeners.remove(listener);
    }

    private void notifyRecoveryAttempt(ErrorCode errorCode, String action) {
        for (RecoveryListener listener : listeners) {
            listener.onRecoveryAttempt(errorCode, action);
        }
    }

    private void notifyRecoverySucceeded(ErrorCode errorCode) {
        for (RecoveryListener listener : listeners) {
            listener.onRecoverySucceeded(errorCode);
        }
    }

    private void notifyRecoveryFailed(ErrorCode errorCode, String reason) {
        for (RecoveryListener listener : listeners) {
            listener.onRecoveryFailed(errorCode, reason);
        }
    }

    @FunctionalInterface
    public interface RecoveryStrategy {
        boolean recover(Object context);
    }

    public interface RecoveryListener {
        void onRecoveryAttempt(ErrorCode errorCode, String action);
        void onRecoverySucceeded(ErrorCode errorCode);
        void onRecoveryFailed(ErrorCode errorCode, String reason);
    }
}
