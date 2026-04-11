package cn.cycle.app.context;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.app.controller.MainController;
import cn.cycle.chart.jni.JniBridge;
import javafx.stage.Stage;

public class AppContext {
    
    private static volatile AppContext INSTANCE;
    private static final Object LOCK = new Object();
    
    private ChartViewer chartViewer;
    private MainController controller;
    private Stage primaryStage;
    private volatile boolean initialized = false;
    
    private AppContext() {
    }
    
    public static AppContext getInstance() {
        if (INSTANCE == null) {
            synchronized (LOCK) {
                if (INSTANCE == null) {
                    INSTANCE = new AppContext();
                }
            }
        }
        return INSTANCE;
    }
    
    public void initialize(Stage stage, ChartViewer viewer) {
        if (initialized) {
            throw new IllegalStateException("AppContext already initialized");
        }
        
        this.primaryStage = stage;
        this.chartViewer = viewer;
        this.controller = new MainController(viewer);
        
        JniBridge.initialize();
        
        initialized = true;
    }
    
    public void shutdown() {
        if (!initialized) {
            return;
        }
        
        JniBridge.shutdown();
        
        this.chartViewer = null;
        this.controller = null;
        this.primaryStage = null;
        
        initialized = false;
    }
    
    public ChartViewer getChartViewer() {
        checkInitialized();
        return chartViewer;
    }
    
    public MainController getController() {
        checkInitialized();
        return controller;
    }
    
    public Stage getPrimaryStage() {
        checkInitialized();
        return primaryStage;
    }
    
    public boolean isInitialized() {
        return initialized;
    }
    
    private void checkInitialized() {
        if (!initialized) {
            throw new IllegalStateException("AppContext not initialized. Call initialize() first.");
        }
    }
}
