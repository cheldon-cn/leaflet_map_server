package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

public class Route {

    private String name;
    private final List<Waypoint> waypoints;
    private double totalDistance;
    private double estimatedTime;

    public Route() {
        this.waypoints = new ArrayList<>();
        this.totalDistance = 0;
        this.estimatedTime = 0;
    }

    public Route(String name) {
        this();
        this.name = name;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void addWaypoint(Waypoint waypoint) {
        if (waypoint != null) {
            waypoints.add(waypoint);
            recalculate();
        }
    }

    public void addWaypoint(int index, Waypoint waypoint) {
        if (waypoint != null && index >= 0 && index <= waypoints.size()) {
            waypoints.add(index, waypoint);
            recalculate();
        }
    }

    public void removeWaypoint(int index) {
        if (index >= 0 && index < waypoints.size()) {
            waypoints.remove(index);
            recalculate();
        }
    }

    public Waypoint getWaypoint(int index) {
        if (index >= 0 && index < waypoints.size()) {
            return waypoints.get(index);
        }
        return null;
    }

    public List<Waypoint> getWaypoints() {
        return Collections.unmodifiableList(waypoints);
    }

    public int getWaypointCount() {
        return waypoints.size();
    }

    public void clearWaypoints() {
        waypoints.clear();
        totalDistance = 0;
        estimatedTime = 0;
    }

    private void recalculate() {
        totalDistance = 0;
        for (int i = 1; i < waypoints.size(); i++) {
            Coordinate prev = waypoints.get(i - 1).getPosition();
            Coordinate curr = waypoints.get(i).getPosition();
            if (prev != null && curr != null) {
                totalDistance += prev.distance(curr);
            }
        }
    }

    public double getTotalDistance() {
        return totalDistance;
    }

    public double getEstimatedTime() {
        return estimatedTime;
    }

    public void setEstimatedTime(double hours) {
        this.estimatedTime = hours;
    }

    public double getEstimatedTime(double speed) {
        if (speed <= 0) {
            return 0;
        }
        return totalDistance / speed;
    }

    public Coordinate getStartPosition() {
        return waypoints.isEmpty() ? null : waypoints.get(0).getPosition();
    }

    public Coordinate getEndPosition() {
        return waypoints.isEmpty() ? null : waypoints.get(waypoints.size() - 1).getPosition();
    }

    public Waypoint findNearestWaypoint(Coordinate position, double maxDistance) {
        if (position == null || waypoints.isEmpty()) {
            return null;
        }
        Waypoint nearest = null;
        double minDist = maxDistance;
        for (Waypoint wp : waypoints) {
            Coordinate wpPos = wp.getPosition();
            if (wpPos != null) {
                double dist = position.distance(wpPos);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = wp;
                }
            }
        }
        return nearest;
    }

    public Route reverse() {
        Route reversed = new Route(name + "_reversed");
        for (int i = waypoints.size() - 1; i >= 0; i--) {
            reversed.addWaypoint(waypoints.get(i));
        }
        return reversed;
    }

    @Override
    public String toString() {
        return String.format("Route[%s: %d waypoints, %.2f nm]", 
            name, waypoints.size(), totalDistance);
    }
}
