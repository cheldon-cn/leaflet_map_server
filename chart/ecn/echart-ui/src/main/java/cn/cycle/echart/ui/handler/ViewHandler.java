package cn.cycle.echart.ui.handler;

import cn.cycle.echart.core.LogUtil;
import cn.cycle.echart.ui.ChartDisplayArea;
import cn.cycle.echart.ui.SideBarManager;
import cn.cycle.echart.ui.RightTabManager;
import cn.cycle.echart.ui.StatusBar;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.control.SplitPane;

public class ViewHandler {

    private static final double STATUS_BAR_PREF_HEIGHT = 28.0;
    private static final double STATUS_BAR_MIN_HEIGHT = 24.0;
    private static final double RIGHT_PANEL_WIDTH = 300;

    private final ChartDisplayArea chartDisplayArea;
    private final SideBarManager sideBarManager;
    private final RightTabManager rightTabManager;
    private final SplitPane centerSplitPane;
    private final StatusBar statusBar;
    private BorderPane mainView;
    private HBox statusBarWrapper;
    private boolean statusBarVisible = true;
    private boolean rightTabVisible = true;
    private double lastRightPanelWidth = RIGHT_PANEL_WIDTH;

    public ViewHandler(ChartDisplayArea chartDisplayArea, 
                       SideBarManager sideBarManager,
                       RightTabManager rightTabManager,
                       SplitPane centerSplitPane,
                       StatusBar statusBar) {
        this.chartDisplayArea = chartDisplayArea;
        this.sideBarManager = sideBarManager;
        this.rightTabManager = rightTabManager;
        this.centerSplitPane = centerSplitPane;
        this.statusBar = statusBar;
    }
    
    public void setMainView(BorderPane mainView) {
        this.mainView = mainView;
    }
    
    public void setStatusBarWrapper(HBox statusBarWrapper) {
        this.statusBarWrapper = statusBarWrapper;
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

    public void onToggleStatusBar() {
        if (mainView == null || statusBarWrapper == null) {
            LogUtil.debug("ViewHandler", "onToggleStatusBar: mainView=%s, statusBarWrapper=%s", mainView, statusBarWrapper);
            return;
        }
        
        LogUtil.debug("ViewHandler", "onToggleStatusBar: before - statusBarVisible=%s, wrapper.prefHeight=%s, wrapper.minHeight=%s", 
            statusBarVisible, statusBarWrapper.getPrefHeight(), statusBarWrapper.getMinHeight());
        
        if (statusBarVisible) {
            statusBarWrapper.setPrefHeight(0);
            statusBarWrapper.setMinHeight(0);
            statusBarWrapper.setMaxHeight(0);
            statusBar.setVisible(false);
            statusBarVisible = false;
        } else {
            statusBarWrapper.setPrefHeight(STATUS_BAR_PREF_HEIGHT);
            statusBarWrapper.setMinHeight(STATUS_BAR_MIN_HEIGHT);
            statusBarWrapper.setMaxHeight(Double.MAX_VALUE);
            statusBar.setVisible(true);
            statusBarVisible = true;
        }
        
        LogUtil.debug("ViewHandler", "onToggleStatusBar: after - statusBarVisible=%s, wrapper.prefHeight=%s, wrapper.minHeight=%s, wrapper.height=%s, mainView.height=%s", 
            statusBarVisible, statusBarWrapper.getPrefHeight(), statusBarWrapper.getMinHeight(), 
            statusBarWrapper.getHeight(), mainView.getHeight());
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
        if (!centerSplitPane.getItems().contains(rightTabManager)) {
            rightTabManager.setVisible(true);
            rightTabManager.setManaged(true);
            rightTabManager.setPrefWidth(lastRightPanelWidth);
            centerSplitPane.getItems().add(rightTabManager);
            rightTabVisible = true;
        }
    }

    public void hideRightTab() {
        if (centerSplitPane.getItems().contains(rightTabManager)) {
            lastRightPanelWidth = rightTabManager.getWidth() > 0 ? rightTabManager.getWidth() : RIGHT_PANEL_WIDTH;
            centerSplitPane.getItems().remove(rightTabManager);
            rightTabManager.setVisible(false);
            rightTabManager.setManaged(false);
            rightTabVisible = false;
        }
    }

    public void toggleRightTab() {
        if (centerSplitPane.getItems().contains(rightTabManager)) {
            hideRightTab();
        } else {
            showRightTab();
        }
    }
    
    public boolean isSideBarVisible() {
        return sideBarManager.isExpanded();
    }
    
    public boolean isRightTabVisible() {
        return centerSplitPane.getItems().contains(rightTabManager);
    }
    
    public boolean isStatusBarVisible() {
        LogUtil.debug("ViewHandler", "isStatusBarVisible: %s", statusBarVisible);
        return statusBarVisible;
    }
}
