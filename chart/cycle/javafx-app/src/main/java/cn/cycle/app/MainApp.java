package cn.cycle.app;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.jni.JniBridge;
import cn.cycle.app.view.MainView;
import javafx.application.Application;
import javafx.scene.Scene;
import javafx.stage.Stage;

public class MainApp extends Application {

    private ChartViewer chartViewer;

    @Override
    public void init() throws Exception {
        JniBridge.initialize();
        chartViewer = new ChartViewer();
        chartViewer.initialize();
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        MainView mainView = new MainView(chartViewer);

        Scene scene = new Scene(mainView, 1200, 800);
        scene.getStylesheets().add(
            getClass().getResource("/css/style.css").toExternalForm());

        primaryStage.setTitle("Cycle Chart Viewer");
        primaryStage.setScene(scene);
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
        if (chartViewer != null && !chartViewer.isDisposed()) {
            chartViewer.close();
        }
        JniBridge.shutdown();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
