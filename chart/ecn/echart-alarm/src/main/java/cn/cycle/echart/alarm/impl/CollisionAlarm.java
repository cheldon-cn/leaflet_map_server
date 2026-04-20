package cn.cycle.echart.alarm.impl;

import cn.cycle.echart.alarm.Alarm;
import cn.cycle.echart.alarm.AlarmLevel;
import cn.cycle.echart.alarm.AlarmType;

/**
 * 碰撞预警。
 * 
 * <p>当本船与他船距离小于安全距离时触发。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class CollisionAlarm extends Alarm {

    private final String targetMmsi;
    private final String targetName;
    private final double distanceNm;
    private final double cpaNm;
    private final double tcpaMinutes;
    private final double relativeBearing;
    private final double targetSog;
    private final double targetCog;

    public CollisionAlarm(String targetMmsi, String targetName, 
                          double distanceNm, double cpaNm, double tcpaMinutes,
                          double relativeBearing, double targetSog, double targetCog) {
        super(AlarmType.COLLISION, determineLevel(tcpaMinutes),
              "碰撞预警", buildMessage(targetName, distanceNm, cpaNm, tcpaMinutes));
        
        this.targetMmsi = targetMmsi;
        this.targetName = targetName;
        this.distanceNm = distanceNm;
        this.cpaNm = cpaNm;
        this.tcpaMinutes = tcpaMinutes;
        this.relativeBearing = relativeBearing;
        this.targetSog = targetSog;
        this.targetCog = targetCog;

        withData("targetMmsi", targetMmsi);
        withData("targetName", targetName);
        withData("distanceNm", distanceNm);
        withData("cpaNm", cpaNm);
        withData("tcpaMinutes", tcpaMinutes);
        withData("relativeBearing", relativeBearing);
        withData("targetSog", targetSog);
        withData("targetCog", targetCog);
    }

    private static AlarmLevel determineLevel(double tcpaMinutes) {
        if (tcpaMinutes < 6) {
            return AlarmLevel.CRITICAL;
        } else if (tcpaMinutes < 12) {
            return AlarmLevel.WARNING;
        } else {
            return AlarmLevel.INFO;
        }
    }

    private static String buildMessage(String targetName, double distanceNm, 
                                        double cpaNm, double tcpaMinutes) {
        return String.format("目标 %s 距离 %.2f 海里，CPA %.2f 海里，TCPA %.1f 分钟",
                targetName != null ? targetName : "未知",
                distanceNm, cpaNm, tcpaMinutes);
    }

    public String getTargetMmsi() {
        return targetMmsi;
    }

    public String getTargetName() {
        return targetName;
    }

    public double getDistanceNm() {
        return distanceNm;
    }

    public double getCpaNm() {
        return cpaNm;
    }

    public double getTcpaMinutes() {
        return tcpaMinutes;
    }

    public double getRelativeBearing() {
        return relativeBearing;
    }

    public double getTargetSog() {
        return targetSog;
    }

    public double getTargetCog() {
        return targetCog;
    }
}
