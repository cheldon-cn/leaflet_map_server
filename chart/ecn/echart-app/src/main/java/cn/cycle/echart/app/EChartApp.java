package cn.cycle.echart.app;

import cn.cycle.echart.core.LogUtil;
import cn.cycle.echart.facade.ApplicationFacade;
import cn.cycle.echart.facade.DefaultApplicationFacade;
import cn.cycle.echart.facade.FacadeException;
import cn.cycle.echart.i18n.I18nManager;
import cn.cycle.echart.theme.ThemeManager;
import cn.cycle.echart.ui.MainView;
import cn.cycle.echart.core.PlatformAdapter;
import cn.cycle.echart.core.ServiceLocator;
import cn.cycle.echart.ui.FxPlatformAdapter;
import javafx.application.Application;
import javafx.geometry.Rectangle2D;
import javafx.scene.Scene;
import javafx.stage.Screen;
import javafx.stage.Stage;

import java.util.Locale;

/**
 * 海图显示预警应用主类。
 * 
 * <p>JavaFX应用程序入口点。</p>
 * 
 * @author Cycle Team
 * @version 1.2.0
 * @since 1.0.0
 */
public class EChartApp extends Application {

    private static final String TAG = "EChartApp";

    private ApplicationFacade facade;
    private ThemeManager themeManager;
    private I18nManager i18nManager;
    private MainView mainView;
    private Stage primaryStage;
    private double deviceRatio = 0.6;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void init() throws Exception {
        super.init();
        
        LogUtil.info(TAG, "Initializing E-Chart Application...");
        
        initializePlatformAdapter();
        initializeFacade();
        initializeThemeManager();
        initializeI18nManager();
    }

    protected void initializePlatformAdapter() {
        PlatformAdapter platformAdapter = new FxPlatformAdapter();
        ServiceLocator.register(PlatformAdapter.class, platformAdapter);
    }

    protected void initializeFacade() throws FacadeException {
        facade = new DefaultApplicationFacade();
        facade.initialize();
        ServiceLocator.register(ApplicationFacade.class, facade);
    }

    protected void initializeThemeManager() {
        themeManager = new ThemeManager();
        ServiceLocator.register(ThemeManager.class, themeManager);
    }

    protected void initializeI18nManager() {
        i18nManager = I18nManager.getInstance();
        i18nManager.setLocale(Locale.getDefault());
        ServiceLocator.register(I18nManager.class, i18nManager);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        this.primaryStage = primaryStage;
        
        try {
            facade.start();
            
            mainView = new MainView();
            mainView.setStage(primaryStage);
            mainView.setThemeManager(themeManager);
            
            Scene scene = createScene();
            
            applyTheme(scene);
            
            mainView.enableWindowResize(scene);
            configureStage(primaryStage, scene);
            
            mainView.initialize();
            mainView.activate();
            
            primaryStage.show();
            
            LogUtil.info(TAG, "E-Chart Application started successfully.");
            
        } catch (Exception e) {
            System.err.println("Failed to start application: " + e.getMessage());
            e.printStackTrace();
            throw e;
        }
    }

    protected Scene createScene() {
        Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
        double initWidth = screenBounds.getWidth() * deviceRatio;
        double initHeight = screenBounds.getHeight() * deviceRatio;
        
        Scene scene = new Scene(mainView, initWidth, initHeight);
        scene.getStylesheets().add(getClass().getResource("/styles/base.css").toExternalForm());
        scene.getStylesheets().add(getClass().getResource("/styles/themes/default.css").toExternalForm());
        return scene;
    }

    protected void applyTheme(Scene scene) {
        String styleSheet = themeManager.getCurrentTheme().getStyleSheet();
        if (styleSheet != null) {
            scene.getStylesheets().add(getClass().getResource(styleSheet).toExternalForm());
        }
    }

    protected void configureStage(Stage stage, Scene scene) {
        stage.setTitle(i18nManager.getMessage("app.title", "E-Chart Display and Alarm Application"));
        stage.setScene(scene);
        stage.setMinWidth(800);
        stage.setMinHeight(600);
        stage.initStyle(javafx.stage.StageStyle.UNDECORATED);
        
        Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
        double initWidth = screenBounds.getWidth() * deviceRatio;
        double initHeight = screenBounds.getHeight() * deviceRatio;
        double initX = screenBounds.getMinX() + (screenBounds.getWidth() - initWidth) / 2;
        double initY = screenBounds.getMinY() + (screenBounds.getHeight() - initHeight) / 2;
        stage.setX(initX);
        stage.setY(initY);
        stage.setWidth(initWidth);
        stage.setHeight(initHeight);
        
        stage.setOnCloseRequest(event -> {
            event.consume();
            shutdown();
        });
    }

    @Override
    public void stop() throws Exception {
        shutdown();
        super.stop();
    }

    protected void shutdown() {
        LogUtil.info(TAG, "Shutting down E-Chart Application...");
        
        try {
            if (facade != null) {
                facade.stop();
                facade.destroy();
            }
            
            ServiceLocator.clear();
            
            LogUtil.info(TAG, "E-Chart Application shutdown complete.");
            
        } catch (Exception e) {
            System.err.println("Error during shutdown: " + e.getMessage());
            e.printStackTrace();
        }
        
        if (primaryStage != null) {
            primaryStage.close();
        }
        
        javafx.application.Platform.exit();
        System.exit(0);
    }

    public ApplicationFacade getFacade() {
        return facade;
    }

    public ThemeManager getThemeManager() {
        return themeManager;
    }

    public I18nManager getI18nManager() {
        return i18nManager;
    }

    public MainView getMainView() {
        return mainView;
    }

    public Stage getPrimaryStage() {
        return primaryStage;
    }
}
