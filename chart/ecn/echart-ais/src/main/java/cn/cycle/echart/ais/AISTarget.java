package cn.cycle.echart.ais;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * AIS目标数据模型。
 * 
 * <p>表示一个AIS目标的基本信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AISTarget {

    private final int mmsi;
    private String name;
    private double latitude;
    private double longitude;
    private double speed;
    private double course;
    private double heading;
    private int navStatus;
    private LocalDateTime lastUpdate;

    public AISTarget(int mmsi) {
        this.mmsi = mmsi;
        this.name = "";
        this.latitude = 0;
        this.longitude = 0;
        this.speed = 0;
        this.course = 0;
        this.heading = 0;
        this.navStatus = 0;
        this.lastUpdate = LocalDateTime.now();
    }

    public int getMmsi() {
        return mmsi;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public double getLatitude() {
        return latitude;
    }

    public void setLatitude(double latitude) {
        this.latitude = latitude;
    }

    public double getLongitude() {
        return longitude;
    }

    public void setLongitude(double longitude) {
        this.longitude = longitude;
    }

    public double getSpeed() {
        return speed;
    }

    public void setSpeed(double speed) {
        this.speed = speed;
    }

    public double getCourse() {
        return course;
    }

    public void setCourse(double course) {
        this.course = course;
    }

    public double getHeading() {
        return heading;
    }

    public void setHeading(double heading) {
        this.heading = heading;
    }

    public int getNavStatus() {
        return navStatus;
    }

    public void setNavStatus(int navStatus) {
        this.navStatus = navStatus;
    }

    public LocalDateTime getLastUpdate() {
        return lastUpdate;
    }

    public void setLastUpdate(LocalDateTime lastUpdate) {
        this.lastUpdate = lastUpdate;
    }

    public void updatePosition(double lat, double lon, double speed, double course) {
        this.latitude = lat;
        this.longitude = lon;
        this.speed = speed;
        this.course = course;
        this.lastUpdate = LocalDateTime.now();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        AISTarget other = (AISTarget) obj;
        return mmsi == other.mmsi;
    }

    @Override
    public int hashCode() {
        return mmsi;
    }

    @Override
    public String toString() {
        return String.format("AISTarget{mmsi=%d, name='%s', lat=%.4f, lon=%.4f, speed=%.1f kn}", 
                mmsi, name, latitude, longitude, speed);
    }
}
