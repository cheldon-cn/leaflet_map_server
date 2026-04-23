package cn.cycle.echart.ui.handler;

import cn.cycle.echart.render.ChartRenderService;
import cn.cycle.echart.ui.ChartDisplayArea;
import cn.cycle.echart.ui.StatusBar;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.File;

public class FileHandler {

    private final Stage stage;
    private final ChartDisplayArea chartDisplayArea;
    private final StatusBar statusBar;
    private final MessageCallback messageCallback;
    
    private ChartRenderService chartRenderService;
    private boolean isChartLoaded = false;

    public FileHandler(Stage stage, ChartDisplayArea chartDisplayArea, 
                       StatusBar statusBar, MessageCallback messageCallback) {
        this.stage = stage;
        this.chartDisplayArea = chartDisplayArea;
        this.statusBar = statusBar;
        this.messageCallback = messageCallback;
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
            openChartFile(selectedFile);
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

    private void openChartFile(File chartFile) {
        try {
            if (chartRenderService == null) {
                chartRenderService = new ChartRenderService();
                setupViewportChangeListener();
            }
            
            int renderWidth = (int) chartDisplayArea.getWidth();
            int renderHeight = (int) chartDisplayArea.getHeight();
            
            if (renderWidth <= 0 || renderHeight <= 0) {
                renderWidth = 800;
                renderHeight = 600;
            }
            
            ChartRenderService.RenderResult result = chartRenderService.loadAndRender(
                    chartFile.getAbsolutePath(), renderWidth, renderHeight);
            
            if (result == null) {
                messageCallback.showInfo("渲染失败", "无法渲染海图");
                return;
            }
            
            if (!result.isSuccess()) {
                messageCallback.showInfo("加载失败", "无法加载海图文件: " + chartFile.getName() + 
                        "\n错误: " + result.getErrorMessage());
                return;
            }
            
            boolean success = chartDisplayArea.loadChartImage(
                    result.getPixels(), result.getWidth(), result.getHeight());
            
            if (success) {
                isChartLoaded = true;
                statusBar.showMessage("已打开海图: " + chartFile.getName());
            } else {
                messageCallback.showInfo("显示失败", "无法显示海图图像");
            }
            
        } catch (UnsatisfiedLinkError e) {
            messageCallback.showInfo("本地库错误", "无法加载本地渲染库: " + e.getMessage());
        } catch (Exception e) {
            messageCallback.showInfo("错误", "打开海图时发生错误: " + e.getMessage());
        }
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
                panAndRender(dx, dy);
            }
            
            @Override
            public void onZoom(double factor, double pivotX, double pivotY) {
                if (!isChartLoaded || chartRenderService == null) {
                    return;
                }
                zoomAndRender(factor, pivotX, pivotY);
            }
        });
    }

    private void panAndRender(double dx, double dy) {
        if (chartRenderService == null || !chartRenderService.hasChartLoaded()) {
            return;
        }
        
        int renderWidth = (int) chartDisplayArea.getWidth();
        int renderHeight = (int) chartDisplayArea.getHeight();
        
        if (renderWidth <= 0 || renderHeight <= 0) {
            return;
        }
        
        ChartRenderService.RenderResult result = chartRenderService.panAndRender(
                renderWidth, renderHeight, dx, dy);
        
        if (result != null && result.isSuccess()) {
            chartDisplayArea.loadChartImage(result.getPixels(), result.getWidth(), result.getHeight());
        }
    }

    private void zoomAndRender(double factor, double pivotX, double pivotY) {
        if (chartRenderService == null || !chartRenderService.hasChartLoaded()) {
            return;
        }
        
        int renderWidth = (int) chartDisplayArea.getWidth();
        int renderHeight = (int) chartDisplayArea.getHeight();
        
        if (renderWidth <= 0 || renderHeight <= 0) {
            return;
        }
        
        ChartRenderService.RenderResult result = chartRenderService.zoomAndRender(
                renderWidth, renderHeight, factor, pivotX, pivotY);
        
        if (result != null && result.isSuccess()) {
            chartDisplayArea.loadChartImage(result.getPixels(), result.getWidth(), result.getHeight());
        }
    }
}
