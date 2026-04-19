package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxRightTabPanel;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;

import java.util.LinkedHashMap;
import java.util.Map;

/**
 * 属性面板。
 * 
 * <p>显示选中对象的属性信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PropertyPanel extends BorderPane implements FxRightTabPanel {

    private static final String TAB_ID = "property-panel";
    
    private final GridPane propertyGrid;
    private final ScrollPane scrollPane;
    private final Label titleLabel;
    private final Tab tab;
    
    private Map<String, String> properties;
    private int row;

    public PropertyPanel() {
        this.propertyGrid = new GridPane();
        this.scrollPane = new ScrollPane(propertyGrid);
        this.titleLabel = new Label("属性");
        this.tab = new Tab("属性");
        this.tab.setContent(this);
        this.tab.setClosable(false);
        this.properties = new LinkedHashMap<>();
        this.row = 0;
        
        initializeLayout();
    }

    private void initializeLayout() {
        propertyGrid.setHgap(10);
        propertyGrid.setVgap(5);
        propertyGrid.setStyle("-fx-padding: 10;");
        
        scrollPane.setFitToWidth(true);
        scrollPane.setFitToHeight(true);
        
        setTop(titleLabel);
        setCenter(scrollPane);
        
        getStyleClass().add("property-panel");
        
        showEmptyProperties();
    }

    private void showEmptyProperties() {
        propertyGrid.getChildren().clear();
        row = 0;
        
        Label emptyLabel = new Label("未选中任何对象");
        emptyLabel.setStyle("-fx-text-fill: gray;");
        propertyGrid.add(emptyLabel, 0, row);
    }

    public void setProperties(Map<String, String> props) {
        this.properties = new LinkedHashMap<>(props);
        refresh();
    }

    public String getPropertyValue(String name) {
        return properties.get(name);
    }

    public Map<String, String> getAllProperties() {
        return new LinkedHashMap<>(properties);
    }

    public void clear() {
        properties.clear();
        showEmptyProperties();
    }

    @Override
    public void refresh() {
        propertyGrid.getChildren().clear();
        row = 0;
        
        if (properties.isEmpty()) {
            showEmptyProperties();
            return;
        }
        
        for (Map.Entry<String, String> entry : properties.entrySet()) {
            Label nameLabel = new Label(entry.getKey() + ":");
            nameLabel.setStyle("-fx-font-weight: bold;");
            
            Label valueLabel = new Label(entry.getValue() != null ? entry.getValue() : "");
            
            propertyGrid.add(nameLabel, 0, row);
            propertyGrid.add(valueLabel, 1, row);
            row++;
        }
    }

    public void setTitle(String title) {
        titleLabel.setText(title);
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
    public javafx.scene.Node getContent() {
        return this;
    }

    @Override
    public void onSelected() {
    }

    @Override
    public void onDeselected() {
    }
}
