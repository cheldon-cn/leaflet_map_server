package cn.cycle.echart.core;

/**
 * 错误处理器接口。
 * 
 * <p>定义错误处理的统一接口。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * public class LoggingErrorHandler implements ErrorHandler {
 *     public void handleError(ErrorCode code, String message, Throwable cause) {
 *         Logger logger = Logger.getLogger(getClass().getName());
 *         logger.log(Level.SEVERE, code + ": " + message, cause);
 *     }
 *     
 *     public void handleWarning(ErrorCode code, String message) {
 *         Logger logger = Logger.getLogger(getClass().getName());
 *         logger.log(Level.WARNING, code + ": " + message);
 *     }
 * }
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface ErrorHandler {

    /**
     * 处理错误。
     * 
     * @param code 错误码，不能为null
     * @param message 错误消息
     * @param cause 异常原因，可以为null
     * @throws IllegalArgumentException 如果code为null
     */
    void handleError(ErrorCode code, String message, Throwable cause);

    /**
     * 处理错误（无异常）。
     * 
     * @param code 错误码，不能为null
     * @param message 错误消息
     * @throws IllegalArgumentException 如果code为null
     */
    void handleError(ErrorCode code, String message);

    /**
     * 处理警告。
     * 
     * @param code 错误码，不能为null
     * @param message 警告消息
     * @throws IllegalArgumentException 如果code为null
     */
    void handleWarning(ErrorCode code, String message);

    /**
     * 处理信息。
     * 
     * @param code 错误码，不能为null
     * @param message 信息消息
     * @throws IllegalArgumentException 如果code为null
     */
    void handleInfo(ErrorCode code, String message);

    /**
     * 检查是否应该继续执行。
     * 
     * <p>当发生错误时，检查是否应该继续执行或中止。</p>
     * 
     * @param code 错误码，不能为null
     * @return 如果应该继续执行返回true
     */
    boolean shouldContinue(ErrorCode code);

    /**
     * 获取最后发生的错误。
     * 
     * @return 最后的错误消息，如果没有错误返回null
     */
    String getLastErrorMessage();

    /**
     * 获取最后的错误码。
     * 
     * @return 最后的错误码，如果没有错误返回null
     */
    ErrorCode getLastErrorCode();

    /**
     * 清除最后的错误。
     */
    void clearLastError();
}
