package cn.cycle.echart.core;

import java.util.Objects;
import java.util.concurrent.atomic.AtomicReference;

/**
 * 默认错误处理器实现。
 * 
 * <p>提供基本的错误处理功能，将错误输出到标准错误流。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DefaultErrorHandler implements ErrorHandler {

    private final AtomicReference<ErrorCode> lastErrorCode;
    private final AtomicReference<String> lastErrorMessage;

    /**
     * 创建默认错误处理器。
     */
    public DefaultErrorHandler() {
        this.lastErrorCode = new AtomicReference<>();
        this.lastErrorMessage = new AtomicReference<>();
    }

    @Override
    public void handleError(ErrorCode code, String message, Throwable cause) {
        Objects.requireNonNull(code, "code cannot be null");
        
        lastErrorCode.set(code);
        lastErrorMessage.set(message);
        
        StringBuilder sb = new StringBuilder();
        sb.append("[ERROR] ").append(code.getCode()).append(": ").append(code.getMessage());
        if (message != null && !message.isEmpty()) {
            sb.append(" - ").append(message);
        }
        
        System.err.println(sb.toString());
        
        if (cause != null) {
            cause.printStackTrace(System.err);
        }
    }

    @Override
    public void handleError(ErrorCode code, String message) {
        handleError(code, message, null);
    }

    @Override
    public void handleWarning(ErrorCode code, String message) {
        Objects.requireNonNull(code, "code cannot be null");
        
        StringBuilder sb = new StringBuilder();
        sb.append("[WARN] ").append(code.getCode()).append(": ").append(code.getMessage());
        if (message != null && !message.isEmpty()) {
            sb.append(" - ").append(message);
        }
        
        System.err.println(sb.toString());
    }

    @Override
    public void handleInfo(ErrorCode code, String message) {
        Objects.requireNonNull(code, "code cannot be null");
        
        StringBuilder sb = new StringBuilder();
        sb.append("[INFO] ").append(code.getCode()).append(": ").append(code.getMessage());
        if (message != null && !message.isEmpty()) {
            sb.append(" - ").append(message);
        }
        
        System.out.println(sb.toString());
    }

    @Override
    public boolean shouldContinue(ErrorCode code) {
        Objects.requireNonNull(code, "code cannot be null");
        
        switch (code) {
            case UNKNOWN_ERROR:
            case INTERNAL_ERROR:
            case INITIALIZATION_FAILED:
            case RESOURCE_NOT_FOUND:
                return false;
            default:
                return true;
        }
    }

    @Override
    public String getLastErrorMessage() {
        return lastErrorMessage.get();
    }

    @Override
    public ErrorCode getLastErrorCode() {
        return lastErrorCode.get();
    }

    @Override
    public void clearLastError() {
        lastErrorCode.set(null);
        lastErrorMessage.set(null);
    }
}
