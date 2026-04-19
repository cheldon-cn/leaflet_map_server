package cn.cycle.echart.plugin;

/**
 * 插件状态枚举。
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum PluginState {

    CREATED("已创建"),
    INITIALIZING("初始化中"),
    INITIALIZED("已初始化"),
    STARTING("启动中"),
    STARTED("已启动"),
    STOPPING("停止中"),
    STOPPED("已停止"),
    DESTROYED("已销毁"),
    ERROR("错误");

    private final String displayName;

    PluginState(String displayName) {
        this.displayName = displayName;
    }

    public String getDisplayName() {
        return displayName;
    }

    public boolean isRunning() {
        return this == STARTED;
    }

    public boolean isStopped() {
        return this == STOPPED || this == DESTROYED || this == ERROR;
    }

    public boolean canStart() {
        return this == INITIALIZED || this == STOPPED;
    }

    public boolean canStop() {
        return this == STARTED;
    }
}
