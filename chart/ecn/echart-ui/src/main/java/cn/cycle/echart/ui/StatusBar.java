package cn.cycle.echart.ui;

import javafx.geometry.Orientation;
import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.Separator;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.paint.Color;

import java.time.format.DateTimeFormatter;

/**
 * 状态栏。
 * 
 * <p>使用ControlsFX的StatusBar控件显示应用状态信息。</p>
 * <p>包含状态文本、坐标、缩放、比例尺、预警数量、图层数量和时间等信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class StatusBar extends org.controlsfx.control.StatusBar {

    private final Label coordinatesLabel;
    private final Label zoomLabel;
    private final Label scaleLabel;
    private final Label alarmCountLabel;
    private final Label layerCountLabel;
    private final Label timeLabel;
    
    private int alarmCount = 0;
    private int layerCount = 0;

    public StatusBar() {
        this.coordinatesLabel = new Label("经度: -- 纬度: --");
        this.zoomLabel = new Label("缩放: 100%");
        this.scaleLabel = new Label("比例尺: 1:10000");
        this.alarmCountLabel = createAlarmCountLabel();
        this.layerCountLabel = createLayerCountLabel();
        this.timeLabel = new Label();
        
        initializeLayout();
        startClock();
    }

    private void initializeLayout() {
        setText("就绪");
        
        coordinatesLabel.getStyleClass().add("status-coordinates");
        zoomLabel.getStyleClass().add("status-zoom");
        scaleLabel.getStyleClass().add("status-scale");
        alarmCountLabel.getStyleClass().add("status-alarm-count");
        layerCountLabel.getStyleClass().add("status-layer-count");
        timeLabel.getStyleClass().add("status-time");
        
        getLeftItems().add(coordinatesLabel);
        getLeftItems().add(createSeparator());
        getLeftItems().add(zoomLabel);
        getLeftItems().add(createSeparator());
        getLeftItems().add(scaleLabel);
        
        getRightItems().add(alarmCountLabel);
        getRightItems().add(createSeparator());
        getRightItems().add(layerCountLabel);
        getRightItems().add(createSeparator());
        getRightItems().add(timeLabel);
    }

    private Label createAlarmCountLabel() {
        HBox container = new HBox(4);
        container.setAlignment(Pos.CENTER);
        
        ImageView icon = createIcon("/icons/alarm_small.png", 12);
        if (icon != null) {
            container.getChildren().add(icon);
        }
        
        Label label = new Label("预警: 0");
        container.getChildren().add(label);
        
        Label result = new Label();
        result.setGraphic(container);
        result.getStyleClass().add("status-alarm-container");
        
        return result;
    }

    private Label createLayerCountLabel() {
        HBox container = new HBox(4);
        container.setAlignment(Pos.CENTER);
        
        ImageView icon = createIcon("/icons/layers_small.png", 12);
        if (icon != null) {
            container.getChildren().add(icon);
        }
        
        Label label = new Label("图层: 0");
        container.getChildren().add(label);
        
        Label result = new Label();
        result.setGraphic(container);
        result.getStyleClass().add("status-layer-container");
        
        return result;
    }

    private ImageView createIcon(String path, int size) {
        try {
            Image image = new Image(getClass().getResourceAsStream(path));
            if (image != null && !image.isError()) {
                ImageView imageView = new ImageView(image);
                imageView.setFitWidth(size);
                imageView.setFitHeight(size);
                return imageView;
            }
        } catch (Exception e) {
        }
        return null;
    }

    private Separator createSeparator() {
        return new Separator(Orientation.VERTICAL);
    }

    public void setStatus(String status) {
        setText(status);
    }

    public void setCoordinates(double longitude, double latitude) {
        coordinatesLabel.setText(String.format("经度: %.6f 纬度: %.6f", longitude, latitude));
    }

    public void setZoom(double zoom) {
        zoomLabel.setText(String.format("缩放: %.0f%%", zoom * 100));
    }

    public void setScale(double scale) {
        scaleLabel.setText(String.format("比例尺: 1:%.0f", scale));
    }

    public void setAlarmCount(int count) {
        this.alarmCount = count;
        updateAlarmCountDisplay();
    }

    public int getAlarmCount() {
        return alarmCount;
    }

    public void incrementAlarmCount() {
        this.alarmCount++;
        updateAlarmCountDisplay();
    }

    public void decrementAlarmCount() {
        if (this.alarmCount > 0) {
            this.alarmCount--;
            updateAlarmCountDisplay();
        }
    }

    private void updateAlarmCountDisplay() {
        HBox container = (HBox) alarmCountLabel.getGraphic();
        if (container != null && container.getChildren().size() > 1) {
            Label label = (Label) container.getChildren().get(1);
            label.setText("预警: " + alarmCount);
            
            if (alarmCount > 0) {
                label.setTextFill(Color.RED);
                alarmCountLabel.getStyleClass().add("has-alarms");
            } else {
                label.setTextFill(Color.BLACK);
                alarmCountLabel.getStyleClass().remove("has-alarms");
            }
        }
    }

    public void setLayerCount(int count) {
        this.layerCount = count;
        updateLayerCountDisplay();
    }

    public int getLayerCount() {
        return layerCount;
    }

    public void incrementLayerCount() {
        this.layerCount++;
        updateLayerCountDisplay();
    }

    public void decrementLayerCount() {
        if (this.layerCount > 0) {
            this.layerCount--;
            updateLayerCountDisplay();
        }
    }

    private void updateLayerCountDisplay() {
        HBox container = (HBox) layerCountLabel.getGraphic();
        if (container != null && container.getChildren().size() > 1) {
            Label label = (Label) container.getChildren().get(1);
            label.setText("图层: " + layerCount);
        }
    }

    private void updateTime() {
        DateTimeFormatter formatter = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
        timeLabel.setText(java.time.LocalDateTime.now().format(formatter));
    }

    private void startClock() {
        updateTime();
        javafx.animation.Timeline timeline = new javafx.animation.Timeline(
                new javafx.animation.KeyFrame(
                        javafx.util.Duration.seconds(1),
                        event -> updateTime()
                )
        );
        timeline.setCycleCount(javafx.animation.Animation.INDEFINITE);
        timeline.play();
    }

    public Label getCoordinatesLabel() {
        return coordinatesLabel;
    }

    public Label getZoomLabel() {
        return zoomLabel;
    }

    public Label getScaleLabel() {
        return scaleLabel;
    }

    public Label getAlarmCountLabel() {
        return alarmCountLabel;
    }

    public Label getLayerCountLabel() {
        return layerCountLabel;
    }

    public Label getTimeLabel() {
        return timeLabel;
    }
}
