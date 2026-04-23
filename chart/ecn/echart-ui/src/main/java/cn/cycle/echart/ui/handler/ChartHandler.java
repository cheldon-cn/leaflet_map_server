package cn.cycle.echart.ui.handler;

import cn.cycle.echart.ui.SideBarManager;
import cn.cycle.echart.ui.panel.LayerManagerPanel;
import javafx.stage.FileChooser;
import javafx.stage.Stage;

import java.io.File;
import java.util.UUID;

public class ChartHandler {

    private final SideBarManager sideBarManager;
    private final MessageCallback messageCallback;
    private LayerManagerPanel layerManagerPanel;
    private Stage stage;

    public ChartHandler(SideBarManager sideBarManager, MessageCallback messageCallback) {
        this.sideBarManager = sideBarManager;
        this.messageCallback = messageCallback;
    }
    
    public void setLayerManagerPanel(LayerManagerPanel layerManagerPanel) {
        this.layerManagerPanel = layerManagerPanel;
    }
    
    public void setStage(Stage stage) {
        this.stage = stage;
    }

    public void onLoadChart() {
        if (stage == null) {
            messageCallback.showInfo("加载海图", "请先初始化窗口");
            return;
        }
        
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("打开海图数据");
        fileChooser.getExtensionFilters().addAll(
            new FileChooser.ExtensionFilter("S57海图文件", "*.000"),
            new FileChooser.ExtensionFilter("S57海图文件", "*.s57"),
            new FileChooser.ExtensionFilter("所有文件", "*.*")
        );
        
        java.util.List<File> files = fileChooser.showOpenMultipleDialog(stage);
        if (files != null && !files.isEmpty()) {
            for (File file : files) {
                loadChartFile(file);
            }
            sideBarManager.showPanel("layers");
        }
    }
    
    private void loadChartFile(File file) {
        String chartId = UUID.randomUUID().toString();
        String chartName = getChartName(file);
        
        if (layerManagerPanel != null) {
            layerManagerPanel.addChartLayer(chartId, chartName, true);
        }
        
        messageCallback.showInfo("加载海图", "已加载海图: " + chartName);
    }
    
    private String getChartName(File file) {
        String fileName = file.getName();
        int dotIndex = fileName.lastIndexOf('.');
        if (dotIndex > 0) {
            return fileName.substring(0, dotIndex);
        }
        return fileName;
    }

    public void onUnloadChart() {
        if (layerManagerPanel != null) {
            layerManagerPanel.clearAllLayers();
            messageCallback.showInfo("卸载海图", "已卸载所有海图");
        } else {
            messageCallback.showInfo("卸载海图", "卸载海图功能开发中...");
        }
    }

    public void onLayerManager() {
        sideBarManager.showPanel("layers");
    }

    public void onPropertyQuery() {
        messageCallback.showInfo("属性查询", "属性查询功能开发中...");
    }

    public void onFeatureSearch() {
        sideBarManager.showPanel("search");
    }
}
