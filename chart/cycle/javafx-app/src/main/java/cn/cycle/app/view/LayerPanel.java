package cn.cycle.app.view;

import cn.cycle.chart.api.layer.Layer;
import cn.cycle.chart.api.layer.LayerManager;
import javafx.beans.property.SimpleBooleanProperty;
import javafx.beans.property.SimpleDoubleProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.FileChooser;

import java.util.List;

public class LayerPanel extends BorderPane {

    private final ListView<LayerItem> layerListView;
    private final ObservableList<LayerItem> layerItems;
    private LayerManager layerManager;

    public LayerPanel() {
        layerItems = FXCollections.observableArrayList();
        layerListView = new ListView<>(layerItems);
        layerListView.setCellFactory(list -> new LayerListCell());

        setCenter(layerListView);
        setTop(createToolBar());
        setPrefWidth(250);
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();

        Button addButton = new Button("添加");
        addButton.setOnAction(e -> onAddLayer());

        Button removeButton = new Button("移除");
        removeButton.setOnAction(e -> onRemoveLayer());

        Button moveUpButton = new Button("上移");
        moveUpButton.setOnAction(e -> onMoveUp());

        Button moveDownButton = new Button("下移");
        moveDownButton.setOnAction(e -> onMoveDown());

        toolBar.getItems().addAll(addButton, removeButton, moveUpButton, moveDownButton);
        return toolBar;
    }

    public void setLayerManager(LayerManager manager) {
        this.layerManager = manager;
        refreshLayers();
    }

    public void refreshLayers() {
        layerItems.clear();
        if (layerManager != null) {
            long count = layerManager.getLayerCount();
            for (int i = 0; i < count; i++) {
                Layer layer = layerManager.getLayer(i);
                if (layer != null) {
                    layerItems.add(new LayerItem(layer, i));
                }
            }
        }
    }

    private void onAddLayer() {
        if (layerManager != null) {
            FileChooser chooser = new FileChooser();
            chooser.setTitle("添加图层");
            chooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("Shapefile", "*.shp"),
                new FileChooser.ExtensionFilter("GeoJSON", "*.geojson", "*.json"),
                new FileChooser.ExtensionFilter("所有文件", "*.*")
            );
            java.io.File file = chooser.showOpenDialog(getScene().getWindow());
            if (file != null) {
                System.out.println("添加图层: " + file.getAbsolutePath());
                refreshLayers();
            }
        }
    }

    private void onRemoveLayer() {
        LayerItem selected = layerListView.getSelectionModel().getSelectedItem();
        if (selected != null && layerManager != null) {
            layerManager.removeLayer(selected.getIndex());
            refreshLayers();
        }
    }

    private void onMoveUp() {
        int selectedIndex = layerListView.getSelectionModel().getSelectedIndex();
        if (selectedIndex > 0 && layerManager != null) {
            layerManager.moveLayer(selectedIndex, selectedIndex - 1);
            refreshLayers();
            layerListView.getSelectionModel().select(selectedIndex - 1);
        }
    }

    private void onMoveDown() {
        int selectedIndex = layerListView.getSelectionModel().getSelectedIndex();
        if (selectedIndex < layerItems.size() - 1 && layerManager != null) {
            layerManager.moveLayer(selectedIndex, selectedIndex + 1);
            refreshLayers();
            layerListView.getSelectionModel().select(selectedIndex + 1);
        }
    }

    public static class LayerItem {
        private final Layer layer;
        private final int index;

        public LayerItem(Layer layer, int index) {
            this.layer = layer;
            this.index = index;
        }

        public Layer getLayer() {
            return layer;
        }

        public int getIndex() {
            return index;
        }

        @Override
        public String toString() {
            return layer.getName();
        }
    }

    private static class LayerListCell extends ListCell<LayerItem> {
        private final CheckBox visibleCheckBox;
        private final HBox content;
        private final Slider opacitySlider;

        public LayerListCell() {
            visibleCheckBox = new CheckBox();
            opacitySlider = new Slider(0, 1, 1);
            opacitySlider.setPrefWidth(80);
            
            Label opacityLabel = new Label("透明度:");
            content = new HBox(5, visibleCheckBox, opacityLabel, opacitySlider);
            content.setStyle("-fx-padding: 5; -fx-alignment: center-left;");
        }

        @Override
        protected void updateItem(LayerItem item, boolean empty) {
            super.updateItem(item, empty);
            if (empty || item == null) {
                setGraphic(null);
            } else {
                Layer layer = item.getLayer();
                visibleCheckBox.setText(layer.getName());
                visibleCheckBox.setSelected(layer.isVisible());
                visibleCheckBox.selectedProperty().addListener((obs, oldVal, newVal) -> {
                    layer.setVisible(newVal);
                });
                opacitySlider.setValue(layer.getOpacity());
                opacitySlider.valueProperty().addListener((obs, oldVal, newVal) -> {
                    layer.setOpacity(newVal.doubleValue());
                });
                setGraphic(content);
            }
        }
    }
}
