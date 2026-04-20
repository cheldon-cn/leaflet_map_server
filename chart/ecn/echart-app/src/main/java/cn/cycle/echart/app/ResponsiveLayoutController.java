package cn.cycle.echart.app;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.scene.layout.Region;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * 响应式布局控制器。
 * 
 * <p>根据窗口大小自动调整布局模式。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ResponsiveLayoutController {

    private static final double COMPACT_BREAKPOINT = 1024.0;
    private static final double WIDE_BREAKPOINT = 1440.0;

    private final ObjectProperty<LayoutMode> currentModeProperty;
    private final List<LayoutModeChangeListener> listeners;
    private final Map<String, LayoutConfig> configs;
    
    private Region monitoredRegion;
    private boolean enabled;

    public ResponsiveLayoutController() {
        this.currentModeProperty = new SimpleObjectProperty<>(LayoutMode.STANDARD);
        this.listeners = new ArrayList<>();
        this.configs = new HashMap<>();
        this.enabled = true;
        
        initializeDefaultConfigs();
    }

    private void initializeDefaultConfigs() {
        configs.put(LayoutMode.COMPACT.name(), 
                new LayoutConfig(200, 0, 1, false, true));
        configs.put(LayoutMode.STANDARD.name(), 
                new LayoutConfig(280, 250, 2, true, true));
        configs.put(LayoutMode.WIDE.name(), 
                new LayoutConfig(320, 300, 3, true, true));
    }

    public void monitor(Region region) {
        Objects.requireNonNull(region, "region cannot be null");
        
        if (this.monitoredRegion != null) {
            stopMonitoring();
        }
        
        this.monitoredRegion = region;
        
        region.widthProperty().addListener((obs, oldVal, newVal) -> {
            if (enabled) {
                updateLayoutMode(newVal.doubleValue());
            }
        });
        
        updateLayoutMode(region.getWidth());
    }

    public void stopMonitoring() {
        monitoredRegion = null;
    }

    private void updateLayoutMode(double width) {
        LayoutMode newMode = determineLayoutMode(width);
        LayoutMode currentMode = currentModeProperty.get();
        
        if (newMode != currentMode) {
            currentModeProperty.set(newMode);
            notifyListeners(currentMode, newMode);
        }
    }

    private LayoutMode determineLayoutMode(double width) {
        if (width < COMPACT_BREAKPOINT) {
            return LayoutMode.COMPACT;
        } else if (width > WIDE_BREAKPOINT) {
            return LayoutMode.WIDE;
        } else {
            return LayoutMode.STANDARD;
        }
    }

    public void setLayoutMode(LayoutMode mode) {
        Objects.requireNonNull(mode, "mode cannot be null");
        LayoutMode oldMode = currentModeProperty.get();
        currentModeProperty.set(mode);
        notifyListeners(oldMode, mode);
    }

    public LayoutMode getCurrentMode() {
        return currentModeProperty.get();
    }

    public ObjectProperty<LayoutMode> currentModeProperty() {
        return currentModeProperty;
    }

    public LayoutConfig getCurrentConfig() {
        return configs.get(currentModeProperty.get().name());
    }

    public LayoutConfig getConfig(LayoutMode mode) {
        return configs.get(mode.name());
    }

    public void setConfig(LayoutMode mode, LayoutConfig config) {
        Objects.requireNonNull(mode, "mode cannot be null");
        Objects.requireNonNull(config, "config cannot be null");
        configs.put(mode.name(), config);
    }

    public void addListener(LayoutModeChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.add(listener);
    }

    public void removeListener(LayoutModeChangeListener listener) {
        listeners.remove(listener);
    }

    private void notifyListeners(LayoutMode oldMode, LayoutMode newMode) {
        for (LayoutModeChangeListener listener : listeners) {
            listener.onLayoutModeChanged(oldMode, newMode);
        }
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public static double getCompactBreakpoint() {
        return COMPACT_BREAKPOINT;
    }

    public static double getWideBreakpoint() {
        return WIDE_BREAKPOINT;
    }

    public enum LayoutMode {
        COMPACT("紧凑模式"),
        STANDARD("标准模式"),
        WIDE("宽屏模式");

        private final String displayName;

        LayoutMode(String displayName) {
            this.displayName = displayName;
        }

        public String getDisplayName() {
            return displayName;
        }
    }

    public interface LayoutModeChangeListener {
        void onLayoutModeChanged(LayoutMode oldMode, LayoutMode newMode);
    }

    public static class LayoutConfig {
        private final double sidebarWidth;
        private final double rightPanelWidth;
        private final int chartColumns;
        private final boolean showRightPanel;
        private final boolean showSidebar;

        public LayoutConfig(double sidebarWidth, double rightPanelWidth, 
                           int chartColumns, boolean showRightPanel, boolean showSidebar) {
            this.sidebarWidth = sidebarWidth;
            this.rightPanelWidth = rightPanelWidth;
            this.chartColumns = chartColumns;
            this.showRightPanel = showRightPanel;
            this.showSidebar = showSidebar;
        }

        public double getSidebarWidth() {
            return sidebarWidth;
        }

        public double getRightPanelWidth() {
            return rightPanelWidth;
        }

        public int getChartColumns() {
            return chartColumns;
        }

        public boolean isShowRightPanel() {
            return showRightPanel;
        }

        public boolean isShowSidebar() {
            return showSidebar;
        }

        public static LayoutConfig forMode(LayoutMode mode) {
            switch (mode) {
                case COMPACT:
                    return new LayoutConfig(200, 0, 1, false, true);
                case WIDE:
                    return new LayoutConfig(320, 300, 3, true, true);
                case STANDARD:
                default:
                    return new LayoutConfig(280, 250, 2, true, true);
            }
        }
    }
}
