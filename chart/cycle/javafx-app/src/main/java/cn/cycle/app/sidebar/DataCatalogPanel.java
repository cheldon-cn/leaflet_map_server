package cn.cycle.app.sidebar;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.model.DataItem;
import cn.cycle.app.model.DataItem.DataType;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.FileChooser;
import javafx.stage.Window;

import java.io.File;

public class DataCatalogPanel extends AbstractSideBarPanel {
    
    private TreeView<DataItem> treeView;
    private TreeItem<DataItem> rootItem;
    private TreeItem<DataItem> chartCategory;
    private TreeItem<DataItem> imageCategory;
    private TreeItem<DataItem> vectorCategory;
    private TreeItem<DataItem> otherCategory;
    
    private DataItem selectedDataItem;
    private Runnable onPreviewCallback;
    private DataItemConsumer onPropertiesCallback;
    private DataItemConsumer onSelectionChangedCallback;
    
    public interface DataItemConsumer {
        void accept(DataItem item);
    }
    
    public DataCatalogPanel() {
        super("data-catalog", "数据目录", "目", Color.DODGERBLUE);
    }
    
    @Override
    protected Node createCenterContent() {
        VBox container = new VBox(5);
        container.setPadding(new javafx.geometry.Insets(5));
        
        ToolBar toolBar = new ToolBar();
        Button addButton = new Button("添加数据");
        addButton.setOnAction(e -> addData());
        Button removeButton = new Button("移除");
        removeButton.setOnAction(e -> removeSelectedData());
        Button refreshButton = new Button("刷新");
        refreshButton.setOnAction(e -> refreshTree());
        toolBar.getItems().addAll(addButton, removeButton, refreshButton);
        
        rootItem = new TreeItem<>(new DataItem("数据目录", DataType.UNKNOWN));
        rootItem.setExpanded(true);
        
        chartCategory = createCategoryItem("海图数据", DataType.CHART);
        imageCategory = createCategoryItem("影像数据", DataType.IMAGE);
        vectorCategory = createCategoryItem("矢量数据", DataType.VECTOR);
        otherCategory = createCategoryItem("其他数据", DataType.UNKNOWN);
        
        rootItem.getChildren().addAll(chartCategory, imageCategory, vectorCategory, otherCategory);
        
        treeView = new TreeView<>(rootItem);
        treeView.setShowRoot(false);
        treeView.setCellFactory(tv -> new DataItemTreeCell());
        
        treeView.getSelectionModel().selectedItemProperty().addListener(new ChangeListener<TreeItem<DataItem>>() {
            @Override
            public void changed(ObservableValue<? extends TreeItem<DataItem>> observable, 
                                TreeItem<DataItem> oldValue, TreeItem<DataItem> newValue) {
                if (newValue != null && newValue.getValue() != null) {
                    DataItem item = newValue.getValue();
                    if (item.getFile() != null) {
                        selectedDataItem = item;
                        if (onSelectionChangedCallback != null) {
                            onSelectionChangedCallback.accept(item);
                        }
                    }
                }
            }
        });
        
        treeView.setOnMouseClicked(event -> {
            if (event.getClickCount() == 2) {
                TreeItem<DataItem> selected = treeView.getSelectionModel().getSelectedItem();
                if (selected != null && selected.getValue().getFile() != null) {
                    previewSelectedData();
                }
            }
        });
        
        VBox.setVgrow(treeView, javafx.scene.layout.Priority.ALWAYS);
        
        container.getChildren().addAll(toolBar, treeView);
        return container;
    }
    
    private TreeItem<DataItem> createCategoryItem(String name, DataType type) {
        DataItem categoryItem = new DataItem(name, type);
        TreeItem<DataItem> item = new TreeItem<>(categoryItem);
        item.setExpanded(true);
        return item;
    }
    
    public void setOnPreviewCallback(Runnable callback) {
        this.onPreviewCallback = callback;
    }
    
    public void setOnPropertiesCallback(DataItemConsumer callback) {
        this.onPropertiesCallback = callback;
    }
    
    public void setOnSelectionChangedCallback(DataItemConsumer callback) {
        this.onSelectionChangedCallback = callback;
    }
    
    private void addData() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("添加数据");
        fileChooser.getExtensionFilters().addAll(
            new FileChooser.ExtensionFilter("支持的文件", "*.000", "*.enc", "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif", "*.shp", "*.tif", "*.tiff"),
            new FileChooser.ExtensionFilter("海图文件", "*.000", "*.enc"),
            new FileChooser.ExtensionFilter("图片文件", "*.png", "*.jpg", "*.jpeg", "*.bmp", "*.gif"),
            new FileChooser.ExtensionFilter("矢量文件", "*.shp"),
            new FileChooser.ExtensionFilter("所有文件", "*.*")
        );
        
        Window window = getWindow();
        File file = fileChooser.showOpenDialog(window);
        if (file != null) {
            addFile(file);
        }
    }
    
    private Window getWindow() {
        if (treeView != null && treeView.getScene() != null) {
            return treeView.getScene().getWindow();
        }
        return null;
    }
    
    public void addFile(File file) {
        if (file != null && file.exists()) {
            if (chartCategory == null) {
                createContent();
            }
            
            DataItem dataItem = new DataItem(file);
            TreeItem<DataItem> item = new TreeItem<>(dataItem);
            
            TreeItem<DataItem> category = getCategoryForDataType(dataItem.getDataType());
            if (category != null) {
                category.getChildren().add(item);
                category.setExpanded(true);
                
                treeView.getSelectionModel().select(item);
                
                AppEventBus.getInstance().publish(
                    new AppEvent(AppEventType.DATA_ADDED, this)
                        .withData("dataItem", dataItem)
                );
            }
        }
    }
    
    private TreeItem<DataItem> getCategoryForDataType(DataType type) {
        switch (type) {
            case CHART:
                return chartCategory;
            case IMAGE:
                return imageCategory;
            case VECTOR:
                return vectorCategory;
            case RASTER:
                return imageCategory;
            default:
                return otherCategory;
        }
    }
    
    private void removeSelectedData() {
        TreeItem<DataItem> selected = treeView.getSelectionModel().getSelectedItem();
        if (selected != null && selected.getValue().getFile() != null) {
            TreeItem<DataItem> parent = selected.getParent();
            if (parent != null) {
                parent.getChildren().remove(selected);
            }
            
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.DATA_REMOVED, this)
                    .withData("dataItem", selected.getValue())
            );
        }
    }
    
    private void refreshTree() {
        treeView.refresh();
    }
    
    private void previewSelectedData() {
        TreeItem<DataItem> selected = treeView.getSelectionModel().getSelectedItem();
        if (selected != null && selected.getValue().getFile() != null) {
            selectedDataItem = selected.getValue();
            if (onPreviewCallback != null) {
                onPreviewCallback.run();
            }
            
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.DATA_PREVIEW, this)
                    .withData("dataItem", selectedDataItem)
            );
        }
    }
    
    private void showProperties() {
        TreeItem<DataItem> selected = treeView.getSelectionModel().getSelectedItem();
        if (selected != null && selected.getValue().getFile() != null) {
            selectedDataItem = selected.getValue();
            if (onPropertiesCallback != null) {
                onPropertiesCallback.accept(selectedDataItem);
            }
            
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.DATA_PROPERTIES, this)
                    .withData("dataItem", selectedDataItem)
            );
        }
    }
    
    public DataItem getSelectedDataItem() {
        return selectedDataItem;
    }
    
    private class DataItemTreeCell extends TreeCell<DataItem> {
        @Override
        protected void updateItem(DataItem item, boolean empty) {
            super.updateItem(item, empty);
            
            if (empty || item == null) {
                setText(null);
                setGraphic(null);
                setContextMenu(null);
            } else {
                setText(item.getName());
                setGraphic(createIcon(item));
                
                if (item.getFile() != null) {
                    setContextMenu(createContextMenu());
                } else {
                    setContextMenu(null);
                }
            }
        }
        
        private Node createIcon(DataItem item) {
            Label icon = new Label();
            if (item.getFile() == null) {
                icon.setText("📁");
            } else {
                switch (item.getDataType()) {
                    case CHART:
                        icon.setText("🗺");
                        break;
                    case IMAGE:
                        icon.setText("🖼");
                        break;
                    case VECTOR:
                        icon.setText("📐");
                        break;
                    default:
                        icon.setText("📄");
                        break;
                }
            }
            return icon;
        }
        
        private ContextMenu createContextMenu() {
            ContextMenu menu = new ContextMenu();
            
            MenuItem previewItem = new MenuItem("预览显示");
            previewItem.setOnAction(e -> previewSelectedData());
            
            MenuItem propertiesItem = new MenuItem("属性");
            propertiesItem.setOnAction(e -> showProperties());
            
            MenuItem removeItem = new MenuItem("移除");
            removeItem.setOnAction(e -> removeSelectedData());
            
            menu.getItems().addAll(previewItem, propertiesItem, new SeparatorMenuItem(), removeItem);
            return menu;
        }
    }
}
