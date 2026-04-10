package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Coordinate;

public class ChartMouseEvent {

    public enum Type {
        PRESSED,
        RELEASED,
        MOVED,
        DRAGGED,
        CLICKED
    }

    private final Type type;
    private final double screenX;
    private final double screenY;
    private final Coordinate worldCoordinate;
    private final ChartViewer viewer;
    private final Object source;

    public ChartMouseEvent(Type type, double screenX, double screenY, 
                          Coordinate worldCoordinate, ChartViewer viewer, Object source) {
        this.type = type;
        this.screenX = screenX;
        this.screenY = screenY;
        this.worldCoordinate = worldCoordinate;
        this.viewer = viewer;
        this.source = source;
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

    public Coordinate getWorldCoordinate() {
        return worldCoordinate;
    }

    public ChartViewer getViewer() {
        return viewer;
    }

    public Object getSource() {
        return source;
    }

    public boolean isPrimaryButtonDown() {
        return source instanceof javafx.scene.input.MouseEvent 
            && ((javafx.scene.input.MouseEvent) source).isPrimaryButtonDown();
    }

    public boolean isSecondaryButtonDown() {
        return source instanceof javafx.scene.input.MouseEvent 
            && ((javafx.scene.input.MouseEvent) source).isSecondaryButtonDown();
    }

    public boolean isMiddleButtonDown() {
        return source instanceof javafx.scene.input.MouseEvent 
            && ((javafx.scene.input.MouseEvent) source).isMiddleButtonDown();
    }

    public boolean isShiftDown() {
        return source instanceof javafx.scene.input.MouseEvent 
            && ((javafx.scene.input.MouseEvent) source).isShiftDown();
    }

    public boolean isControlDown() {
        return source instanceof javafx.scene.input.MouseEvent 
            && ((javafx.scene.input.MouseEvent) source).isControlDown();
    }

    public boolean isAltDown() {
        return source instanceof javafx.scene.input.MouseEvent 
            && ((javafx.scene.input.MouseEvent) source).isAltDown();
    }
}
