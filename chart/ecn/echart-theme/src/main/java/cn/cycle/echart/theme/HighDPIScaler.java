package cn.cycle.echart.theme;

import javafx.stage.Screen;

import java.util.ArrayList;
import java.util.List;

/**
 * 高DPI缩放器。
 * 
 * <p>检测和处理高DPI显示器的缩放需求。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class HighDPIScaler {

    private static final double STANDARD_DPI = 96.0;
    private static final double[] SCALE_LEVELS = {1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0};

    private double currentScale;
    private double baseDpi;
    private double actualDpi;
    private boolean autoDetect;
    private final List<ScaleChangeListener> listeners;

    public HighDPIScaler() {
        this.currentScale = 1.0;
        this.baseDpi = STANDARD_DPI;
        this.actualDpi = STANDARD_DPI;
        this.autoDetect = true;
        this.listeners = new ArrayList<>();
        
        detectDpi();
    }

    public void detectDpi() {
        try {
            Screen primaryScreen = Screen.getPrimary();
            this.actualDpi = primaryScreen.getDpi();
            
            if (autoDetect) {
                calculateScale();
            }
        } catch (Exception e) {
            this.actualDpi = STANDARD_DPI;
            this.currentScale = 1.0;
        }
    }

    private void calculateScale() {
        double rawScale = actualDpi / baseDpi;
        this.currentScale = findNearestScaleLevel(rawScale);
        notifyScaleChanged();
    }

    private double findNearestScaleLevel(double scale) {
        double nearest = SCALE_LEVELS[0];
        double minDiff = Math.abs(scale - nearest);
        
        for (double level : SCALE_LEVELS) {
            double diff = Math.abs(scale - level);
            if (diff < minDiff) {
                minDiff = diff;
                nearest = level;
            }
        }
        
        return nearest;
    }

    public double scaleValue(double value) {
        return value * currentScale;
    }

    public int scaleValue(int value) {
        return (int) Math.round(value * currentScale);
    }

    public double unscaleValue(double value) {
        return value / currentScale;
    }

    public int unscaleValue(int value) {
        return (int) Math.round(value / currentScale);
    }

    public double scaleFont(double fontSize) {
        return fontSize * currentScale;
    }

    public double unscaleFont(double fontSize) {
        return fontSize / currentScale;
    }

    public double scaleDimension(double dimension) {
        return dimension * currentScale;
    }

    public double unscaleDimension(double dimension) {
        return dimension / currentScale;
    }

    public void setScale(double scale) {
        double oldScale = this.currentScale;
        this.currentScale = Math.max(0.5, Math.min(3.0, scale));
        
        if (oldScale != this.currentScale) {
            notifyScaleChanged();
        }
    }

    public double getScale() {
        return currentScale;
    }

    public double getBaseDpi() {
        return baseDpi;
    }

    public void setBaseDpi(double baseDpi) {
        this.baseDpi = baseDpi;
        if (autoDetect) {
            calculateScale();
        }
    }

    public double getActualDpi() {
        return actualDpi;
    }

    public void setAutoDetect(boolean autoDetect) {
        this.autoDetect = autoDetect;
        if (autoDetect) {
            detectDpi();
        }
    }

    public boolean isAutoDetect() {
        return autoDetect;
    }

    public boolean isHighDpi() {
        return currentScale > 1.0;
    }

    public boolean is4K() {
        return currentScale >= 2.0;
    }

    public static double[] getScaleLevels() {
        return SCALE_LEVELS.clone();
    }

    public void addScaleChangeListener(ScaleChangeListener listener) {
        listeners.add(listener);
    }

    public void removeScaleChangeListener(ScaleChangeListener listener) {
        listeners.remove(listener);
    }

    private void notifyScaleChanged() {
        ScaleChangeEvent event = new ScaleChangeEvent(currentScale, baseDpi, actualDpi);
        for (ScaleChangeListener listener : listeners) {
            listener.onScaleChanged(event);
        }
    }

    public interface ScaleChangeListener {
        void onScaleChanged(ScaleChangeEvent event);
    }

    public static class ScaleChangeEvent {
        private final double scale;
        private final double baseDpi;
        private final double actualDpi;

        public ScaleChangeEvent(double scale, double baseDpi, double actualDpi) {
            this.scale = scale;
            this.baseDpi = baseDpi;
            this.actualDpi = actualDpi;
        }

        public double getScale() {
            return scale;
        }

        public double getBaseDpi() {
            return baseDpi;
        }

        public double getActualDpi() {
            return actualDpi;
        }
    }
}
