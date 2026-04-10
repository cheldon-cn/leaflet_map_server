package cn.cycle.chart.app.model;

import javafx.beans.property.*;

public class ChartModel {
    
    private final StringProperty chartPath = new SimpleStringProperty();
    private final BooleanProperty loaded = new SimpleBooleanProperty(false);
    private final DoubleProperty scale = new SimpleDoubleProperty(1.0);
    private final DoubleProperty centerX = new SimpleDoubleProperty(0);
    private final DoubleProperty centerY = new SimpleDoubleProperty(0);
    private final BooleanProperty rendering = new SimpleBooleanProperty(false);
    private final StringProperty chartName = new SimpleStringProperty("");
    
    public StringProperty chartPathProperty() { 
        return chartPath; 
    }
    
    public BooleanProperty loadedProperty() { 
        return loaded; 
    }
    
    public DoubleProperty scaleProperty() { 
        return scale; 
    }
    
    public DoubleProperty centerXProperty() { 
        return centerX; 
    }
    
    public DoubleProperty centerYProperty() { 
        return centerY; 
    }
    
    public BooleanProperty renderingProperty() { 
        return rendering; 
    }
    
    public StringProperty chartNameProperty() { 
        return chartName; 
    }
    
    public String getChartPath() {
        return chartPath.get();
    }
    
    public void setChartPath(String path) {
        chartPath.set(path);
    }
    
    public boolean isLoaded() {
        return loaded.get();
    }
    
    public void setLoaded(boolean value) {
        loaded.set(value);
    }
    
    public double getScale() {
        return scale.get();
    }
    
    public void setScale(double value) {
        scale.set(value);
    }
    
    public double getCenterX() {
        return centerX.get();
    }
    
    public void setCenterX(double value) {
        centerX.set(value);
    }
    
    public double getCenterY() {
        return centerY.get();
    }
    
    public void setCenterY(double value) {
        centerY.set(value);
    }
    
    public boolean isRendering() {
        return rendering.get();
    }
    
    public void setRendering(boolean value) {
        rendering.set(value);
    }
    
    public String getChartName() {
        return chartName.get();
    }
    
    public void setChartName(String name) {
        chartName.set(name);
    }
    
    public void reset() {
        chartPath.set("");
        loaded.set(false);
        scale.set(1.0);
        centerX.set(0);
        centerY.set(0);
        rendering.set(false);
        chartName.set("");
    }
}
