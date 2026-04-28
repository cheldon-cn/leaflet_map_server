package cn.cycle.chart.api.style;

public final class DrawStyle {

    private Color fillColor;
    private Color strokeColor;
    private double strokeWidth;
    private double opacity;
    private String lineCap;
    private String lineJoin;
    private double[] dashArray;

    public DrawStyle() {
        this.fillColor = new Color(200, 200, 200, 128);
        this.strokeColor = new Color(0, 0, 0, 255);
        this.strokeWidth = 1.0;
        this.opacity = 1.0;
        this.lineCap = "round";
        this.lineJoin = "round";
        this.dashArray = null;
    }

    public DrawStyle(Color fillColor, Color strokeColor, double strokeWidth) {
        this.fillColor = fillColor;
        this.strokeColor = strokeColor;
        this.strokeWidth = strokeWidth;
        this.opacity = 1.0;
        this.lineCap = "round";
        this.lineJoin = "round";
        this.dashArray = null;
    }

    public Color getFillColor() {
        return fillColor;
    }

    public void setFillColor(Color fillColor) {
        this.fillColor = fillColor;
    }

    public Color getStrokeColor() {
        return strokeColor;
    }

    public void setStrokeColor(Color strokeColor) {
        this.strokeColor = strokeColor;
    }

    public double getStrokeWidth() {
        return strokeWidth;
    }

    public void setStrokeWidth(double strokeWidth) {
        this.strokeWidth = strokeWidth;
    }

    public double getOpacity() {
        return opacity;
    }

    public void setOpacity(double opacity) {
        this.opacity = opacity;
    }

    public String getLineCap() {
        return lineCap;
    }

    public void setLineCap(String lineCap) {
        this.lineCap = lineCap;
    }

    public String getLineJoin() {
        return lineJoin;
    }

    public void setLineJoin(String lineJoin) {
        this.lineJoin = lineJoin;
    }

    public double[] getDashArray() {
        return dashArray;
    }

    public void setDashArray(double[] dashArray) {
        this.dashArray = dashArray;
    }

    public void setFillOpacity(double fillOpacity) {
        if (fillColor != null) {
            this.fillColor = new Color(fillColor.getRed(), fillColor.getGreen(),
                                       fillColor.getBlue(), (int)(fillOpacity * 255));
        }
    }

    public static DrawStyle defaultLine() {
        DrawStyle style = new DrawStyle();
        style.setFillOpacity(0);
        return style;
    }

    public static DrawStyle defaultPolygon() {
        DrawStyle style = new DrawStyle();
        style.setStrokeColor(new Color(0, 0, 0));
        style.setFillColor(new Color(200, 200, 200));
        return style;
    }

    public static DrawStyle defaultPoint() {
        DrawStyle style = new DrawStyle();
        style.setStrokeColor(new Color(0, 0, 0));
        style.setFillColor(new Color(255, 0, 0));
        return style;
    }
}
