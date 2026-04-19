package cn.cycle.echart.ui.render;

import cn.cycle.echart.render.Layer;
import cn.cycle.echart.render.RenderContext;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;

import java.util.ArrayList;
import java.util.List;

/**
 * 测量工具基础框架。
 * 
 * <p>提供距离、面积、方位测量功能的基础类。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class MeasurementTool extends Layer {

    public enum MeasurementMode {
        NONE,
        DISTANCE,
        AREA,
        BEARING
    }

    protected final List<Point> points;
    protected MeasurementMode mode;
    protected Color lineColor;
    protected Color pointColor;
    protected Color textColor;
    protected boolean active;
    protected double totalDistance;
    protected double totalArea;
    protected double currentBearing;

    public MeasurementTool() {
        super("measurement");
        this.points = new ArrayList<>();
        this.mode = MeasurementMode.NONE;
        this.lineColor = Color.rgb(255, 0, 0, 0.8);
        this.pointColor = Color.RED;
        this.textColor = Color.BLACK;
        this.active = false;
        this.totalDistance = 0;
        this.totalArea = 0;
        this.currentBearing = 0;
    }

    public void setMode(MeasurementMode mode) {
        this.mode = mode;
        this.points.clear();
        resetCalculations();
    }

    public MeasurementMode getMode() {
        return mode;
    }

    public void addPoint(double x, double y) {
        points.add(new Point(x, y));
        recalculate();
    }

    public void removeLastPoint() {
        if (!points.isEmpty()) {
            points.remove(points.size() - 1);
            recalculate();
        }
    }

    public void clearPoints() {
        points.clear();
        resetCalculations();
    }

    public void setActive(boolean active) {
        this.active = active;
    }

    public boolean isActive() {
        return active;
    }

    protected void resetCalculations() {
        totalDistance = 0;
        totalArea = 0;
        currentBearing = 0;
    }

    protected void recalculate() {
        resetCalculations();
        
        if (points.size() < 2) {
            return;
        }
        
        totalDistance = calculateTotalDistance();
        
        if (mode == MeasurementMode.AREA && points.size() >= 3) {
            totalArea = calculateArea();
        }
        
        if (mode == MeasurementMode.BEARING && points.size() >= 2) {
            Point p1 = points.get(points.size() - 2);
            Point p2 = points.get(points.size() - 1);
            currentBearing = calculateBearing(p1, p2);
        }
    }

    protected double calculateTotalDistance() {
        double distance = 0;
        for (int i = 1; i < points.size(); i++) {
            Point p1 = points.get(i - 1);
            Point p2 = points.get(i);
            distance += calculateDistance(p1, p2);
        }
        return distance;
    }

    protected double calculateDistance(Point p1, Point p2) {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        return Math.sqrt(dx * dx + dy * dy);
    }

    protected double calculateArea() {
        double area = 0;
        int n = points.size();
        for (int i = 0; i < n; i++) {
            Point p1 = points.get(i);
            Point p2 = points.get((i + 1) % n);
            area += p1.x * p2.y;
            area -= p2.x * p1.y;
        }
        return Math.abs(area) / 2.0;
    }

    protected double calculateBearing(Point p1, Point p2) {
        double dx = p2.x - p1.x;
        double dy = p2.y - p1.y;
        double angle = Math.toDegrees(Math.atan2(dy, dx));
        if (angle < 0) {
            angle += 360;
        }
        return angle;
    }

    @Override
    public void render(RenderContext context) {
        if (!(context instanceof FxRenderContext) || points.isEmpty()) {
            return;
        }
        
        FxRenderContext fxContext = (FxRenderContext) context;
        GraphicsContext gc = fxContext.getGraphicsContext();
        
        gc.save();
        
        drawLines(gc);
        drawPoints(gc);
        drawLabels(gc);
        
        gc.restore();
    }

    protected void drawLines(GraphicsContext gc) {
        if (points.size() < 2) {
            return;
        }
        
        gc.setStroke(lineColor);
        gc.setLineWidth(2);
        gc.beginPath();
        
        Point first = points.get(0);
        gc.moveTo(first.x, first.y);
        
        for (int i = 1; i < points.size(); i++) {
            Point p = points.get(i);
            gc.lineTo(p.x, p.y);
        }
        
        if (mode == MeasurementMode.AREA && points.size() >= 3) {
            gc.closePath();
        }
        
        gc.stroke();
    }

    protected void drawPoints(GraphicsContext gc) {
        gc.setFill(pointColor);
        
        for (Point p : points) {
            gc.fillOval(p.x - 4, p.y - 4, 8, 8);
        }
    }

    protected void drawLabels(GraphicsContext gc) {
        gc.setFill(textColor);
        gc.setFont(Font.font(12));
        
        String label = "";
        switch (mode) {
            case DISTANCE:
                label = String.format("距离: %.2f nm", totalDistance);
                break;
            case AREA:
                label = String.format("面积: %.2f nm²", totalArea);
                break;
            case BEARING:
                label = String.format("方位: %.1f°", currentBearing);
                break;
            default:
                break;
        }
        
        if (!label.isEmpty() && !points.isEmpty()) {
            Point last = points.get(points.size() - 1);
            gc.fillText(label, last.x + 10, last.y - 10);
        }
    }

    @Override
    public boolean needsRender(RenderContext context) {
        return active && !points.isEmpty();
    }

    public double getTotalDistance() {
        return totalDistance;
    }

    public double getTotalArea() {
        return totalArea;
    }

    public double getCurrentBearing() {
        return currentBearing;
    }

    public List<Point> getPoints() {
        return new ArrayList<>(points);
    }

    public void setLineColor(Color color) {
        this.lineColor = color;
    }

    public void setPointColor(Color color) {
        this.pointColor = color;
    }

    public void setTextColor(Color color) {
        this.textColor = color;
    }

    public static class Point {
        public final double x;
        public final double y;

        public Point(double x, double y) {
            this.x = x;
            this.y = y;
        }
    }
}
