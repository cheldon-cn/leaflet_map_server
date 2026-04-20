package cn.cycle.echart.alarm.impl;

import cn.cycle.echart.alarm.Alarm;
import cn.cycle.echart.alarm.AlarmLevel;
import cn.cycle.echart.alarm.AlarmType;

/**
 * 浅水预警。
 * 
 * <p>当船舶进入浅水区域时触发。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ShallowWaterAlarm extends Alarm {

    private final double currentDepthMeters;
    private final double safeDepthMeters;
    private final double ukcMeters;
    private final double minUkcRequired;
    private final double latitude;
    private final double longitude;

    public ShallowWaterAlarm(double currentDepthMeters, double safeDepthMeters,
                             double ukcMeters, double minUkcRequired,
                             double latitude, double longitude) {
        super(AlarmType.SHALLOW_WATER, determineLevel(ukcMeters, minUkcRequired),
              "浅水预警", buildMessage(currentDepthMeters, ukcMeters, minUkcRequired));
        
        this.currentDepthMeters = currentDepthMeters;
        this.safeDepthMeters = safeDepthMeters;
        this.ukcMeters = ukcMeters;
        this.minUkcRequired = minUkcRequired;
        this.latitude = latitude;
        this.longitude = longitude;

        withData("currentDepthMeters", currentDepthMeters);
        withData("safeDepthMeters", safeDepthMeters);
        withData("ukcMeters", ukcMeters);
        withData("minUkcRequired", minUkcRequired);
        withData("latitude", latitude);
        withData("longitude", longitude);
    }

    private static AlarmLevel determineLevel(double ukc, double minUkc) {
        if (ukc < 0) {
            return AlarmLevel.CRITICAL;
        } else if (ukc < minUkc * 0.5) {
            return AlarmLevel.CRITICAL;
        } else if (ukc < minUkc) {
            return AlarmLevel.WARNING;
        } else {
            return AlarmLevel.INFO;
        }
    }

    private static String buildMessage(double depth, double ukc, double minUkc) {
        return String.format("水深 %.1f 米，富余水深 %.1f 米（要求 %.1f 米）",
                depth, ukc, minUkc);
    }

    public double getCurrentDepthMeters() {
        return currentDepthMeters;
    }

    public double getSafeDepthMeters() {
        return safeDepthMeters;
    }

    public double getUkcMeters() {
        return ukcMeters;
    }

    public double getMinUkcRequired() {
        return minUkcRequired;
    }

    public double getLatitude() {
        return latitude;
    }

    public double getLongitude() {
        return longitude;
    }
}
