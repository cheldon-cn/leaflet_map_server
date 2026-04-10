package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

public class NavigationEngine {

    private Route currentRoute;
    private int currentWaypointIndex;
    private Coordinate currentPosition;
    private double currentSpeed;
    private double currentHeading;
    private boolean navigating;
    private final List<NavigationListener> listeners;

    public NavigationEngine() {
        this.listeners = new CopyOnWriteArrayList<>();
        this.currentWaypointIndex = 0;
        this.navigating = false;
    }

    public void setRoute(Route route) {
        this.currentRoute = route;
        this.currentWaypointIndex = 0;
        if (route != null && route.getWaypointCount() > 0) {
            fireRouteChanged();
        }
    }

    public Route getRoute() {
        return currentRoute;
    }

    public void startNavigation() {
        if (currentRoute != null && currentRoute.getWaypointCount() > 0) {
            navigating = true;
            fireNavigationStarted();
        }
    }

    public void stopNavigation() {
        navigating = false;
        fireNavigationStopped();
    }

    public boolean isNavigating() {
        return navigating;
    }

    public void updatePosition(Coordinate position, double speed, double heading) {
        this.currentPosition = position;
        this.currentSpeed = speed;
        this.currentHeading = heading;
        
        if (navigating && currentRoute != null) {
            checkWaypointArrival();
            firePositionUpdated();
        }
    }

    private void checkWaypointArrival() {
        if (currentRoute == null || currentWaypointIndex >= currentRoute.getWaypointCount()) {
            return;
        }
        
        Waypoint current = currentRoute.getWaypoint(currentWaypointIndex);
        if (current != null && currentPosition != null) {
            double distance = currentPosition.distance(current.getPosition());
            if (distance < 0.01) {
                fireWaypointArrived(current, currentWaypointIndex);
                currentWaypointIndex++;
                if (currentWaypointIndex >= currentRoute.getWaypointCount()) {
                    fireRouteCompleted();
                    navigating = false;
                }
            }
        }
    }

    public Waypoint getCurrentWaypoint() {
        if (currentRoute != null && currentWaypointIndex < currentRoute.getWaypointCount()) {
            return currentRoute.getWaypoint(currentWaypointIndex);
        }
        return null;
    }

    public int getCurrentWaypointIndex() {
        return currentWaypointIndex;
    }

    public double getDistanceToWaypoint() {
        Waypoint wp = getCurrentWaypoint();
        if (wp != null && currentPosition != null) {
            return currentPosition.distance(wp.getPosition());
        }
        return Double.NaN;
    }

    public double getBearingToWaypoint() {
        Waypoint wp = getCurrentWaypoint();
        if (wp != null && currentPosition != null) {
            Coordinate wpPos = wp.getPosition();
            double dx = wpPos.getX() - currentPosition.getX();
            double dy = wpPos.getY() - currentPosition.getY();
            return Math.toDegrees(Math.atan2(dx, dy));
        }
        return Double.NaN;
    }

    public double getXTE() {
        return 0;
    }

    public double getTimeToWaypoint() {
        double distance = getDistanceToWaypoint();
        if (!Double.isNaN(distance) && currentSpeed > 0) {
            return distance / currentSpeed;
        }
        return Double.NaN;
    }

    public Coordinate getCurrentPosition() {
        return currentPosition;
    }

    public double getCurrentSpeed() {
        return currentSpeed;
    }

    public double getCurrentHeading() {
        return currentHeading;
    }

    public void addListener(NavigationListener listener) {
        if (listener != null) {
            listeners.add(listener);
        }
    }

    public void removeListener(NavigationListener listener) {
        listeners.remove(listener);
    }

    private void fireRouteChanged() {
        for (NavigationListener listener : listeners) {
            listener.onRouteChanged(currentRoute);
        }
    }

    private void fireNavigationStarted() {
        for (NavigationListener listener : listeners) {
            listener.onNavigationStarted();
        }
    }

    private void fireNavigationStopped() {
        for (NavigationListener listener : listeners) {
            listener.onNavigationStopped();
        }
    }

    private void firePositionUpdated() {
        for (NavigationListener listener : listeners) {
            listener.onPositionUpdated(currentPosition, currentSpeed, currentHeading);
        }
    }

    private void fireWaypointArrived(Waypoint waypoint, int index) {
        for (NavigationListener listener : listeners) {
            listener.onWaypointArrived(waypoint, index);
        }
    }

    private void fireRouteCompleted() {
        for (NavigationListener listener : listeners) {
            listener.onRouteCompleted();
        }
    }

    public interface NavigationListener {
        void onRouteChanged(Route route);
        void onNavigationStarted();
        void onNavigationStopped();
        void onPositionUpdated(Coordinate position, double speed, double heading);
        void onWaypointArrived(Waypoint waypoint, int index);
        void onRouteCompleted();
    }
}
