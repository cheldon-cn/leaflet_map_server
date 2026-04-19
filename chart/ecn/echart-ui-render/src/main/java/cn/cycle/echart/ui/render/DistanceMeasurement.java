package cn.cycle.echart.ui.render;

/**
 * 距离测量功能。
 * 
 * <p>实现两点或多点之间的距离测量。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DistanceMeasurement extends MeasurementTool {

    public DistanceMeasurement() {
        super();
        setMode(MeasurementMode.DISTANCE);
    }

    public double getDistanceInNauticalMiles() {
        return getTotalDistance();
    }

    public double getDistanceInKilometers() {
        return getTotalDistance() * 1.852;
    }

    public double getDistanceInMeters() {
        return getTotalDistance() * 1852;
    }

    public String getFormattedDistance() {
        double nm = getTotalDistance();
        if (nm < 1) {
            return String.format("%.0f m", nm * 1852);
        } else {
            return String.format("%.2f nm", nm);
        }
    }

    public double getSegmentDistance(int index) {
        if (index < 0 || index >= getPoints().size() - 1) {
            return 0;
        }
        
        java.util.List<Point> points = getPoints();
        Point p1 = points.get(index);
        Point p2 = points.get(index + 1);
        return calculateDistance(p1, p2);
    }

    public double getAverageSpeed(double timeInHours) {
        if (timeInHours <= 0) {
            return 0;
        }
        return getTotalDistance() / timeInHours;
    }

    public double getEstimatedTimeOfArrival(double speedInKnots) {
        if (speedInKnots <= 0) {
            return Double.POSITIVE_INFINITY;
        }
        return getTotalDistance() / speedInKnots;
    }
}
