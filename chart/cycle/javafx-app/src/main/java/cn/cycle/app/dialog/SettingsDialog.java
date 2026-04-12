package cn.cycle.app.dialog;

import javafx.collections.FXCollections;
import javafx.geometry.Insets;
import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Hyperlink;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.Separator;
import javafx.scene.control.TableView;
import javafx.scene.control.TableColumn;
import javafx.scene.control.TextField;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.stage.Modality;
import javafx.stage.Stage;

public class SettingsDialog {
    
    private Stage dialogStage;
    private BorderPane mainLayout;
    private ListView<String> categoryList;
    private StackPane contentArea;
    
    public SettingsDialog(Stage owner) {
        createDialog(owner);
    }
    
    private void createDialog(Stage owner) {
        dialogStage = new Stage();
        dialogStage.initOwner(owner);
        dialogStage.initModality(Modality.APPLICATION_MODAL);
        dialogStage.setTitle("设置");
        
        mainLayout = new BorderPane();
        mainLayout.setStyle("-fx-background-color: -fx-background;");
        
        createCategoryList();
        createContentArea();
        createButtonBar();
        
        dialogStage.setOnShown(e -> {
            categoryList.getSelectionModel().select(0);
        });
    }
    
    private void createCategoryList() {
        categoryList = new ListView<>();
        categoryList.getItems().addAll("常规", "显示", "语言", "快捷键", "数据", "关于");
        categoryList.setPrefWidth(120);
        categoryList.setStyle("-fx-background-color: derive(-fx-base, -5%);");
        
        categoryList.getSelectionModel().selectedItemProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal != null) {
                updateContent(newVal);
            }
        });
    }
    
    private void createContentArea() {
        contentArea = new StackPane();
        contentArea.setPadding(new Insets(15));
        
        Separator separator = new Separator();
        separator.setOrientation(Orientation.VERTICAL);
        
        HBox centerLayout = new HBox();
        centerLayout.getChildren().addAll(categoryList, separator, contentArea);
        HBox.setHgrow(contentArea, Priority.ALWAYS);
        
        mainLayout.setCenter(centerLayout);
    }
    
    private void createButtonBar() {
        HBox buttonBar = new HBox(10);
        buttonBar.setPadding(new Insets(10, 15, 10, 15));
        buttonBar.setAlignment(Pos.CENTER_RIGHT);
        buttonBar.setStyle("-fx-background-color: derive(-fx-base, -10%); -fx-border-color: derive(-fx-base, -20%); -fx-border-width: 1 0 0 0;");
        
        Button applyButton = new Button("应用");
        applyButton.setOnAction(e -> applySettings());
        
        Button okButton = new Button("确定");
        okButton.setOnAction(e -> {
            applySettings();
            dialogStage.close();
        });
        
        Button cancelButton = new Button("取消");
        cancelButton.setOnAction(e -> dialogStage.close());
        
        buttonBar.getChildren().addAll(applyButton, okButton, cancelButton);
        mainLayout.setBottom(buttonBar);
    }
    
    private void updateContent(String category) {
        contentArea.getChildren().clear();
        switch (category) {
            case "常规":
                contentArea.getChildren().add(createGeneralSettingsPane());
                break;
            case "显示":
                contentArea.getChildren().add(createDisplaySettingsPane());
                break;
            case "语言":
                contentArea.getChildren().add(createLanguageSettingsPane());
                break;
            case "快捷键":
                contentArea.getChildren().add(createShortcutSettingsPane());
                break;
            case "数据":
                contentArea.getChildren().add(createDataSettingsPane());
                break;
            case "关于":
                contentArea.getChildren().add(createAboutPane());
                break;
        }
    }
    
    private VBox createGeneralSettingsPane() {
        VBox pane = new VBox(15);
        pane.setPadding(new Insets(10));
        
        Label titleLabel = new Label("常规设置");
        titleLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");
        
        CheckBox autoStartBox = new CheckBox("开机自动启动");
        CheckBox autoUpdateBox = new CheckBox("自动检查更新");
        CheckBox minimizeToTrayBox = new CheckBox("最小化到系统托盘");
        
        HBox themeRow = new HBox(10);
        themeRow.setAlignment(Pos.CENTER_LEFT);
        themeRow.getChildren().addAll(new Label("默认主题:"), new ComboBox<>(FXCollections.observableArrayList("浅色", "深色", "跟随系统")));
        
        pane.getChildren().addAll(titleLabel, new Separator(), autoStartBox, autoUpdateBox, minimizeToTrayBox, themeRow);
        return pane;
    }
    
    private VBox createDisplaySettingsPane() {
        VBox pane = new VBox(15);
        pane.setPadding(new Insets(10));
        
        Label titleLabel = new Label("显示设置");
        titleLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");
        
        HBox scaleRow = new HBox(10);
        scaleRow.setAlignment(Pos.CENTER_LEFT);
        ComboBox<String> scaleCombo = new ComboBox<>(FXCollections.observableArrayList("100%", "125%", "150%", "200%"));
        scaleCombo.setValue("100%");
        scaleRow.getChildren().addAll(new Label("界面缩放:"), scaleCombo);
        
        CheckBox showStatusBarBox = new CheckBox("显示状态栏");
        showStatusBarBox.setSelected(true);
        CheckBox showSidebarBox = new CheckBox("显示侧边栏");
        showSidebarBox.setSelected(true);
        
        pane.getChildren().addAll(titleLabel, new Separator(), scaleRow, showStatusBarBox, showSidebarBox);
        return pane;
    }
    
    private VBox createLanguageSettingsPane() {
        VBox pane = new VBox(15);
        pane.setPadding(new Insets(10));
        
        Label titleLabel = new Label("语言设置");
        titleLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");
        
        HBox langRow = new HBox(10);
        langRow.setAlignment(Pos.CENTER_LEFT);
        ComboBox<String> langCombo = new ComboBox<>(FXCollections.observableArrayList("简体中文", "English", "日本語"));
        langCombo.setValue("简体中文");
        langRow.getChildren().addAll(new Label("界面语言:"), langCombo);
        
        Label noteLabel = new Label("更改语言后需要重启应用程序才能生效");
        noteLabel.setStyle("-fx-text-fill: gray; -fx-font-size: 11px;");
        
        pane.getChildren().addAll(titleLabel, new Separator(), langRow, noteLabel);
        return pane;
    }
    
    private VBox createShortcutSettingsPane() {
        VBox pane = new VBox(15);
        pane.setPadding(new Insets(10));
        
        Label titleLabel = new Label("快捷键设置");
        titleLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");
        
        TableView<String> table = new TableView<>();
        table.setColumnResizePolicy(TableView.CONSTRAINED_RESIZE_POLICY);
        
        TableColumn<String, String> actionCol = new TableColumn<>("操作");
        TableColumn<String, String> shortcutCol = new TableColumn<>("快捷键");
        
        table.getColumns().addAll(actionCol, shortcutCol);
        table.getItems().addAll("打开文件", "保存", "撤销", "重做", "查找");
        
        pane.getChildren().addAll(titleLabel, new Separator(), table);
        VBox.setVgrow(table, Priority.ALWAYS);
        return pane;
    }
    
    private VBox createDataSettingsPane() {
        VBox pane = new VBox(15);
        pane.setPadding(new Insets(10));
        
        Label titleLabel = new Label("数据设置");
        titleLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold;");
        
        HBox cacheRow = new HBox(10);
        cacheRow.setAlignment(Pos.CENTER_LEFT);
        Label cacheLabel = new Label("缓存大小: 128 MB");
        Button clearCacheBtn = new Button("清除缓存");
        cacheRow.getChildren().addAll(cacheLabel, clearCacheBtn);
        
        CheckBox autoBackupBox = new CheckBox("自动备份数据");
        autoBackupBox.setSelected(true);
        
        HBox backupRow = new HBox(10);
        backupRow.setAlignment(Pos.CENTER_LEFT);
        backupRow.getChildren().addAll(new Label("备份路径:"), new TextField("C:\\Users\\Backup"), new Button("浏览..."));
        
        pane.getChildren().addAll(titleLabel, new Separator(), cacheRow, autoBackupBox, backupRow);
        return pane;
    }
    
    private VBox createAboutPane() {
        VBox pane = new VBox(15);
        pane.setPadding(new Insets(10));
        pane.setAlignment(Pos.CENTER);
        
        Label appLabel = new Label("Cycle Chart");
        appLabel.setStyle("-fx-font-size: 18px; -fx-font-weight: bold;");
        
        Label versionLabel = new Label("版本: 1.0.0");
        Label copyrightLabel = new Label("© 2024 Cycle Software. All rights reserved.");
        copyrightLabel.setStyle("-fx-text-fill: gray; -fx-font-size: 11px;");
        
        Hyperlink websiteLink = new Hyperlink("访问官网");
        Hyperlink updateLink = new Hyperlink("检查更新");
        
        pane.getChildren().addAll(appLabel, versionLabel, copyrightLabel, new Separator(), websiteLink, updateLink);
        return pane;
    }
    
    private void applySettings() {
        System.out.println("应用设置");
    }
    
    public void show() {
        javafx.scene.Scene scene = new javafx.scene.Scene(mainLayout, 600, 400);
        dialogStage.setScene(scene);
        dialogStage.showAndWait();
    }
}
