package cn.cycle.echart.ui.handler;

import cn.cycle.echart.ui.ChartDisplayArea;
import cn.cycle.echart.ui.SideBarManager;
import cn.cycle.echart.ui.RightTabManager;
import cn.cycle.echart.ui.StatusBar;
import javafx.scene.control.SplitPane;

public class ViewHandler {

    private final ChartDisplayArea chartDisplayArea;
    private final SideBarManager sideBarManager;
    private final RightTabManager rightTabManager;
    private final SplitPane centerSplitPane;
    private final StatusBar statusBar;
    private final Runnable updateDividerCallback;

    public ViewHandler(ChartDisplayArea chartDisplayArea, 
                       SideBarManager sideBarManager,
                       RightTabManager rightTabManager,
                       SplitPane centerSplitPane,
                       StatusBar statusBar,
                       Runnable updateDividerCallback) {
        this.chartDisplayArea = chartDisplayArea;
        this.sideBarManager = sideBarManager;
        this.rightTabManager = rightTabManager;
        this.centerSplitPane = centerSplitPane;
        this.statusBar = statusBar;
        this.updateDividerCallback = updateDividerCallback;
    }

    public void onZoomIn() {
        chartDisplayArea.zoomIn();
    }

    public void onZoomOut() {
        chartDisplayArea.zoomOut();
    }

    public void onFitToWindow() {
        chartDisplayArea.fitToWindow();
    }

    public void onToggleSideBar() {
        toggleSideBar();
    }

    public void onToggleRightTab() {
        toggleRightTab();
    }

    public void onToggleStatusBar(javafx.scene.layout.BorderPane parent) {
        if (parent.getBottom() == null) {
            parent.setBottom(statusBar);
        } else {
            parent.setBottom(null);
        }
    }

    public void showSideBar() {
        sideBarManager.expandPanel();
    }

    public void hideSideBar() {
        sideBarManager.collapsePanel();
    }

    public void toggleSideBar() {
        if (sideBarManager.isExpanded()) {
            hideSideBar();
        } else {
            showSideBar();
        }
    }

    public void showRightTab() {
        if (!centerSplitPane.getItems().contains(rightTabManager)) {
            centerSplitPane.getItems().add(rightTabManager);
            javafx.application.Platform.runLater(() -> {
                if (updateDividerCallback != null) {
                    updateDividerCallback.run();
                }
            });
        }
    }

    public void hideRightTab() {
        centerSplitPane.getItems().remove(rightTabManager);
    }

    public void toggleRightTab() {
        if (centerSplitPane.getItems().contains(rightTabManager)) {
            hideRightTab();
        } else {
            showRightTab();
        }
    }
}
