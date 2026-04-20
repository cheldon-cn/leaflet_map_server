package cn.cycle.echart.alarm;

/**
 * 预警状态枚举。
 * 
 * <p>定义预警的生命周期状态。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum AlarmStatus {

    ACTIVE("活动", "预警正在活动中"),
    ACKNOWLEDGED("已确认", "预警已被操作员确认"),
    CLEARED("已清除", "预警已被清除"),
    SUPPRESSED("已抑制", "预警被抑制，不显示"),
    ESCALATED("已升级", "预警已升级到更高级别");

    private final String displayName;
    private final String description;

    AlarmStatus(String displayName, String description) {
        this.displayName = displayName;
        this.description = description;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getDescription() {
        return description;
    }

    public boolean isActive() {
        return this == ACTIVE;
    }

    public boolean isAcknowledged() {
        return this == ACKNOWLEDGED;
    }

    public boolean isCleared() {
        return this == CLEARED;
    }
}
