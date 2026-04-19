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

    // 用户交互事件
    USER_SELECTION_CHANGED("user.selection.changed", "用户选择变化"),
    USER_ACTION("user.action", "用户操作"),

    // 插件事件
    PLUGIN_LOADED("plugin.loaded", "插件加载"),
    PLUGIN_UNLOADED("plugin.unloaded", "插件卸载"),
    PLUGIN_ERROR("plugin.error", "插件错误");

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
