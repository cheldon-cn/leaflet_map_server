package cn.cycle.echart.core;

/**
 * 错误码枚举。
 * 
 * <p>定义系统中所有可能的错误码。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum ErrorCode {

    // 系统错误 (1xxx)
    UNKNOWN_ERROR(1000, "未知错误"),
    INTERNAL_ERROR(1001, "内部错误"),
    CONFIGURATION_ERROR(1002, "配置错误"),
    INITIALIZATION_FAILED(1003, "初始化失败"),
    RESOURCE_NOT_FOUND(1004, "资源未找到"),
    INVALID_ARGUMENT(1005, "无效参数"),
    NULL_POINTER(1006, "空指针异常"),
    ILLEGAL_STATE(1007, "非法状态"),

    // 文件操作错误 (2xxx)
    FILE_NOT_FOUND(2000, "文件未找到"),
    FILE_READ_ERROR(2001, "文件读取错误"),
    FILE_WRITE_ERROR(2002, "文件写入错误"),
    FILE_PARSE_ERROR(2003, "文件解析错误"),
    FILE_FORMAT_ERROR(2004, "文件格式错误"),
    FILE_PERMISSION_DENIED(2005, "文件权限被拒绝"),

    // 海图错误 (3xxx)
    CHART_LOAD_FAILED(3000, "海图加载失败"),
    CHART_PARSE_FAILED(3001, "海图解析失败"),
    CHART_RENDER_FAILED(3002, "海图渲染失败"),
    CHART_NOT_FOUND(3003, "海图未找到"),
    CHART_DATA_CORRUPTED(3004, "海图数据损坏"),
    CHART_UNSUPPORTED_VERSION(3005, "不支持的海图版本"),

    // 渲染错误 (4xxx)
    RENDER_CONTEXT_ERROR(4000, "渲染上下文错误"),
    RENDER_DEVICE_ERROR(4001, "渲染设备错误"),
    RENDER_ENGINE_ERROR(4002, "渲染引擎错误"),
    RENDER_OUT_OF_MEMORY(4003, "渲染内存不足"),
    RENDER_LAYER_ERROR(4004, "图层渲染错误"),

    // 数据库错误 (5xxx)
    DATABASE_CONNECTION_FAILED(5000, "数据库连接失败"),
    DATABASE_QUERY_ERROR(5001, "数据库查询错误"),
    DATABASE_INSERT_ERROR(5002, "数据库插入错误"),
    DATABASE_UPDATE_ERROR(5003, "数据库更新错误"),
    DATABASE_DELETE_ERROR(5004, "数据库删除错误"),

    // 网络错误 (6xxx)
    NETWORK_CONNECTION_FAILED(6000, "网络连接失败"),
    NETWORK_TIMEOUT(6001, "网络超时"),
    NETWORK_DATA_ERROR(6002, "网络数据错误"),

    // 预警错误 (7xxx)
    ALARM_ENGINE_ERROR(7000, "预警引擎错误"),
    ALARM_RULE_ERROR(7001, "预警规则错误"),
    ALARM_NOTIFICATION_ERROR(7002, "预警通知错误"),

    // AIS错误 (8xxx)
    AIS_DATA_PARSE_ERROR(8000, "AIS数据解析错误"),
    AIS_TARGET_NOT_FOUND(8001, "AIS目标未找到"),

    // 航线错误 (9xxx)
    ROUTE_PARSE_ERROR(9000, "航线解析错误"),
    ROUTE_VALIDATION_ERROR(9001, "航线验证错误"),
    ROUTE_CALCULATION_ERROR(9002, "航线计算错误"),

    // 插件错误 (10xxx)
    PLUGIN_LOAD_FAILED(10000, "插件加载失败"),
    PLUGIN_INIT_FAILED(10001, "插件初始化失败"),
    PLUGIN_EXECUTION_ERROR(10002, "插件执行错误");

    private final int code;
    private final String message;

    ErrorCode(int code, String message) {
        this.code = code;
        this.message = message;
    }

    /**
     * 获取错误码。
     * 
     * @return 错误码
     */
    public int getCode() {
        return code;
    }

    /**
     * 获取错误消息。
     * 
     * @return 错误消息
     */
    public String getMessage() {
        return message;
    }

    /**
     * 根据错误码查找枚举。
     * 
     * @param code 错误码
     * @return 错误码枚举，如果未找到返回UNKNOWN_ERROR
     */
    public static ErrorCode fromCode(int code) {
        for (ErrorCode ec : values()) {
            if (ec.code == code) {
                return ec;
            }
        }
        return UNKNOWN_ERROR;
    }

    @Override
    public String toString() {
        return "ErrorCode{" +
               "code=" + code +
               ", message='" + message + '\'' +
               '}';
    }
}
