package cn.cycle.echart.route;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

/**
 * 航点数据模型。
 * 
 * <p>表示航线中的一个航点。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class Waypoint {

    private final String id;
    private String name;
    private double latitude;
    private double longitude;
    private double arrivalRadius;
    private double turnRadius;
    private int type;
    private String description;
    private boolean arrival;
    private LocalDateTime eta;
    private int sequence;

    public Waypoint(double latitude, double longitude) {
        this.id = UUID.randomUUID().toString();
        this.name = "";
        this.latitude = latitude;
        this.longitude = longitude;
        this.arrivalRadius = 0.5;
        this.turnRadius = 0.5;
        this.type = 0;
        this.description = "";
        this.arrival = false;
        this.eta = null;
        this.sequence = 0;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name != null ? name : "";
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

    public double getArrivalRadius() {
        return arrivalRadius;
    }

    public void setArrivalRadius(double arrivalRadius) {
        this.arrivalRadius = arrivalRadius;
    }

    public double getTurnRadius() {
        return turnRadius;
    }

    public void setTurnRadius(double turnRadius) {
        this.turnRadius = turnRadius;
    }

    public int getType() {
        return type;
    }

    public void setType(int type) {
        this.type = type;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description != null ? description : "";
    }

    public boolean isArrival() {
        return arrival;
    }

    public void setArrival(boolean arrival) {
        this.arrival = arrival;
    }

    public LocalDateTime getEta() {
        return eta;
    }

    public void setEta(LocalDateTime eta) {
        this.eta = eta;
    }

    public int getSequence() {
        return sequence;
    }

    public void setSequence(int sequence) {
        this.sequence = sequence;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Waypoint other = (Waypoint) obj;
        return id.equals(other.id);
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    @Override
    public String toString() {
        return String.format("Waypoint{id='%s', name='%s', lat=%.4f, lon=%.4f}", 
                id, name, latitude, longitude);
    }
}
