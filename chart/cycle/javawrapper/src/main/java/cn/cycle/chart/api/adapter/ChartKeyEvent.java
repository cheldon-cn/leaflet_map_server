package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.ChartViewer;

public class ChartKeyEvent {

    public enum Type {
        PRESSED,
        RELEASED
    }

    private final Type type;
    private final String character;
    private final String text;
    private final String code;
    private final ChartViewer viewer;
    private final Object source;

    public ChartKeyEvent(Type type, String character, String text, String code,
                        ChartViewer viewer, Object source) {
        this.type = type;
        this.character = character;
        this.text = text;
        this.code = code;
        this.viewer = viewer;
        this.source = source;
    }

    public Type getType() {
        return type;
    }

    public String getCharacter() {
        return character;
    }

    public String getText() {
        return text;
    }

    public String getCode() {
        return code;
    }

    public ChartViewer getViewer() {
        return viewer;
    }

    public Object getSource() {
        return source;
    }

    public boolean isShiftDown() {
        return source instanceof javafx.scene.input.KeyEvent 
            && ((javafx.scene.input.KeyEvent) source).isShiftDown();
    }

    public boolean isControlDown() {
        return source instanceof javafx.scene.input.KeyEvent 
            && ((javafx.scene.input.KeyEvent) source).isControlDown();
    }

    public boolean isAltDown() {
        return source instanceof javafx.scene.input.KeyEvent 
            && ((javafx.scene.input.KeyEvent) source).isAltDown();
    }
}
