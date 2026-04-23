package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxSideBarPanel;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.image.Image;

import java.io.IOException;

/**
 * 图层管理面板。
 * 
 * <p>管理海图图层的显示顺序和可见性，使用FXML布局。</p>
 * 
 * @author Cycle Team
 * @version 1.2.0
 * @since 1.0.0
 */
public class LayerManagerPanel implements FxSideBarPanel {

    private static final String PANEL_ID = "layer-manager-panel";
    
    private Node content;
    private LayerManagerPanelController controller;
    
    public LayerManagerPanel() {
        loadFXML();
    }
    
    private void loadFXML() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/fxml/LayerManagerPanel.fxml"));
            content = loader.load();
            controller = loader.getController();
        } catch (IOException e) {
            throw new RuntimeException("Failed to load LayerManagerPanel.fxml", e);
        }
    }

    @Override
    public String getPanelId() {
        return PANEL_ID;
    }

    @Override
    public String getTitle() {
        return "图层管理";
    }

    @Override
    public Image getIconImage() {
        return null;
    }

    @Override
    public Node getContent() {
        return content;
    }

    @Override
    public void onActivate() {
    }

    @Override
    public void onDeactivate() {
    }
    
    public LayerManagerPanelController getController() {
        return controller;
    }
    
    public void addChartLayer(String id, String name, boolean visible) {
        if (controller != null) {
            controller.addChartLayer(id, name, visible);
        }
    }
    
    public void addRouteLayer(String id, String name, boolean visible) {
        if (controller != null) {
            controller.addRouteLayer(id, name, visible);
        }
    }
    
    public void addAISLayer(String id, String name, boolean visible) {
        if (controller != null) {
            controller.addAISLayer(id, name, visible);
        }
    }
    
    public void addAlarmLayer(String id, String name, boolean visible) {
        if (controller != null) {
            controller.addAlarmLayer(id, name, visible);
        }
    }
    
    public void addOverlayLayer(String id, String name, boolean visible) {
        if (controller != null) {
            controller.addOverlayLayer(id, name, visible);
        }
    }
    
    public void removeLayer(String id) {
        if (controller != null) {
            controller.removeLayer(id);
        }
    }
    
    public void clearAllLayers() {
        if (controller != null) {
            controller.clearAllLayers();
        }
    }
    
    public void setOnLayerSelected(java.util.function.Consumer<LayerManagerPanelController.LayerNode> callback) {
        if (controller != null) {
            controller.setOnLayerSelected(callback);
        }
    }
    
    public void setOnHidePanel(Runnable callback) {
        if (controller != null) {
            controller.setOnHidePanel(callback);
        }
    }
}
