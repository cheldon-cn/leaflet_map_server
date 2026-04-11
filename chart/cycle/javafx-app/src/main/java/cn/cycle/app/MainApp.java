package cn.cycle.app;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.jni.JniBridge;
import cn.cycle.app.i18n.I18nManager;
import cn.cycle.app.splash.SplashScreen;
import cn.cycle.app.theme.ThemeManager;
import cn.cycle.app.view.MainView;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.geometry.Rectangle2D;
import javafx.scene.Scene;
import javafx.stage.Screen;
import javafx.stage.Stage;

public class MainApp extends Application {

    private ChartViewer chartViewer;
    private MainView mainView;
    private SplashScreen splashScreen;
    private Stage primaryStage;

    @Override
    public void init() throws Exception {
        JniBridge.initialize();
        chartViewer = new ChartViewer();
        chartViewer.initialize();
        I18nManager.getInstance().initialize();
        ThemeManager.getInstance().initialize();
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        this.primaryStage = primaryStage;
        
        splashScreen = new SplashScreen();
        splashScreen.show();
        
        new Thread(() -> {
            try {
                splashScreen.updateProgress(0.1, "正在初始化JNI...");
                Thread.sleep(100);
                
                splashScreen.updateProgress(0.3, "正在初始化海图查看器...");
                Thread.sleep(100);
                
                splashScreen.updateProgress(0.5, "正在加载国际化资源...");
                Thread.sleep(100);
                
                splashScreen.updateProgress(0.7, "正在初始化主题管理...");
                Thread.sleep(100);
                
                splashScreen.updateProgress(0.9, "正在准备主界面...");
                Thread.sleep(100);
                
                Platform.runLater(() -> {
                    createAndShowMainWindow();
                    splashScreen.updateProgress(1.0, "启动完成");
                    try {
                        Thread.sleep(300);
                    } catch (InterruptedException e) {
                    }
                    splashScreen.close();
                });
                
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }).start();
    }
    
    private void createAndShowMainWindow() {
        mainView = new MainView(chartViewer);
        
        Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
        double width = screenBounds.getWidth() * 0.9;
        double height = screenBounds.getHeight() * 0.9;
        
        Scene scene = new Scene(mainView, width, height);
        
        String styleCss = getClass().getResource("/css/style.css").toExternalForm();
        scene.getStylesheets().add(styleCss);
        
        ThemeManager.getInstance().setScene(scene);
        
        mainView.initialize();
        mainView.activate();

        primaryStage.setTitle("Cycle Chart Viewer");
        primaryStage.setScene(scene);
        
        primaryStage.setX((screenBounds.getWidth() - width) / 2);
        primaryStage.setY((screenBounds.getHeight() - height) / 2);
        
        primaryStage.setOnCloseRequest(event -> {
            cleanup();
        });
        
        primaryStage.show();
    }

    @Override
    public void stop() throws Exception {
        cleanup();
    }

    private void cleanup() {
        if (mainView != null) {
            mainView.deactivate();
            mainView.dispose();
        }
        if (chartViewer != null) {
            chartViewer.dispose();
        }
        JniBridge.shutdown();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
