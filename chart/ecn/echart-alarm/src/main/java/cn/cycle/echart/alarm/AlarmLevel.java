package cn.cycle.echart.alarm;

/**
 * 预警级别枚举。
 * 
 * <p>定义预警的严重程度级别。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum AlarmLevel {

    CRITICAL(0, "紧急", "#FF0000"),
    WARNING(1, "警告", "#FFA500"),
    INFO(2, "提示", "#0000FF"),
    DEBUG(3, "调试", "#808080");

    private final int priority;
    private final String displayName;
    private final String color;

    AlarmLevel(int priority, String displayName, String color) {
        this.priority = priority;
        this.displayName = displayName;
        this.color = color;
    }

    public int getPriority() {
        return priority;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getColor() {
        return color;
    }

    public boolean isMoreSevereThan(AlarmLevel other) {
        return this.priority < other.priority;
    }

    public static AlarmLevel fromPriority(int priority) {
        for (AlarmLevel level : values()) {
            if (level.priority == priority) {
                return level;
            }
        }
        return INFO;
    }
}
