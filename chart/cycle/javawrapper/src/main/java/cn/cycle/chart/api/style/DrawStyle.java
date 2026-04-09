package cn.cycle.chart.api.style;

public final class DrawStyle {

    private Color fillColor;
    private Color strokeColor;
    private double strokeWidth;
    private double opacity;

    public DrawStyle() {
        this.fillColor = new Color(200, 200, 200, 128);
        this.strokeColor = new Color(0, 0, 0, 255);
        this.strokeWidth = 1.0;
        this.opacity = 1.0;
    }

    public DrawStyle(Color fillColor, Color strokeColor, double strokeWidth) {
        this.fillColor = fillColor;
        this.strokeColor = strokeColor;
        this.strokeWidth = strokeWidth;
        this.opacity = 1.0;
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
}
