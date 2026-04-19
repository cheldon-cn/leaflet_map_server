package cn.cycle.echart.event;

/**
 * 事件类型枚举。
 * 
 * <p>扩展的事件类型定义，用于特定模块的事件。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum EventType {

    RENDER_FRAME_START("render.frame.start", "渲染帧开始"),
    RENDER_FRAME_END("render.frame.end", "渲染帧结束"),
    RENDER_LAYER_START("render.layer.start", "图层渲染开始"),
    RENDER_LAYER_END("render.layer.end", "图层渲染结束"),
    
    DATA_SOURCE_CONNECTED("data.source.connected", "数据源已连接"),
    DATA_SOURCE_DISCONNECTED("data.source.disconnected", "数据源已断开"),
    DATA_QUERY_START("data.query.start", "数据查询开始"),
    DATA_QUERY_END("data.query.end", "数据查询结束"),
    
    ALARM_CONFIG_CHANGED("alarm.config.changed", "预警配置变化"),
    ALARM_ZONE_UPDATED("alarm.zone.updated", "预警区域更新"),
    
    AIS_DATA_RECEIVED("ais.data.received", "AIS数据接收"),
    AIS_STATUS_CHANGED("ais.status.changed", "AIS状态变化"),
    
    ROUTE_WAYPOINT_ADDED("route.waypoint.added", "航路点添加"),
    ROUTE_WAYPOINT_REMOVED("route.waypoint.removed", "航路点移除"),
    ROUTE_CALCULATED("route.calculated", "航线计算完成");

    private final String code;
    private final String description;

    EventType(String code, String description) {
        this.code = code;
        this.description = description;
    }

    /**
     * 获取事件代码。
     * 
     * @return 事件代码
     */
    public String getCode() {
        return code;
    }

    /**
     * 获取事件描述。
     * 
     * @return 事件描述
     */
    public String getDescription() {
        return description;
    }

    /**
     * 根据代码查找事件类型。
     * 
     * @param code 事件代码
     * @return 事件类型，如果未找到返回null
     */
    public static EventType fromCode(String code) {
        for (EventType type : values()) {
            if (type.code.equals(code)) {
                return type;
            }
        }
        return null;
    }
}
