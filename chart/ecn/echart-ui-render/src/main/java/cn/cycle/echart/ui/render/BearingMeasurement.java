package cn.cycle.echart.ui.render;

/**
 * 方位测量功能。
 * 
 * <p>实现两点之间的方位角测量。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class BearingMeasurement extends MeasurementTool {

    public BearingMeasurement() {
        super();
        setMode(MeasurementMode.BEARING);
    }

    public double getBearingInDegrees() {
        return getCurrentBearing();
    }

    public double getBearingInRadians() {
        return Math.toRadians(getCurrentBearing());
    }

    public String getFormattedBearing() {
        double bearing = getCurrentBearing();
        String direction = getCompassDirection(bearing);
        return String.format("%.1f° (%s)", bearing, direction);
    }

    public String getCompassDirection(double bearing) {
        String[] directions = {
            "N", "NNE", "NE", "ENE",
            "E", "ESE", "SE", "SSE",
            "S", "SSW", "SW", "WSW",
            "W", "WNW", "NW", "NNW"
        };
        
        int index = (int) Math.round(bearing / 22.5) % 16;
        if (index < 0) {
            index += 16;
        }
        return directions[index];
    }

    public double getReciprocalBearing() {
        double reciprocal = getCurrentBearing() + 180;
        if (reciprocal >= 360) {
            reciprocal -= 360;
        }
        return reciprocal;
    }

    public String getFormattedReciprocalBearing() {
        double reciprocal = getReciprocalBearing();
        String direction = getCompassDirection(reciprocal);
        return String.format("%.1f° (%s)", reciprocal, direction);
    }

    public double getRelativeBearing(double heading) {
        double relative = getCurrentBearing() - heading;
        if (relative < 0) {
            relative += 360;
        }
        if (relative > 180) {
            relative -= 360;
        }
        return relative;
    }

    public boolean isAhead(double heading) {
        double relative = getRelativeBearing(heading);
        return Math.abs(relative) <= 45;
    }

    public boolean isAstern(double heading) {
        double relative = getRelativeBearing(heading);
        return Math.abs(relative) >= 135;
    }

    public boolean isPort(double heading) {
        double relative = getRelativeBearing(heading);
        return relative < -45 && relative > -135;
    }

    public boolean isStarboard(double heading) {
        double relative = getRelativeBearing(heading);
        return relative > 45 && relative < 135;
    }

    public double getQuadrant() {
        double bearing = getCurrentBearing();
        if (bearing >= 0 && bearing < 90) {
            return 1;
        } else if (bearing >= 90 && bearing < 180) {
            return 2;
        } else if (bearing >= 180 && bearing < 270) {
            return 3;
        } else {
            return 4;
        }
    }
}
