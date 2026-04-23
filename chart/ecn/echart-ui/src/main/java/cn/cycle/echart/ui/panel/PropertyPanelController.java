package cn.cycle.echart.ui.panel;

import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;

import java.net.URL;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.ResourceBundle;

public class PropertyPanelController implements Initializable {

    @FXML private Button refreshButton;
    @FXML private Button pinButton;
    @FXML private TitledPane commonPropertiesPane;
    @FXML private TitledPane dataPropertiesPane;
    @FXML private GridPane commonPropertiesGrid;
    @FXML private GridPane dataPropertiesGrid;
    @FXML private VBox emptyPlaceholder;
    
    private Map<String, String> commonProperties;
    private Map<String, String> dataProperties;
    private String currentObjectName;
    private boolean pinned;
    
    @Override
    public void initialize(URL location, ResourceBundle resources) {
        commonProperties = new LinkedHashMap<>();
        dataProperties = new LinkedHashMap<>();
        pinned = false;
        
        initializeIcons();
        showEmptyState();
    }
    
    private void initializeIcons() {
        setButtonIcon(refreshButton, "/icons/property.png");
    }
    
    private void setButtonIcon(Button button, String iconPath) {
        if (button != null && button.getGraphic() instanceof ImageView) {
            try {
                Image image = new Image(getClass().getResourceAsStream(iconPath));
                if (image != null) {
                    ((ImageView) button.getGraphic()).setImage(image);
                }
            } catch (Exception e) {
            }
        }
    }
    
    @FXML
    private void onRefresh() {
        refreshDisplay();
    }
    
    @FXML
    private void onPin() {
        pinned = !pinned;
        if (pinButton != null) {
            pinButton.setStyle(pinned ? "-fx-background-color: derive(-fx-accent, 20%);" : "");
        }
    }
    
    public void setObject(String objectName, Map<String, String> commonProps, Map<String, String> dataProps) {
        this.currentObjectName = objectName;
        this.commonProperties = commonProps != null ? new LinkedHashMap<>(commonProps) : new LinkedHashMap<>();
        this.dataProperties = dataProps != null ? new LinkedHashMap<>(dataProps) : new LinkedHashMap<>();
        
        refreshDisplay();
    }
    
    public void setCommonProperties(Map<String, String> properties) {
        this.commonProperties = properties != null ? new LinkedHashMap<>(properties) : new LinkedHashMap<>();
        refreshDisplay();
    }
    
    public void setDataProperties(Map<String, String> properties) {
        this.dataProperties = properties != null ? new LinkedHashMap<>(properties) : new LinkedHashMap<>();
        refreshDisplay();
    }
    
    public void addCommonProperty(String name, String value) {
        commonProperties.put(name, value);
        refreshDisplay();
    }
    
    public void addDataProperty(String name, String value) {
        dataProperties.put(name, value);
        refreshDisplay();
    }
    
    public void clear() {
        currentObjectName = null;
        commonProperties.clear();
        dataProperties.clear();
        showEmptyState();
    }
    
    public void refreshDisplay() {
        if (currentObjectName == null && commonProperties.isEmpty() && dataProperties.isEmpty()) {
            showEmptyState();
            return;
        }
        
        showPropertiesState();
        
        populateGrid(commonPropertiesGrid, commonProperties);
        populateGrid(dataPropertiesGrid, dataProperties);
        
        commonPropertiesPane.setVisible(!commonProperties.isEmpty());
        commonPropertiesPane.setManaged(!commonProperties.isEmpty());
        
        dataPropertiesPane.setVisible(!dataProperties.isEmpty());
        dataPropertiesPane.setManaged(!dataProperties.isEmpty());
    }
    
    private void populateGrid(GridPane grid, Map<String, String> properties) {
        grid.getChildren().clear();
        
        int row = 0;
        for (Map.Entry<String, String> entry : properties.entrySet()) {
            Label nameLabel = new Label(entry.getKey());
            nameLabel.setStyle("-fx-font-weight: bold; -fx-text-fill: -fx-text-background-color;");
            
            Label valueLabel = new Label(entry.getValue() != null ? entry.getValue() : "-");
            valueLabel.setWrapText(true);
            valueLabel.setMaxWidth(Double.MAX_VALUE);
            
            grid.add(nameLabel, 0, row);
            grid.add(valueLabel, 1, row);
            
            GridPane.setMargin(nameLabel, new Insets(2, 0, 2, 0));
            GridPane.setMargin(valueLabel, new Insets(2, 0, 2, 0));
            
            row++;
        }
        
        if (properties.isEmpty()) {
            Label emptyLabel = new Label("无属性信息");
            emptyLabel.setStyle("-fx-text-fill: gray; -fx-font-style: italic;");
            grid.add(emptyLabel, 0, 0, 2, 1);
        }
    }
    
    private void showEmptyState() {
        emptyPlaceholder.setVisible(true);
        emptyPlaceholder.setManaged(true);
        commonPropertiesPane.setVisible(false);
        commonPropertiesPane.setManaged(false);
        dataPropertiesPane.setVisible(false);
        dataPropertiesPane.setManaged(false);
    }
    
    private void showPropertiesState() {
        emptyPlaceholder.setVisible(false);
        emptyPlaceholder.setManaged(false);
    }
    
    public String getCurrentObjectName() {
        return currentObjectName;
    }
    
    public boolean isPinned() {
        return pinned;
    }
    
    public Map<String, String> getCommonProperties() {
        return new LinkedHashMap<>(commonProperties);
    }
    
    public Map<String, String> getDataProperties() {
        return new LinkedHashMap<>(dataProperties);
    }
}
