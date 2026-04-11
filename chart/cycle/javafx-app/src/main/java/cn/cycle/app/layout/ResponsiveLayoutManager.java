package cn.cycle.app.layout;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.beans.property.DoubleProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.beans.value.ChangeListener;
import javafx.scene.layout.Region;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;

public class ResponsiveLayoutManager extends AbstractLifecycleComponent {
    
    public enum LayoutMode {
        COMPACT,
        STANDARD,
        WIDE
    }
    
    private static final int COMPACT_WIDTH = 1024;
    private static final int STANDARD_WIDTH = 1440;
    
    private final DoubleProperty windowWidth = new SimpleDoubleProperty();
    private final DoubleProperty windowHeight = new SimpleDoubleProperty();
    
    private LayoutMode currentMode = LayoutMode.STANDARD;
    private final List<Consumer<LayoutMode>> layoutChangeListeners = new ArrayList<>();
    
    private Region sideBar;
    private Region rightPanel;
    private Region statusBar;
    
    private final ChangeListener<Number> widthListener = (obs, oldVal, newVal) -> {
        onWindowResize(newVal.doubleValue());
    };
    
    public void setSideBar(Region sideBar) {
        this.sideBar = sideBar;
    }
    
    public void setRightPanel(Region rightPanel) {
        this.rightPanel = rightPanel;
    }
    
    public void setStatusBar(Region statusBar) {
        this.statusBar = statusBar;
    }
    
    public void bindToWindow(Region root) {
        windowWidth.bind(root.widthProperty());
        windowHeight.bind(root.heightProperty());
    }
    
    public void onWindowResize(double width) {
        LayoutMode newMode;
        if (width < COMPACT_WIDTH) {
            newMode = LayoutMode.COMPACT;
        } else if (width < STANDARD_WIDTH) {
            newMode = LayoutMode.STANDARD;
        } else {
            newMode = LayoutMode.WIDE;
        }
        
        if (newMode != currentMode) {
            currentMode = newMode;
            applyLayout(newMode);
            notifyLayoutChange(newMode);
        }
    }
    
    private void applyLayout(LayoutMode mode) {
        switch (mode) {
            case COMPACT:
                applyCompactLayout();
                break;
            case STANDARD:
                applyStandardLayout();
                break;
            case WIDE:
                applyWideLayout();
                break;
        }
    }
    
    private void applyCompactLayout() {
        if (sideBar != null) {
            sideBar.setPrefWidth(40);
            sideBar.setMinWidth(40);
        }
        if (rightPanel != null) {
            rightPanel.setPrefWidth(0);
            rightPanel.setMinWidth(0);
        }
    }
    
    private void applyStandardLayout() {
        if (sideBar != null) {
            sideBar.setPrefWidth(200);
            sideBar.setMinWidth(180);
            sideBar.setMaxWidth(350);
        }
        if (rightPanel != null) {
            rightPanel.setPrefWidth(250);
            rightPanel.setMinWidth(200);
            rightPanel.setMaxWidth(450);
        }
    }
    
    private void applyWideLayout() {
        if (sideBar != null) {
            sideBar.setPrefWidth(250);
            sideBar.setMinWidth(200);
            sideBar.setMaxWidth(350);
        }
        if (rightPanel != null) {
            rightPanel.setPrefWidth(300);
            rightPanel.setMinWidth(250);
            rightPanel.setMaxWidth(450);
        }
    }
    
    public void addLayoutChangeListener(Consumer<LayoutMode> listener) {
        layoutChangeListeners.add(listener);
    }
    
    public void removeLayoutChangeListener(Consumer<LayoutMode> listener) {
        layoutChangeListeners.remove(listener);
    }
    
    private void notifyLayoutChange(LayoutMode mode) {
        for (Consumer<LayoutMode> listener : layoutChangeListeners) {
            listener.accept(mode);
        }
    }
    
    public LayoutMode getCurrentMode() {
        return currentMode;
    }
    
    public double getWindowWidth() {
        return windowWidth.get();
    }
    
    public double getWindowHeight() {
        return windowHeight.get();
    }
    
    @Override
    protected void doInitialize() {
        windowWidth.addListener(widthListener);
    }
    
    @Override
    protected void doDispose() {
        windowWidth.removeListener(widthListener);
        windowWidth.unbind();
        windowHeight.unbind();
        layoutChangeListeners.clear();
    }
}
