package cn.cycle.echart.ui;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.scene.layout.Region;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 响应式布局管理器。
 * 
 * <p>实现断点设计，支持三种布局模式：</p>
 * <ul>
 *   <li>紧凑模式：宽度 &lt; 1024px</li>
 *   <li>标准模式：宽度 1024px - 1440px</li>
 *   <li>宽屏模式：宽度 &gt; 1440px</li>
 * </ul>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ResponsiveLayoutManager {

    private static final double COMPACT_BREAKPOINT = 1024.0;
    private static final double WIDE_BREAKPOINT = 1440.0;

    private final ObjectProperty<LayoutMode> currentModeProperty;
    private final List<LayoutModeChangeListener> listeners;
    private Region monitoredRegion;
    private boolean enabled = true;

    public ResponsiveLayoutManager() {
        this.currentModeProperty = new SimpleObjectProperty<>(LayoutMode.STANDARD);
        this.listeners = new ArrayList<>();
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
        this.monitoredRegion = null;
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

    public LayoutMode getCurrentMode() {
        return currentModeProperty.get();
    }

    public ObjectProperty<LayoutMode> currentModeProperty() {
        return currentModeProperty;
    }

    public boolean isCompactMode() {
        return getCurrentMode() == LayoutMode.COMPACT;
    }

    public boolean isStandardMode() {
        return getCurrentMode() == LayoutMode.STANDARD;
    }

    public boolean isWideMode() {
        return getCurrentMode() == LayoutMode.WIDE;
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

    public static LayoutMode determineLayoutModeForWidth(double width) {
        if (width < COMPACT_BREAKPOINT) {
            return LayoutMode.COMPACT;
        } else if (width > WIDE_BREAKPOINT) {
            return LayoutMode.WIDE;
        } else {
            return LayoutMode.STANDARD;
        }
    }

    public enum LayoutMode {
        COMPACT("紧凑模式", COMPACT_BREAKPOINT),
        STANDARD("标准模式", WIDE_BREAKPOINT),
        WIDE("宽屏模式", Double.MAX_VALUE);

        private final String displayName;
        private final double maxWidth;

        LayoutMode(String displayName, double maxWidth) {
            this.displayName = displayName;
            this.maxWidth = maxWidth;
        }

        public String getDisplayName() {
            return displayName;
        }

        public double getMaxWidth() {
            return maxWidth;
        }
    }

    public interface LayoutModeChangeListener {
        void onLayoutModeChanged(LayoutMode oldMode, LayoutMode newMode);
    }

    public static class LayoutConfig {
        private final LayoutMode mode;
        private final double sidebarWidth;
        private final double rightPanelWidth;
        private final boolean showActivityBar;
        private final boolean showRightPanel;
        private final int chartColumns;

        public LayoutConfig(LayoutMode mode, double sidebarWidth, double rightPanelWidth,
                           boolean showActivityBar, boolean showRightPanel, int chartColumns) {
            this.mode = mode;
            this.sidebarWidth = sidebarWidth;
            this.rightPanelWidth = rightPanelWidth;
            this.showActivityBar = showActivityBar;
            this.showRightPanel = showRightPanel;
            this.chartColumns = chartColumns;
        }

        public static LayoutConfig forMode(LayoutMode mode) {
            switch (mode) {
                case COMPACT:
                    return new LayoutConfig(mode, 170, 0, true, false, 1);
                case STANDARD:
                    return new LayoutConfig(mode, 200, 200, true, true, 1);
                case WIDE:
                    return new LayoutConfig(mode, 300, 300, true, true, 2);
                default:
                    return new LayoutConfig(mode, 200, 200, true, true, 1);
            }
        }

        public LayoutMode getMode() {
            return mode;
        }

        public double getSidebarWidth() {
            return sidebarWidth;
        }

        public double getRightPanelWidth() {
            return rightPanelWidth;
        }

        public boolean isShowActivityBar() {
            return showActivityBar;
        }

        public boolean isShowRightPanel() {
            return showRightPanel;
        }

        public int getChartColumns() {
            return chartColumns;
        }
    }
}
