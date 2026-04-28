package cn.cycle.echart.ui.handler;

import cn.cycle.echart.ui.ChartDisplayArea;
import cn.cycle.echart.ui.SideBarManager;
import cn.cycle.echart.ui.RightTabManager;
import cn.cycle.echart.ui.StatusBar;
import javafx.scene.layout.HBox;

public class ViewHandler {

    private final ChartDisplayArea chartDisplayArea;
    private final SideBarManager sideBarManager;
    private final RightTabManager rightTabManager;
    private final HBox centerBox;
    private final StatusBar statusBar;

    public ViewHandler(ChartDisplayArea chartDisplayArea, 
                       SideBarManager sideBarManager,
                       RightTabManager rightTabManager,
                       HBox centerBox,
                       StatusBar statusBar) {
        this.chartDisplayArea = chartDisplayArea;
        this.sideBarManager = sideBarManager;
        this.rightTabManager = rightTabManager;
        this.centerBox = centerBox;
        this.statusBar = statusBar;
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
        if (!sideBarManager.isExpanded()) {
            String panelId = sideBarManager.getFirstPanelId();
            if (panelId != null) {
                sideBarManager.showPanel(panelId);
            } else {
                sideBarManager.expandPanel();
            }
        }
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
        if (!centerBox.getChildren().contains(rightTabManager)) {
            centerBox.getChildren().add(rightTabManager);
        }
    }

    public void hideRightTab() {
        centerBox.getChildren().remove(rightTabManager);
    }

    public void toggleRightTab() {
        if (centerBox.getChildren().contains(rightTabManager)) {
            hideRightTab();
        } else {
            showRightTab();
        }
    }
}
