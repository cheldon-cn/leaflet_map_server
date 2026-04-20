package cn.cycle.echart.event;

import cn.cycle.echart.core.AppEventType;

/**
 * 事件类型枚举。
 * 
 * <p><strong>已废弃</strong>：请使用 {@link cn.cycle.echart.core.AppEventType} 代替。</p>
 * 
 * <p>此类中的所有事件类型已合并到 AppEventType 中，建议直接使用 AppEventType。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 * @deprecated 使用 {@link cn.cycle.echart.core.AppEventType} 代替
 * @see cn.cycle.echart.core.AppEventType
 */
@Deprecated
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
     * @deprecated 使用 {@link AppEventType#getCode()} 代替
     */
    @Deprecated
    public String getCode() {
        return code;
    }

    /**
     * 获取事件描述。
     * 
     * @return 事件描述
     * @deprecated 使用 {@link AppEventType#getDescription()} 代替
     */
    @Deprecated
    public String getDescription() {
        return description;
    }

    /**
     * 根据代码查找事件类型。
     * 
     * @param code 事件代码
     * @return 事件类型，如果未找到返回null
     * @deprecated 使用 {@link AppEventType#fromCode(String)} 代替
     */
    @Deprecated
    public static EventType fromCode(String code) {
        for (EventType type : values()) {
            if (type.code.equals(code)) {
                return type;
            }
        }
        return null;
    }

    /**
     * 转换为 AppEventType。
     * 
     * <p>提供从废弃类型到新类型的转换。</p>
     * 
     * @return 对应的 AppEventType，如果未找到返回 null
     */
    public AppEventType toAppEventType() {
        return AppEventType.fromCode(this.code);
    }

    /**
     * 从 AppEventType 转换。
     * 
     * @param appEventType AppEventType 实例
     * @return 对应的 EventType，如果未找到返回 null
     */
    public static EventType fromAppEventType(AppEventType appEventType) {
        if (appEventType == null) {
            return null;
        }
        return fromCode(appEventType.getCode());
    }
}
