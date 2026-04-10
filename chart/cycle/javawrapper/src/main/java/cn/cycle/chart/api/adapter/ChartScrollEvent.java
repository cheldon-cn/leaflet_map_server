package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Coordinate;

public class ChartScrollEvent {

    public enum Type {
        SCROLL
    }

    private final Type type;
    private final double screenX;
    private final double screenY;
    private final double deltaX;
    private final double deltaY;
    private final double delta;
    private final ChartViewer viewer;
    private final Object source;
    private final Coordinate worldCoordinate;

    public ChartScrollEvent(Type type, double screenX, double screenY,
                           double deltaX, double deltaY, double delta,
                           ChartViewer viewer, Object source) {
        this(type, screenX, screenY, deltaX, deltaY, delta, viewer, source, null);
    }

    public ChartScrollEvent(Type type, double screenX, double screenY,
                           double deltaX, double deltaY, double delta,
                           ChartViewer viewer, Object source, Coordinate worldCoordinate) {
        this.type = type;
        this.screenX = screenX;
        this.screenY = screenY;
        this.deltaX = deltaX;
        this.deltaY = deltaY;
        this.delta = delta;
        this.viewer = viewer;
        this.source = source;
        this.worldCoordinate = worldCoordinate;
    }

    public Type getType() {
        return type;
    }

    public double getScreenX() {
        return screenX;
    }

    public double getScreenY() {
        return screenY;
    }

    public double getDeltaX() {
        return deltaX;
    }

    public double getDeltaY() {
        return deltaY;
    }

    public double getDelta() {
        return delta;
    }

    public ChartViewer getViewer() {
        return viewer;
    }

    public Object getSource() {
        return source;
    }

    public Coordinate getWorldCoordinate() {
        return worldCoordinate;
    }

    public boolean isShiftDown() {
        return source instanceof javafx.scene.input.ScrollEvent 
            && ((javafx.scene.input.ScrollEvent) source).isShiftDown();
    }

    public boolean isControlDown() {
        return source instanceof javafx.scene.input.ScrollEvent 
            && ((javafx.scene.input.ScrollEvent) source).isControlDown();
    }

    public boolean isAltDown() {
        return source instanceof javafx.scene.input.ScrollEvent 
            && ((javafx.scene.input.ScrollEvent) source).isAltDown();
    }
}
