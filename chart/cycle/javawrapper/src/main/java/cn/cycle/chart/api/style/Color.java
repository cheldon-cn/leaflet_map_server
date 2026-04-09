package cn.cycle.chart.api.style;

public final class Color {

    private int red;
    private int green;
    private int blue;
    private int alpha;

    public Color(int red, int green, int blue) {
        this(red, green, blue, 255);
    }

    public Color(int red, int green, int blue, int alpha) {
        this.red = clamp(red);
        this.green = clamp(green);
        this.blue = clamp(blue);
        this.alpha = clamp(alpha);
    }

    public int getRed() {
        return red;
    }

    public void setRed(int red) {
        this.red = clamp(red);
    }

    public int getGreen() {
        return green;
    }

    public void setGreen(int green) {
        this.green = clamp(green);
    }

    public int getBlue() {
        return blue;
    }

    public void setBlue(int blue) {
        this.blue = clamp(blue);
    }

    public int getAlpha() {
        return alpha;
    }

    public void setAlpha(int alpha) {
        this.alpha = clamp(alpha);
    }

    public int getRGB() {
        return (alpha << 24) | (red << 16) | (green << 8) | blue;
    }

    public static Color fromRGB(int rgb) {
        int a = (rgb >> 24) & 0xFF;
        int r = (rgb >> 16) & 0xFF;
        int g = (rgb >> 8) & 0xFF;
        int b = rgb & 0xFF;
        return new Color(r, g, b, a);
    }

    public javafx.scene.paint.Color toJavaFXColor() {
        return new javafx.scene.paint.Color(
            red / 255.0, green / 255.0, blue / 255.0, alpha / 255.0);
    }

    public static Color fromJavaFXColor(javafx.scene.paint.Color fxColor) {
        return new Color(
            (int) (fxColor.getRed() * 255),
            (int) (fxColor.getGreen() * 255),
            (int) (fxColor.getBlue() * 255),
            (int) (fxColor.getOpacity() * 255));
    }

    private static int clamp(int value) {
        if (value < 0) return 0;
        if (value > 255) return 255;
        return value;
    }
}
