package cn.cycle.app.sidebar;

import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;

import java.io.File;

public class DataCatalogPanel extends AbstractSideBarPanel {
    
    private TreeView<FileItem> treeView;
    private TreeItem<FileItem> rootItem;
    
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
        Button refreshButton = new Button("刷新");
        refreshButton.setOnAction(e -> refreshTree());
        toolBar.getItems().addAll(addButton, refreshButton);
        
        rootItem = new TreeItem<>(new FileItem("数据目录", null));
        rootItem.setExpanded(true);
        
        treeView = new TreeView<>(rootItem);
        treeView.setShowRoot(true);
        treeView.setCellFactory(tv -> new FileTreeCell());
        
        VBox.setVgrow(treeView, javafx.scene.layout.Priority.ALWAYS);
        
        container.getChildren().addAll(toolBar, treeView);
        return container;
    }
    
    private void addData() {
        System.out.println("添加数据...");
    }
    
    private void refreshTree() {
        System.out.println("刷新数据目录...");
    }
    
    public void addFile(File file) {
        if (file != null && file.exists()) {
            TreeItem<FileItem> item = new TreeItem<>(new FileItem(file.getName(), file));
            rootItem.getChildren().add(item);
        }
    }
    
    public static class FileItem {
        private final String name;
        private final File file;
        
        public FileItem(String name, File file) {
            this.name = name;
            this.file = file;
        }
        
        public String getName() {
            return name;
        }
        
        public File getFile() {
            return file;
        }
        
        @Override
        public String toString() {
            return name;
        }
    }
    
    private static class FileTreeCell extends TreeCell<FileItem> {
        @Override
        protected void updateItem(FileItem item, boolean empty) {
            super.updateItem(item, empty);
            if (empty || item == null) {
                setText(null);
                setGraphic(null);
            } else {
                setText(item.getName());
                if (item.getFile() != null && item.getFile().isDirectory()) {
                    setGraphic(new Label("📁"));
                } else {
                    setGraphic(new Label("📄"));
                }
            }
        }
    }
}
