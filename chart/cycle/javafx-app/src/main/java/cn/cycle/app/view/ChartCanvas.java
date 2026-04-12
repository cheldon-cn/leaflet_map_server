package cn.cycle.app.view;

import cn.cycle.chart.api.adapter.CanvasAdapter;
import cn.cycle.chart.api.adapter.PanHandler;
import cn.cycle.chart.api.adapter.ZoomHandler;
import cn.cycle.chart.api.core.ChartViewer;
import cn.cycle.chart.api.core.Viewport;
import cn.cycle.chart.api.geometry.Coordinate;
import javafx.scene.ImageCursor;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.ContextMenu;
import javafx.scene.control.MenuItem;
import javafx.scene.control.SeparatorMenuItem;
import javafx.scene.image.Image;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.Pane;

public class ChartCanvas extends Pane {

    private Canvas canvas;
    private ChartViewer chartViewer;
    private CanvasAdapter canvasAdapter;
    private PanHandler panHandler;
    private ZoomHandler zoomHandler;
    private ContextMenu contextMenu;
    
    private enum InteractionMode {
        SELECT,
        PAN,
        ZOOM_IN,
        ZOOM_OUT
    }
    
    private InteractionMode currentMode = InteractionMode.SELECT;
    private double lastMouseX;
    private double lastMouseY;
    private boolean isDragging = false;
    private ImageCursor zoomInCursor;
    private ImageCursor zoomOutCursor;

    public ChartCanvas(ChartViewer chartViewer) {
        this.chartViewer = chartViewer;
        this.canvas = new Canvas();
        getChildren().add(canvas);
        
        canvas.widthProperty().bind(widthProperty());
        canvas.heightProperty().bind(heightProperty());
        
        widthProperty().addListener(obs -> render());
        heightProperty().addListener(obs -> render());

        loadCursors();
        setupAdapter();
        setupContextMenu();
        setupMouseHandlers();
        setStyle("-fx-background-color: white;");
    }

    private void loadCursors() {
        try {
            Image zoomInImage = new Image(getClass().getResourceAsStream("/control/Png_ZoomIn_16.png"));
            zoomInCursor = new ImageCursor(zoomInImage, 8, 8);
        } catch (Exception e) {
            zoomInCursor = null;
        }
        
        try {
            Image zoomOutImage = new Image(getClass().getResourceAsStream("/control/Png_ZoomOut_16.png"));
            zoomOutCursor = new ImageCursor(zoomOutImage, 8, 8);
        } catch (Exception e) {
            zoomOutCursor = null;
        }
    }

    private void setupAdapter() {
        if (chartViewer != null && canvas != null) {
            canvasAdapter = new CanvasAdapter(canvas, chartViewer);
            
            panHandler = new PanHandler();
            zoomHandler = new ZoomHandler();
            
            canvasAdapter.addHandler(panHandler);
            canvasAdapter.addHandler(zoomHandler);
        }
    }

    private void setupContextMenu() {
        contextMenu = new ContextMenu();
        
        MenuItem selectModeItem = new MenuItem("选择模式");
        selectModeItem.setOnAction(e -> setMode(InteractionMode.SELECT));
        
        MenuItem panModeItem = new MenuItem("移动视图");
        panModeItem.setOnAction(e -> setMode(InteractionMode.PAN));
        
        MenuItem zoomInItem = new MenuItem("放大");
        zoomInItem.setOnAction(e -> setMode(InteractionMode.ZOOM_IN));
        
        MenuItem zoomOutItem = new MenuItem("缩小");
        zoomOutItem.setOnAction(e -> setMode(InteractionMode.ZOOM_OUT));
        
        MenuItem fitWindowItem = new MenuItem("适应窗口");
        fitWindowItem.setOnAction(e -> fitToWindow());
        
        MenuItem actualSizeItem = new MenuItem("实际大小");
        actualSizeItem.setOnAction(e -> actualSize());
        
        MenuItem resetViewItem = new MenuItem("重置视图");
        resetViewItem.setOnAction(e -> resetView());
        
        MenuItem centerViewItem = new MenuItem("居中显示");
        centerViewItem.setOnAction(e -> centerView());
        
        contextMenu.getItems().addAll(
            selectModeItem,
            panModeItem,
            zoomInItem,
            zoomOutItem,
            new SeparatorMenuItem(),
            fitWindowItem,
            actualSizeItem,
            new SeparatorMenuItem(),
            resetViewItem,
            centerViewItem
        );
        
        setOnContextMenuRequested(e -> {
            contextMenu.show(this, e.getScreenX(), e.getScreenY());
            e.consume();
        });
    }

    private void setupMouseHandlers() {
        setOnMousePressed(this::handleMousePressed);
        setOnMouseReleased(this::handleMouseReleased);
        setOnMouseDragged(this::handleMouseDragged);
        setOnScroll(this::handleScroll);
    }

    private void setMode(InteractionMode mode) {
        this.currentMode = mode;
        updateCursor();
    }

    private void updateCursor() {
        switch (currentMode) {
            case SELECT:
                setCursor(javafx.scene.Cursor.DEFAULT);
                break;
            case PAN:
                setCursor(javafx.scene.Cursor.MOVE);
                break;
            case ZOOM_IN:
                if (zoomInCursor != null) {
                    setCursor(zoomInCursor);
                } else {
                    setCursor(javafx.scene.Cursor.CROSSHAIR);
                }
                break;
            case ZOOM_OUT:
                if (zoomOutCursor != null) {
                    setCursor(zoomOutCursor);
                } else {
                    setCursor(javafx.scene.Cursor.CROSSHAIR);
                }
                break;
        }
    }

    private void handleMousePressed(MouseEvent e) {
        if (e.getButton() == MouseButton.PRIMARY) {
            lastMouseX = e.getX();
            lastMouseY = e.getY();
            isDragging = true;
            
            if (currentMode == InteractionMode.PAN) {
                setCursor(javafx.scene.Cursor.CLOSED_HAND);
            }
        }
    }

    private void handleMouseReleased(MouseEvent e) {
        if (e.getButton() == MouseButton.PRIMARY) {
            boolean wasDrag = Math.abs(e.getX() - lastMouseX) > 5 || Math.abs(e.getY() - lastMouseY) > 5;
            
            if (!wasDrag) {
                if (currentMode == InteractionMode.ZOOM_IN) {
                    zoomInAt(e.getX(), e.getY());
                } else if (currentMode == InteractionMode.ZOOM_OUT) {
                    zoomOutAt(e.getX(), e.getY());
                }
            }
            
            isDragging = false;
            updateCursor();
        }
    }

    private void handleMouseDragged(MouseEvent e) {
        if (isDragging && e.getButton() == MouseButton.PRIMARY) {
            double dx = e.getX() - lastMouseX;
            double dy = e.getY() - lastMouseY;
            
            if (currentMode == InteractionMode.PAN || currentMode == InteractionMode.SELECT) {
                if (chartViewer != null) {
                    chartViewer.pan(-dx, -dy);
                    render();
                } else {
                    System.out.println("[Java:DRAG] ERROR: chartViewer is null!");
                }
            }
            
            lastMouseX = e.getX();
            lastMouseY = e.getY();
        }
    }

    private void handleScroll(ScrollEvent e) {
        if (chartViewer != null) {
            e.consume();
        } else {
            System.out.println("[Java:SCROLL] ERROR: chartViewer is null!");
        }
    }

    private void zoomIn() {
        zoomInAt(getWidth() / 2, getHeight() / 2);
    }

    private void zoomInAt(double screenX, double screenY) {
        if (chartViewer != null) {
            Coordinate worldCoord = chartViewer.screenToWorld(screenX, screenY);
            if (worldCoord != null) {
                chartViewer.getViewportObject().zoom(1.25, worldCoord.getX(), worldCoord.getY());
                render();
            }
        }
    }

    private void zoomOut() {
        zoomOutAt(getWidth() / 2, getHeight() / 2);
    }

    private void zoomOutAt(double screenX, double screenY) {
        if (chartViewer != null) {
            Coordinate worldCoord = chartViewer.screenToWorld(screenX, screenY);
            if (worldCoord != null) {
                chartViewer.getViewportObject().zoom(0.8, worldCoord.getX(), worldCoord.getY());
                render();
            }
        }
    }

    private void fitToWindow() {
        if (chartViewer != null) {
            chartViewer.fitToWindow();
            render();
        }
    }

    private void actualSize() {
        if (chartViewer != null) {
            chartViewer.setZoom(1.0);
            render();
        }
    }

    private void resetView() {
        if (chartViewer != null) {
            chartViewer.resetView();
            render();
        }
    }

    private void centerView() {
        if (chartViewer != null) {
            chartViewer.centerView();
            render();
        }
    }

    public void setChartViewer(ChartViewer viewer) {
        if (canvasAdapter != null) {
            canvasAdapter.dispose();
        }
        this.chartViewer = viewer;
        setupAdapter();
        render();
    }

    public void render() {
        if (canvasAdapter != null && !canvasAdapter.isDisposed()) {
            canvasAdapter.render();
        }
    }

    public void clear() {
        if (canvasAdapter != null) {
            canvasAdapter.clear();
        }
    }

    public CanvasAdapter getCanvasAdapter() {
        return canvasAdapter;
    }

    public void dispose() {
        if (canvasAdapter != null) {
            canvasAdapter.dispose();
            canvasAdapter = null;
        }
    }

    public boolean isDisposed() {
        return canvasAdapter == null || canvasAdapter.isDisposed();
    }
}
