package cn.cycle.echart.data;

/**
 * 数据导入异常。
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DataImportException extends Exception {

    private static final long serialVersionUID = 1L;

    public DataImportException(String message) {
        super(message);
    }

    public DataImportException(String message, Throwable cause) {
        super(message, cause);
    }

    public DataImportException(Throwable cause) {
        super(cause);
    }
}
