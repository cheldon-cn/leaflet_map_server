package cn.cycle.app.panel;

import cn.cycle.app.model.DataItem;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.control.ScrollPane;
import javafx.scene.control.Tab;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;

import java.util.Map;

public class PropertiesPanel implements RightTabPanel {
    
    private static final String ID = "properties";
    private static final String TITLE = "属性";
    
    private DataItem currentDataItem;
    private VBox content;
    private GridPane propertiesGrid;
    private boolean initialized = false;
    private boolean active = false;
    
    public PropertiesPanel() {
    }
    
    private Node createContent() {
        content = new VBox(10);
        content.setPadding(new Insets(10));
        
        Label titleLabel = new Label("数据属性");
        titleLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");
        
        propertiesGrid = new GridPane();
        propertiesGrid.setHgap(10);
        propertiesGrid.setVgap(8);
        propertiesGrid.setPadding(new Insets(10, 0, 0, 0));
        
        ScrollPane scrollPane = new ScrollPane(propertiesGrid);
        scrollPane.setFitToWidth(true);
        scrollPane.setHbarPolicy(ScrollPane.ScrollBarPolicy.NEVER);
        scrollPane.setVbarPolicy(ScrollPane.ScrollBarPolicy.AS_NEEDED);
        
        VBox.setVgrow(scrollPane, javafx.scene.layout.Priority.ALWAYS);
        
        content.getChildren().addAll(titleLabel, scrollPane);
        
        showEmptyProperties();
        
        return content;
    }
    
    public void setDataItem(DataItem item) {
        this.currentDataItem = item;
        updateProperties();
    }
    
    public DataItem getDataItem() {
        return currentDataItem;
    }
    
    private void updateProperties() {
        propertiesGrid.getChildren().clear();
        
        if (currentDataItem == null) {
            showEmptyProperties();
            return;
        }
        
        int row = 0;
        
        addProperty("名称:", currentDataItem.getName(), row++);
        addProperty("类型:", currentDataItem.getDataType().getDisplayName(), row++);
        addProperty("格式:", currentDataItem.getDataFormat().getDisplayName(), row++);
        
        if (currentDataItem.getWidth() > 0 && currentDataItem.getHeight() > 0) {
            addProperty("尺寸:", currentDataItem.getDimensions(), row++);
        }
        
        addProperty("大小:", currentDataItem.getFileSizeFormatted(), row++);
        addProperty("位置:", currentDataItem.getFilePath(), row++);
        addProperty("创建时间:", currentDataItem.getCreateTimeFormatted(), row++);
        addProperty("修改时间:", currentDataItem.getModifyTimeFormatted(), row++);
    }
    
    private void addProperty(String labelText, String value, int row) {
        Label label = new Label(labelText);
        label.setStyle("-fx-font-weight: bold; -fx-text-fill: derive(-fx-text-fill, -20%);");
        
        Label valueLabel = new Label(value != null ? value : "-");
        valueLabel.setWrapText(true);
        valueLabel.setMaxWidth(200);
        
        propertiesGrid.add(label, 0, row);
        propertiesGrid.add(valueLabel, 1, row);
    }
    
    private void showEmptyProperties() {
        Label emptyLabel = new Label("请选择数据项查看属性");
        emptyLabel.setStyle("-fx-text-fill: derive(-fx-text-fill, -30%); -fx-font-style: italic;");
        propertiesGrid.add(emptyLabel, 0, 0);
    }
    
    @Override
    public String getId() {
        return ID;
    }
    
    @Override
    public String getTitle() {
        return TITLE;
    }
    
    @Override
    public Node getIcon() {
        return null;
    }
    
    @Override
    public Tab createTab() {
        Tab tab = new Tab(TITLE);
        tab.setContent(createContent());
        tab.setClosable(false);
        return tab;
    }
    
    @Override
    public void initialize() {
        initialized = true;
    }
    
    @Override
    public void activate() {
        active = true;
    }
    
    @Override
    public void deactivate() {
        active = false;
    }
    
    @Override
    public void dispose() {
        currentDataItem = null;
        initialized = false;
        active = false;
    }
    
    @Override
    public boolean isInitialized() {
        return initialized;
    }
    
    @Override
    public boolean isActive() {
        return active;
    }
    
    @Override
    public Map<String, Object> saveState() {
        return null;
    }
    
    @Override
    public void restoreState(Map<String, Object> state) {
    }
    
    @Override
    public String getStateKey() {
        return "righttab.panel." + ID;
    }
}
