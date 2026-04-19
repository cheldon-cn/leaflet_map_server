package cn.cycle.echart.ui;

import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Region;
import javafx.scene.layout.Priority;

/**
 * 状态栏。
 * 
 * <p>显示应用状态信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class StatusBar extends HBox {

    private final Label statusLabel;
    private final Label coordinatesLabel;
    private final Label zoomLabel;
    private final Label scaleLabel;
    private final Label timeLabel;

    public StatusBar() {
        this.statusLabel = new Label("就绪");
        this.coordinatesLabel = new Label("经度: -- 纬度: --");
        this.zoomLabel = new Label("缩放: 100%");
        this.scaleLabel = new Label("比例尺: 1:10000");
        this.timeLabel = new Label();
        
        initializeLayout();
        updateTime();
    }

    private void initializeLayout() {
        setAlignment(Pos.CENTER_LEFT);
        setSpacing(10);
        setPadding(new javafx.geometry.Insets(5, 10, 5, 10));
        
        Region spacer = new Region();
        HBox.setHgrow(spacer, Priority.ALWAYS);
        
        getChildren().addAll(
                statusLabel,
                createSeparator(),
                coordinatesLabel,
                createSeparator(),
                zoomLabel,
                createSeparator(),
                scaleLabel,
                spacer,
                timeLabel
        );
        
        getStyleClass().add("status-bar");
        
        statusLabel.getStyleClass().add("status-label");
        coordinatesLabel.getStyleClass().add("coordinates-label");
        zoomLabel.getStyleClass().add("zoom-label");
        scaleLabel.getStyleClass().add("scale-label");
        timeLabel.getStyleClass().add("time-label");
    }

    private Label createSeparator() {
        Label separator = new Label("|");
        separator.getStyleClass().add("status-separator");
        return separator;
    }

    public void setStatus(String status) {
        statusLabel.setText(status);
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

    private void updateTime() {
        java.time.format.DateTimeFormatter formatter = 
                java.time.format.DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
        timeLabel.setText(java.time.LocalDateTime.now().format(formatter));
    }

    public void startClock() {
        javafx.animation.Timeline timeline = new javafx.animation.Timeline(
                new javafx.animation.KeyFrame(
                        javafx.util.Duration.seconds(1),
                        event -> updateTime()
                )
        );
        timeline.setCycleCount(javafx.animation.Animation.INDEFINITE);
        timeline.play();
    }

    public Label getStatusLabel() {
        return statusLabel;
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

    public Label getTimeLabel() {
        return timeLabel;
    }
}
