package cn.cycle.echart.core;

import java.util.EventObject;
import java.util.Objects;

/**
 * 应用事件类型枚举。
 * 
 * <p>定义系统中所有可能的事件类型。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum AppEventType {

    // 系统事件
    SYSTEM_STARTUP("system.startup", "系统启动"),
    SYSTEM_SHUTDOWN("system.shutdown", "系统关闭"),
    SYSTEM_ERROR("system.error", "系统错误"),

    // 视图事件
    VIEW_CHANGED("view.changed", "视图变化"),
    VIEW_ZOOM("view.zoom", "视图缩放"),
    VIEW_PAN("view.pan", "视图平移"),

    // 图层事件
    LAYER_ADDED("layer.added", "图层添加"),
    LAYER_REMOVED("layer.removed", "图层移除"),
    LAYER_VISIBILITY_CHANGED("layer.visibility.changed", "图层可见性变化"),
    LAYER_ORDER_CHANGED("layer.order.changed", "图层顺序变化"),

    // 海图事件
    CHART_LOADED("chart.loaded", "海图加载"),
    CHART_UNLOADED("chart.unloaded", "海图卸载"),
    CHART_UPDATED("chart.updated", "海图更新"),

    // 预警事件
    ALARM_TRIGGERED("alarm.triggered", "预警触发"),
    ALARM_ACKNOWLEDGED("alarm.acknowledged", "预警确认"),
    ALARM_CLEARED("alarm.cleared", "预警清除"),

    // AIS事件
    AIS_TARGET_UPDATED("ais.target.updated", "AIS目标更新"),
    AIS_TARGET_LOST("ais.target.lost", "AIS目标丢失"),

    // 航线事件
    ROUTE_CREATED("route.created", "航线创建"),
    ROUTE_MODIFIED("route.modified", "航线修改"),
    ROUTE_DELETED("route.deleted", "航线删除"),

    // 工作区事件
    WORKSPACE_SAVED("workspace.saved", "工作区保存"),
    WORKSPACE_LOADED("workspace.loaded", "工作区加载"),
    WORKSPACE_RESET("workspace.reset", "工作区重置"),

    // 面板事件
    PANEL_REGISTERED("panel.registered", "面板注册"),
    PANEL_UNREGISTERED("panel.unregistered", "面板注销"),
    PANEL_SHOWN("panel.shown", "面板显示"),
    PANEL_HIDDEN("panel.hidden", "面板隐藏"),

    // 用户交互事件
    USER_SELECTION_CHANGED("user.selection.changed", "用户选择变化"),
    USER_ACTION("user.action", "用户操作"),

    // 插件事件
    PLUGIN_LOADED("plugin.loaded", "插件加载"),
    PLUGIN_UNLOADED("plugin.unloaded", "插件卸载"),
    PLUGIN_ERROR("plugin.error", "插件错误"),

    // UI相关事件（设计文档7.2要求）
    SIDEBAR_PANEL_CHANGED("sidebar.panel.changed", "侧边栏面板切换"),
    RIGHT_TAB_CHANGED("right.tab.changed", "右侧标签页切换"),
    STATUS_MESSAGE("status.message", "状态栏消息"),
    ZOOM_CHANGED("zoom.changed", "缩放级别变化"),
    CENTER_CHANGED("center.changed", "中心点变化"),
    FEATURE_SELECTED("feature.selected", "要素选中"),
    FEATURE_DESELECTED("feature.deselected", "要素取消选中"),

    // 服务事件
    SERVICE_CONNECTED("service.connected", "服务连接"),
    SERVICE_DISCONNECTED("service.disconnected", "服务断开"),
    SERVICE_ERROR("service.error", "服务错误"),

    // 渲染事件（从echart-event合并）
    RENDER_FRAME_START("render.frame.start", "渲染帧开始"),
    RENDER_FRAME_END("render.frame.end", "渲染帧结束"),
    RENDER_LAYER_START("render.layer.start", "图层渲染开始"),
    RENDER_LAYER_END("render.layer.end", "图层渲染结束"),

    // 数据源事件（从echart-event合并）
    DATA_SOURCE_CONNECTED("data.source.connected", "数据源已连接"),
    DATA_SOURCE_DISCONNECTED("data.source.disconnected", "数据源已断开"),
    DATA_QUERY_START("data.query.start", "数据查询开始"),
    DATA_QUERY_END("data.query.end", "数据查询结束"),

    // 预警扩展事件（从echart-event合并）
    ALARM_CONFIG_CHANGED("alarm.config.changed", "预警配置变化"),
    ALARM_ZONE_UPDATED("alarm.zone.updated", "预警区域更新"),

    // AIS扩展事件（从echart-event合并）
    AIS_DATA_RECEIVED("ais.data.received", "AIS数据接收"),
    AIS_STATUS_CHANGED("ais.status.changed", "AIS状态变化"),

    // 航线扩展事件（从echart-event合并）
    ROUTE_WAYPOINT_ADDED("route.waypoint.added", "航路点添加"),
    ROUTE_WAYPOINT_REMOVED("route.waypoint.removed", "航路点移除"),
    ROUTE_CALCULATED("route.calculated", "航线计算完成"),

    // 航点事件
    WAYPOINT_ADDED("waypoint.added", "航点添加"),
    WAYPOINT_REMOVED("waypoint.removed", "航点移除"),
    WAYPOINT_MODIFIED("waypoint.modified", "航点修改"),

    // 日志事件
    LOG_DEBUG("log.debug", "调试日志"),
    LOG_INFO("log.info", "信息日志"),
    LOG_WARN("log.warn", "警告日志"),
    LOG_ERROR("log.error", "错误日志");

    private final String code;
    private final String description;

    AppEventType(String code, String description) {
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
    public static AppEventType fromCode(String code) {
        for (AppEventType type : values()) {
            if (type.code.equals(code)) {
                return type;
            }
        }
        return null;
    }
}
