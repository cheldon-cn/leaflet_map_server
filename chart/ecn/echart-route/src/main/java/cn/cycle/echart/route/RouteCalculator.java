package cn.cycle.echart.route;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.List;

/**
 * 航线计算器。
 * 
 * <p>计算航线相关参数。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RouteCalculator {

    private static final double EARTH_RADIUS_NM = 3440.065;

    public RouteCalculator() {
    }

    /**
     * 计算两点间距离。
     * 
     * @param lat1 纬度1
     * @param lon1 经度1
     * @param lat2 纬度2
     * @param lon2 经度2
     * @return 距离（海里）
     */
    public double calculateDistance(double lat1, double lon1, double lat2, double lon2) {
        double dLat = Math.toRadians(lat2 - lat1);
        double dLon = Math.toRadians(lon2 - lon1);
        
        double a = Math.sin(dLat / 2) * Math.sin(dLat / 2) +
                   Math.cos(Math.toRadians(lat1)) * Math.cos(Math.toRadians(lat2)) *
                   Math.sin(dLon / 2) * Math.sin(dLon / 2);
        
        double c = 2 * Math.atan2(Math.sqrt(a), Math.sqrt(1 - a));
        
        return EARTH_RADIUS_NM * c;
    }

    /**
     * 计算航段距离。
     * 
     * @param from 起点
     * @param to 终点
     * @return 距离（海里）
     */
    public double calculateLegDistance(Waypoint from, Waypoint to) {
        return calculateDistance(
                from.getLatitude(), from.getLongitude(),
                to.getLatitude(), to.getLongitude());
    }

    /**
     * 计算航线总距离。
     * 
     * @param route 航线
     * @return 总距离（海里）
     */
    public double calculateTotalDistance(Route route) {
        double total = 0;
        List<Waypoint> waypoints = route.getWaypoints();
        
        for (int i = 1; i < waypoints.size(); i++) {
            total += calculateLegDistance(waypoints.get(i - 1), waypoints.get(i));
        }
        
        return total;
    }

    /**
     * 计算方位角。
     * 
     * @param lat1 纬度1
     * @param lon1 经度1
     * @param lat2 纬度2
     * @param lon2 经度2
     * @return 方位角（度）
     */
    public double calculateBearing(double lat1, double lon1, double lat2, double lon2) {
        double dLon = Math.toRadians(lon2 - lon1);
        double lat1Rad = Math.toRadians(lat1);
        double lat2Rad = Math.toRadians(lat2);
        
        double x = Math.sin(dLon) * Math.cos(lat2Rad);
        double y = Math.cos(lat1Rad) * Math.sin(lat2Rad) -
                   Math.sin(lat1Rad) * Math.cos(lat2Rad) * Math.cos(dLon);
        
        double bearing = Math.toDegrees(Math.atan2(x, y));
        
        return (bearing + 360) % 360;
    }

    /**
     * 计算航段方位角。
     * 
     * @param from 起点
     * @param to 终点
     * @return 方位角（度）
     */
    public double calculateLegBearing(Waypoint from, Waypoint to) {
        return calculateBearing(
                from.getLatitude(), from.getLongitude(),
                to.getLatitude(), to.getLongitude());
    }

    /**
     * 计算ETA。
     * 
     * @param route 航线
     * @param speed 速度（节）
     * @param startTime 开始时间
     * @return ETA
     */
    public LocalDateTime calculateEta(Route route, double speed, LocalDateTime startTime) {
        if (speed <= 0) {
            return null;
        }
        
        double totalDistance = calculateTotalDistance(route);
        double hours = totalDistance / speed;
        
        return startTime.plusHours((long) hours).plusMinutes((long) ((hours % 1) * 60));
    }

    /**
     * 计算各航点ETA。
     * 
     * @param route 航线
     * @param speed 速度（节）
     * @param startTime 开始时间
     * @return 各航点ETA列表
     */
    public List<LocalDateTime> calculateWaypointEtas(Route route, double speed, LocalDateTime startTime) {
        List<LocalDateTime> etas = new ArrayList<>();
        List<Waypoint> waypoints = route.getWaypoints();
        
        if (waypoints.isEmpty()) {
            return etas;
        }
        
        etas.add(startTime);
        LocalDateTime currentTime = startTime;
        
        for (int i = 1; i < waypoints.size(); i++) {
            double distance = calculateLegDistance(waypoints.get(i - 1), waypoints.get(i));
            double hours = distance / speed;
            currentTime = currentTime.plusHours((long) hours).plusMinutes((long) ((hours % 1) * 60));
            etas.add(currentTime);
        }
        
        return etas;
    }

    /**
     * 计算剩余距离。
     * 
     * @param route 航线
     * @param currentLat 当前纬度
     * @param currentLon 当前经度
     * @return 剩余距离（海里）
     */
    public double calculateRemainingDistance(Route route, double currentLat, double currentLon) {
        Waypoint currentWp = route.getCurrentWaypoint();
        if (currentWp == null) {
            return 0;
        }
        
        double distance = calculateDistance(
                currentLat, currentLon,
                currentWp.getLatitude(), currentWp.getLongitude());
        
        List<Waypoint> waypoints = route.getWaypoints();
        int currentIndex = route.getCurrentWaypointIndex();
        
        for (int i = currentIndex + 1; i < waypoints.size(); i++) {
            distance += calculateLegDistance(waypoints.get(i - 1), waypoints.get(i));
        }
        
        return distance;
    }

    /**
     * 计算偏航距离。
     * 
     * @param currentLat 当前纬度
     * @param currentLon 当前经度
     * @param fromLat 航段起点纬度
     * @param fromLon 航段起点经度
     * @param toLat 航段终点纬度
     * @param toLon 航段终点经度
     * @return 偏航距离（海里）
     */
    public double calculateCrossTrackDistance(double currentLat, double currentLon,
            double fromLat, double fromLon, double toLat, double toLon) {
        
        double distance13 = calculateDistance(fromLat, fromLon, currentLat, currentLon);
        double bearing13 = Math.toRadians(calculateBearing(fromLat, fromLon, currentLat, currentLon));
        double bearing12 = Math.toRadians(calculateBearing(fromLat, fromLon, toLat, toLon));
        
        return Math.asin(Math.sin(distance13 / EARTH_RADIUS_NM) * Math.sin(bearing13 - bearing12)) * EARTH_RADIUS_NM;
    }
}
