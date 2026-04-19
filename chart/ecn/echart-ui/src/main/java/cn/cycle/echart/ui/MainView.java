package cn.cycle.echart.ui;

import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;

/**
 * 主视图。
 * 
 * <p>应用主布局框架。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class MainView extends BorderPane {

    private final RibbonMenuBar ribbonMenuBar;
    private final ActivityBar activityBar;
    private final SideBarManager sideBarManager;
    private final ChartDisplayArea chartDisplayArea;
    private final RightTabManager rightTabManager;
    private final StatusBar statusBar;

    public MainView() {
        this.ribbonMenuBar = new RibbonMenuBar();
        this.activityBar = new ActivityBar();
        this.sideBarManager = new SideBarManager();
        this.chartDisplayArea = new ChartDisplayArea();
        this.rightTabManager = new RightTabManager();
        this.statusBar = new StatusBar();
        
        initializeLayout();
    }

    private void initializeLayout() {
        VBox topContainer = new VBox();
        topContainer.getChildren().addAll(ribbonMenuBar, activityBar);
        setTop(topContainer);
        
        setLeft(sideBarManager);
        setCenter(chartDisplayArea);
        setRight(rightTabManager);
        setBottom(statusBar);
        
        getStyleClass().add("main-view");
    }

    public RibbonMenuBar getRibbonMenuBar() {
        return ribbonMenuBar;
    }

    public ActivityBar getActivityBar() {
        return activityBar;
    }

    public SideBarManager getSideBarManager() {
        return sideBarManager;
    }

    public ChartDisplayArea getChartDisplayArea() {
        return chartDisplayArea;
    }

    public RightTabManager getRightTabManager() {
        return rightTabManager;
    }

    public StatusBar getStatusBar() {
        return statusBar;
    }

    public void showSideBar() {
        setLeft(sideBarManager);
    }

    public void hideSideBar() {
        setLeft(null);
    }

    public void toggleSideBar() {
        if (getLeft() == null) {
            showSideBar();
        } else {
            hideSideBar();
        }
    }

    public void showRightTab() {
        setRight(rightTabManager);
    }

    public void hideRightTab() {
        setRight(null);
    }

    public void toggleRightTab() {
        if (getRight() == null) {
            showRightTab();
        } else {
            hideRightTab();
        }
    }
}
