package cn.cycle.app.view;

import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.stage.DirectoryChooser;

public class SettingsView extends VBox {

    private final TextField chartPathField;
    private final TextField cachePathField;
    private final Spinner<Integer> cacheSizeSpinner;
    private final CheckBox showGridCheckBox;
    private final CheckBox showScaleBarCheckBox;
    private final CheckBox showCompassCheckBox;
    private final CheckBox enableHardwareAccelCheckBox;
    private final ComboBox<String> languageComboBox;
    private final ComboBox<String> themeComboBox;

    public SettingsView() {
        setSpacing(15);
        setPadding(new Insets(15));

        chartPathField = new TextField();
        cachePathField = new TextField();
        cacheSizeSpinner = new Spinner<>(100, 10000, 500, 100);
        showGridCheckBox = new CheckBox("显示网格");
        showScaleBarCheckBox = new CheckBox("显示比例尺");
        showCompassCheckBox = new CheckBox("显示指北针");
        enableHardwareAccelCheckBox = new CheckBox("启用硬件加速");
        languageComboBox = new ComboBox<>();
        themeComboBox = new ComboBox<>();

        Label titleLabel = new Label("设置");
        titleLabel.setStyle("-fx-font-size: 16px; -fx-font-weight: bold;");

        getChildren().addAll(
            titleLabel,
            createPathSettings(),
            createDisplaySettings(),
            createPerformanceSettings(),
            createLanguageSettings(),
            createButtonBar()
        );
    }

    private TitledPane createPathSettings() {
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(10));

        chartPathField.setPrefWidth(300);
        Button chartBrowseBtn = new Button("浏览...");
        chartBrowseBtn.setOnAction(e -> browseChartPath());

        cachePathField.setPrefWidth(300);
        Button cacheBrowseBtn = new Button("浏览...");
        cacheBrowseBtn.setOnAction(e -> browseCachePath());

        grid.add(new Label("海图路径:"), 0, 0);
        grid.add(chartPathField, 1, 0);
        grid.add(chartBrowseBtn, 2, 0);
        grid.add(new Label("缓存路径:"), 0, 1);
        grid.add(cachePathField, 1, 1);
        grid.add(cacheBrowseBtn, 2, 1);

        TitledPane pane = new TitledPane("路径设置", grid);
        pane.setCollapsible(true);
        return pane;
    }

    private TitledPane createDisplaySettings() {
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(10));

        showGridCheckBox.setSelected(true);
        showScaleBarCheckBox.setSelected(true);
        showCompassCheckBox.setSelected(true);

        grid.add(showGridCheckBox, 0, 0);
        grid.add(showScaleBarCheckBox, 1, 0);
        grid.add(showCompassCheckBox, 2, 0);

        TitledPane pane = new TitledPane("显示设置", grid);
        pane.setCollapsible(true);
        return pane;
    }

    private TitledPane createPerformanceSettings() {
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(10));

        cacheSizeSpinner.setPrefWidth(100);
        enableHardwareAccelCheckBox.setSelected(true);

        grid.add(new Label("缓存大小(MB):"), 0, 0);
        grid.add(cacheSizeSpinner, 1, 0);
        grid.add(enableHardwareAccelCheckBox, 0, 1, 2, 1);

        TitledPane pane = new TitledPane("性能设置", grid);
        pane.setCollapsible(true);
        return pane;
    }

    private TitledPane createLanguageSettings() {
        GridPane grid = new GridPane();
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(10));

        languageComboBox.getItems().addAll("简体中文", "English");
        languageComboBox.getSelectionModel().selectFirst();

        themeComboBox.getItems().addAll("默认", "深色", "浅色");
        themeComboBox.getSelectionModel().selectFirst();

        grid.add(new Label("语言:"), 0, 0);
        grid.add(languageComboBox, 1, 0);
        grid.add(new Label("主题:"), 0, 1);
        grid.add(themeComboBox, 1, 1);

        TitledPane pane = new TitledPane("语言和主题", grid);
        pane.setCollapsible(true);
        return pane;
    }

    private HBox createButtonBar() {
        HBox buttonBar = new HBox(10);
        buttonBar.setPadding(new Insets(10, 0, 0, 0));

        Button saveButton = new Button("保存");
        saveButton.setOnAction(e -> saveSettings());

        Button resetButton = new Button("重置");
        resetButton.setOnAction(e -> resetSettings());

        Button cancelButton = new Button("取消");
        cancelButton.setOnAction(e -> cancelSettings());

        buttonBar.getChildren().addAll(saveButton, resetButton, cancelButton);
        return buttonBar;
    }

    private void browseChartPath() {
        DirectoryChooser chooser = new DirectoryChooser();
        chooser.setTitle("选择海图路径");
        java.io.File dir = chooser.showDialog(getScene().getWindow());
        if (dir != null) {
            chartPathField.setText(dir.getAbsolutePath());
        }
    }

    private void browseCachePath() {
        DirectoryChooser chooser = new DirectoryChooser();
        chooser.setTitle("选择缓存路径");
        java.io.File dir = chooser.showDialog(getScene().getWindow());
        if (dir != null) {
            cachePathField.setText(dir.getAbsolutePath());
        }
    }

    private void saveSettings() {
        System.out.println("保存设置...");
    }

    private void resetSettings() {
        chartPathField.clear();
        cachePathField.clear();
        cacheSizeSpinner.getValueFactory().setValue(500);
        showGridCheckBox.setSelected(true);
        showScaleBarCheckBox.setSelected(true);
        showCompassCheckBox.setSelected(true);
        enableHardwareAccelCheckBox.setSelected(true);
        languageComboBox.getSelectionModel().selectFirst();
        themeComboBox.getSelectionModel().selectFirst();
    }

    private void cancelSettings() {
        resetSettings();
    }

    public String getChartPath() {
        return chartPathField.getText();
    }

    public void setChartPath(String path) {
        chartPathField.setText(path);
    }

    public String getCachePath() {
        return cachePathField.getText();
    }

    public void setCachePath(String path) {
        cachePathField.setText(path);
    }

    public boolean isShowGrid() {
        return showGridCheckBox.isSelected();
    }

    public void setShowGrid(boolean show) {
        showGridCheckBox.setSelected(show);
    }

    public boolean isShowScaleBar() {
        return showScaleBarCheckBox.isSelected();
    }

    public void setShowScaleBar(boolean show) {
        showScaleBarCheckBox.setSelected(show);
    }

    public boolean isShowCompass() {
        return showCompassCheckBox.isSelected();
    }

    public void setShowCompass(boolean show) {
        showCompassCheckBox.setSelected(show);
    }
}
