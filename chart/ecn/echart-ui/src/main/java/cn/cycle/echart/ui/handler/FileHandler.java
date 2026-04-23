package cn.cycle.echart.ui.handler;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventBus;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.render.ChartRenderService;
import cn.cycle.echart.ui.ChartDisplayArea;
import cn.cycle.echart.ui.StatusBar;
import cn.cycle.echart.ui.service.ChartLoadService;
import javafx.animation.AnimationTimer;
import javafx.concurrent.WorkerStateEvent;
import javafx.event.EventHandler;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.File;

public class FileHandler {

    private static final long RENDER_THROTTLE_INTERVAL = 50_000_000L;
    
    private final Stage stage;
    private final ChartDisplayArea chartDisplayArea;
    private final StatusBar statusBar;
    private final MessageCallback messageCallback;
    private final AppEventBus eventBus;
    
    private ChartRenderService chartRenderService;
    private ChartLoadService chartLoadService;
    private boolean isChartLoaded = false;
    
    private double pendingPanDx = 0;
    private double pendingPanDy = 0;
    private double pendingZoomFactor = 1.0;
    private double pendingZoomPivotX = 0;
    private double pendingZoomPivotY = 0;
    private boolean hasPendingPan = false;
    private boolean hasPendingZoom = false;
    private long lastRenderTime = 0;
    private final AnimationTimer renderThrottleTimer;
    private boolean isThrottleTimerRunning = false;

    public FileHandler(Stage stage, ChartDisplayArea chartDisplayArea, 
                       StatusBar statusBar, MessageCallback messageCallback) {
        this(stage, chartDisplayArea, statusBar, messageCallback, AppEventBus.getInstance());
    }
    
    public FileHandler(Stage stage, ChartDisplayArea chartDisplayArea, 
                       StatusBar statusBar, MessageCallback messageCallback,
                       AppEventBus eventBus) {
        this.stage = stage;
        this.chartDisplayArea = chartDisplayArea;
        this.statusBar = statusBar;
        this.messageCallback = messageCallback;
        this.eventBus = eventBus;
        this.renderThrottleTimer = createRenderThrottleTimer();
        this.chartLoadService = new ChartLoadService();
        setupLoadServiceHandlers();
    }
    
    private AnimationTimer createRenderThrottleTimer() {
        return new AnimationTimer() {
            @Override
            public void handle(long now) {
                if (now - lastRenderTime < RENDER_THROTTLE_INTERVAL) {
                    return;
                }
                
                if (hasPendingPan || hasPendingZoom) {
                    executeThrottledRender();
                    lastRenderTime = now;
                }
            }
        };
    }
    
    private void setupLoadServiceHandlers() {
        chartLoadService.setOnRunning(new EventHandler<WorkerStateEvent>() {
            @Override
            public void handle(WorkerStateEvent event) {
                statusBar.showMessage("正在加载海图...");
            }
        });
        
        chartLoadService.setOnSucceeded(new EventHandler<WorkerStateEvent>() {
            @Override
            public void handle(WorkerStateEvent event) {
                ChartLoadService.LoadResult result = chartLoadService.getValue();
                if (result.isSuccess()) {
                    AppEvent chartEvent = new AppEvent(FileHandler.this, AppEventType.CHART_LOADED)
                        .withData("pixels", result.getPixels())
                        .withData("width", result.getWidth())
                        .withData("height", result.getHeight())
                        .withData("fileName", result.getFileName());
                    eventBus.publish(chartEvent);
                    isChartLoaded = true;
                    chartRenderService = chartLoadService.getChartRenderService();
                    statusBar.showMessage("已打开海图: " + result.getFileName());
                    
                    if (!isThrottleTimerRunning) {
                        renderThrottleTimer.start();
                        isThrottleTimerRunning = true;
                    }
                } else {
                    AppEvent errorEvent = new AppEvent(FileHandler.this, AppEventType.CHART_LOADED)
                        .withData("success", false)
                        .withData("fileName", result.getFileName())
                        .withData("errorMessage", result.getErrorMessage());
                    eventBus.publish(errorEvent);
                    messageCallback.showInfo("加载失败", 
                            "无法加载海图文件: " + result.getFileName() + 
                            "\n错误: " + result.getErrorMessage());
                }
            }
        });
        
        chartLoadService.setOnFailed(new EventHandler<WorkerStateEvent>() {
            @Override
            public void handle(WorkerStateEvent event) {
                Throwable exception = chartLoadService.getException();
                String errorMsg = exception != null ? exception.getMessage() : "未知错误";
                AppEvent errorEvent = new AppEvent(FileHandler.this, AppEventType.CHART_LOADED)
                    .withData("success", false)
                    .withData("errorMessage", errorMsg);
                eventBus.publish(errorEvent);
                messageCallback.showInfo("加载失败", "打开海图时发生错误: " + errorMsg);
            }
        });
    }

    public void onNewWorkspace() {
        messageCallback.showInfo("新建工作区", "新建工作区功能开发中...");
    }

    public void onOpenWorkspace() {
        openFile();
    }

    public void onSaveWorkspace() {
        messageCallback.showInfo("保存工作区", "保存工作区功能开发中...");
    }

    public void onExport() {
        messageCallback.showInfo("导出", "导出功能开发中...");
    }

    public void onPrint() {
        messageCallback.showInfo("打印", "打印功能开发中...");
    }

    private void openFile() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("打开文件");
        
        FileChooser.ExtensionFilter imageFilter = new FileChooser.ExtensionFilter(
                "图片文件 (*.png, *.jpg, *.bmp, *.webp)", 
                "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.webp");
        FileChooser.ExtensionFilter chartFilter = new FileChooser.ExtensionFilter(
                "海图数据 (*.000)", "*.000");
        FileChooser.ExtensionFilter allFilter = new FileChooser.ExtensionFilter(
                "所有文件 (*.*)", "*.*");
        
        fileChooser.getExtensionFilters().addAll(chartFilter,imageFilter,  allFilter);
        
        File selectedFile = fileChooser.showOpenDialog(stage);
        if (selectedFile == null) {
            return;
        }
        
        String fileName = selectedFile.getName().toLowerCase();
        
        if (isImageFile(fileName)) {
            openImageFile(selectedFile);
        } else if (fileName.endsWith(".000")) {
            openChartFileAsync(selectedFile);
        } else {
            messageCallback.showInfo("不支持的文件类型", "不支持的文件格式: " + fileName);
        }
    }

    private boolean isImageFile(String fileName) {
        return fileName.endsWith(".png") || 
               fileName.endsWith(".jpg") || 
               fileName.endsWith(".jpeg") || 
               fileName.endsWith(".bmp") || 
               fileName.endsWith(".webp");
    }

    private void openImageFile(File imageFile) {
        boolean success = chartDisplayArea.loadImage(imageFile);
        if (success) {
            statusBar.showMessage("已打开图片: " + imageFile.getName());
        } else {
            messageCallback.showInfo("打开失败", "无法加载图片文件: " + imageFile.getName());
        }
    }

    private void openChartFileAsync(File chartFile) {
        if (chartLoadService.isRunning()) {
            chartLoadService.cancel();
        }
        
        if (chartLoadService.getState() == javafx.concurrent.Worker.State.SUCCEEDED ||
            chartLoadService.getState() == javafx.concurrent.Worker.State.CANCELLED ||
            chartLoadService.getState() == javafx.concurrent.Worker.State.FAILED) {
            chartLoadService.reset();
        }
        
        int renderWidth = (int) chartDisplayArea.getWidth();
        int renderHeight = (int) chartDisplayArea.getHeight();
        
        if (renderWidth <= 0 || renderHeight <= 0) {
            renderWidth = 800;
            renderHeight = 600;
        }
        
        chartLoadService.setChartFile(chartFile);
        chartLoadService.setRenderSize(renderWidth, renderHeight);
        if (chartRenderService != null) {
            chartLoadService.setChartRenderService(chartRenderService);
        }
        
        setupViewportChangeListener();
        chartLoadService.start();
    }

    private void setupViewportChangeListener() {
        chartDisplayArea.setViewportChangeListener(new ChartDisplayArea.ViewportChangeListener() {
            @Override
            public void onViewportChanged(double centerX, double centerY, double scale) {
            }
            
            @Override
            public void onPan(double dx, double dy) {
                if (!isChartLoaded || chartRenderService == null) {
                    return;
                }
                schedulePanRender(dx, dy);
            }
            
            @Override
            public void onZoom(double factor, double pivotX, double pivotY) {
                if (!isChartLoaded || chartRenderService == null) {
                    return;
                }
                scheduleZoomRender(factor, pivotX, pivotY);
            }
        });
    }
    
    private void schedulePanRender(double dx, double dy) {
        pendingPanDx += dx;
        pendingPanDy += dy;
        hasPendingPan = true;
    }
    
    private void scheduleZoomRender(double factor, double pivotX, double pivotY) {
        pendingZoomFactor *= factor;
        pendingZoomPivotX = pivotX;
        pendingZoomPivotY = pivotY;
        hasPendingZoom = true;
    }
    
    private void executeThrottledRender() {
        if (chartRenderService == null || !chartRenderService.hasChartLoaded()) {
            resetPendingOperations();
            return;
        }
        
        int renderWidth = (int) chartDisplayArea.getWidth();
        int renderHeight = (int) chartDisplayArea.getHeight();
        
        if (renderWidth <= 0 || renderHeight <= 0) {
            resetPendingOperations();
            return;
        }
        
        ChartRenderService.RenderResult result = null;
        
        if (hasPendingZoom) {
            result = chartRenderService.zoomAndRender(
                    renderWidth, renderHeight, pendingZoomFactor, 
                    pendingZoomPivotX, pendingZoomPivotY);
        } else if (hasPendingPan) {
            result = chartRenderService.panAndRender(
                    renderWidth, renderHeight, pendingPanDx, pendingPanDy);
        }
        
        if (result != null && result.isSuccess()) {
            AppEvent updateEvent = new AppEvent(this, AppEventType.CHART_UPDATED)
                .withData("pixels", result.getPixels())
                .withData("width", result.getWidth())
                .withData("height", result.getHeight());
            eventBus.publish(updateEvent);
        }
        
        resetPendingOperations();
    }
    
    private void resetPendingOperations() {
        pendingPanDx = 0;
        pendingPanDy = 0;
        pendingZoomFactor = 1.0;
        pendingZoomPivotX = 0;
        pendingZoomPivotY = 0;
        hasPendingPan = false;
        hasPendingZoom = false;
    }
    
    public void dispose() {
        renderThrottleTimer.stop();
        isThrottleTimerRunning = false;
        chartLoadService.cancel();
        if (chartRenderService != null) {
            chartRenderService.dispose();
        }
    }
}
