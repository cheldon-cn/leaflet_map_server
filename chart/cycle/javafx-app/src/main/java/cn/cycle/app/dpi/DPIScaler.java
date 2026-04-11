package cn.cycle.app.dpi;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.Node;
import javafx.scene.Scene;
import javafx.stage.Screen;

public class DPIScaler extends AbstractLifecycleComponent {
    
    private static final DPIScaler INSTANCE = new DPIScaler();
    private static final double BASE_DPI = 96.0;
    
    private volatile double currentScale = 1.0;
    private volatile double currentDPI = BASE_DPI;
    
    private DPIScaler() {
    }
    
    public static DPIScaler getInstance() {
        return INSTANCE;
    }
    
    public double getScaleFactor() {
        return currentScale;
    }
    
    public double getCurrentDPI() {
        return currentDPI;
    }
    
    public void refreshScale() {
        Screen screen = Screen.getPrimary();
        currentDPI = screen.getDpi();
        currentScale = currentDPI / BASE_DPI;
    }
    
    public void applyDPIScaling(Node node) {
        if (node == null) {
            return;
        }
        
        double scale = getScaleFactor();
        if (Math.abs(scale - 1.0) > 0.01) {
            node.setScaleX(scale);
            node.setScaleY(scale);
        }
    }
    
    public void applyDPIScaling(Scene scene) {
        if (scene == null || scene.getRoot() == null) {
            return;
        }
        
        double scale = getScaleFactor();
        if (Math.abs(scale - 1.0) > 0.01) {
            scene.getRoot().setStyle(String.format(
                "-fx-font-size: %.1fpx;", 12 * scale
            ));
        }
    }
    
    public double getScaledValue(double value) {
        return value * currentScale;
    }
    
    public double getUnscaledValue(double value) {
        if (currentScale == 0) {
            return value;
        }
        return value / currentScale;
    }
    
    public int getScaledInt(int value) {
        return (int) Math.round(value * currentScale);
    }
    
    public String getIconSizeDir() {
        return getIconSizeDir(currentScale);
    }
    
    public String getIconSizeDir(double scale) {
        if (scale >= 2.0) {
            return "32x32";
        }
        if (scale >= 1.5) {
            return "24x24";
        }
        if (scale >= 1.25) {
            return "20x20";
        }
        return "16x16";
    }
    
    public int getIconSize() {
        return getIconSize(currentScale);
    }
    
    public int getIconSize(double scale) {
        if (scale >= 2.0) {
            return 32;
        }
        if (scale >= 1.5) {
            return 24;
        }
        if (scale >= 1.25) {
            return 20;
        }
        return 16;
    }
    
    @Override
    protected void doInitialize() {
        refreshScale();
    }
    
    @Override
    protected void doDispose() {
        currentScale = 1.0;
        currentDPI = BASE_DPI;
    }
}
