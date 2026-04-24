package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxSideBarPanel;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.image.Image;

import java.io.IOException;
import java.util.List;
import java.util.function.Consumer;

/**
 * 测量工具面板。
 * 
 * <p>提供距离测量、面积测量、角度测量等功能。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class MeasurePanel implements FxSideBarPanel {

    private static final String PANEL_ID = "measure-panel";
    
    private Node content;
    private MeasurePanelController controller;
    
    public MeasurePanel() {
        loadFXML();
    }
    
    private void loadFXML() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/fxml/MeasurePanel.fxml"));
            content = loader.load();
            controller = loader.getController();
        } catch (IOException e) {
            throw new RuntimeException("Failed to load MeasurePanel.fxml", e);
        }
    }

    @Override
    public String getPanelId() {
        return PANEL_ID;
    }

    @Override
    public String getTitle() {
        return "测量工具";
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
        if (controller != null) {
            controller.onActivate();
        }
    }

    @Override
    public void onDeactivate() {
        if (controller != null) {
            controller.onDeactivate();
        }
    }
    
    public MeasurePanelController getController() {
        return controller;
    }
    
    public void setMeasureMode(MeasurePanelController.MeasureMode mode) {
        if (controller != null) {
            controller.setMeasureMode(mode);
        }
    }
    
    public void clearMeasurements() {
        if (controller != null) {
            controller.clearMeasurements();
        }
    }
    
    public void addMeasurement(MeasurePanelController.Measurement measurement) {
        if (controller != null) {
            controller.addMeasurement(measurement);
        }
    }
    
    public void setMeasurements(List<MeasurePanelController.Measurement> measurements) {
        if (controller != null) {
            controller.setMeasurements(measurements);
        }
    }
    
    public void setOnMeasurementSelected(Consumer<MeasurePanelController.Measurement> callback) {
        if (controller != null) {
            controller.setOnMeasurementSelected(callback);
        }
    }
    
    public void setOnHidePanel(Runnable callback) {
        if (controller != null) {
            controller.setOnHidePanel(callback);
        }
    }
    
    public void setOnMeasureModeChanged(Consumer<MeasurePanelController.MeasureMode> callback) {
        if (controller != null) {
            controller.setOnMeasureModeChanged(callback);
        }
    }
}
