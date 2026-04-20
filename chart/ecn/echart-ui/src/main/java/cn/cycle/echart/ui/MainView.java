package cn.cycle.echart.ui;

import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;

import java.util.Objects;

/**
 * 主视图。
 * 
 * <p>应用主布局框架，实现LifecycleComponent接口，整合ResponsiveLayoutManager。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class MainView extends BorderPane implements LifecycleComponent {

    private final RibbonMenuBar ribbonMenuBar;
    private final ActivityBar activityBar;
    private final SideBarManager sideBarManager;
    private final ChartDisplayArea chartDisplayArea;
    private final RightTabManager rightTabManager;
    private final StatusBar statusBar;
    private final ResponsiveLayoutManager responsiveLayoutManager;

    private boolean initialized = false;
    private boolean active = false;
    private boolean disposed = false;

    public MainView() {
        this.ribbonMenuBar = new RibbonMenuBar();
        this.activityBar = new ActivityBar();
        this.sideBarManager = new SideBarManager();
        this.chartDisplayArea = new ChartDisplayArea();
        this.rightTabManager = new RightTabManager();
        this.statusBar = new StatusBar();
        this.responsiveLayoutManager = new ResponsiveLayoutManager();
        
        initializeLayout();
        setupResponsiveLayout();
    }

    private void initializeLayout() {
        VBox topContainer = new VBox();
        topContainer.getChildren().addAll(ribbonMenuBar);
        setTop(topContainer);
        
        setLeft(activityBar);
        setCenter(createCenterContent());
        setRight(rightTabManager);
        setBottom(statusBar);
        
        getStyleClass().add("main-view");
    }

    private BorderPane createCenterContent() {
        BorderPane centerPane = new BorderPane();
        centerPane.setLeft(sideBarManager);
        centerPane.setCenter(chartDisplayArea);
        return centerPane;
    }

    private void setupResponsiveLayout() {
        responsiveLayoutManager.addListener((oldMode, newMode) -> {
            applyLayoutConfig(ResponsiveLayoutManager.LayoutConfig.forMode(newMode));
        });
        
        responsiveLayoutManager.monitor(this);
        
        setupActivityBarHandler();
    }

    private void setupActivityBarHandler() {
        activityBar.setSelectionHandler((itemId, selected) -> {
            if (selected) {
                sideBarManager.showPanel(itemId);
            } else {
                sideBarManager.hidePanel(itemId);
            }
        });
    }

    private void applyLayoutConfig(ResponsiveLayoutManager.LayoutConfig config) {
        sideBarManager.setExpandedWidth(config.getSidebarWidth());
        
        if (config.isShowRightPanel()) {
            showRightTab();
            rightTabManager.setPrefWidth(config.getRightPanelWidth());
        } else {
            hideRightTab();
        }
        
        chartDisplayArea.setColumnCount(config.getChartColumns());
    }

    @Override
    public void initialize() {
        if (initialized) {
            return;
        }
        
        ribbonMenuBar.initialize();
        chartDisplayArea.initialize();
        sideBarManager.getPanels().values().forEach(panel -> {
            if (panel.getContent() instanceof LifecycleComponent) {
                ((LifecycleComponent) panel.getContent()).initialize();
            }
        });
        
        if (rightTabManager instanceof LifecycleComponent) {
            ((LifecycleComponent) rightTabManager).initialize();
        }
        
        initialized = true;
    }

    @Override
    public void activate() {
        if (!initialized || active) {
            return;
        }
        
        active = true;
        
        chartDisplayArea.activate();
        
        ResponsiveLayoutManager.LayoutConfig config = 
                ResponsiveLayoutManager.LayoutConfig.forMode(responsiveLayoutManager.getCurrentMode());
        applyLayoutConfig(config);
    }

    @Override
    public void deactivate() {
        if (!active) {
            return;
        }
        
        active = false;
        chartDisplayArea.deactivate();
    }

    @Override
    public void dispose() {
        if (disposed) {
            return;
        }
        
        deactivate();
        
        chartDisplayArea.dispose();
        
        sideBarManager.getPanels().values().forEach(panel -> {
            if (panel.getContent() instanceof LifecycleComponent) {
                ((LifecycleComponent) panel.getContent()).dispose();
            }
        });
        
        if (rightTabManager instanceof LifecycleComponent) {
            ((LifecycleComponent) rightTabManager).dispose();
        }
        
        responsiveLayoutManager.stopMonitoring();
        
        disposed = true;
    }

    @Override
    public boolean isInitialized() {
        return initialized;
    }

    @Override
    public boolean isActive() {
        return active;
    }

    @Override
    public boolean isDisposed() {
        return disposed;
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

    public ResponsiveLayoutManager getResponsiveLayoutManager() {
        return responsiveLayoutManager;
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

    public void setLayoutMode(ResponsiveLayoutManager.LayoutMode mode) {
        Objects.requireNonNull(mode, "mode cannot be null");
        applyLayoutConfig(ResponsiveLayoutManager.LayoutConfig.forMode(mode));
    }

    public ResponsiveLayoutManager.LayoutMode getLayoutMode() {
        return responsiveLayoutManager.getCurrentMode();
    }
}
