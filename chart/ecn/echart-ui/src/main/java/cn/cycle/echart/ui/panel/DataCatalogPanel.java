package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxSideBarPanel;
import cn.cycle.echart.data.ChartFile;
import cn.cycle.echart.facade.ApplicationFacade;
import cn.cycle.echart.facade.FacadeException;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.layout.BorderPane;
import javafx.stage.FileChooser;

import java.io.File;
import java.util.List;
import java.util.Objects;

/**
 * 数据目录面板。
 * 
 * <p>显示海图文件目录结构。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DataCatalogPanel extends BorderPane implements FxSideBarPanel {

    private static final String PANEL_ID = "data-catalog-panel";
    
    private final TreeView<String> treeView;
    private final ApplicationFacade applicationFacade;
    
    public DataCatalogPanel(ApplicationFacade applicationFacade) {
        this.applicationFacade = Objects.requireNonNull(applicationFacade, "applicationFacade cannot be null");
        this.treeView = new TreeView<>();
        
        initializeLayout();
        loadData();
    }

    private void initializeLayout() {
        TreeItem<String> root = new TreeItem<>("海图目录");
        root.setExpanded(true);
        treeView.setRoot(root);
        
        treeView.setCellFactory(param -> new TreeCell<String>() {
            @Override
            protected void updateItem(String item, boolean empty) {
                super.updateItem(item, empty);
                setText(empty ? null : item);
            }
        });
        
        setTop(createToolBar());
        setCenter(treeView);
        
        getStyleClass().add("data-catalog-panel");
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();
        
        Button addButton = new Button("添加");
        addButton.setOnAction(e -> addChart());
        
        Button removeButton = new Button("移除");
        removeButton.setOnAction(e -> removeChart());
        
        Button refreshButton = new Button("刷新");
        refreshButton.setOnAction(e -> loadData());
        
        toolBar.getItems().addAll(addButton, removeButton, refreshButton);
        return toolBar;
    }

    private void loadData() {
        TreeItem<String> root = treeView.getRoot();
        root.getChildren().clear();
        
        List<ChartFile> charts = applicationFacade.getLoadedCharts();
        for (ChartFile chart : charts) {
            TreeItem<String> item = new TreeItem<>(chart.getName());
            root.getChildren().add(item);
        }
    }

    private void addChart() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("添加海图文件");
        fileChooser.getExtensionFilters().addAll(
                new FileChooser.ExtensionFilter("S57文件", "*.s57", "*.000"),
                new FileChooser.ExtensionFilter("S63文件", "*.s63"),
                new FileChooser.ExtensionFilter("GeoJSON文件", "*.geojson"),
                new FileChooser.ExtensionFilter("所有文件", "*.*"));
        
        File file = fileChooser.showOpenDialog(getScene().getWindow());
        if (file != null) {
            try {
                applicationFacade.loadChart(file.getAbsolutePath());
                loadData();
            } catch (FacadeException e) {
                showError("加载失败", "无法加载海图文件: " + file.getName());
            }
        }
    }

    private void removeChart() {
        TreeItem<String> selected = treeView.getSelectionModel().getSelectedItem();
        if (selected == null || selected == treeView.getRoot()) {
            showWarning("请先选择一个海图");
            return;
        }
        
        String chartName = selected.getValue();
        List<ChartFile> charts = applicationFacade.getLoadedCharts();
        for (ChartFile chart : charts) {
            if (chart.getName().equals(chartName)) {
                try {
                    applicationFacade.unloadChart(chart.getId());
                } catch (FacadeException e) {
                    showError("卸载失败", "无法卸载海图: " + chartName);
                }
                loadData();
                break;
            }
        }
    }

    private void showError(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle(title);
        alert.setContentText(message);
        alert.showAndWait();
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
        return "数据目录";
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
        loadData();
    }

    @Override
    public void onDeactivate() {
    }
}
