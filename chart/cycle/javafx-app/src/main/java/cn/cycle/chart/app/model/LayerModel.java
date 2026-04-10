package cn.cycle.chart.app.model;

import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;

public class LayerModel {
    
    private final ObservableList<LayerItem> layers = FXCollections.observableArrayList();
    private final ObjectProperty<LayerItem> selectedLayer = new SimpleObjectProperty<>();
    
    public ObservableList<LayerItem> getLayers() { 
        return layers; 
    }
    
    public ObjectProperty<LayerItem> selectedLayerProperty() { 
        return selectedLayer; 
    }
    
    public LayerItem getSelectedLayer() {
        return selectedLayer.get();
    }
    
    public void setSelectedLayer(LayerItem layer) {
        selectedLayer.set(layer);
    }
    
    public void addLayer(LayerItem layer) {
        layers.add(layer);
    }
    
    public void removeLayer(LayerItem layer) {
        layers.remove(layer);
    }
    
    public void clearLayers() {
        layers.clear();
        selectedLayer.set(null);
    }
    
    public void moveLayer(int fromIndex, int toIndex) {
        if (fromIndex >= 0 && fromIndex < layers.size() && 
            toIndex >= 0 && toIndex < layers.size()) {
            LayerItem item = layers.remove(fromIndex);
            layers.add(toIndex, item);
        }
    }
    
    public static class LayerItem {
        private final String name;
        private final String type;
        private boolean visible;
        private double opacity;
        
        public LayerItem(String name, String type) {
            this.name = name;
            this.type = type;
            this.visible = true;
            this.opacity = 1.0;
        }
        
        public String getName() {
            return name;
        }
        
        public String getType() {
            return type;
        }
        
        public boolean isVisible() {
            return visible;
        }
        
        public void setVisible(boolean visible) {
            this.visible = visible;
        }
        
        public double getOpacity() {
            return opacity;
        }
        
        public void setOpacity(double opacity) {
            this.opacity = opacity;
        }
        
        @Override
        public String toString() {
            return name;
        }
    }
}
