package cn.cycle.echart.facade;

import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.route.Route;
import cn.cycle.echart.route.RouteCalculator;
import cn.cycle.echart.route.RouteChecker;
import cn.cycle.echart.route.RouteExporter;
import cn.cycle.echart.route.RouteImporter;
import cn.cycle.echart.route.RouteManager;
import cn.cycle.echart.route.Waypoint;
import cn.cycle.echart.route.WaypointManager;

import java.io.IOException;
import java.nio.file.Path;
import java.util.List;
import java.util.Objects;

/**
 * 航线业务门面。
 * 
 * <p>提供航线系统的统一访问接口。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RouteFacade {

    private final RouteManager routeManager;
    private final RouteCalculator routeCalculator;
    private final RouteChecker routeChecker;
    private final RouteExporter routeExporter;
    private final RouteImporter routeImporter;
    private final EventBus eventBus;

    public RouteFacade(RouteManager routeManager, EventBus eventBus) {
        this.routeManager = Objects.requireNonNull(routeManager, "routeManager cannot be null");
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.routeCalculator = new RouteCalculator();
        this.routeChecker = new RouteChecker();
        this.routeExporter = new RouteExporter();
        this.routeImporter = new RouteImporter();
    }

    public Route createRoute(String name) {
        return routeManager.createRoute(name);
    }

    public Route createRoute(String name, List<double[]> waypoints) {
        Route route = routeManager.createRoute(name);
        
        for (double[] wp : waypoints) {
            if (wp.length >= 2) {
                route.addWaypoint(new Waypoint(wp[0], wp[1]));
            }
        }
        
        return route;
    }

    public void deleteRoute(String routeId) {
        routeManager.deleteRoute(routeId);
    }

    public Route getRoute(String routeId) {
        return routeManager.getRoute(routeId);
    }

    public List<Route> getAllRoutes() {
        return routeManager.getAllRoutes();
    }

    public int getRouteCount() {
        return routeManager.getRouteCount();
    }

    public void setActiveRoute(String routeId) {
        routeManager.setActiveRoute(routeId);
    }

    public Route getActiveRoute() {
        return routeManager.getActiveRoute();
    }

    public void clearActiveRoute() {
        routeManager.clearActiveRoute();
    }

    public void addWaypoint(String routeId, double latitude, double longitude) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            route.addWaypoint(new Waypoint(latitude, longitude));
            routeManager.updateRoute(route);
        }
    }

    public void addWaypoint(String routeId, double latitude, double longitude, String name) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            Waypoint wp = new Waypoint(latitude, longitude);
            wp.setName(name);
            route.addWaypoint(wp);
            routeManager.updateRoute(route);
        }
    }

    public void removeWaypoint(String routeId, int index) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            route.removeWaypoint(index);
            routeManager.updateRoute(route);
        }
    }

    public void moveWaypoint(String routeId, int index, double newLatitude, double newLongitude) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            List<Waypoint> waypoints = route.getWaypoints();
            if (index >= 0 && index < waypoints.size()) {
                Waypoint wp = waypoints.get(index);
                wp.setLatitude(newLatitude);
                wp.setLongitude(newLongitude);
                routeManager.updateRoute(route);
            }
        }
    }

    public double calculateTotalDistance(String routeId) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            return routeCalculator.calculateTotalDistance(route);
        }
        return 0;
    }

    public double calculateLegDistance(String routeId, int legIndex) {
        Route route = routeManager.getRoute(routeId);
        if (route != null && legIndex >= 0 && legIndex < route.getWaypointCount()) {
            List<Waypoint> waypoints = route.getWaypoints();
            if (legIndex > 0) {
                return routeCalculator.calculateLegDistance(waypoints.get(legIndex - 1), waypoints.get(legIndex));
            }
        }
        return 0;
    }

    public double calculateBearing(String routeId, int legIndex) {
        Route route = routeManager.getRoute(routeId);
        if (route != null && legIndex >= 0 && legIndex < route.getWaypointCount()) {
            List<Waypoint> waypoints = route.getWaypoints();
            if (legIndex > 0) {
                return routeCalculator.calculateLegBearing(waypoints.get(legIndex - 1), waypoints.get(legIndex));
            }
        }
        return 0;
    }

    public List<RouteChecker.CheckResult> checkRoute(String routeId) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            return routeChecker.checkRoute(route);
        }
        return new java.util.ArrayList<>();
    }

    public void exportRoute(String routeId, Path filePath) throws IOException {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            routeExporter.exportToFile(route, filePath.toFile());
        }
    }

    public Route importRoute(Path filePath) throws IOException {
        Route route = routeImporter.importFromFile(filePath.toFile());
        if (route != null) {
            routeManager.createRoute(route.getName());
        }
        return route;
    }

    public Waypoint getNextWaypoint(String routeId, int currentIndex) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            List<Waypoint> waypoints = route.getWaypoints();
            if (currentIndex >= 0 && currentIndex < waypoints.size() - 1) {
                return waypoints.get(currentIndex + 1);
            }
        }
        return null;
    }

    public Waypoint getPreviousWaypoint(String routeId, int currentIndex) {
        Route route = routeManager.getRoute(routeId);
        if (route != null) {
            List<Waypoint> waypoints = route.getWaypoints();
            if (currentIndex > 0 && currentIndex < waypoints.size()) {
                return waypoints.get(currentIndex - 1);
            }
        }
        return null;
    }

    public int findNearestWaypointIndex(String routeId, double latitude, double longitude) {
        Route route = routeManager.getRoute(routeId);
        if (route == null) {
            return -1;
        }

        List<Waypoint> waypoints = route.getWaypoints();
        int nearestIndex = -1;
        double minDistance = Double.MAX_VALUE;

        for (int i = 0; i < waypoints.size(); i++) {
            Waypoint wp = waypoints.get(i);
            double distance = calculateDistance(latitude, longitude, 
                    wp.getLatitude(), wp.getLongitude());
            if (distance < minDistance) {
                minDistance = distance;
                nearestIndex = i;
            }
        }

        return nearestIndex;
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

    public Route copyRoute(String routeId, String newName) {
        Route original = routeManager.getRoute(routeId);
        if (original == null) {
            return null;
        }

        Route copy = routeManager.createRoute(newName);
        
        for (Waypoint wp : original.getWaypoints()) {
            Waypoint newWp = new Waypoint(wp.getLatitude(), wp.getLongitude());
            newWp.setName(wp.getName());
            newWp.setArrivalRadius(wp.getArrivalRadius());
            newWp.setTurnRadius(wp.getTurnRadius());
            copy.addWaypoint(newWp);
        }

        return copy;
    }

    public void reverseRoute(String routeId) {
        Route route = routeManager.getRoute(routeId);
        if (route == null) {
            return;
        }

        List<Waypoint> waypoints = route.getWaypoints();
        java.util.Collections.reverse(waypoints);
        
        for (int i = 0; i < waypoints.size(); i++) {
            waypoints.get(i).setSequence(i);
        }
        
        routeManager.updateRoute(route);
    }
}
