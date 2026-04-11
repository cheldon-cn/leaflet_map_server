package cn.cycle.app.view;

import cn.cycle.app.context.AppContext;
import cn.cycle.app.dpi.DPIScaler;
import cn.cycle.app.dpi.IconLoader;
import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.i18n.I18nManager;
import cn.cycle.app.layout.ResponsiveLayoutManager;
import cn.cycle.app.lifecycle.LifecycleComponent;
import cn.cycle.app.panel.RightTabManager;
import cn.cycle.app.shortcut.ShortcutManager;
import cn.cycle.app.sidebar.SideBarManager;
import cn.cycle.app.status.StatusBar;
import cn.cycle.app.theme.ThemeManager;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.app.controller.MainController;
import com.cycle.control.Ribbon;
import com.cycle.control.ribbon.RibbonGroup;
import com.cycle.control.ribbon.RibbonTab;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.ContentDisplay;
import javafx.scene.control.MenuButton;
import javafx.scene.control.MenuItem;
import javafx.scene.control.RadioMenuItem;
import javafx.scene.control.TabPane;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.Region;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;

public class MainView extends BorderPane implements LifecycleComponent {

    private static final String ICON_PATH = "/control/";

    private Ribbon ribbon;
    private ChartCanvas chartCanvas;
    private MainController controller;
    
    private VBox sideBarButtonContainer;
    private StackPane sideBarPanelContainer;
    private SideBarManager sideBarManager;
    
    private TabPane rightTabPane;
    private RightTabManager rightTabManager;
    
    private StatusBar statusBar;
    
    private ResponsiveLayoutManager layoutManager;
    private DPIScaler dpiScaler;
    private IconLoader iconLoader;
    
    private volatile boolean initialized = false;
    private volatile boolean active = false;
    private final Object lock = new Object();

    public MainView(ChartViewer chartViewer) {
        this.controller = new MainController(chartViewer);
        this.chartCanvas = new ChartCanvas(chartViewer);
        createView();
        setupControllerCallbacks();
    }
    
    private void setupControllerCallbacks() {
        controller.setRenderCallback(() -> {
            if (chartCanvas != null) {
                chartCanvas.render();
            }
        });
        
        controller.setStatusUpdateCallback(() -> {
            if (statusBar != null) {
                statusBar.setMessage("海图已加载");
            }
        });
    }
    
    public void createView() {
        HBox topContainer = createTopContainer();
        this.setTop(topContainer);
        
        BorderPane centerLayout = createCenterLayout();
        this.setCenter(centerLayout);
        
        statusBar = new StatusBar();
        this.setBottom(statusBar.getContainer());
    }
    
    private HBox createTopContainer() {
        HBox topContainer = new HBox();
        topContainer.getStyleClass().add("top-container");
        
        ribbon = createRibbon();
        HBox.setHgrow(ribbon, Priority.ALWAYS);
        
        HBox windowControls = createWindowControls();
        
        topContainer.getChildren().addAll(ribbon, windowControls);
        
        return topContainer;
    }
    
    private HBox createWindowControls() {
        HBox controls = new HBox();
        controls.getStyleClass().add("window-controls");
        controls.setStyle("-fx-background-color: derive(-fx-base, -20%);");
        controls.setAlignment(javafx.geometry.Pos.CENTER_RIGHT);
        
        Button settingsBtn = createWindowButton("设置", "应用程序设置", "Png_online_16.png");
        settingsBtn.setOnAction(e -> showSettings());
        
        Button loginBtn = createWindowButton("登录", "用户登录", "Png_img_16.png");
        loginBtn.setOnAction(e -> showLogin());
        
        Button minimizeBtn = createWindowButton("", "最小化", "Png_ZoomIn_16.png");
        minimizeBtn.setPrefWidth(40);
        minimizeBtn.setOnAction(e -> minimizeWindow());
        
        Button maximizeBtn = createWindowButton("", "最大化/还原", "Png_show_16.png");
        maximizeBtn.setPrefWidth(40);
        maximizeBtn.setOnAction(e -> maximizeWindow());
        
        Button closeBtn = createWindowButton("", "关闭", "close.png");
        closeBtn.setPrefWidth(40);
        closeBtn.setStyle(closeBtn.getStyle() + "-fx-background-color: transparent;");
        closeBtn.setOnMouseEntered(e -> closeBtn.setStyle("-fx-background-color: #e74c3c; -fx-text-fill: white;"));
        closeBtn.setOnMouseExited(e -> closeBtn.setStyle("-fx-background-color: transparent; -fx-text-fill: -fx-text-base-color;"));
        closeBtn.setOnAction(e -> closeWindow());
        
        Region spacer = new Region();
        HBox.setHgrow(spacer, Priority.ALWAYS);
        
        controls.getChildren().addAll(settingsBtn, loginBtn, spacer, minimizeBtn, maximizeBtn, closeBtn);
        controls.setPrefHeight(16);
        controls.setMinHeight(16);
        controls.setMaxHeight(16);
        
        return controls;
    }
    
    private Button createWindowButton(String text, String tooltip, String iconName) {
        Button button = new Button(text);
        button.setTooltip(new javafx.scene.control.Tooltip(tooltip));
        button.setGraphic(createIcon(iconName));
        button.setContentDisplay(ContentDisplay.GRAPHIC_ONLY);
        button.setPrefSize(60, 60);
        button.getStyleClass().add("window-control-button");
        button.setStyle("-fx-background-color: transparent; -fx-border-color: transparent;");
        
        button.setOnMouseEntered(e -> {
            button.setStyle("-fx-background-color: derive(-fx-base, 20%); -fx-border-color: derive(-fx-base, 30%);");
        });
        button.setOnMouseExited(e -> {
            button.setStyle("-fx-background-color: transparent; -fx-border-color: transparent;");
        });
        
        return button;
    }
    
    private void showSettings() {
        System.out.println("显示设置对话框");
    }
    
    private void showLogin() {
        System.out.println("显示登录对话框");
    }
    
    private void minimizeWindow() {
        javafx.stage.Window window = this.getScene().getWindow();
        if (window instanceof javafx.stage.Stage) {
            ((javafx.stage.Stage) window).setIconified(true);
        }
    }
    
    private void maximizeWindow() {
        javafx.stage.Window window = this.getScene().getWindow();
        if (window instanceof javafx.stage.Stage) {
            javafx.stage.Stage stage = (javafx.stage.Stage) window;
            stage.setMaximized(!stage.isMaximized());
        }
    }
    
    private void closeWindow() {
        javafx.stage.Window window = this.getScene().getWindow();
        if (window instanceof javafx.stage.Stage) {
            ((javafx.stage.Stage) window).close();
        }
    }
    
    private BorderPane createCenterLayout() {
        BorderPane centerLayout = new BorderPane();
        
        sideBarButtonContainer = new VBox();
        sideBarButtonContainer.setPrefWidth(48);
        sideBarButtonContainer.setMinWidth(48);
        sideBarButtonContainer.setMaxWidth(56);
        sideBarButtonContainer.setSpacing(8);
        sideBarButtonContainer.getStyleClass().add("sidebar");
        sideBarButtonContainer.setStyle("-fx-border-color: derive(-fx-base, -20%); -fx-border-width: 0 1 0 0; -fx-padding: 8 4 8 4;");
        
        sideBarPanelContainer = new StackPane();
        sideBarPanelContainer.setPrefWidth(250);
        sideBarPanelContainer.setMinWidth(180);
        sideBarPanelContainer.setMaxWidth(350);
        sideBarPanelContainer.getStyleClass().add("sidebar-panel");
        
        BorderPane sideBar = new BorderPane();
        sideBar.setLeft(sideBarButtonContainer);
        sideBar.setCenter(sideBarPanelContainer);
        
        sideBarManager = new SideBarManager(sideBarButtonContainer, sideBarPanelContainer);
        
        sideBarManager.registerPanel(new cn.cycle.app.sidebar.DataCatalogPanel());
        sideBarManager.registerPanel(new cn.cycle.app.sidebar.DataConvertPanel());
        sideBarManager.registerPanel(new cn.cycle.app.sidebar.QueryPanel());
        sideBarManager.registerPanel(new cn.cycle.app.sidebar.StylePanel());
        
        centerLayout.setLeft(sideBar);
        
        centerLayout.setCenter(chartCanvas);
        
        rightTabPane = new TabPane();
        rightTabPane.setPrefWidth(300);
        rightTabPane.setMinWidth(200);
        rightTabPane.setMaxWidth(450);
        rightTabPane.setTabClosingPolicy(TabPane.TabClosingPolicy.UNAVAILABLE);
        
        rightTabManager = new RightTabManager(rightTabPane);
        
        centerLayout.setRight(rightTabPane);
        
        layoutManager = new ResponsiveLayoutManager();
        layoutManager.setSideBar(sideBar);
        layoutManager.setRightPanel(rightTabPane);
        layoutManager.bindToWindow(this);
        
        return centerLayout;
    }

    private Ribbon createRibbon() {
        Ribbon ribbon = new Ribbon();

        RibbonTab fileTab = createFileTab();
        RibbonTab viewTab = createViewTab();
        RibbonTab toolsTab = createToolsTab();
        RibbonTab layerTab = createLayerTab();
        RibbonTab settingsTab = createSettingsTab();
        RibbonTab helpTab = createHelpTab();

        ribbon.getTabs().addAll(fileTab, viewTab, toolsTab, layerTab, settingsTab, helpTab);

        return ribbon;
    }

    private RibbonTab createFileTab() {
        RibbonTab tab = new RibbonTab("文件");

        RibbonGroup fileGroup = new RibbonGroup();
        fileGroup.setTitle("文件操作");

        Button openButton = createRibbonButton("打开", "打开海图文件", "icons8_Open_32px_3.png");
        openButton.setOnAction(e -> controller.openChart());

        MenuButton saveMenu = createSaveMenuButton();

        Button closeButton = createRibbonButton("关闭", "关闭当前海图", "icons8_close_32px.png");
        closeButton.setOnAction(e -> controller.closeChart());

        HBox fileBox = new HBox(5);
        fileBox.getChildren().addAll(openButton, saveMenu, closeButton);
        fileGroup.getNodes().add(fileBox);

        RibbonGroup editGroup = new RibbonGroup();
        editGroup.setTitle("编辑");

        Button undoButton = createRibbonButton("撤销", "撤销操作", "icons8_Undo_16px.png");
        Button redoButton = createRibbonButton("重做", "重做操作", "icons8_Redo_16px.png");

        HBox editBox = new HBox(5);
        editBox.getChildren().addAll(undoButton, redoButton);
        editGroup.getNodes().add(editBox);

        RibbonGroup exitGroup = new RibbonGroup();
        exitGroup.setTitle("退出");

        Button exitButton = createRibbonButton("退出", "退出应用程序", "icons8_closew_32px.png");
        exitButton.setOnAction(e -> controller.exitApplication());

        exitGroup.getNodes().add(exitButton);

        tab.getRibbonGroups().addAll(fileGroup, editGroup, exitGroup);

        return tab;
    }

    private MenuButton createSaveMenuButton() {
        MenuButton saveMenu = new MenuButton("保存");
        saveMenu.setGraphic(createIcon("icons8_Save_32px.png"));
        saveMenu.setContentDisplay(ContentDisplay.TOP);
        saveMenu.setPrefSize(60, 60);

        MenuItem saveItem = new MenuItem("保存", createIcon("icons8_Save_16px.png"));
        saveItem.setOnAction(e -> controller.saveChart());

        MenuItem saveAsItem = new MenuItem("另存为", createIcon("icons8_Save_as_32px.png"));
        saveAsItem.setOnAction(e -> System.out.println("另存为..."));

        saveMenu.getItems().addAll(saveItem, saveAsItem);
        return saveMenu;
    }

    private RibbonTab createViewTab() {
        RibbonTab tab = new RibbonTab("视图");

        RibbonGroup zoomGroup = new RibbonGroup();
        zoomGroup.setTitle("缩放");

        Button zoomInButton = createRibbonButton("放大", "放大视图", "icons8_run_32px.png");
        zoomInButton.setOnAction(e -> controller.zoomIn());

        Button zoomOutButton = createRibbonButton("缩小", "缩小视图", "icons8_runb_32px.png");
        zoomOutButton.setOnAction(e -> controller.zoomOut());

        HBox zoomBox = new HBox(5);
        zoomBox.getChildren().addAll(zoomInButton, zoomOutButton);
        zoomGroup.getNodes().add(zoomBox);

        RibbonGroup ratioGroup = new RibbonGroup();
        ratioGroup.setTitle("缩放比例");

        ComboBox<String> ratioCombo = new ComboBox<>();
        ratioCombo.getItems().addAll("25%", "50%", "75%", "100%", "150%", "200%", "400%");
        ratioCombo.setValue("100%");
        ratioCombo.setPrefWidth(80);
        ratioCombo.setOnAction(e -> {
            String value = ratioCombo.getValue().replace("%", "");
            double ratio = Double.parseDouble(value) / 100.0;
            controller.setZoomRatio(ratio);
        });

        ratioGroup.getNodes().add(ratioCombo);

        RibbonGroup fitGroup = new RibbonGroup();
        fitGroup.setTitle("适应");

        Button fitButton = createRibbonButton("适应窗口", "海图适应窗口大小", "icons8_Globe_32px.png");
        fitButton.setOnAction(e -> controller.fitToWindow());

        fitGroup.getNodes().add(fitButton);

        RibbonGroup panelGroup = new RibbonGroup();
        panelGroup.setTitle("面板");

        Button toggleSideBarBtn = createRibbonButton("侧边栏", "显示/隐藏侧边栏", "Png_Folder_16.png");
        toggleSideBarBtn.setOnAction(e -> toggleSideBar());
        
        Button toggleRightBtn = createRibbonButton("右侧面板", "显示/隐藏右侧面板", "icons8_Info_32px.png");
        toggleRightBtn.setOnAction(e -> toggleRightPanel());

        HBox panelBox = new HBox(5);
        panelBox.getChildren().addAll(toggleSideBarBtn, toggleRightBtn);
        panelGroup.getNodes().add(panelBox);

        tab.getRibbonGroups().addAll(zoomGroup, ratioGroup, fitGroup, panelGroup);

        return tab;
    }

    private RibbonTab createToolsTab() {
        RibbonTab tab = new RibbonTab("工具");

        RibbonGroup measureGroup = new RibbonGroup();
        measureGroup.setTitle("测量");

        MenuButton measureMenu = new MenuButton("测量");
        measureMenu.setGraphic(createIcon("icons8_Tool_32px.png"));
        measureMenu.setContentDisplay(ContentDisplay.TOP);
        measureMenu.setPrefSize(60, 60);

        MenuItem distanceItem = new MenuItem("距离测量");
        distanceItem.setOnAction(e -> controller.startDistanceMeasure());

        MenuItem areaItem = new MenuItem("面积测量");
        areaItem.setOnAction(e -> controller.startAreaMeasure());

        measureMenu.getItems().addAll(distanceItem, areaItem);

        measureGroup.getNodes().add(measureMenu);

        RibbonGroup routeGroup = new RibbonGroup();
        routeGroup.setTitle("航线");

        Button routeButton = createRibbonButton("航线规划", "打开航线规划工具", "icons8_Location_16px.png");
        routeButton.setOnAction(e -> controller.openRoutePlanner());

        routeGroup.getNodes().add(routeButton);

        RibbonGroup toolGroup = new RibbonGroup();
        toolGroup.setTitle("工具");

        Button toolButton = createRibbonButton("工具箱", "打开工具箱", "icons8_Tool_32px.png");
        toolGroup.getNodes().add(toolButton);

        tab.getRibbonGroups().addAll(measureGroup, routeGroup, toolGroup);

        return tab;
    }

    private RibbonTab createLayerTab() {
        RibbonTab tab = new RibbonTab("图层");

        RibbonGroup layerGroup = new RibbonGroup();
        layerGroup.setTitle("图层管理");

        Button toggleLayerButton = createRibbonButton("图层面板", "显示/隐藏图层面板", "icons8_Lock_16px.png");
        toggleLayerButton.setOnAction(e -> controller.toggleLayerPanel());

        layerGroup.getNodes().add(toggleLayerButton);

        RibbonGroup visibleGroup = new RibbonGroup();
        visibleGroup.setTitle("可见性");

        CheckBox baseLayerCheck = new CheckBox("基础图层");
        baseLayerCheck.setSelected(true);
        CheckBox overlayCheck = new CheckBox("叠加图层");
        overlayCheck.setSelected(true);
        CheckBox gridCheck = new CheckBox("网格");
        gridCheck.setSelected(false);

        VBox checkBoxContainer = new VBox(5);
        checkBoxContainer.getChildren().addAll(baseLayerCheck, overlayCheck, gridCheck);
        visibleGroup.getNodes().add(checkBoxContainer);

        tab.getRibbonGroups().addAll(layerGroup, visibleGroup);

        return tab;
    }

    private RibbonTab createSettingsTab() {
        RibbonTab tab = new RibbonTab("设置");

        RibbonGroup displayGroup = new RibbonGroup();
        displayGroup.setTitle("显示设置");

        MenuButton themeMenu = createThemeMenuButton();

        displayGroup.getNodes().add(themeMenu);
        
        RibbonGroup langGroup = new RibbonGroup();
        langGroup.setTitle("语言设置");
        
        MenuButton langMenu = createLanguageMenuButton();
        
        langGroup.getNodes().add(langMenu);

        RibbonGroup optionsGroup = new RibbonGroup();
        optionsGroup.setTitle("选项");

        ToggleButton showGridBtn = new ToggleButton("显示网格");
        showGridBtn.setSelected(false);
        showGridBtn.setPrefWidth(80);

        ToggleButton showScaleBtn = new ToggleButton("显示比例尺");
        showScaleBtn.setSelected(true);
        showScaleBtn.setPrefWidth(80);

        VBox optionsBox = new VBox(5);
        optionsBox.getChildren().addAll(showGridBtn, showScaleBtn);
        optionsGroup.getNodes().add(optionsBox);

        tab.getRibbonGroups().addAll(displayGroup, langGroup, optionsGroup);

        return tab;
    }

    private MenuButton createThemeMenuButton() {
        MenuButton themeMenu = new MenuButton("主题");
        themeMenu.setGraphic(createIcon("icons8_Change_Theme_16px.png"));
        themeMenu.setContentDisplay(ContentDisplay.TOP);
        themeMenu.setPrefSize(60, 60);

        ToggleGroup themeGroup = new ToggleGroup();

        RadioMenuItem lightTheme = new RadioMenuItem("浅色主题");
        lightTheme.setToggleGroup(themeGroup);
        lightTheme.setSelected(true);

        RadioMenuItem darkTheme = new RadioMenuItem("深色主题");
        darkTheme.setToggleGroup(themeGroup);

        RadioMenuItem seaTheme = new RadioMenuItem("海洋主题");
        seaTheme.setToggleGroup(themeGroup);

        lightTheme.setOnAction(e -> {
            ThemeManager.getInstance().setTheme("light");
            controller.setTheme("light");
        });
        darkTheme.setOnAction(e -> {
            ThemeManager.getInstance().setTheme("dark");
            controller.setTheme("dark");
        });
        seaTheme.setOnAction(e -> {
            ThemeManager.getInstance().setTheme("blue");
            controller.setTheme("blue");
        });

        themeMenu.getItems().addAll(lightTheme, darkTheme, seaTheme);
        return themeMenu;
    }
    
    private MenuButton createLanguageMenuButton() {
        MenuButton langMenu = new MenuButton("语言");
        langMenu.setGraphic(createIcon("icons8_Globe_32px.png"));
        langMenu.setContentDisplay(ContentDisplay.TOP);
        langMenu.setPrefSize(60, 60);

        ToggleGroup langGroup = new ToggleGroup();

        RadioMenuItem chineseItem = new RadioMenuItem("中文");
        chineseItem.setToggleGroup(langGroup);
        chineseItem.setSelected(true);

        RadioMenuItem englishItem = new RadioMenuItem("English");
        englishItem.setToggleGroup(langGroup);

        chineseItem.setOnAction(e -> {
            I18nManager.getInstance().setLocale(java.util.Locale.CHINESE);
        });
        englishItem.setOnAction(e -> {
            I18nManager.getInstance().setLocale(java.util.Locale.ENGLISH);
        });

        langMenu.getItems().addAll(chineseItem, englishItem);
        return langMenu;
    }

    private RibbonTab createHelpTab() {
        RibbonTab tab = new RibbonTab("帮助");

        RibbonGroup helpGroup = new RibbonGroup();
        helpGroup.setTitle("帮助");

        Button helpButton = createRibbonButton("帮助", "查看帮助文档", "icons8_Help_32px.png");
        helpButton.setOnAction(e -> controller.showHelp());

        Button aboutButton = createRibbonButton("关于", "关于本程序", "icons8_Info_32px.png");
        aboutButton.setOnAction(e -> controller.showAbout());

        HBox helpBox = new HBox(5);
        helpBox.getChildren().addAll(helpButton, aboutButton);
        helpGroup.getNodes().add(helpBox);

        tab.getRibbonGroups().add(helpGroup);

        return tab;
    }

    private Button createRibbonButton(String text, String tooltip, String iconName) {
        Button button = new Button(text);
        button.setTooltip(new javafx.scene.control.Tooltip(tooltip));
        button.setGraphic(createIcon(iconName));
        button.setContentDisplay(ContentDisplay.TOP);
        button.setPrefSize(60, 60);
        button.getStyleClass().add("ribbon-button");
        button.focusedProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal) {
                button.getParent().requestFocus();
            }
        });
        return button;
    }

    private ImageView createIcon(String iconName) {
        try {
            Image image = new Image(getClass().getResourceAsStream(ICON_PATH + iconName));
            ImageView imageView = new ImageView(image);
            imageView.setFitWidth(24);
            imageView.setFitHeight(24);
            return imageView;
        } catch (Exception e) {
            System.err.println("无法加载图标: " + iconName);
            return new ImageView();
        }
    }
    
    private void toggleSideBar() {
        if (sideBarPanelContainer != null) {
            boolean visible = sideBarPanelContainer.isVisible();
            sideBarPanelContainer.setVisible(!visible);
            sideBarPanelContainer.setManaged(!visible);
        }
    }
    
    private void toggleRightPanel() {
        if (rightTabPane != null) {
            boolean visible = rightTabPane.isVisible();
            rightTabPane.setVisible(!visible);
            rightTabPane.setManaged(!visible);
        }
    }
    
    public void updateMousePosition(double x, double y) {
        if (statusBar != null) {
            statusBar.setPosition(x, y);
        }
    }
    
    public void updateZoomInfo(double zoom, int tileLevel) {
        if (statusBar != null) {
            statusBar.setZoomRatio(zoom);
            statusBar.setTileLevel(tileLevel);
        }
    }
    
    public void setStatusMessage(String message) {
        if (statusBar != null) {
            statusBar.setMessage(message);
        }
    }

    public ChartCanvas getChartCanvas() {
        return chartCanvas;
    }

    public MainController getController() {
        return controller;
    }
    
    public SideBarManager getSideBarManager() {
        return sideBarManager;
    }
    
    public RightTabManager getRightTabManager() {
        return rightTabManager;
    }
    
    public StatusBar getStatusBar() {
        return statusBar;
    }
    
    public BorderPane getRootLayout() {
        return this;
    }

    @Override
    public void initialize() {
        synchronized (lock) {
            if (initialized) {
                return;
            }
            doInitialize();
            initialized = true;
        }
    }
    
    private void doInitialize() {
        if (dpiScaler == null) {
            dpiScaler = DPIScaler.getInstance();
        }
        dpiScaler.initialize();
        
        if (iconLoader == null) {
            iconLoader = IconLoader.getInstance();
        }
        iconLoader.setDpiScaler(dpiScaler);
        iconLoader.initialize();
        
        if (layoutManager != null) {
            layoutManager.initialize();
        }
        
        if (sideBarManager != null) {
            sideBarManager.initialize();
        }
        
        if (rightTabManager != null) {
            rightTabManager.initialize();
        }
        
        if (statusBar != null) {
            statusBar.initialize();
        }
        
        ShortcutManager.getInstance().initialize();
        
        setupEventListeners();
    }
    
    private void setupEventListeners() {
        if (chartCanvas != null) {
            chartCanvas.addEventHandler(MouseEvent.MOUSE_MOVED, e -> {
                updateMousePosition(e.getX(), e.getY());
            });
        }
        
        AppEventBus.getInstance().subscribe(AppEventType.VIEW_CHANGED, event -> {
            Object zoom = event.getData("zoom");
            Object level = event.getData("tileLevel");
            if (zoom instanceof Number && level instanceof Number) {
                updateZoomInfo(((Number) zoom).doubleValue(), ((Number) level).intValue());
            }
        });
    }

    @Override
    public void activate() {
        synchronized (lock) {
            if (!initialized) {
                initialize();
            }
            if (active) {
                return;
            }
            doActivate();
            active = true;
        }
    }
    
    private void doActivate() {
        if (layoutManager != null) {
            layoutManager.activate();
        }
        if (sideBarManager != null) {
            sideBarManager.activate();
        }
        if (rightTabManager != null) {
            rightTabManager.activate();
        }
        if (statusBar != null) {
            statusBar.activate();
        }
    }

    @Override
    public void deactivate() {
        synchronized (lock) {
            if (!active) {
                return;
            }
            doDeactivate();
            active = false;
        }
    }
    
    private void doDeactivate() {
        if (statusBar != null) {
            statusBar.deactivate();
        }
        if (rightTabManager != null) {
            rightTabManager.deactivate();
        }
        if (sideBarManager != null) {
            sideBarManager.deactivate();
        }
        if (layoutManager != null) {
            layoutManager.deactivate();
        }
    }

    @Override
    public void dispose() {
        synchronized (lock) {
            deactivate();
            if (!initialized) {
                return;
            }
            doDispose();
            initialized = false;
        }
    }
    
    private void doDispose() {
        ShortcutManager.getInstance().dispose();
        
        if (statusBar != null) {
            statusBar.dispose();
        }
        if (rightTabManager != null) {
            rightTabManager.dispose();
        }
        if (sideBarManager != null) {
            sideBarManager.dispose();
        }
        if (layoutManager != null) {
            layoutManager.dispose();
        }
        if (iconLoader != null) {
            iconLoader.dispose();
        }
        if (dpiScaler != null) {
            dpiScaler.dispose();
        }
    }
    
    @Override
    public boolean isInitialized() {
        return initialized;
    }
    
    @Override
    public boolean isActive() {
        return active;
    }
}
