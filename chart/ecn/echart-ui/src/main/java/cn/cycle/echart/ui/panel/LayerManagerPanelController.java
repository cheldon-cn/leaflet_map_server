package cn.cycle.echart.ui.panel;

import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleStringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.control.cell.CheckBoxTreeCell;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

import java.net.URL;
import java.util.*;
import java.util.function.Consumer;

public class LayerManagerPanelController implements Initializable {

    @FXML private Button configButton;
    @FXML private Button hideButton;
    @FXML private Button addButton;
    @FXML private Button removeButton;
    @FXML private Button moveUpButton;
    @FXML private Button moveDownButton;
    @FXML private Button expandAllButton;
    @FXML private Button collapseAllButton;
    @FXML private TreeView<LayerNode> layerTreeView;
    @FXML private ContextMenu treeContextMenu;
    
    private TreeItem<LayerNode> rootNode;
    private Map<String, TreeItem<LayerNode>> categoryNodes;
    private Consumer<LayerNode> onLayerSelected;
    private Consumer<LayerNode> onLayerAction;
    private Runnable onHidePanel;
    private Runnable onConfigPanel;
    private Consumer<LayerNode> onViewProperties;
    private boolean showLayerIcon = true;
    private boolean autoExpandChart = true;
    
    private static final String CATEGORY_CHART = "海图数据";
    private static final String CATEGORY_ROUTE = "航线数据";
    private static final String CATEGORY_AIS = "AIS数据";
    private static final String CATEGORY_ALARM = "预警区域";
    private static final String CATEGORY_OVERLAY = "叠加图层";
    
    @Override
    public void initialize(URL location, ResourceBundle resources) {
        categoryNodes = new LinkedHashMap<>();
        initializeTree();
        initializeIcons();
        initializeListeners();
    }
    
    private void initializeTree() {
        LayerNode rootData = new LayerNode("root", "图层", LayerNodeType.ROOT);
        rootNode = new TreeItem<>(rootData);
        rootNode.setExpanded(true);
        
        addCategory(CATEGORY_CHART, LayerNodeType.CATEGORY_CHART);
        addCategory(CATEGORY_ROUTE, LayerNodeType.CATEGORY_ROUTE);
        addCategory(CATEGORY_AIS, LayerNodeType.CATEGORY_AIS);
        addCategory(CATEGORY_ALARM, LayerNodeType.CATEGORY_ALARM);
        addCategory(CATEGORY_OVERLAY, LayerNodeType.CATEGORY_OVERLAY);
        
        layerTreeView.setRoot(rootNode);
        layerTreeView.setShowRoot(false);
        
        layerTreeView.setCellFactory(tree -> new LayerTreeCell());
    }
    
    private void addCategory(String name, LayerNodeType type) {
        LayerNode categoryData = new LayerNode(name.toLowerCase().replace(" ", "-"), name, type);
        TreeItem<LayerNode> categoryItem = new TreeItem<>(categoryData);
        categoryItem.setExpanded(true);
        rootNode.getChildren().add(categoryItem);
        categoryNodes.put(name, categoryItem);
    }
    
    private void initializeIcons() {
        setButtonIcon(configButton, "/icons/settings.png");
        setButtonIcon(hideButton, "/icons/sidebar_32x321.png");
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
    
    private void initializeListeners() {
        layerTreeView.getSelectionModel().selectedItemProperty().addListener((obs, oldItem, newItem) -> {
            if (newItem != null && onLayerSelected != null) {
                onLayerSelected.accept(newItem.getValue());
            }
        });
        
        layerTreeView.setOnContextMenuRequested(event -> {
            TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
            if (selectedItem != null && selectedItem.getValue().getType() != LayerNodeType.ROOT) {
                treeContextMenu.show(layerTreeView, event.getScreenX(), event.getScreenY());
            }
        });
    }
    
    @FXML
    private void onConfig() {
        Dialog<Void> dialog = new Dialog<>();
        dialog.setTitle("面板配置");
        dialog.setHeaderText("图层管理面板配置");
        
        VBox content = new VBox(15);
        content.setPadding(new Insets(20));
        
        CheckBox showIconCheckBox = new CheckBox("显示图层图标");
        showIconCheckBox.setSelected(showLayerIcon);
        
        CheckBox autoExpandCheckBox = new CheckBox("自动展开海图节点");
        autoExpandCheckBox.setSelected(autoExpandChart);
        
        content.getChildren().addAll(showIconCheckBox, autoExpandCheckBox);
        
        ButtonType okButton = new ButtonType("确定", ButtonBar.ButtonData.OK_DONE);
        ButtonType cancelButton = new ButtonType("取消", ButtonBar.ButtonData.CANCEL_CLOSE);
        dialog.getDialogPane().getButtonTypes().addAll(okButton, cancelButton);
        dialog.getDialogPane().setContent(content);
        
        dialog.setResultConverter(buttonType -> {
            if (buttonType == okButton) {
                showLayerIcon = showIconCheckBox.isSelected();
                autoExpandChart = autoExpandCheckBox.isSelected();
                refreshTreeDisplay();
            }
            return null;
        });
        
        dialog.showAndWait();
        
        if (onConfigPanel != null) {
            onConfigPanel.run();
        }
    }
    
    private void refreshTreeDisplay() {
        layerTreeView.refresh();
    }
    
    @FXML
    private void onHide() {
        if (onHidePanel != null) {
            onHidePanel.run();
        }
    }
    
    @FXML
    private void onAddLayer() {
        TextInputDialog dialog = new TextInputDialog();
        dialog.setTitle("添加图层");
        dialog.setHeaderText("请输入图层名称");
        dialog.setContentText("名称:");
        
        dialog.showAndWait().ifPresent(name -> {
            if (!name.trim().isEmpty()) {
                addLayerItem(CATEGORY_OVERLAY, name.trim(), name.trim(), true);
            }
        });
    }
    
    @FXML
    private void onRemoveLayer() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem != null && selectedItem.getParent() != rootNode) {
            selectedItem.getParent().getChildren().remove(selectedItem);
        }
    }
    
    @FXML
    private void onMoveUp() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem == null || selectedItem.getParent() == rootNode) {
            return;
        }
        
        ObservableList<TreeItem<LayerNode>> siblings = selectedItem.getParent().getChildren();
        int index = siblings.indexOf(selectedItem);
        if (index > 0) {
            siblings.remove(index);
            siblings.add(index - 1, selectedItem);
            layerTreeView.getSelectionModel().select(selectedItem);
        }
    }
    
    @FXML
    private void onMoveDown() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem == null || selectedItem.getParent() == rootNode) {
            return;
        }
        
        ObservableList<TreeItem<LayerNode>> siblings = selectedItem.getParent().getChildren();
        int index = siblings.indexOf(selectedItem);
        if (index < siblings.size() - 1) {
            siblings.remove(index);
            siblings.add(index + 1, selectedItem);
            layerTreeView.getSelectionModel().select(selectedItem);
        }
    }
    
    @FXML
    private void onExpandAll() {
        expandCollapseAll(rootNode, true);
    }
    
    @FXML
    private void onCollapseAll() {
        expandCollapseAll(rootNode, false);
    }
    
    private void expandCollapseAll(TreeItem<LayerNode> item, boolean expand) {
        item.setExpanded(expand);
        for (TreeItem<LayerNode> child : item.getChildren()) {
            expandCollapseAll(child, expand);
        }
    }
    
    @FXML
    private void onToggleVisibility() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem != null) {
            LayerNode node = selectedItem.getValue();
            node.setVisible(!node.isVisible());
        }
    }
    
    @FXML
    private void onRename() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem == null) {
            return;
        }
        
        LayerNode node = selectedItem.getValue();
        if (node.getType().isCategory()) {
            return;
        }
        
        TextInputDialog dialog = new TextInputDialog(node.getName());
        dialog.setTitle("重命名");
        dialog.setHeaderText("请输入新名称");
        dialog.setContentText("名称:");
        
        dialog.showAndWait().ifPresent(name -> {
            if (!name.trim().isEmpty()) {
                node.setName(name.trim());
            }
        });
    }
    
    @FXML
    private void onLocateLayer() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem != null && onLayerAction != null) {
            onLayerAction.accept(selectedItem.getValue());
        }
    }
    
    @FXML
    private void onViewProperties() {
        TreeItem<LayerNode> selectedItem = layerTreeView.getSelectionModel().getSelectedItem();
        if (selectedItem != null) {
            LayerNode node = selectedItem.getValue();
            if (onViewProperties != null) {
                onViewProperties.accept(node);
            }
        }
    }
    
    @FXML
    private void onDeleteLayer() {
        onRemoveLayer();
    }
    
    public void addChartLayer(String id, String name, boolean visible) {
        addLayerItem(CATEGORY_CHART, id, name, visible);
    }
    
    public void addRouteLayer(String id, String name, boolean visible) {
        addLayerItem(CATEGORY_ROUTE, id, name, visible);
    }
    
    public void addAISLayer(String id, String name, boolean visible) {
        addLayerItem(CATEGORY_AIS, id, name, visible);
    }
    
    public void addAlarmLayer(String id, String name, boolean visible) {
        addLayerItem(CATEGORY_ALARM, id, name, visible);
    }
    
    public void addOverlayLayer(String id, String name, boolean visible) {
        addLayerItem(CATEGORY_OVERLAY, id, name, visible);
    }
    
    private void addLayerItem(String category, String id, String name, boolean visible) {
        TreeItem<LayerNode> categoryItem = categoryNodes.get(category);
        if (categoryItem == null) {
            categoryItem = categoryNodes.get(CATEGORY_OVERLAY);
        }
        
        if (categoryItem != null) {
            LayerNodeType type = LayerNodeType.DATA;
            if (category == CATEGORY_CHART) type = LayerNodeType.DATA_CHART;
            else if (category == CATEGORY_ROUTE) type = LayerNodeType.DATA_ROUTE;
            else if (category == CATEGORY_AIS) type = LayerNodeType.DATA_AIS;
            else if (category == CATEGORY_ALARM) type = LayerNodeType.DATA_ALARM;
            
            LayerNode nodeData = new LayerNode(id, name, type);
            nodeData.setVisible(visible);
            TreeItem<LayerNode> nodeItem = new TreeItem<>(nodeData);
            categoryItem.getChildren().add(nodeItem);
        }
    }
    
    public void removeLayer(String id) {
        for (TreeItem<LayerNode> category : rootNode.getChildren()) {
            for (TreeItem<LayerNode> item : new ArrayList<>(category.getChildren())) {
                if (item.getValue().getId().equals(id)) {
                    category.getChildren().remove(item);
                    return;
                }
            }
        }
    }
    
    public void clearCategory(String category) {
        TreeItem<LayerNode> categoryItem = categoryNodes.get(category);
        if (categoryItem != null) {
            categoryItem.getChildren().clear();
        }
    }
    
    public void clearAllLayers() {
        for (TreeItem<LayerNode> category : rootNode.getChildren()) {
            category.getChildren().clear();
        }
    }
    
    public void setOnLayerSelected(Consumer<LayerNode> callback) {
        this.onLayerSelected = callback;
    }
    
    public void setOnLayerAction(Consumer<LayerNode> callback) {
        this.onLayerAction = callback;
    }
    
    public void setOnHidePanel(Runnable callback) {
        this.onHidePanel = callback;
    }
    
    public void setOnConfigPanel(Runnable callback) {
        this.onConfigPanel = callback;
    }
    
    public void setOnViewProperties(Consumer<LayerNode> callback) {
        this.onViewProperties = callback;
    }
    
    public TreeView<LayerNode> getLayerTreeView() {
        return layerTreeView;
    }
    
    private static class LayerTreeCell extends TreeCell<LayerNode> {
        private final HBox content;
        private final CheckBox checkBox;
        private final Label label;
        private final ImageView iconView;
        
        public LayerTreeCell() {
            checkBox = new CheckBox();
            label = new Label();
            iconView = new ImageView();
            iconView.setFitWidth(16);
            iconView.setFitHeight(16);
            iconView.setPreserveRatio(true);
            content = new HBox(8, checkBox, iconView, label);
            content.setAlignment(javafx.geometry.Pos.CENTER_LEFT);
        }
        
        @Override
        protected void updateItem(LayerNode item, boolean empty) {
            super.updateItem(item, empty);
            
            if (empty || item == null) {
                setGraphic(null);
                setText(null);
            } else {
                label.setText(item.getName());
                checkBox.setSelected(item.isVisible());
                checkBox.selectedProperty().addListener((obs, oldVal, newVal) -> {
                    item.setVisible(newVal);
                });
                
                updateIcon(item.getType());
                
                setGraphic(content);
                setText(null);
                
                updateStyle(item.getType());
            }
        }
        
        private void updateIcon(LayerNodeType type) {
            String iconPath = getIconPathForType(type);
            if (iconPath != null) {
                try {
                    Image icon = new Image(getClass().getResourceAsStream(iconPath));
                    if (icon != null) {
                        iconView.setImage(icon);
                        iconView.setVisible(true);
                        iconView.setManaged(true);
                    } else {
                        iconView.setVisible(false);
                        iconView.setManaged(false);
                    }
                } catch (Exception e) {
                    iconView.setVisible(false);
                    iconView.setManaged(false);
                }
            } else {
                iconView.setVisible(false);
                iconView.setManaged(false);
            }
        }
        
        private String getIconPathForType(LayerNodeType type) {
            switch (type) {
                case CATEGORY_CHART:
                    return "/icons/layers.png";
                case CATEGORY_ROUTE:
                    return "/icons/route.png";
                case CATEGORY_AIS:
                    return "/icons/ais.png";
                case CATEGORY_ALARM:
                    return "/icons/alarm.png";
                case CATEGORY_OVERLAY:
                    return "/icons/layers_small.png";
                case DATA_CHART:
                    return "/icons/chart_32x32.png";
                case DATA_ROUTE:
                    return "/icons/route.png";
                case DATA_AIS:
                    return "/icons/ais.png";
                case DATA_ALARM:
                    return "/icons/alarm.png";
                default:
                    return "/icons/layers_small.png";
            }
        }
        
        private void updateStyle(LayerNodeType type) {
            getStyleClass().removeAll("category-node", "data-node");
            if (type.isCategory()) {
                getStyleClass().add("category-node");
            } else {
                getStyleClass().add("data-node");
            }
        }
    }
    
    public static class LayerNode {
        private final String id;
        private final SimpleStringProperty name;
        private final LayerNodeType type;
        private final SimpleBooleanProperty visible;
        private final Map<String, String> properties;
        
        public LayerNode(String id, String name, LayerNodeType type) {
            this.id = id;
            this.name = new SimpleStringProperty(name);
            this.type = type;
            this.visible = new SimpleBooleanProperty(true);
            this.properties = new LinkedHashMap<>();
        }
        
        public String getId() { return id; }
        public String getName() { return name.get(); }
        public void setName(String name) { this.name.set(name); }
        public SimpleStringProperty nameProperty() { return name; }
        public LayerNodeType getType() { return type; }
        public boolean isVisible() { return visible.get(); }
        public void setVisible(boolean visible) { this.visible.set(visible); }
        public SimpleBooleanProperty visibleProperty() { return visible; }
        public Map<String, String> getProperties() { return properties; }
        public void setProperty(String key, String value) { properties.put(key, value); }
        public String getProperty(String key) { return properties.get(key); }
    }
    
    public enum LayerNodeType {
        ROOT(false),
        CATEGORY_CHART(true),
        CATEGORY_ROUTE(true),
        CATEGORY_AIS(true),
        CATEGORY_ALARM(true),
        CATEGORY_OVERLAY(true),
        DATA(false),
        DATA_CHART(false),
        DATA_ROUTE(false),
        DATA_AIS(false),
        DATA_ALARM(false);
        
        private final boolean category;
        
        LayerNodeType(boolean category) {
            this.category = category;
        }
        
        public boolean isCategory() {
            return category;
        }
    }
}
