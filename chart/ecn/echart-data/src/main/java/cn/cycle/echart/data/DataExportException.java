package cn.cycle.echart.data;

/**
 * 数据导出异常。
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DataExportException extends Exception {

    private static final long serialVersionUID = 1L;

    public DataExportException(String message) {
        super(message);
    }

    public DataExportException(String message, Throwable cause) {
        super(message, cause);
    }

    public DataExportException(Throwable cause) {
        super(cause);
    }
}
