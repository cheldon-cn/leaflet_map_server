package cn.cycle.app.view;

import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.geometry.Coordinate;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.paint.Color;

public class ChartCanvas extends Canvas {

    private ChartViewer chartViewer;
    private double lastX;
    private double lastY;

    public ChartCanvas(ChartViewer chartViewer) {
        this.chartViewer = chartViewer;
        setWidth(800);
        setHeight(600);

        setupEventHandlers();
    }

    private void setupEventHandlers() {
        setOnMousePressed(this::onMousePressed);
        setOnMouseDragged(this::onMouseDragged);
        setOnMouseReleased(this::onMouseReleased);
        setOnScroll(this::onScroll);
        widthProperty().addListener(obs -> render());
        heightProperty().addListener(obs -> render());
    }

    public void setChartViewer(ChartViewer viewer) {
        this.chartViewer = viewer;
        render();
    }

    public void render() {
        if (chartViewer == null || chartViewer.isDisposed()) {
            return;
        }

        GraphicsContext gc = getGraphicsContext2D();
        gc.setFill(Color.WHITE);
        gc.fillRect(0, 0, getWidth(), getHeight());

        chartViewer.render(0, (int) getWidth(), (int) getHeight());
    }

    private void onMousePressed(MouseEvent e) {
        lastX = e.getX();
        lastY = e.getY();
    }

    private void onMouseDragged(MouseEvent e) {
        if (chartViewer == null) {
            return;
        }

        double dx = e.getX() - lastX;
        double dy = e.getY() - lastY;

        chartViewer.pan(-dx, -dy);

        lastX = e.getX();
        lastY = e.getY();

        render();
    }

    private void onMouseReleased(MouseEvent e) {
        if (chartViewer == null) {
            return;
        }

        Coordinate worldCoord = chartViewer.screenToWorld(e.getX(), e.getY());
        System.out.println("Clicked at: " + worldCoord.getX() + ", " + worldCoord.getY());
    }

    private void onScroll(ScrollEvent e) {
        if (chartViewer == null) {
            return;
        }

        double factor = e.getDeltaY() > 0 ? 1.1 : 0.9;
        chartViewer.zoom(factor, e.getX(), e.getY());

        render();
    }
}
