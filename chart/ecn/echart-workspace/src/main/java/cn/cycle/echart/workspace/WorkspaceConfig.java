package cn.cycle.echart.workspace;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 工作区配置。
 * 
 * <p>存储工作区的配置信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WorkspaceConfig {

    private double centerLatitude;
    private double centerLongitude;
    private double zoomLevel;
    private double rotation;
    
    private boolean showAisLayer;
    private boolean showRouteLayer;
    private boolean showAlarmLayer;
    private boolean showChartLayer;
    
    private String displayMode;
    private String colorScheme;
    
    private final Map<String, String> layerSettings;
    private final Map<String, Object> customSettings;

    public WorkspaceConfig() {
        this.centerLatitude = 0;
        this.centerLongitude = 0;
        this.zoomLevel = 1.0;
        this.rotation = 0;
        
        this.showAisLayer = true;
        this.showRouteLayer = true;
        this.showAlarmLayer = true;
        this.showChartLayer = true;
        
        this.displayMode = "normal";
        this.colorScheme = "day";
        
        this.layerSettings = new HashMap<>();
        this.customSettings = new HashMap<>();
    }

    public double getCenterLatitude() {
        return centerLatitude;
    }

    public void setCenterLatitude(double centerLatitude) {
        this.centerLatitude = centerLatitude;
    }

    public double getCenterLongitude() {
        return centerLongitude;
    }

    public void setCenterLongitude(double centerLongitude) {
        this.centerLongitude = centerLongitude;
    }

    public double getZoomLevel() {
        return zoomLevel;
    }

    public void setZoomLevel(double zoomLevel) {
        this.zoomLevel = zoomLevel;
    }

    public double getRotation() {
        return rotation;
    }

    public void setRotation(double rotation) {
        this.rotation = rotation;
    }

    public boolean isShowAisLayer() {
        return showAisLayer;
    }

    public void setShowAisLayer(boolean showAisLayer) {
        this.showAisLayer = showAisLayer;
    }

    public boolean isShowRouteLayer() {
        return showRouteLayer;
    }

    public void setShowRouteLayer(boolean showRouteLayer) {
        this.showRouteLayer = showRouteLayer;
    }

    public boolean isShowAlarmLayer() {
        return showAlarmLayer;
    }

    public void setShowAlarmLayer(boolean showAlarmLayer) {
        this.showAlarmLayer = showAlarmLayer;
    }

    public boolean isShowChartLayer() {
        return showChartLayer;
    }

    public void setShowChartLayer(boolean showChartLayer) {
        this.showChartLayer = showChartLayer;
    }

    public String getDisplayMode() {
        return displayMode;
    }

    public void setDisplayMode(String displayMode) {
        this.displayMode = displayMode != null ? displayMode : "normal";
    }

    public String getColorScheme() {
        return colorScheme;
    }

    public void setColorScheme(String colorScheme) {
        this.colorScheme = colorScheme != null ? colorScheme : "day";
    }

    public Map<String, String> getLayerSettings() {
        return layerSettings;
    }

    public void setLayerSetting(String layerName, String setting) {
        Objects.requireNonNull(layerName, "layerName cannot be null");
        layerSettings.put(layerName, setting);
    }

    public String getLayerSetting(String layerName) {
        return layerSettings.get(layerName);
    }

    public Map<String, Object> getCustomSettings() {
        return customSettings;
    }

    public void setCustomSetting(String key, Object value) {
        Objects.requireNonNull(key, "key cannot be null");
        customSettings.put(key, value);
    }

    @SuppressWarnings("unchecked")
    public <T> T getCustomSetting(String key, Class<T> type) {
        Object value = customSettings.get(key);
        if (value != null && type.isInstance(value)) {
            return (T) value;
        }
        return null;
    }

    public void clear() {
        centerLatitude = 0;
        centerLongitude = 0;
        zoomLevel = 1.0;
        rotation = 0;
        
        showAisLayer = true;
        showRouteLayer = true;
        showAlarmLayer = true;
        showChartLayer = true;
        
        displayMode = "normal";
        colorScheme = "day";
        
        layerSettings.clear();
        customSettings.clear();
    }

    @Override
    public String toString() {
        return String.format("WorkspaceConfig{center=(%.4f, %.4f), zoom=%.2f, mode=%s}", 
                centerLatitude, centerLongitude, zoomLevel, displayMode);
    }
}
