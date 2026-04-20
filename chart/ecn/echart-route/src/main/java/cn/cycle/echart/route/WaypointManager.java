package cn.cycle.echart.route;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 航点管理器。
 * 
 * <p>管理航线中的航点，包括航点的增删改查和顺序管理。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WaypointManager {

    private final EventBus eventBus;
    private final Route route;
    private final List<Waypoint> waypoints;
    private final List<WaypointListener> listeners;
    private final Map<String, Waypoint> waypointMap;

    public WaypointManager(EventBus eventBus, Route route) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.route = Objects.requireNonNull(route, "route cannot be null");
        this.waypoints = new ArrayList<>();
        this.listeners = new ArrayList<>();
        this.waypointMap = new ConcurrentHashMap<>();
    }

    public Waypoint addWaypoint(double latitude, double longitude) {
        Waypoint waypoint = new Waypoint(latitude, longitude);
        waypoint.setSequence(waypoints.size());
        waypoints.add(waypoint);
        waypointMap.put(waypoint.getId(), waypoint);
        notifyWaypointAdded(waypoint);
        return waypoint;
    }

    public Waypoint addWaypoint(double latitude, double longitude, String name) {
        Waypoint waypoint = addWaypoint(latitude, longitude);
        waypoint.setName(name);
        return waypoint;
    }

    public Waypoint insertWaypoint(int index, double latitude, double longitude) {
        if (index < 0 || index > waypoints.size()) {
            throw new IndexOutOfBoundsException("Invalid index: " + index);
        }
        
        Waypoint waypoint = new Waypoint(latitude, longitude);
        waypoints.add(index, waypoint);
        waypointMap.put(waypoint.getId(), waypoint);
        updateSequences();
        notifyWaypointAdded(waypoint);
        return waypoint;
    }

    public boolean removeWaypoint(String waypointId) {
        Waypoint waypoint = waypointMap.remove(waypointId);
        if (waypoint != null) {
            waypoints.remove(waypoint);
            updateSequences();
            notifyWaypointRemoved(waypoint);
            return true;
        }
        return false;
    }

    public boolean removeWaypoint(int index) {
        if (index < 0 || index >= waypoints.size()) {
            return false;
        }
        
        Waypoint waypoint = waypoints.remove(index);
        waypointMap.remove(waypoint.getId());
        updateSequences();
        notifyWaypointRemoved(waypoint);
        return true;
    }

    public Waypoint getWaypoint(String waypointId) {
        return waypointMap.get(waypointId);
    }

    public Waypoint getWaypoint(int index) {
        if (index < 0 || index >= waypoints.size()) {
            return null;
        }
        return waypoints.get(index);
    }

    public List<Waypoint> getAllWaypoints() {
        return new ArrayList<>(waypoints);
    }

    public int getWaypointCount() {
        return waypoints.size();
    }

    public void updateWaypoint(Waypoint waypoint) {
        Objects.requireNonNull(waypoint, "waypoint cannot be null");
        if (waypointMap.containsKey(waypoint.getId())) {
            notifyWaypointUpdated(waypoint);
        }
    }

    public void moveWaypoint(String waypointId, int newIndex) {
        Waypoint waypoint = waypointMap.get(waypointId);
        if (waypoint == null) {
            return;
        }
        
        int oldIndex = waypoints.indexOf(waypoint);
        if (oldIndex == -1 || newIndex < 0 || newIndex >= waypoints.size()) {
            return;
        }
        
        waypoints.remove(oldIndex);
        waypoints.add(newIndex, waypoint);
        updateSequences();
        notifyWaypointMoved(waypoint, oldIndex, newIndex);
    }

    public void moveWaypointUp(String waypointId) {
        Waypoint waypoint = waypointMap.get(waypointId);
        if (waypoint == null) {
            return;
        }
        
        int index = waypoints.indexOf(waypoint);
        if (index > 0) {
            moveWaypoint(waypointId, index - 1);
        }
    }

    public void moveWaypointDown(String waypointId) {
        Waypoint waypoint = waypointMap.get(waypointId);
        if (waypoint == null) {
            return;
        }
        
        int index = waypoints.indexOf(waypoint);
        if (index < waypoints.size() - 1) {
            moveWaypoint(waypointId, index + 1);
        }
    }

    public void clearAllWaypoints() {
        List<Waypoint> toRemove = new ArrayList<>(waypoints);
        waypoints.clear();
        waypointMap.clear();
        
        for (Waypoint waypoint : toRemove) {
            notifyWaypointRemoved(waypoint);
        }
    }

    public Waypoint findNearestWaypoint(double latitude, double longitude, double maxDistanceNm) {
        Waypoint nearest = null;
        double minDistance = Double.MAX_VALUE;
        
        for (Waypoint waypoint : waypoints) {
            double distance = calculateDistance(latitude, longitude, 
                    waypoint.getLatitude(), waypoint.getLongitude());
            if (distance < minDistance && distance <= maxDistanceNm) {
                minDistance = distance;
                nearest = waypoint;
            }
        }
        
        return nearest;
    }

    public int findWaypointIndex(String waypointId) {
        Waypoint waypoint = waypointMap.get(waypointId);
        return waypoint != null ? waypoints.indexOf(waypoint) : -1;
    }

    public Waypoint getNextWaypoint(String waypointId) {
        int index = findWaypointIndex(waypointId);
        if (index >= 0 && index < waypoints.size() - 1) {
            return waypoints.get(index + 1);
        }
        return null;
    }

    public Waypoint getPreviousWaypoint(String waypointId) {
        int index = findWaypointIndex(waypointId);
        if (index > 0) {
            return waypoints.get(index - 1);
        }
        return null;
    }

    public Waypoint getFirstWaypoint() {
        return waypoints.isEmpty() ? null : waypoints.get(0);
    }

    public Waypoint getLastWaypoint() {
        return waypoints.isEmpty() ? null : waypoints.get(waypoints.size() - 1);
    }

    private void updateSequences() {
        for (int i = 0; i < waypoints.size(); i++) {
            waypoints.get(i).setSequence(i);
        }
    }

    private double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        double earthRadiusNm = 3440.065;
        
        double lat1Rad = Math.toRadians(lat1);
        double lat2Rad = Math.toRadians(lat2);
        double deltaLat = Math.toRadians(lat2 - lat1);
        double deltaLon = Math.toRadians(lon2 - lon1);
        
        double a = Math.sin(deltaLat / 2) * Math.sin(deltaLat / 2) +
                   Math.cos(lat1Rad) * Math.cos(lat2Rad) *
                   Math.sin(deltaLon / 2) * Math.sin(deltaLon / 2);
        double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
        
        return earthRadiusNm * c;
    }

    public void addListener(WaypointListener listener) {
        listeners.add(listener);
    }

    public void removeListener(WaypointListener listener) {
        listeners.remove(listener);
    }

    private void notifyWaypointAdded(Waypoint waypoint) {
        eventBus.publish(new AppEvent(this, AppEventType.WAYPOINT_ADDED, waypoint));
        
        for (WaypointListener listener : listeners) {
            listener.onWaypointAdded(waypoint);
        }
    }

    private void notifyWaypointRemoved(Waypoint waypoint) {
        eventBus.publish(new AppEvent(this, AppEventType.WAYPOINT_REMOVED, waypoint));
        
        for (WaypointListener listener : listeners) {
            listener.onWaypointRemoved(waypoint);
        }
    }

    private void notifyWaypointUpdated(Waypoint waypoint) {
        eventBus.publish(new AppEvent(this, AppEventType.WAYPOINT_MODIFIED, waypoint));
        
        for (WaypointListener listener : listeners) {
            listener.onWaypointUpdated(waypoint);
        }
    }

    private void notifyWaypointMoved(Waypoint waypoint, int oldIndex, int newIndex) {
        for (WaypointListener listener : listeners) {
            listener.onWaypointMoved(waypoint, oldIndex, newIndex);
        }
    }

    public interface WaypointListener {
        void onWaypointAdded(Waypoint waypoint);
        void onWaypointRemoved(Waypoint waypoint);
        void onWaypointUpdated(Waypoint waypoint);
        void onWaypointMoved(Waypoint waypoint, int oldIndex, int newIndex);
    }
}
