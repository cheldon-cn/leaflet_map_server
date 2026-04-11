package cn.cycle.app.loading;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.application.Platform;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.scene.control.Label;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

public class LoadingStateManager extends AbstractLifecycleComponent {
    
    private static final LoadingStateManager INSTANCE = new LoadingStateManager();
    
    private final AtomicBoolean isLoading = new AtomicBoolean(false);
    private final AtomicInteger loadingCount = new AtomicInteger(0);
    private final Object lock = new Object();
    
    private StackPane loadingOverlay;
    private ProgressIndicator progressIndicator;
    private Label statusLabel;
    
    private LoadingStateManager() {
    }
    
    public static LoadingStateManager getInstance() {
        return INSTANCE;
    }
    
    public void setLoadingOverlay(StackPane overlay) {
        this.loadingOverlay = overlay;
        if (overlay != null) {
            createLoadingUI();
        }
    }
    
    private void createLoadingUI() {
        progressIndicator = new ProgressIndicator();
        progressIndicator.setMaxSize(60, 60);
        
        statusLabel = new Label("加载中...");
        statusLabel.setStyle("-fx-text-fill: white; -fx-font-size: 14px;");
        
        VBox loadingBox = new VBox(10, progressIndicator, statusLabel);
        loadingBox.setStyle("-fx-background-color: rgba(0, 0, 0, 0.5); -fx-padding: 20; -fx-background-radius: 10;");
        loadingBox.setAlignment(javafx.geometry.Pos.CENTER);
        
        loadingOverlay.getChildren().add(loadingBox);
        loadingOverlay.setVisible(false);
    }
    
    public void showLoading() {
        showLoading("加载中...");
    }
    
    public void showLoading(String message) {
        if (loadingCount.incrementAndGet() == 1) {
            isLoading.set(true);
            Platform.runLater(() -> {
                if (statusLabel != null) {
                    statusLabel.setText(message);
                }
                if (loadingOverlay != null) {
                    loadingOverlay.setVisible(true);
                }
            });
            
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.LOADING_STATE_CHANGED, this)
                    .withData("loading", true)
                    .withData("message", message)
            );
        }
    }
    
    public void hideLoading() {
        if (loadingCount.decrementAndGet() <= 0) {
            loadingCount.set(0);
            isLoading.set(false);
            Platform.runLater(() -> {
                if (loadingOverlay != null) {
                    loadingOverlay.setVisible(false);
                }
            });
            
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.LOADING_STATE_CHANGED, this)
                    .withData("loading", false)
            );
        }
    }
    
    public void updateProgress(double progress) {
        updateProgress(progress, null);
    }
    
    public void updateProgress(double progress, String message) {
        Platform.runLater(() -> {
            if (progressIndicator != null) {
                if (progress < 0) {
                    progressIndicator.setProgress(-1);
                } else {
                    progressIndicator.setProgress(progress);
                }
            }
            if (message != null && statusLabel != null) {
                statusLabel.setText(message);
            }
        });
    }
    
    public boolean isLoading() {
        return isLoading.get();
    }
    
    public int getLoadingCount() {
        return loadingCount.get();
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        isLoading.set(false);
        loadingCount.set(0);
        loadingOverlay = null;
        progressIndicator = null;
        statusLabel = null;
    }
}
