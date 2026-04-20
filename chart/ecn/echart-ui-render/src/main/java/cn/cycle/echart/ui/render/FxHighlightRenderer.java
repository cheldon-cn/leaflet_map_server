package cn.cycle.echart.ui.render;

import javafx.scene.canvas.GraphicsContext;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * JavaFX高亮渲染器。
 * 
 * <p>实现要素高亮显示功能，支持多种高亮模式和样式配置。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FxHighlightRenderer {

    private static final Color DEFAULT_HIGHLIGHT_COLOR = Color.rgb(255, 255, 0, 0.3);
    private static final Color DEFAULT_SELECTION_COLOR = Color.rgb(0, 120, 215, 0.5);
    private static final Color DEFAULT_HOVER_COLOR = Color.rgb(100, 149, 237, 0.3);

    private final Map<String, HighlightStyle> highlightStyles;
    private final List<String> highlightedFeatures;
    private final List<String> selectedFeatures;
    private String hoveredFeature;
    
    private HighlightMode mode;
    private boolean showLabels;
    private double labelOffset;

    public FxHighlightRenderer() {
        this.highlightStyles = new HashMap<>();
        this.highlightedFeatures = new ArrayList<>();
        this.selectedFeatures = new ArrayList<>();
        this.hoveredFeature = null;
        this.mode = HighlightMode.FILL;
        this.showLabels = true;
        this.labelOffset = 5.0;
        
        initializeDefaultStyles();
    }

    private void initializeDefaultStyles() {
        highlightStyles.put("highlight", new HighlightStyle(DEFAULT_HIGHLIGHT_COLOR, 2.0));
        highlightStyles.put("selection", new HighlightStyle(DEFAULT_SELECTION_COLOR, 3.0));
        highlightStyles.put("hover", new HighlightStyle(DEFAULT_HOVER_COLOR, 1.5));
    }

    public void render(GraphicsContext gc, List<HighlightGeometry> geometries) {
        Objects.requireNonNull(gc, "gc cannot be null");
        Objects.requireNonNull(geometries, "geometries cannot be null");
        
        for (HighlightGeometry geometry : geometries) {
            renderHighlight(gc, geometry);
        }
    }

    private void renderHighlight(GraphicsContext gc, HighlightGeometry geometry) {
        String featureId = geometry.getFeatureId();
        HighlightStyle style = determineStyle(featureId);
        
        if (style == null) {
            return;
        }
        
        gc.save();
        
        applyStyle(gc, style);
        
        switch (geometry.getType()) {
            case POINT:
                renderPointHighlight(gc, geometry, style);
                break;
            case LINE:
                renderLineHighlight(gc, geometry, style);
                break;
            case POLYGON:
                renderPolygonHighlight(gc, geometry, style);
                break;
            case MULTI_POINT:
                renderMultiPointHighlight(gc, geometry, style);
                break;
            case MULTI_LINE:
                renderMultiLineHighlight(gc, geometry, style);
                break;
            case MULTI_POLYGON:
                renderMultiPolygonHighlight(gc, geometry, style);
                break;
        }
        
        if (showLabels && geometry.getLabel() != null) {
            renderLabel(gc, geometry, style);
        }
        
        gc.restore();
    }

    private HighlightStyle determineStyle(String featureId) {
        if (selectedFeatures.contains(featureId)) {
            return highlightStyles.get("selection");
        } else if (featureId.equals(hoveredFeature)) {
            return highlightStyles.get("hover");
        } else if (highlightedFeatures.contains(featureId)) {
            return highlightStyles.get("highlight");
        }
        return null;
    }

    private void applyStyle(GraphicsContext gc, HighlightStyle style) {
        Paint fillPaint = style.getFillPaint();
        Paint strokePaint = style.getStrokePaint();
        
        if (mode == HighlightMode.FILL || mode == HighlightMode.BOTH) {
            if (fillPaint != null) {
                gc.setFill(fillPaint);
            }
        }
        
        if (mode == HighlightMode.STROKE || mode == HighlightMode.BOTH) {
            if (strokePaint != null) {
                gc.setStroke(strokePaint);
                gc.setLineWidth(style.getStrokeWidth());
            }
        }
    }

    private void renderPointHighlight(GraphicsContext gc, HighlightGeometry geometry, HighlightStyle style) {
        double[] coords = geometry.getCoordinates();
        if (coords == null || coords.length < 2) {
            return;
        }
        
        double x = coords[0];
        double y = coords[1];
        double size = style.getPointSize();
        
        if (mode == HighlightMode.FILL || mode == HighlightMode.BOTH) {
            gc.fillOval(x - size / 2, y - size / 2, size, size);
        }
        
        if (mode == HighlightMode.STROKE || mode == HighlightMode.BOTH) {
            gc.strokeOval(x - size / 2, y - size / 2, size, size);
        }
    }

    private void renderLineHighlight(GraphicsContext gc, HighlightGeometry geometry, HighlightStyle style) {
        double[] coords = geometry.getCoordinates();
        if (coords == null || coords.length < 4) {
            return;
        }
        
        double[] xCoords = new double[coords.length / 2];
        double[] yCoords = new double[coords.length / 2];
        
        for (int i = 0; i < coords.length / 2; i++) {
            xCoords[i] = coords[i * 2];
            yCoords[i] = coords[i * 2 + 1];
        }
        
        gc.strokePolyline(xCoords, yCoords, xCoords.length);
    }

    private void renderPolygonHighlight(GraphicsContext gc, HighlightGeometry geometry, HighlightStyle style) {
        double[] coords = geometry.getCoordinates();
        if (coords == null || coords.length < 6) {
            return;
        }
        
        double[] xCoords = new double[coords.length / 2];
        double[] yCoords = new double[coords.length / 2];
        
        for (int i = 0; i < coords.length / 2; i++) {
            xCoords[i] = coords[i * 2];
            yCoords[i] = coords[i * 2 + 1];
        }
        
        if (mode == HighlightMode.FILL || mode == HighlightMode.BOTH) {
            gc.fillPolygon(xCoords, yCoords, xCoords.length);
        }
        
        if (mode == HighlightMode.STROKE || mode == HighlightMode.BOTH) {
            gc.strokePolygon(xCoords, yCoords, xCoords.length);
        }
    }

    private void renderMultiPointHighlight(GraphicsContext gc, HighlightGeometry geometry, HighlightStyle style) {
        for (double[] point : geometry.getMultiCoordinates()) {
            double size = style.getPointSize();
            if (mode == HighlightMode.FILL || mode == HighlightMode.BOTH) {
                gc.fillOval(point[0] - size / 2, point[1] - size / 2, size, size);
            }
            if (mode == HighlightMode.STROKE || mode == HighlightMode.BOTH) {
                gc.strokeOval(point[0] - size / 2, point[1] - size / 2, size, size);
            }
        }
    }

    private void renderMultiLineHighlight(GraphicsContext gc, HighlightGeometry geometry, HighlightStyle style) {
        for (double[] line : geometry.getMultiCoordinates()) {
            double[] xCoords = new double[line.length / 2];
            double[] yCoords = new double[line.length / 2];
            for (int i = 0; i < line.length / 2; i++) {
                xCoords[i] = line[i * 2];
                yCoords[i] = line[i * 2 + 1];
            }
            gc.strokePolyline(xCoords, yCoords, xCoords.length);
        }
    }

    private void renderMultiPolygonHighlight(GraphicsContext gc, HighlightGeometry geometry, HighlightStyle style) {
        for (double[] polygon : geometry.getMultiCoordinates()) {
            double[] xCoords = new double[polygon.length / 2];
            double[] yCoords = new double[polygon.length / 2];
            for (int i = 0; i < polygon.length / 2; i++) {
                xCoords[i] = polygon[i * 2];
                yCoords[i] = polygon[i * 2 + 1];
            }
            if (mode == HighlightMode.FILL || mode == HighlightMode.BOTH) {
                gc.fillPolygon(xCoords, yCoords, xCoords.length);
            }
            if (mode == HighlightMode.STROKE || mode == HighlightMode.BOTH) {
                gc.strokePolygon(xCoords, yCoords, xCoords.length);
            }
        }
    }

    private void renderLabel(GraphicsContext gc, HighlightGeometry geometry, HighlightStyle style) {
        double[] coords = geometry.getCoordinates();
        if (coords == null || coords.length < 2) {
            return;
        }
        
        double x = coords[0];
        double y = coords[1];
        
        gc.setFill(Color.BLACK);
        gc.setFont(javafx.scene.text.Font.font(12));
        gc.fillText(geometry.getLabel(), x + labelOffset, y - labelOffset);
    }

    public void highlightFeature(String featureId) {
        Objects.requireNonNull(featureId, "featureId cannot be null");
        if (!highlightedFeatures.contains(featureId)) {
            highlightedFeatures.add(featureId);
        }
    }

    public void unhighlightFeature(String featureId) {
        highlightedFeatures.remove(featureId);
    }

    public void clearHighlights() {
        highlightedFeatures.clear();
    }

    public void selectFeature(String featureId) {
        Objects.requireNonNull(featureId, "featureId cannot be null");
        if (!selectedFeatures.contains(featureId)) {
            selectedFeatures.add(featureId);
        }
    }

    public void deselectFeature(String featureId) {
        selectedFeatures.remove(featureId);
    }

    public void clearSelections() {
        selectedFeatures.clear();
    }

    public void setHoveredFeature(String featureId) {
        this.hoveredFeature = featureId;
    }

    public void clearHover() {
        this.hoveredFeature = null;
    }

    public void setHighlightStyle(String name, HighlightStyle style) {
        Objects.requireNonNull(name, "name cannot be null");
        Objects.requireNonNull(style, "style cannot be null");
        highlightStyles.put(name, style);
    }

    public HighlightStyle getHighlightStyle(String name) {
        return highlightStyles.get(name);
    }

    public void setMode(HighlightMode mode) {
        Objects.requireNonNull(mode, "mode cannot be null");
        this.mode = mode;
    }

    public HighlightMode getMode() {
        return mode;
    }

    public void setShowLabels(boolean showLabels) {
        this.showLabels = showLabels;
    }

    public boolean isShowLabels() {
        return showLabels;
    }

    public void setLabelOffset(double labelOffset) {
        this.labelOffset = labelOffset;
    }

    public double getLabelOffset() {
        return labelOffset;
    }

    public enum HighlightMode {
        FILL,
        STROKE,
        BOTH
    }

    public static class HighlightStyle {
        private Paint fillPaint;
        private Paint strokePaint;
        private double strokeWidth;
        private double pointSize;

        public HighlightStyle(Paint fillPaint, double strokeWidth) {
            this.fillPaint = fillPaint;
            this.strokePaint = fillPaint;
            this.strokeWidth = strokeWidth;
            this.pointSize = 10.0;
        }

        public HighlightStyle(Paint fillPaint, Paint strokePaint, double strokeWidth) {
            this.fillPaint = fillPaint;
            this.strokePaint = strokePaint;
            this.strokeWidth = strokeWidth;
            this.pointSize = 10.0;
        }

        public Paint getFillPaint() {
            return fillPaint;
        }

        public void setFillPaint(Paint fillPaint) {
            this.fillPaint = fillPaint;
        }

        public Paint getStrokePaint() {
            return strokePaint;
        }

        public void setStrokePaint(Paint strokePaint) {
            this.strokePaint = strokePaint;
        }

        public double getStrokeWidth() {
            return strokeWidth;
        }

        public void setStrokeWidth(double strokeWidth) {
            this.strokeWidth = strokeWidth;
        }

        public double getPointSize() {
            return pointSize;
        }

        public void setPointSize(double pointSize) {
            this.pointSize = pointSize;
        }
    }

    public static class HighlightGeometry {
        private final String featureId;
        private final GeometryType type;
        private final double[] coordinates;
        private final double[][] multiCoordinates;
        private final String label;

        public HighlightGeometry(String featureId, GeometryType type, double[] coordinates) {
            this(featureId, type, coordinates, null, null);
        }

        public HighlightGeometry(String featureId, GeometryType type, double[] coordinates, String label) {
            this(featureId, type, coordinates, null, label);
        }

        public HighlightGeometry(String featureId, GeometryType type, double[][] multiCoordinates) {
            this(featureId, type, null, multiCoordinates, null);
        }

        public HighlightGeometry(String featureId, GeometryType type, double[] coordinates, 
                                double[][] multiCoordinates, String label) {
            this.featureId = featureId;
            this.type = type;
            this.coordinates = coordinates;
            this.multiCoordinates = multiCoordinates;
            this.label = label;
        }

        public String getFeatureId() {
            return featureId;
        }

        public GeometryType getType() {
            return type;
        }

        public double[] getCoordinates() {
            return coordinates;
        }

        public double[][] getMultiCoordinates() {
            return multiCoordinates;
        }

        public String getLabel() {
            return label;
        }
    }

    public enum GeometryType {
        POINT,
        LINE,
        POLYGON,
        MULTI_POINT,
        MULTI_LINE,
        MULTI_POLYGON
    }
}
