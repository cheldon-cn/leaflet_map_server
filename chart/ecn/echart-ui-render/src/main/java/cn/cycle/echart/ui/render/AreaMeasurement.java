package cn.cycle.echart.ui.render;

/**
 * 面积测量功能。
 * 
 * <p>实现多边形区域的面积测量。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AreaMeasurement extends MeasurementTool {

    public AreaMeasurement() {
        super();
        setMode(MeasurementMode.AREA);
    }

    public double getAreaInSquareNauticalMiles() {
        return getTotalArea();
    }

    public double getAreaInSquareKilometers() {
        return getTotalArea() * 3.429904;
    }

    public double getAreaInSquareMeters() {
        return getTotalArea() * 3429904;
    }

    public String getFormattedArea() {
        double nm2 = getTotalArea();
        if (nm2 < 1) {
            return String.format("%.0f m²", nm2 * 3429904);
        } else if (nm2 < 100) {
            return String.format("%.2f nm²", nm2);
        } else {
            return String.format("%.1f nm²", nm2);
        }
    }

    public double getPerimeter() {
        double perimeter = 0;
        java.util.List<Point> points = getPoints();
        
        if (points.size() < 2) {
            return 0;
        }
        
        for (int i = 0; i < points.size(); i++) {
            Point p1 = points.get(i);
            Point p2 = points.get((i + 1) % points.size());
            perimeter += calculateDistance(p1, p2);
        }
        
        return perimeter;
    }

    public double getPerimeterInNauticalMiles() {
        return getPerimeter();
    }

    public double getPerimeterInKilometers() {
        return getPerimeter() * 1.852;
    }

    public Point getCentroid() {
        java.util.List<Point> points = getPoints();
        
        if (points.isEmpty()) {
            return new Point(0, 0);
        }
        
        double cx = 0;
        double cy = 0;
        
        for (Point p : points) {
            cx += p.x;
            cy += p.y;
        }
        
        return new Point(cx / points.size(), cy / points.size());
    }

    public boolean isPointInside(double x, double y) {
        java.util.List<Point> points = getPoints();
        
        if (points.size() < 3) {
            return false;
        }
        
        int n = points.size();
        boolean inside = false;
        
        for (int i = 0, j = n - 1; i < n; j = i++) {
            Point pi = points.get(i);
            Point pj = points.get(j);
            
            if (((pi.y > y) != (pj.y > y)) &&
                    (x < (pj.x - pi.x) * (y - pi.y) / (pj.y - pi.y) + pi.x)) {
                inside = !inside;
            }
        }
        
        return inside;
    }
}
