package cn.cycle.echart.ui.panel;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

import java.net.URL;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;
import java.util.function.Consumer;

/**
 * 测量工具面板控制器。
 * 
 * @author Cycle Team
 * @version 1.0.0
 */
public class MeasurePanelController implements Initializable {

    @FXML private ToggleButton distanceButton;
    @FXML private ToggleButton areaButton;
    @FXML private ToggleButton angleButton;
    @FXML private ToggleButton coordinateButton;
    @FXML private Button clearButton;
    @FXML private Button hideButton;
    @FXML private Button undoButton;
    @FXML private Button redoButton;
    @FXML private ListView<Measurement> measurementListView;
    @FXML private Label currentModeLabel;
    @FXML private Label resultLabel;
    @FXML private VBox emptyPlaceholder;
    
    private ToggleGroup measureToggleGroup;
    private ObservableList<Measurement> measurements;
    private MeasureMode currentMode = MeasureMode.DISTANCE;
    private Consumer<Measurement> onMeasurementSelected;
    private Runnable onHidePanel;
    private Consumer<MeasureMode> onMeasureModeChanged;
    
    public enum MeasureMode {
        DISTANCE("距离测量"),
        AREA("面积测量"),
        ANGLE("角度测量"),
        COORDINATE("坐标拾取");
        
        private final String displayName;
        
        MeasureMode(String displayName) {
            this.displayName = displayName;
        }
        
        @Override
        public String toString() {
            return displayName;
        }
    }
    
    public static class Measurement {
        private final String id;
        private final MeasureMode mode;
        private final String value;
        private final String unit;
        private final String description;
        private final LocalDateTime timestamp;
        private final List<double[]> points;
        
        public Measurement(String id, MeasureMode mode, String value, String unit, String description) {
            this.id = id;
            this.mode = mode;
            this.value = value;
            this.unit = unit;
            this.description = description;
            this.timestamp = LocalDateTime.now();
            this.points = new ArrayList<>();
        }
        
        public String getId() { return id; }
        public MeasureMode getMode() { return mode; }
        public String getValue() { return value; }
        public String getUnit() { return unit; }
        public String getDescription() { return description; }
        public LocalDateTime getTimestamp() { return timestamp; }
        public List<double[]> getPoints() { return points; }
        
        public void addPoint(double x, double y) {
            points.add(new double[]{x, y});
        }
        
        @Override
        public String toString() {
            return mode.toString() + ": " + value + " " + unit;
        }
    }
    
    @Override
    public void initialize(URL location, ResourceBundle resources) {
        measurements = FXCollections.observableArrayList();
        measureToggleGroup = new ToggleGroup();
        
        initializeControls();
        initializeListeners();
        showEmptyState();
    }
    
    private void initializeControls() {
        distanceButton.setToggleGroup(measureToggleGroup);
        areaButton.setToggleGroup(measureToggleGroup);
        angleButton.setToggleGroup(measureToggleGroup);
        coordinateButton.setToggleGroup(measureToggleGroup);
        
        distanceButton.setSelected(true);
        
        measurementListView.setItems(measurements);
        measurementListView.setCellFactory(list -> new MeasurementListCell());
        
        setToggleButtonIcon(distanceButton, "/icons/measure.png");
        setToggleButtonIcon(areaButton, "/icons/layers_32x32.png");
        setToggleButtonIcon(angleButton, "/icons/compass.png");
        setToggleButtonIcon(coordinateButton, "/icons/location.png");
        setButtonIcon(clearButton, "/icons/close_32x32.png");
        setButtonIcon(hideButton, "/icons/sidebar_32x321.png");
        setButtonIcon(undoButton, "/icons/back.png");
        setButtonIcon(redoButton, "/icons/forward.png");
        
        updateModeLabel();
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
    
    private void setToggleButtonIcon(ToggleButton button, String iconPath) {
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
        measureToggleGroup.selectedToggleProperty().addListener((obs, oldToggle, newToggle) -> {
            if (newToggle == distanceButton) {
                setMeasureMode(MeasureMode.DISTANCE);
            } else if (newToggle == areaButton) {
                setMeasureMode(MeasureMode.AREA);
            } else if (newToggle == angleButton) {
                setMeasureMode(MeasureMode.ANGLE);
            } else if (newToggle == coordinateButton) {
                setMeasureMode(MeasureMode.COORDINATE);
            }
        });
        
        measurementListView.getSelectionModel().selectedItemProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal != null && onMeasurementSelected != null) {
                onMeasurementSelected.accept(newVal);
            }
        });
        
        measurementListView.setOnMouseClicked(event -> {
            if (event.getClickCount() == 2) {
                Measurement selected = measurementListView.getSelectionModel().getSelectedItem();
                if (selected != null && onMeasurementSelected != null) {
                    onMeasurementSelected.accept(selected);
                }
            }
        });
    }
    
    @FXML
    private void onClear() {
        Alert confirm = new Alert(Alert.AlertType.CONFIRMATION);
        confirm.setTitle("清空测量");
        confirm.setHeaderText("确认清空所有测量结果？");
        confirm.setContentText("此操作将删除所有测量记录。");
        
        confirm.showAndWait().ifPresent(response -> {
            if (response == ButtonType.OK) {
                clearMeasurements();
            }
        });
    }
    
    @FXML
    private void onHide() {
        if (onHidePanel != null) {
            onHidePanel.run();
        }
    }
    
    @FXML
    private void onUndo() {
        showInfo("撤销", "撤销上一个测量点");
    }
    
    @FXML
    private void onRedo() {
        showInfo("重做", "重做上一个测量点");
    }
    
    public void setMeasureMode(MeasureMode mode) {
        this.currentMode = mode;
        updateModeLabel();
        
        if (onMeasureModeChanged != null) {
            onMeasureModeChanged.accept(mode);
        }
    }
    
    public void clearMeasurements() {
        measurements.clear();
        resultLabel.setText("等待测量...");
        showEmptyState();
    }
    
    public void addMeasurement(Measurement measurement) {
        measurements.add(0, measurement);
        updateResult(measurement);
        showResultsState();
    }
    
    public void setMeasurements(List<Measurement> measurements) {
        this.measurements.clear();
        if (measurements != null) {
            this.measurements.addAll(measurements);
        }
        
        if (this.measurements.isEmpty()) {
            showEmptyState();
        } else {
            showResultsState();
        }
    }
    
    private void updateModeLabel() {
        currentModeLabel.setText("当前模式: " + currentMode.toString());
    }
    
    private void updateResult(Measurement measurement) {
        resultLabel.setText("结果: " + measurement.getValue() + " " + measurement.getUnit());
    }
    
    private void showEmptyState() {
        emptyPlaceholder.setVisible(true);
        emptyPlaceholder.setManaged(true);
        measurementListView.setVisible(false);
        measurementListView.setManaged(false);
    }
    
    private void showResultsState() {
        emptyPlaceholder.setVisible(false);
        emptyPlaceholder.setManaged(false);
        measurementListView.setVisible(true);
        measurementListView.setManaged(true);
    }
    
    public void onActivate() {
    }
    
    public void onDeactivate() {
    }
    
    public void setOnMeasurementSelected(Consumer<Measurement> callback) {
        this.onMeasurementSelected = callback;
    }
    
    public void setOnHidePanel(Runnable callback) {
        this.onHidePanel = callback;
    }
    
    public void setOnMeasureModeChanged(Consumer<MeasureMode> callback) {
        this.onMeasureModeChanged = callback;
    }
    
    private void showInfo(String title, String message) {
        Alert alert = new Alert(Alert.AlertType.INFORMATION);
        alert.setTitle(title);
        alert.setHeaderText(null);
        alert.setContentText(message);
        alert.showAndWait();
    }
    
    private static class MeasurementListCell extends ListCell<Measurement> {
        private static final DateTimeFormatter TIME_FORMATTER = DateTimeFormatter.ofPattern("HH:mm:ss");
        
        @Override
        protected void updateItem(Measurement item, boolean empty) {
            super.updateItem(item, empty);
            
            if (empty || item == null) {
                setGraphic(null);
                setText(null);
                return;
            }
            
            VBox container = new VBox(4);
            container.setPadding(new Insets(6, 8, 6, 8));
            
            HBox header = new HBox(8);
            header.setAlignment(javafx.geometry.Pos.CENTER_LEFT);
            
            Label modeLabel = new Label(item.getMode().toString());
            modeLabel.setStyle("-fx-background-color: derive(-fx-accent, 80%); -fx-background-radius: 3; " +
                             "-fx-padding: 2 6 2 6; -fx-font-size: 10px; -fx-text-fill: -fx-text-background-color;");
            
            Label valueLabel = new Label(item.getValue() + " " + item.getUnit());
            valueLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 12px; -fx-text-fill: -fx-text-background-color;");
            
            header.getChildren().addAll(modeLabel, valueLabel);
            
            Label descLabel = new Label(item.getDescription());
            descLabel.setStyle("-fx-font-size: 11px; -fx-text-fill: derive(-fx-text-background-color, -20%);");
            descLabel.setWrapText(true);
            
            Label timeLabel = new Label("时间: " + item.getTimestamp().format(TIME_FORMATTER));
            timeLabel.setStyle("-fx-font-size: 10px; -fx-text-fill: derive(-fx-text-background-color, -40%);");
            
            container.getChildren().addAll(header, descLabel, timeLabel);
            
            setGraphic(container);
            setText(null);
        }
    }
}
