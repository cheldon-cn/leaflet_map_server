package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxSideBarPanel;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;

import java.util.HashMap;
import java.util.Map;
import java.util.UUID;

/**
 * 图层管理面板。
 * 
 * <p>管理海图图层的显示顺序和可见性。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class LayerManagerPanel extends BorderPane implements FxSideBarPanel {

    private static final String PANEL_ID = "layer-manager-panel";
    
    private final ListView<LayerItem> layerListView;
    private final Map<String, LayerItem> layers;
    
    public LayerManagerPanel() {
        this.layerListView = new ListView<>();
        this.layers = new HashMap<>();
        
        initializeLayout();
        loadDefaultLayers();
    }

    private void initializeLayout() {
        layerListView.setCellFactory(param -> new ListCell<LayerItem>() {
            @Override
            protected void updateItem(LayerItem item, boolean empty) {
                super.updateItem(item, empty);
                if (empty || item == null) {
                    setGraphic(null);
                    setText(null);
                } else {
                    CheckBox checkBox = new CheckBox(item.getName());
                    checkBox.setSelected(item.isVisible());
                    checkBox.selectedProperty().addListener((obs, oldVal, newVal) -> {
                        item.setVisible(newVal);
                    });
                    setGraphic(checkBox);
                }
            }
        });
        
        setTop(createToolBar());
        setCenter(layerListView);
        
        getStyleClass().add("layer-manager-panel");
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();
        
        Button addButton = new Button("添加");
        addButton.setOnAction(e -> addLayer());
        
        Button removeButton = new Button("移除");
        removeButton.setOnAction(e -> removeLayer());
        
        Button upButton = new Button("上移");
        upButton.setOnAction(e -> moveUp());
        
        Button downButton = new Button("下移");
        downButton.setOnAction(e -> moveDown());
        
        toolBar.getItems().addAll(addButton, removeButton, new Separator(), upButton, downButton);
        return toolBar;
    }

    private void loadDefaultLayers() {
        addLayer("水深", true);
        addLayer("等深线", true);
        addLayer("海岸线", true);
        addLayer("航标", true);
        addLayer("航道", true);
        addLayer("地名标注", true);
    }

    private void addLayer() {
        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("添加图层");
        dialog.setHeaderText("请输入图层名称");
        dialog.setContentText("名称:");
        
        dialog.showAndWait().ifPresent(name -> {
            if (!name.trim().isEmpty()) {
                addLayer(name.trim(), true);
            }
        });
    }

    public void addLayer(String name, boolean visible) {
        LayerItem layer = new LayerItem(name, visible);
        layers.put(layer.getLayerId(), layer);
        layerListView.getItems().add(layer);
    }

    private void removeLayer() {
        LayerItem selected = layerListView.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showWarning("请先选择一个图层");
            return;
        }
        
        layers.remove(selected.getLayerId());
        layerListView.getItems().remove(selected);
    }

    private void moveUp() {
        int index = layerListView.getSelectionModel().getSelectedIndex();
        if (index <= 0) {
            return;
        }
        
        LayerItem item = layerListView.getItems().remove(index);
        layerListView.getItems().add(index - 1, item);
        layerListView.getSelectionModel().select(index - 1);
    }

    private void moveDown() {
        int index = layerListView.getSelectionModel().getSelectedIndex();
        if (index < 0 || index >= layerListView.getItems().size() - 1) {
            return;
        }
        
        LayerItem item = layerListView.getItems().remove(index);
        layerListView.getItems().add(index + 1, item);
        layerListView.getSelectionModel().select(index + 1);
    }

    private void showWarning(String message) {
        Alert alert = new Alert(Alert.AlertType.WARNING);
        alert.setTitle("警告");
        alert.setContentText(message);
        alert.showAndWait();
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
    public javafx.scene.Node getContent() {
        return this;
    }

    @Override
    public void onActivate() {
    }

    @Override
    public void onDeactivate() {
    }

    public static class LayerItem {
        private final String layerId;
        private final String name;
        private boolean visible;

        public LayerItem(String name, boolean visible) {
            this.layerId = UUID.randomUUID().toString();
            this.name = name;
            this.visible = visible;
        }

        public String getLayerId() {
            return layerId;
        }

        public String getName() {
            return name;
        }

        public boolean isVisible() {
            return visible;
        }

        public void setVisible(boolean visible) {
            this.visible = visible;
        }
    }
}
