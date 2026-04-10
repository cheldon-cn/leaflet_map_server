package cn.cycle.app.view;

import cn.cycle.chart.api.geometry.Coordinate;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Envelope;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.text.Font;

public class PositionView extends VBox {

    private final Label coordinateLabel;
    private final Label scaleLabel;
    private final Label extentLabel;
    private final Label zoomLabel;

    private ChartViewer chartViewer;

    public PositionView() {
        setSpacing(5);
        setPadding(new Insets(5));
        setStyle("-fx-background-color: #f0f0f0; -fx-border-color: #cccccc; -fx-border-width: 1;");

        coordinateLabel = createLabel("坐标: --, --");
        scaleLabel = createLabel("比例尺: 1:--");
        extentLabel = createLabel("范围: --");
        zoomLabel = createLabel("缩放: --");

        getChildren().addAll(coordinateLabel, scaleLabel, extentLabel, zoomLabel);
        setAlignment(Pos.CENTER_LEFT);
    }

    private Label createLabel(String text) {
        Label label = new Label(text);
        label.setFont(Font.font("Monospaced", 10));
        return label;
    }

    public void setChartViewer(ChartViewer viewer) {
        this.chartViewer = viewer;
    }

    public void updateCoordinate(double screenX, double screenY) {
        if (chartViewer != null) {
            Coordinate coord = chartViewer.screenToWorld(screenX, screenY);
            if (coord != null) {
                coordinateLabel.setText(String.format("坐标: %.6f, %.6f", coord.getX(), coord.getY()));
            }
        }
    }

    public void updateScale(double scale) {
        scaleLabel.setText(String.format("比例尺: 1:%.0f", scale));
    }

    public void updateExtent(Envelope extent) {
        if (extent != null) {
            extentLabel.setText(String.format("范围: %.2f, %.2f - %.2f, %.2f",
                extent.getMinX(), extent.getMinY(), extent.getMaxX(), extent.getMaxY()));
        }
    }

    public void updateZoom(double zoom) {
        zoomLabel.setText(String.format("缩放: %.2fx", zoom));
    }

    public void updateFromViewport(Viewport viewport) {
        if (viewport != null) {
            updateScale(viewport.getScale());
            updateExtent(viewport.getExtent());
        }
    }

    public void clear() {
        coordinateLabel.setText("坐标: --, --");
        scaleLabel.setText("比例尺: 1:--");
        extentLabel.setText("范围: --");
        zoomLabel.setText("缩放: --");
    }
}
