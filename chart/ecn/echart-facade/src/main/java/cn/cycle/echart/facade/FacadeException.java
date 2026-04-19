package cn.cycle.echart.facade;

/**
 * 门面异常。
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FacadeException extends Exception {

    private static final long serialVersionUID = 1L;
    
    private final String operation;

    public FacadeException(String message) {
        super(message);
        this.operation = null;
    }

    public FacadeException(String message, Throwable cause) {
        super(message, cause);
        this.operation = null;
    }

    public FacadeException(String operation, String message) {
        super(message);
        this.operation = operation;
    }

    public FacadeException(String operation, String message, Throwable cause) {
        super(message, cause);
        this.operation = operation;
    }

    public String getOperation() {
        return operation;
    }
}
