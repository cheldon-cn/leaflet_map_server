package cn.cycle.chart.api.adapter;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Envelope;

public interface ChartEventHandler {

    void install(CanvasAdapter adapter);

    void uninstall();

    interface MouseHandler extends ChartEventHandler {
        void onMousePressed(ChartMouseEvent event);
        void onMouseReleased(ChartMouseEvent event);
        void onMouseMoved(ChartMouseEvent event);
        void onMouseDragged(ChartMouseEvent event);
        void onMouseClicked(ChartMouseEvent event);
    }

    interface ScrollHandler extends ChartEventHandler {
        void onScroll(ChartScrollEvent event);
    }

    interface KeyHandler extends ChartEventHandler {
        void onKeyPressed(ChartKeyEvent event);
        void onKeyReleased(ChartKeyEvent event);
    }
}
