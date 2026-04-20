package cn.cycle.echart.alarm.impl;

import cn.cycle.echart.alarm.Alarm;
import cn.cycle.echart.alarm.AlarmLevel;
import cn.cycle.echart.alarm.AlarmType;

/**
 * 偏航预警。
 * 
 * <p>当船舶偏离计划航线时触发。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DeviationAlarm extends Alarm {

    private final String routeName;
    private final int legIndex;
    private final double deviationDistanceNm;
    private final double maxAllowedDeviationNm;
    private final double currentLatitude;
    private final double currentLongitude;
    private final double courseToWaypoint;

    public DeviationAlarm(String routeName, int legIndex, 
                          double deviationDistanceNm, double maxAllowedDeviationNm,
                          double currentLatitude, double currentLongitude,
                          double courseToWaypoint) {
        super(AlarmType.DEVIATION, determineLevel(deviationDistanceNm, maxAllowedDeviationNm),
              "偏航预警", buildMessage(routeName, deviationDistanceNm, maxAllowedDeviationNm));
        
        this.routeName = routeName;
        this.legIndex = legIndex;
        this.deviationDistanceNm = deviationDistanceNm;
        this.maxAllowedDeviationNm = maxAllowedDeviationNm;
        this.currentLatitude = currentLatitude;
        this.currentLongitude = currentLongitude;
        this.courseToWaypoint = courseToWaypoint;

        withData("routeName", routeName);
        withData("legIndex", legIndex);
        withData("deviationDistanceNm", deviationDistanceNm);
        withData("maxAllowedDeviationNm", maxAllowedDeviationNm);
        withData("currentLatitude", currentLatitude);
        withData("currentLongitude", currentLongitude);
        withData("courseToWaypoint", courseToWaypoint);
    }

    private static AlarmLevel determineLevel(double deviation, double maxAllowed) {
        double ratio = deviation / maxAllowed;
        if (ratio >= 1.5) {
            return AlarmLevel.CRITICAL;
        } else if (ratio >= 1.0) {
            return AlarmLevel.WARNING;
        } else {
            return AlarmLevel.INFO;
        }
    }

    private static String buildMessage(String routeName, double deviation, double maxAllowed) {
        return String.format("航线 %s 偏航 %.2f 海里（允许 %.2f 海里）",
                routeName != null ? routeName : "当前航线", deviation, maxAllowed);
    }

    public String getRouteName() {
        return routeName;
    }

    public int getLegIndex() {
        return legIndex;
    }

    public double getDeviationDistanceNm() {
        return deviationDistanceNm;
    }

    public double getMaxAllowedDeviationNm() {
        return maxAllowedDeviationNm;
    }

    public double getCurrentLatitude() {
        return currentLatitude;
    }

    public double getCurrentLongitude() {
        return currentLongitude;
    }

    public double getCourseToWaypoint() {
        return courseToWaypoint;
    }
}
