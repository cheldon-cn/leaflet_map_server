package cn.cycle.echart.ais;

import cn.cycle.echart.alarm.CpaResult;

/**
 * CPA/TCPA计算器。
 * 
 * <p>计算最近会遇距离和最近会遇时间。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class CPATCPACalculator {

    private static final double EARTH_RADIUS_NM = 3440.065;

    public CPATCPACalculator() {
    }

    /**
     * 计算CPA和TCPA。
     * 
     * @param ownLat 本船纬度（度）
     * @param ownLon 本船经度（度）
     * @param ownSpeed 本船速度（节）
     * @param ownCourse 本船航向（度）
     * @param tgtLat 目标纬度（度）
     * @param tgtLon 目标经度（度）
     * @param tgtSpeed 目标速度（节）
     * @param tgtCourse 目标航向（度）
     * @return CPA计算结果
     */
    public CpaResult calculate(double ownLat, double ownLon, double ownSpeed, 
            double ownCourse, double tgtLat, double tgtLon, double tgtSpeed, double tgtCourse) {
        
        double distance = calculateDistance(ownLat, ownLon, tgtLat, tgtLon);
        double bearing = calculateBearing(ownLat, ownLon, tgtLat, tgtLon);
        
        double ownVx = ownSpeed * Math.sin(Math.toRadians(ownCourse));
        double ownVy = ownSpeed * Math.cos(Math.toRadians(ownCourse));
        double tgtVx = tgtSpeed * Math.sin(Math.toRadians(tgtCourse));
        double tgtVy = tgtSpeed * Math.cos(Math.toRadians(tgtCourse));
        
        double relVx = tgtVx - ownVx;
        double relVy = tgtVy - ownVy;
        
        double relSpeed = Math.sqrt(relVx * relVx + relVy * relVy);
        
        if (relSpeed < 0.001) {
            return new CpaResult(distance, Double.POSITIVE_INFINITY, distance, bearing, distance < 2.0);
        }
        
        double dx = distance * Math.sin(Math.toRadians(bearing));
        double dy = distance * Math.cos(Math.toRadians(bearing));
        
        double tcpa = -(dx * relVx + dy * relVy) / (relSpeed * relSpeed);
        
        double cpaX = dx + relVx * tcpa;
        double cpaY = dy + relVy * tcpa;
        double cpa = Math.sqrt(cpaX * cpaX + cpaY * cpaY);
        
        boolean dangerous = cpa < 2.0 && tcpa > 0 && tcpa < 60;
        
        return new CpaResult(cpa, tcpa * 60, distance, bearing, dangerous);
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
}
