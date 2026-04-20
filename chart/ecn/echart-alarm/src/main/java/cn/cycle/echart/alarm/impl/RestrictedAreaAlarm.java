package cn.cycle.echart.alarm.impl;

import cn.cycle.echart.alarm.Alarm;
import cn.cycle.echart.alarm.AlarmLevel;
import cn.cycle.echart.alarm.AlarmType;

/**
 * 禁航区预警。
 * 
 * <p>当船舶进入禁航区域时触发。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RestrictedAreaAlarm extends Alarm {

    private final String areaName;
    private final String areaType;
    private final double distanceToBoundaryNm;
    private final double latitude;
    private final double longitude;
    private final boolean insideArea;

    public RestrictedAreaAlarm(String areaName, String areaType,
                               double distanceToBoundaryNm,
                               double latitude, double longitude,
                               boolean insideArea) {
        super(AlarmType.RESTRICTED_AREA, determineLevel(insideArea, distanceToBoundaryNm),
              "禁航区预警", buildMessage(areaName, areaType, insideArea, distanceToBoundaryNm));
        
        this.areaName = areaName;
        this.areaType = areaType;
        this.distanceToBoundaryNm = distanceToBoundaryNm;
        this.latitude = latitude;
        this.longitude = longitude;
        this.insideArea = insideArea;

        withData("areaName", areaName);
        withData("areaType", areaType);
        withData("distanceToBoundaryNm", distanceToBoundaryNm);
        withData("latitude", latitude);
        withData("longitude", longitude);
        withData("insideArea", insideArea);
    }

    private static AlarmLevel determineLevel(boolean inside, double distance) {
        if (inside) {
            return AlarmLevel.CRITICAL;
        } else if (distance < 0.5) {
            return AlarmLevel.WARNING;
        } else {
            return AlarmLevel.INFO;
        }
    }

    private static String buildMessage(String areaName, String areaType,
                                        boolean inside, double distance) {
        if (inside) {
            return String.format("已进入%s %s", 
                    areaType != null ? areaType : "禁航区",
                    areaName != null ? areaName : "");
        } else {
            return String.format("距离%s %s %.2f 海里",
                    areaType != null ? areaType : "禁航区",
                    areaName != null ? areaName : "",
                    distance);
        }
    }

    public String getAreaName() {
        return areaName;
    }

    public String getAreaType() {
        return areaType;
    }

    public double getDistanceToBoundaryNm() {
        return distanceToBoundaryNm;
    }

    public double getLatitude() {
        return latitude;
    }

    public double getLongitude() {
        return longitude;
    }

    public boolean isInsideArea() {
        return insideArea;
    }
}
