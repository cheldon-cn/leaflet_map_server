package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxRightTabPanel;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.control.Tab;

import java.io.IOException;
import java.util.LinkedHashMap;
import java.util.Map;

/**
 * 属性面板。
 * 
 * <p>显示选中对象的属性信息，使用FXML布局。</p>
 * 
 * @author Cycle Team
 * @version 1.2.0
 * @since 1.0.0
 */
public class PropertyPanel implements FxRightTabPanel {

    private static final String TAB_ID = "property-panel";
    
    private Node content;
    private PropertyPanelController controller;
    private Tab tab;
    
    public PropertyPanel() {
        loadFXML();
    }
    
    private void loadFXML() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/fxml/PropertyPanel.fxml"));
            content = loader.load();
            controller = loader.getController();
            
            tab = new Tab("属性");
            tab.setContent(content);
            tab.setClosable(false);
        } catch (IOException e) {
            throw new RuntimeException("Failed to load PropertyPanel.fxml", e);
        }
    }

    @Override
    public String getTabId() {
        return TAB_ID;
    }

    @Override
    public String getIcon() {
        return "/icons/property.png";
    }

    @Override
    public Tab getTab() {
        return tab;
    }

    @Override
    public Node getContent() {
        return content;
    }

    @Override
    public void onSelected() {
    }

    @Override
    public void onDeselected() {
    }
    
    @Override
    public void refresh() {
        if (controller != null) {
            controller.refreshDisplay();
        }
    }
    
    public PropertyPanelController getController() {
        return controller;
    }
    
    public void setObject(String objectName, Map<String, String> commonProps, Map<String, String> dataProps) {
        if (controller != null) {
            controller.setObject(objectName, commonProps, dataProps);
        }
    }
    
    public void setCommonProperties(Map<String, String> properties) {
        if (controller != null) {
            controller.setCommonProperties(properties);
        }
    }
    
    public void setDataProperties(Map<String, String> properties) {
        if (controller != null) {
            controller.setDataProperties(properties);
        }
    }
    
    public void addCommonProperty(String name, String value) {
        if (controller != null) {
            controller.addCommonProperty(name, value);
        }
    }
    
    public void addDataProperty(String name, String value) {
        if (controller != null) {
            controller.addDataProperty(name, value);
        }
    }
    
    public void clear() {
        if (controller != null) {
            controller.clear();
        }
    }
    
    public String getCurrentObjectName() {
        return controller != null ? controller.getCurrentObjectName() : null;
    }
    
    public boolean isPinned() {
        return controller != null && controller.isPinned();
    }
}
