package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxSideBarPanel;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.image.Image;

import java.io.IOException;
import java.util.Map;
import java.util.function.Consumer;

/**
 * 设置面板。
 * 
 * <p>提供应用程序的各项设置功能。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class SettingsPanel implements FxSideBarPanel {

    private static final String PANEL_ID = "settings-panel";
    
    private Node content;
    private SettingsPanelController controller;
    
    public SettingsPanel() {
        loadFXML();
    }
    
    private void loadFXML() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/fxml/SettingsPanel.fxml"));
            content = loader.load();
            controller = loader.getController();
        } catch (IOException e) {
            throw new RuntimeException("Failed to load SettingsPanel.fxml", e);
        }
    }

    @Override
    public String getPanelId() {
        return PANEL_ID;
    }

    @Override
    public String getTitle() {
        return "设置";
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
    
    public SettingsPanelController getController() {
        return controller;
    }
    
    public void applySettings() {
        if (controller != null) {
            controller.applySettings();
        }
    }
    
    public void resetToDefaults() {
        if (controller != null) {
            controller.resetToDefaults();
        }
    }
    
    public void setOnSettingsChanged(Runnable callback) {
        if (controller != null) {
            controller.setOnSettingsChanged(callback);
        }
    }
    
    public void setOnHidePanel(Runnable callback) {
        if (controller != null) {
            controller.setOnHidePanel(callback);
        }
    }
    
    public Object getSetting(String key) {
        return controller != null ? controller.getSetting(key) : null;
    }
    
    public void setSetting(String key, Object value) {
        if (controller != null) {
            controller.setSetting(key, value);
        }
    }
    
    public Map<String, Object> getAllSettings() {
        return controller != null ? controller.getAllSettings() : null;
    }
}
