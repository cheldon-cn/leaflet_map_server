package cn.cycle.echart.route;

import java.util.ArrayList;
import java.util.List;

/**
 * 航线检查器。
 * 
 * <p>检查航线安全性和有效性。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RouteChecker {

    private final RouteCalculator calculator;
    private double minWaypointDistance;
    private double maxWaypointDistance;
    private double minTurnRadius;

    public RouteChecker() {
        this.calculator = new RouteCalculator();
        this.minWaypointDistance = 0.1;
        this.maxWaypointDistance = 500.0;
        this.minTurnRadius = 0.1;
    }

    /**
     * 设置最小航点距离。
     * 
     * @param distance 距离（海里）
     */
    public void setMinWaypointDistance(double distance) {
        this.minWaypointDistance = distance;
    }

    /**
     * 设置最大航点距离。
     * 
     * @param distance 距离（海里）
     */
    public void setMaxWaypointDistance(double distance) {
        this.maxWaypointDistance = distance;
    }

    /**
     * 设置最小转弯半径。
     * 
     * @param radius 半径（海里）
     */
    public void setMinTurnRadius(double radius) {
        this.minTurnRadius = radius;
    }

    /**
     * 检查航线。
     * 
     * @param route 航线
     * @return 检查结果列表
     */
    public List<CheckResult> checkRoute(Route route) {
        List<CheckResult> results = new ArrayList<>();
        
        checkBasicInfo(route, results);
        checkWaypoints(route, results);
        checkDistances(route, results);
        checkTurns(route, results);
        
        return results;
    }

    private void checkBasicInfo(Route route, List<CheckResult> results) {
        if (route.getName() == null || route.getName().isEmpty()) {
            results.add(new CheckResult(CheckSeverity.WARNING, "航线名称为空", -1));
        }
        
        if (route.getWaypointCount() < 2) {
            results.add(new CheckResult(CheckSeverity.ERROR, "航线至少需要2个航点", -1));
        }
    }

    private void checkWaypoints(Route route, List<CheckResult> results) {
        List<Waypoint> waypoints = route.getWaypoints();
        
        for (int i = 0; i < waypoints.size(); i++) {
            Waypoint wp = waypoints.get(i);
            
            if (wp.getLatitude() < -90 || wp.getLatitude() > 90) {
                results.add(new CheckResult(CheckSeverity.ERROR, 
                        String.format("航点%d纬度无效: %.4f", i + 1, wp.getLatitude()), i));
            }
            
            if (wp.getLongitude() < -180 || wp.getLongitude() > 180) {
                results.add(new CheckResult(CheckSeverity.ERROR, 
                        String.format("航点%d经度无效: %.4f", i + 1, wp.getLongitude()), i));
            }
            
            if (wp.getArrivalRadius() <= 0) {
                results.add(new CheckResult(CheckSeverity.WARNING, 
                        String.format("航点%d到达半径无效: %.2f", i + 1, wp.getArrivalRadius()), i));
            }
        }
    }

    private void checkDistances(Route route, List<CheckResult> results) {
        List<Waypoint> waypoints = route.getWaypoints();
        
        for (int i = 1; i < waypoints.size(); i++) {
            Waypoint prev = waypoints.get(i - 1);
            Waypoint curr = waypoints.get(i);
            
            double distance = calculator.calculateLegDistance(prev, curr);
            
            if (distance < minWaypointDistance) {
                results.add(new CheckResult(CheckSeverity.WARNING, 
                        String.format("航段%d-%d距离过短: %.2f nm", i, i + 1, distance), i));
            }
            
            if (distance > maxWaypointDistance) {
                results.add(new CheckResult(CheckSeverity.WARNING, 
                        String.format("航段%d-%d距离过长: %.2f nm", i, i + 1, distance), i));
            }
        }
    }

    private void checkTurns(Route route, List<CheckResult> results) {
        List<Waypoint> waypoints = route.getWaypoints();
        
        for (int i = 1; i < waypoints.size() - 1; i++) {
            Waypoint prev = waypoints.get(i - 1);
            Waypoint curr = waypoints.get(i);
            Waypoint next = waypoints.get(i + 1);
            
            double bearing1 = calculator.calculateLegBearing(prev, curr);
            double bearing2 = calculator.calculateLegBearing(curr, next);
            
            double turnAngle = Math.abs(bearing2 - bearing1);
            if (turnAngle > 180) {
                turnAngle = 360 - turnAngle;
            }
            
            if (turnAngle > 90 && curr.getTurnRadius() < minTurnRadius) {
                results.add(new CheckResult(CheckSeverity.WARNING, 
                        String.format("航点%d转弯角度大(%.1f°)，建议增大转弯半径", i + 1, turnAngle), i));
            }
        }
    }

    /**
     * 检查结果。
     */
    public static class CheckResult {
        private final CheckSeverity severity;
        private final String message;
        private final int waypointIndex;

        public CheckResult(CheckSeverity severity, String message, int waypointIndex) {
            this.severity = severity;
            this.message = message;
            this.waypointIndex = waypointIndex;
        }

        public CheckSeverity getSeverity() {
            return severity;
        }

        public String getMessage() {
            return message;
        }

        public int getWaypointIndex() {
            return waypointIndex;
        }

        @Override
        public String toString() {
            return String.format("[%s] %s", severity, message);
        }
    }

    /**
     * 检查严重程度。
     */
    public enum CheckSeverity {
        INFO,
        WARNING,
        ERROR
    }
}
