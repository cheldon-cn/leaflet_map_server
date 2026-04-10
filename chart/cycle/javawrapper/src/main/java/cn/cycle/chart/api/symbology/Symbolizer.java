package cn.cycle.chart.api.symbology;

import cn.cycle.chart.api.style.Color;
import cn.cycle.chart.api.style.DrawStyle;

public class Symbolizer {

    public enum Type {
        POINT,
        LINE,
        POLYGON,
        TEXT,
        RASTER
    }

    private final Type type;
    private String name;
    private DrawStyle style;
    private double minScale;
    private double maxScale;
    private String filter;
    private boolean enabled;

    public Symbolizer(Type type) {
        this.type = type;
        this.enabled = true;
        this.minScale = 0;
        this.maxScale = Double.MAX_VALUE;
    }

    public Type getType() {
        return type;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public DrawStyle getStyle() {
        return style;
    }

    public void setStyle(DrawStyle style) {
        this.style = style;
    }

    public double getMinScale() {
        return minScale;
    }

    public void setMinScale(double minScale) {
        this.minScale = minScale;
    }

    public double getMaxScale() {
        return maxScale;
    }

    public void setMaxScale(double maxScale) {
        this.maxScale = maxScale;
    }

    public String getFilter() {
        return filter;
    }

    public void setFilter(String filter) {
        this.filter = filter;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public boolean isScaleInRange(double scale) {
        return scale >= minScale && scale <= maxScale;
    }

    public static Symbolizer createPointSymbolizer() {
        Symbolizer s = new Symbolizer(Type.POINT);
        s.setName("Point");
        s.setStyle(DrawStyle.defaultPoint());
        return s;
    }

    public static Symbolizer createLineSymbolizer() {
        Symbolizer s = new Symbolizer(Type.LINE);
        s.setName("Line");
        s.setStyle(DrawStyle.defaultLine());
        return s;
    }

    public static Symbolizer createPolygonSymbolizer() {
        Symbolizer s = new Symbolizer(Type.POLYGON);
        s.setName("Polygon");
        s.setStyle(DrawStyle.defaultPolygon());
        return s;
    }

    public static Symbolizer createTextSymbolizer() {
        Symbolizer s = new Symbolizer(Type.TEXT);
        s.setName("Text");
        return s;
    }
}
