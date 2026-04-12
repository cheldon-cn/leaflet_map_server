package cn.cycle.app.view;

import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.input.MouseButton;
import javafx.scene.input.MouseEvent;
import javafx.scene.input.ScrollEvent;
import javafx.scene.layout.Pane;

import java.io.File;

public class ImageCanvas extends Pane {

    private Canvas canvas;
    private Image image;
    private double offsetX = 0;
    private double offsetY = 0;
    private double scale = 1.0;
    private double lastMouseX;
    private double lastMouseY;
    private boolean isDragging = false;
    private double minScale = 0.1;
    private double maxScale = 10.0;

    public ImageCanvas() {
        this.canvas = new Canvas();
        getChildren().add(canvas);
        
        canvas.widthProperty().bind(widthProperty());
        canvas.heightProperty().bind(heightProperty());
        
        widthProperty().addListener(obs -> render());
        heightProperty().addListener(obs -> render());
        
        setupMouseHandlers();
        setStyle("-fx-background-color: #333333;");
    }

    private void setupMouseHandlers() {
        setOnMousePressed(this::handleMousePressed);
        setOnMouseReleased(this::handleMouseReleased);
        setOnMouseDragged(this::handleMouseDragged);
        setOnScroll(this::handleScroll);
    }

    public boolean loadImage(String filePath) {
        try {
            File file = new File(filePath);
            if (!file.exists()) {
                System.out.println("[ERROR] File not found: " + filePath);
                return false;
            }
            
            image = new Image(file.toURI().toString());
            if (image.isError()) {
                System.out.println("[ERROR] Failed to load image: " + image.getException().getMessage());
                return false;
            }
            
            System.out.println("[DEBUG] Image loaded: " + image.getWidth() + " x " + image.getHeight());
            
            resetView();
            return true;
        } catch (Exception e) {
            System.out.println("[ERROR] Exception loading image: " + e.getMessage());
            e.printStackTrace();
            return false;
        }
    }

    public void resetView() {
        if (image == null) return;
        
        scale = 1.0;
        offsetX = 0;
        offsetY = 0;
        
        fitToWindow();
    }

    public void fitToWindow() {
        if (image == null) return;
        
        double canvasWidth = getWidth();
        double canvasHeight = getHeight();
        
        if (canvasWidth <= 0 || canvasHeight <= 0) return;
        
        double scaleX = canvasWidth / image.getWidth();
        double scaleY = canvasHeight / image.getHeight();
        scale = Math.min(scaleX, scaleY);
        
        if (scale < minScale) scale = minScale;
        if (scale > maxScale) scale = maxScale;
        
        double scaledWidth = image.getWidth() * scale;
        double scaledHeight = image.getHeight() * scale;
        
        offsetX = (canvasWidth - scaledWidth) / 2;
        offsetY = (canvasHeight - scaledHeight) / 2;
        
        render();
    }

    public void actualSize() {
        if (image == null) return;
        
        scale = 1.0;
        
        double canvasWidth = getWidth();
        double canvasHeight = getHeight();
        double scaledWidth = image.getWidth() * scale;
        double scaledHeight = image.getHeight() * scale;
        
        offsetX = (canvasWidth - scaledWidth) / 2;
        offsetY = (canvasHeight - scaledHeight) / 2;
        
        render();
    }

    public void zoomIn() {
        zoomAt(getWidth() / 2, getHeight() / 2, 1.25);
    }

    public void zoomOut() {
        zoomAt(getWidth() / 2, getHeight() / 2, 0.8);
    }

    private void zoomAt(double centerX, double centerY, double factor) {
        if (image == null) return;
        
        double newScale = scale * factor;
        if (newScale < minScale || newScale > maxScale) return;
        
        double worldX = (centerX - offsetX) / scale;
        double worldY = (centerY - offsetY) / scale;
        
        scale = newScale;
        
        offsetX = centerX - worldX * scale;
        offsetY = centerY - worldY * scale;
        
        render();
    }

    private void handleMousePressed(MouseEvent e) {
        if (e.getButton() == MouseButton.PRIMARY || e.getButton() == MouseButton.MIDDLE) {
            lastMouseX = e.getX();
            lastMouseY = e.getY();
            isDragging = true;
        }
    }

    private void handleMouseReleased(MouseEvent e) {
        if (e.getButton() == MouseButton.PRIMARY || e.getButton() == MouseButton.MIDDLE) {
            isDragging = false;
        }
    }

    private void handleMouseDragged(MouseEvent e) {
        if (isDragging) {
            double dx = e.getX() - lastMouseX;
            double dy = e.getY() - lastMouseY;
            
            offsetX += dx;
            offsetY += dy;
            
            lastMouseX = e.getX();
            lastMouseY = e.getY();
            
            render();
        }
    }

    private void handleScroll(ScrollEvent e) {
        double delta = e.getDeltaY();
        if (delta != 0) {
            double factor = delta > 0 ? 1.1 : 0.9;
            zoomAt(e.getX(), e.getY(), factor);
            e.consume();
        }
    }

    public void render() {
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
        
        if (image == null) return;
        
        gc.save();
        gc.translate(offsetX, offsetY);
        gc.scale(scale, scale);
        gc.drawImage(image, 0, 0);
        gc.restore();
    }

    public void clear() {
        image = null;
        offsetX = 0;
        offsetY = 0;
        scale = 1.0;
        GraphicsContext gc = canvas.getGraphicsContext2D();
        gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
    }

    public boolean hasImage() {
        return image != null;
    }

    public double getScale() {
        return scale;
    }
}
