package cn.cycle.echart.alarm;

/**
 * 预警类型枚举。
 * 
 * <p>定义系统中所有预警类型，独立于外部API。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public enum AlarmType {

    COLLISION("碰撞预警", "本船与他船距离小于安全距离"),
    DEVIATION("偏航预警", "船舶偏离计划航线"),
    SHALLOW_WATER("浅水预警", "进入浅水区域"),
    RESTRICTED_AREA("禁航区预警", "进入禁航区域"),
    WEATHER("气象预警", "恶劣天气预报"),
    EQUIPMENT("设备故障预警", "AIS/雷达设备异常"),
    WATCH("值班报警", "操作员未在设定时间内响应"),
    TIDE("潮汐预警", "潮汐变化导致水深低于安全值"),
    AIS("AIS预警", "AIS目标相关预警"),
    NAVIGATION("导航预警", "导航相关预警"),
    SYSTEM("系统预警", "系统运行状态预警"),
    SAFETY("安全预警", "安全相关预警");

    private final String displayName;
    private final String description;

    AlarmType(String displayName, String description) {
        this.displayName = displayName;
        this.description = description;
    }

    public String getDisplayName() {
        return displayName;
    }

    public String getDescription() {
        return description;
    }
}
