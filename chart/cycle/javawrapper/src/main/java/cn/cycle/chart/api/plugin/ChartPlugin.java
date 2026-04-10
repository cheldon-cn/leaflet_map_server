package cn.cycle.chart.api.plugin;

import cn.cycle.chart.api.core.ChartViewer;

public interface ChartPlugin {

    String getName();

    String getVersion();

    String getDescription();

    void initialize(ChartViewer viewer);

    void shutdown();

    boolean isEnabled();

    void setEnabled(boolean enabled);

    default void onChartLoaded(String chartPath) {
    }

    default void onChartClosed() {
    }

    default void onViewportChanged() {
    }
}
