package cn.cycle.echart.route;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Objects;
import java.util.UUID;

/**
 * 航线数据模型。
 * 
 * <p>表示一条完整的航线。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class Route {

    private final String id;
    private String name;
    private int status;
    private final List<Waypoint> waypoints;
    private int currentWaypointIndex;
    private double totalDistance;
    private LocalDateTime eta;
    private LocalDateTime createdTime;
    private LocalDateTime modifiedTime;

    public Route() {
        this.id = UUID.randomUUID().toString();
        this.name = "";
        this.status = 0;
        this.waypoints = new ArrayList<>();
        this.currentWaypointIndex = 0;
        this.totalDistance = 0;
        this.eta = null;
        this.createdTime = LocalDateTime.now();
        this.modifiedTime = LocalDateTime.now();
    }

    public Route(String name) {
        this();
        this.name = name != null ? name : "";
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name != null ? name : "";
        this.modifiedTime = LocalDateTime.now();
    }

    public int getStatus() {
        return status;
    }

    public void setStatus(int status) {
        this.status = status;
        this.modifiedTime = LocalDateTime.now();
    }

    public List<Waypoint> getWaypoints() {
        return Collections.unmodifiableList(waypoints);
    }

    public int getWaypointCount() {
        return waypoints.size();
    }

    public Waypoint getWaypoint(int index) {
        if (index >= 0 && index < waypoints.size()) {
            return waypoints.get(index);
        }
        return null;
    }

    public void addWaypoint(Waypoint waypoint) {
        Objects.requireNonNull(waypoint, "waypoint cannot be null");
        waypoint.setSequence(waypoints.size());
        waypoints.add(waypoint);
        recalculateDistance();
        this.modifiedTime = LocalDateTime.now();
    }

    public void insertWaypoint(int index, Waypoint waypoint) {
        Objects.requireNonNull(waypoint, "waypoint cannot be null");
        if (index < 0) index = 0;
        if (index > waypoints.size()) index = waypoints.size();
        
        waypoints.add(index, waypoint);
        updateSequences();
        recalculateDistance();
        this.modifiedTime = LocalDateTime.now();
    }

    public void removeWaypoint(int index) {
        if (index >= 0 && index < waypoints.size()) {
            waypoints.remove(index);
            updateSequences();
            recalculateDistance();
            this.modifiedTime = LocalDateTime.now();
        }
    }

    public void clearWaypoints() {
        waypoints.clear();
        currentWaypointIndex = 0;
        totalDistance = 0;
        this.modifiedTime = LocalDateTime.now();
    }

    public void reverse() {
        Collections.reverse(waypoints);
        updateSequences();
        currentWaypointIndex = waypoints.size() - 1 - currentWaypointIndex;
        this.modifiedTime = LocalDateTime.now();
    }

    public Waypoint getCurrentWaypoint() {
        if (currentWaypointIndex >= 0 && currentWaypointIndex < waypoints.size()) {
            return waypoints.get(currentWaypointIndex);
        }
        return null;
    }

    public int getCurrentWaypointIndex() {
        return currentWaypointIndex;
    }

    public boolean advanceToNextWaypoint() {
        if (currentWaypointIndex < waypoints.size() - 1) {
            waypoints.get(currentWaypointIndex).setArrival(true);
            currentWaypointIndex++;
            return true;
        }
        return false;
    }

    public void setCurrentWaypointIndex(int index) {
        if (index >= 0 && index < waypoints.size()) {
            this.currentWaypointIndex = index;
        }
    }

    public double getTotalDistance() {
        return totalDistance;
    }

    public LocalDateTime getEta() {
        return eta;
    }

    public void setEta(LocalDateTime eta) {
        this.eta = eta;
    }

    public LocalDateTime getCreatedTime() {
        return createdTime;
    }

    public LocalDateTime getModifiedTime() {
        return modifiedTime;
    }

    private void updateSequences() {
        for (int i = 0; i < waypoints.size(); i++) {
            waypoints.get(i).setSequence(i);
        }
    }

    private void recalculateDistance() {
        totalDistance = 0;
        for (int i = 1; i < waypoints.size(); i++) {
            Waypoint prev = waypoints.get(i - 1);
            Waypoint curr = waypoints.get(i);
            totalDistance += calculateDistance(
                    prev.getLatitude(), prev.getLongitude(),
                    curr.getLatitude(), curr.getLongitude());
        }
    }

    private double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        double earthRadiusNm = 3440.065;
        double dLat = Math.toRadians(lat2 - lat1);
        double dLon = Math.toRadians(lon2 - lon1);
        
        double a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
                   Math.cos(Math.toRadians(lat1)) * Math.cos(Math.toRadians(lat2)) *
                   Math.sin(dLon / 2) * Math.sin(dLon / 2);
        
        double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
        
        return earthRadiusNm * c;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Route other = (Route) obj;
        return id.equals(other.id);
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    @Override
    public String toString() {
        return String.format("Route{id='%s', name='%s', waypoints=%d, distance=%.2f nm}", 
                id, name, waypoints.size(), totalDistance);
    }
}
