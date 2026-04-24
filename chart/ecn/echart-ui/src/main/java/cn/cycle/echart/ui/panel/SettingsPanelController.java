package cn.cycle.echart.ui.panel;

import javafx.beans.value.ChangeListener;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.FileChooser;

import java.io.File;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;
import java.util.ResourceBundle;

/**
 * 设置面板控制器。
 * 
 * @author Cycle Team
 * @version 1.0.0
 */
public class SettingsPanelController implements Initializable {

    @FXML private Button hideButton;
    @FXML private Button applyButton;
    @FXML private Button resetButton;
    @FXML private Accordion settingsAccordion;
    
    @FXML private TitledPane displayPane;
    @FXML private TitledPane chartPane;
    @FXML private TitledPane navigationPane;
    @FXML private TitledPane performancePane;
    @FXML private TitledPane advancedPane;
    
    @FXML private CheckBox darkThemeCheckBox;
    @FXML private CheckBox showGridCheckBox;
    @FXML private CheckBox showScaleBarCheckBox;
    @FXML private CheckBox showCompassCheckBox;
    @FXML private CheckBox showCoordinatesCheckBox;
    @FXML private Slider zoomSpeedSlider;
    @FXML private Label zoomSpeedLabel;
    
    @FXML private ComboBox<String> chartStyleComboBox;
    @FXML private ComboBox<String> depthUnitComboBox;
    @FXML private ComboBox<String> coordinateFormatComboBox;
    @FXML private CheckBox autoLoadChartsCheckBox;
    @FXML private CheckBox showChartInfoCheckBox;
    @FXML private TextField chartCachePathField;
    @FXML private Button browseCachePathButton;
    
    @FXML private CheckBox enableNavigationCheckBox;
    @FXML private CheckBox autoCenterCheckBox;
    @FXML private CheckBox showRouteCheckBox;
    @FXML private CheckBox showTrackCheckBox;
    @FXML private Slider trackIntervalSlider;
    @FXML private Label trackIntervalLabel;
    
    @FXML private CheckBox enableCacheCheckBox;
    @FXML private Slider cacheSizeSlider;
    @FXML private Label cacheSizeLabel;
    @FXML private CheckBox enableGpuAccelerationCheckBox;
    @FXML private CheckBox enableMultithreadingCheckBox;
    @FXML private ComboBox<String> threadCountComboBox;
    
    @FXML private CheckBox enableLoggingCheckBox;
    @FXML private ComboBox<String> logLevelComboBox;
    @FXML private CheckBox autoSaveCheckBox;
    @FXML private Slider autoSaveIntervalSlider;
    @FXML private Label autoSaveIntervalLabel;
    
    private Map<String, Object> settings;
    private Map<String, Object> defaultSettings;
    private Runnable onSettingsChanged;
    private Runnable onHidePanel;
    private boolean isUpdating = false;
    
    @Override
    public void initialize(URL location, ResourceBundle resources) {
        settings = new HashMap<>();
        defaultSettings = createDefaultSettings();
        
        initializeControls();
        initializeListeners();
        loadDefaultSettings();
    }
    
    private Map<String, Object> createDefaultSettings() {
        Map<String, Object> defaults = new HashMap<>();
        
        defaults.put("display.darkTheme", false);
        defaults.put("display.showGrid", true);
        defaults.put("display.showScaleBar", true);
        defaults.put("display.showCompass", true);
        defaults.put("display.showCoordinates", true);
        defaults.put("display.zoomSpeed", 1.0);
        
        defaults.put("chart.style", "S57");
        defaults.put("chart.depthUnit", "米");
        defaults.put("chart.coordinateFormat", "度分秒");
        defaults.put("chart.autoLoadCharts", true);
        defaults.put("chart.showChartInfo", true);
        defaults.put("chart.cachePath", System.getProperty("user.home") + "/echart/cache");
        
        defaults.put("navigation.enabled", true);
        defaults.put("navigation.autoCenter", true);
        defaults.put("navigation.showRoute", true);
        defaults.put("navigation.showTrack", true);
        defaults.put("navigation.trackInterval", 10);
        
        defaults.put("performance.enableCache", true);
        defaults.put("performance.cacheSize", 500);
        defaults.put("performance.enableGpuAcceleration", true);
        defaults.put("performance.enableMultithreading", true);
        defaults.put("performance.threadCount", "4");
        
        defaults.put("advanced.enableLogging", true);
        defaults.put("advanced.logLevel", "INFO");
        defaults.put("advanced.autoSave", true);
        defaults.put("advanced.autoSaveInterval", 5);
        
        return defaults;
    }
    
    private void initializeControls() {
        chartStyleComboBox.setItems(javafx.collections.FXCollections.observableArrayList(
            "S57", "S52", "自定义"
        ));
        
        depthUnitComboBox.setItems(javafx.collections.FXCollections.observableArrayList(
            "米", "英尺", "英寻"
        ));
        
        coordinateFormatComboBox.setItems(javafx.collections.FXCollections.observableArrayList(
            "度分秒", "度", "弧度"
        ));
        
        logLevelComboBox.setItems(javafx.collections.FXCollections.observableArrayList(
            "DEBUG", "INFO", "WARNING", "ERROR"
        ));
        
        threadCountComboBox.setItems(javafx.collections.FXCollections.observableArrayList(
            "1", "2", "4", "8", "自动"
        ));
        
        setButtonIcon(hideButton, "/icons/sidebar_32x321.png");
        setButtonIcon(applyButton, "/icons/check_32x32.png");
        setButtonIcon(resetButton, "/icons/refresh_32x32.png");
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
        ChangeListener<Object> changeListener = (obs, oldVal, newVal) -> {
            if (!isUpdating) {
                updateSettingsFromControls();
            }
        };
        
        darkThemeCheckBox.selectedProperty().addListener(changeListener);
        showGridCheckBox.selectedProperty().addListener(changeListener);
        showScaleBarCheckBox.selectedProperty().addListener(changeListener);
        showCompassCheckBox.selectedProperty().addListener(changeListener);
        showCoordinatesCheckBox.selectedProperty().addListener(changeListener);
        zoomSpeedSlider.valueProperty().addListener((obs, oldVal, newVal) -> {
            zoomSpeedLabel.setText(String.format("%.1fx", newVal.doubleValue()));
            if (!isUpdating) {
                updateSettingsFromControls();
            }
        });
        
        chartStyleComboBox.valueProperty().addListener(changeListener);
        depthUnitComboBox.valueProperty().addListener(changeListener);
        coordinateFormatComboBox.valueProperty().addListener(changeListener);
        autoLoadChartsCheckBox.selectedProperty().addListener(changeListener);
        showChartInfoCheckBox.selectedProperty().addListener(changeListener);
        chartCachePathField.textProperty().addListener(changeListener);
        
        enableNavigationCheckBox.selectedProperty().addListener(changeListener);
        autoCenterCheckBox.selectedProperty().addListener(changeListener);
        showRouteCheckBox.selectedProperty().addListener(changeListener);
        showTrackCheckBox.selectedProperty().addListener(changeListener);
        trackIntervalSlider.valueProperty().addListener((obs, oldVal, newVal) -> {
            trackIntervalLabel.setText(newVal.intValue() + " 秒");
            if (!isUpdating) {
                updateSettingsFromControls();
            }
        });
        
        enableCacheCheckBox.selectedProperty().addListener(changeListener);
        cacheSizeSlider.valueProperty().addListener((obs, oldVal, newVal) -> {
            cacheSizeLabel.setText(newVal.intValue() + " MB");
            if (!isUpdating) {
                updateSettingsFromControls();
            }
        });
        enableGpuAccelerationCheckBox.selectedProperty().addListener(changeListener);
        enableMultithreadingCheckBox.selectedProperty().addListener(changeListener);
        threadCountComboBox.valueProperty().addListener(changeListener);
        
        enableLoggingCheckBox.selectedProperty().addListener(changeListener);
        logLevelComboBox.valueProperty().addListener(changeListener);
        autoSaveCheckBox.selectedProperty().addListener(changeListener);
        autoSaveIntervalSlider.valueProperty().addListener((obs, oldVal, newVal) -> {
            autoSaveIntervalLabel.setText(newVal.intValue() + " 分钟");
            if (!isUpdating) {
                updateSettingsFromControls();
            }
        });
    }
    
    private void loadDefaultSettings() {
        isUpdating = true;
        try {
            settings.putAll(defaultSettings);
            updateControlsFromSettings();
        } finally {
            isUpdating = false;
        }
    }
    
    private void updateControlsFromSettings() {
        darkThemeCheckBox.setSelected((Boolean) settings.getOrDefault("display.darkTheme", false));
        showGridCheckBox.setSelected((Boolean) settings.getOrDefault("display.showGrid", true));
        showScaleBarCheckBox.setSelected((Boolean) settings.getOrDefault("display.showScaleBar", true));
        showCompassCheckBox.setSelected((Boolean) settings.getOrDefault("display.showCompass", true));
        showCoordinatesCheckBox.setSelected((Boolean) settings.getOrDefault("display.showCoordinates", true));
        zoomSpeedSlider.setValue((Double) settings.getOrDefault("display.zoomSpeed", 1.0));
        
        chartStyleComboBox.setValue((String) settings.getOrDefault("chart.style", "S57"));
        depthUnitComboBox.setValue((String) settings.getOrDefault("chart.depthUnit", "米"));
        coordinateFormatComboBox.setValue((String) settings.getOrDefault("chart.coordinateFormat", "度分秒"));
        autoLoadChartsCheckBox.setSelected((Boolean) settings.getOrDefault("chart.autoLoadCharts", true));
        showChartInfoCheckBox.setSelected((Boolean) settings.getOrDefault("chart.showChartInfo", true));
        chartCachePathField.setText((String) settings.getOrDefault("chart.cachePath", ""));
        
        enableNavigationCheckBox.setSelected((Boolean) settings.getOrDefault("navigation.enabled", true));
        autoCenterCheckBox.setSelected((Boolean) settings.getOrDefault("navigation.autoCenter", true));
        showRouteCheckBox.setSelected((Boolean) settings.getOrDefault("navigation.showRoute", true));
        showTrackCheckBox.setSelected((Boolean) settings.getOrDefault("navigation.showTrack", true));
        trackIntervalSlider.setValue((Integer) settings.getOrDefault("navigation.trackInterval", 10));
        
        enableCacheCheckBox.setSelected((Boolean) settings.getOrDefault("performance.enableCache", true));
        cacheSizeSlider.setValue((Integer) settings.getOrDefault("performance.cacheSize", 500));
        enableGpuAccelerationCheckBox.setSelected((Boolean) settings.getOrDefault("performance.enableGpuAcceleration", true));
        enableMultithreadingCheckBox.setSelected((Boolean) settings.getOrDefault("performance.enableMultithreading", true));
        threadCountComboBox.setValue((String) settings.getOrDefault("performance.threadCount", "4"));
        
        enableLoggingCheckBox.setSelected((Boolean) settings.getOrDefault("advanced.enableLogging", true));
        logLevelComboBox.setValue((String) settings.getOrDefault("advanced.logLevel", "INFO"));
        autoSaveCheckBox.setSelected((Boolean) settings.getOrDefault("advanced.autoSave", true));
        autoSaveIntervalSlider.setValue((Integer) settings.getOrDefault("advanced.autoSaveInterval", 5));
    }
    
    private void updateSettingsFromControls() {
        settings.put("display.darkTheme", darkThemeCheckBox.isSelected());
        settings.put("display.showGrid", showGridCheckBox.isSelected());
        settings.put("display.showScaleBar", showScaleBarCheckBox.isSelected());
        settings.put("display.showCompass", showCompassCheckBox.isSelected());
        settings.put("display.showCoordinates", showCoordinatesCheckBox.isSelected());
        settings.put("display.zoomSpeed", zoomSpeedSlider.getValue());
        
        settings.put("chart.style", chartStyleComboBox.getValue());
        settings.put("chart.depthUnit", depthUnitComboBox.getValue());
        settings.put("chart.coordinateFormat", coordinateFormatComboBox.getValue());
        settings.put("chart.autoLoadCharts", autoLoadChartsCheckBox.isSelected());
        settings.put("chart.showChartInfo", showChartInfoCheckBox.isSelected());
        settings.put("chart.cachePath", chartCachePathField.getText());
        
        settings.put("navigation.enabled", enableNavigationCheckBox.isSelected());
        settings.put("navigation.autoCenter", autoCenterCheckBox.isSelected());
        settings.put("navigation.showRoute", showRouteCheckBox.isSelected());
        settings.put("navigation.showTrack", showTrackCheckBox.isSelected());
        settings.put("navigation.trackInterval", (int) trackIntervalSlider.getValue());
        
        settings.put("performance.enableCache", enableCacheCheckBox.isSelected());
        settings.put("performance.cacheSize", (int) cacheSizeSlider.getValue());
        settings.put("performance.enableGpuAcceleration", enableGpuAccelerationCheckBox.isSelected());
        settings.put("performance.enableMultithreading", enableMultithreadingCheckBox.isSelected());
        settings.put("performance.threadCount", threadCountComboBox.getValue());
        
        settings.put("advanced.enableLogging", enableLoggingCheckBox.isSelected());
        settings.put("advanced.logLevel", logLevelComboBox.getValue());
        settings.put("advanced.autoSave", autoSaveCheckBox.isSelected());
        settings.put("advanced.autoSaveInterval", (int) autoSaveIntervalSlider.getValue());
        
        if (onSettingsChanged != null) {
            onSettingsChanged.run();
        }
    }
    
    @FXML
    private void onHide() {
        if (onHidePanel != null) {
            onHidePanel.run();
        }
    }
    
    @FXML
    private void onApply() {
        applySettings();
    }
    
    @FXML
    private void onReset() {
        resetToDefaults();
    }
    
    @FXML
    private void onBrowseCachePath() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("选择缓存目录");
        fileChooser.setInitialDirectory(new File(System.getProperty("user.home")));
        
        File selectedDir = fileChooser.showOpenDialog(chartCachePathField.getScene().getWindow());
        if (selectedDir != null) {
            chartCachePathField.setText(selectedDir.getAbsolutePath());
        }
    }
    
    public void applySettings() {
        updateSettingsFromControls();
        
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle("设置已保存");
        alert.setHeaderText(null);
        alert.setContentText("设置已成功应用！");
        alert.showAndWait();
    }
    
    public void resetToDefaults() {
        Alert confirm = new Alert(Alert.AlertType.CONFIRMATION);
        confirm.setTitle("重置设置");
        confirm.setHeaderText("确认重置所有设置为默认值？");
        confirm.setContentText("此操作将撤销所有自定义设置。");
        
        confirm.showAndWait().ifPresent(response -> {
            if (response == ButtonType.OK) {
                loadDefaultSettings();
                
                if (onSettingsChanged != null) {
                    onSettingsChanged.run();
                }
            }
        });
    }
    
    public Object getSetting(String key) {
        return settings.get(key);
    }
    
    public void setSetting(String key, Object value) {
        settings.put(key, value);
        isUpdating = true;
        try {
            updateControlsFromSettings();
        } finally {
            isUpdating = false;
        }
    }
    
    public Map<String, Object> getAllSettings() {
        return new HashMap<>(settings);
    }
    
    public void onActivate() {
    }
    
    public void onDeactivate() {
    }
    
    public void setOnSettingsChanged(Runnable callback) {
        this.onSettingsChanged = callback;
    }
    
    public void setOnHidePanel(Runnable callback) {
        this.onHidePanel = callback;
    }
}
