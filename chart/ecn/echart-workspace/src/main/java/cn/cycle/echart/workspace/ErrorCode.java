package cn.cycle.echart.workspace;

/**
 * 错误码枚举。
 * 
 * <p>定义工作区模块中所有可能的错误码。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum ErrorCode {

    WORKSPACE_NOT_FOUND("WS001", "工作区未找到"),
    WORKSPACE_LOAD_FAILED("WS002", "工作区加载失败"),
    WORKSPACE_SAVE_FAILED("WS003", "工作区保存失败"),
    WORKSPACE_EXPORT_FAILED("WS004", "工作区导出失败"),
    WORKSPACE_IMPORT_FAILED("WS005", "工作区导入失败"),
    WORKSPACE_RECOVERY_FAILED("WS006", "工作区恢复失败"),
    WORKSPACE_CORRUPTED("WS007", "工作区数据损坏"),
    
    CONFIG_INVALID("CFG001", "配置无效"),
    CONFIG_LOAD_FAILED("CFG002", "配置加载失败"),
    CONFIG_SAVE_FAILED("CFG003", "配置保存失败"),
    
    PANEL_NOT_FOUND("PNL001", "面板未找到"),
    PANEL_LOAD_FAILED("PNL002", "面板加载失败"),
    PANEL_SAVE_FAILED("PNL003", "面板保存失败"),
    
    FILE_NOT_FOUND("FILE001", "文件未找到"),
    FILE_ACCESS_DENIED("FILE002", "文件访问被拒绝"),
    FILE_CORRUPTED("FILE003", "文件损坏"),
    FILE_FORMAT_UNSUPPORTED("FILE004", "不支持的文件格式"),
    
    UNKNOWN_ERROR("ERR999", "未知错误");

    private final String code;
    private final String message;

    ErrorCode(String code, String message) {
        this.code = code;
        this.message = message;
    }

    public String getCode() {
        return code;
    }

    public String getMessage() {
        return message;
    }

    public String getFullMessage() {
        return "[" + code + "] " + message;
    }

    public static ErrorCode fromCode(String code) {
        for (ErrorCode errorCode : values()) {
            if (errorCode.code.equals(code)) {
                return errorCode;
            }
        }
        return UNKNOWN_ERROR;
    }
}
