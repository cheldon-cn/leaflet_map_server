package cn.cycle.chart.app.model;

import javafx.beans.property.*;

public class SettingsModel {
    
    private final BooleanProperty antialiasing = new SimpleBooleanProperty(true);
    private final StringProperty quality = new SimpleStringProperty("中");
    private final DoubleProperty cacheSize = new SimpleDoubleProperty(512);
    private final IntegerProperty threadCount = new SimpleIntegerProperty(4);
    private final BooleanProperty preloadEnabled = new SimpleBooleanProperty(true);
    private final StringProperty chartPath = new SimpleStringProperty("");
    private final StringProperty cachePath = new SimpleStringProperty("");
    private final BooleanProperty showGrid = new SimpleBooleanProperty(true);
    private final BooleanProperty showScaleBar = new SimpleBooleanProperty(true);
    private final BooleanProperty showCompass = new SimpleBooleanProperty(true);
    private final BooleanProperty hardwareAccel = new SimpleBooleanProperty(true);
    private final StringProperty language = new SimpleStringProperty("zh_CN");
    private final StringProperty theme = new SimpleStringProperty("default");
    
    public BooleanProperty antialiasingProperty() { 
        return antialiasing; 
    }
    
    public StringProperty qualityProperty() { 
        return quality; 
    }
    
    public DoubleProperty cacheSizeProperty() { 
        return cacheSize; 
    }
    
    public IntegerProperty threadCountProperty() { 
        return threadCount; 
    }
    
    public BooleanProperty preloadEnabledProperty() { 
        return preloadEnabled; 
    }
    
    public StringProperty chartPathProperty() {
        return chartPath;
    }
    
    public StringProperty cachePathProperty() {
        return cachePath;
    }
    
    public BooleanProperty showGridProperty() {
        return showGrid;
    }
    
    public BooleanProperty showScaleBarProperty() {
        return showScaleBar;
    }
    
    public BooleanProperty showCompassProperty() {
        return showCompass;
    }
    
    public BooleanProperty hardwareAccelProperty() {
        return hardwareAccel;
    }
    
    public StringProperty languageProperty() {
        return language;
    }
    
    public StringProperty themeProperty() {
        return theme;
    }
    
    public boolean isAntialiasing() {
        return antialiasing.get();
    }
    
    public void setAntialiasing(boolean value) {
        antialiasing.set(value);
    }
    
    public String getQuality() {
        return quality.get();
    }
    
    public void setQuality(String value) {
        quality.set(value);
    }
    
    public double getCacheSize() {
        return cacheSize.get();
    }
    
    public void setCacheSize(double value) {
        cacheSize.set(value);
    }
    
    public int getThreadCount() {
        return threadCount.get();
    }
    
    public void setThreadCount(int value) {
        threadCount.set(value);
    }
    
    public boolean isPreloadEnabled() {
        return preloadEnabled.get();
    }
    
    public void setPreloadEnabled(boolean value) {
        preloadEnabled.set(value);
    }
    
    public String getChartPath() {
        return chartPath.get();
    }
    
    public void setChartPath(String path) {
        chartPath.set(path);
    }
    
    public String getCachePath() {
        return cachePath.get();
    }
    
    public void setCachePath(String path) {
        cachePath.set(path);
    }
    
    public boolean isShowGrid() {
        return showGrid.get();
    }
    
    public void setShowGrid(boolean value) {
        showGrid.set(value);
    }
    
    public boolean isShowScaleBar() {
        return showScaleBar.get();
    }
    
    public void setShowScaleBar(boolean value) {
        showScaleBar.set(value);
    }
    
    public boolean isShowCompass() {
        return showCompass.get();
    }
    
    public void setShowCompass(boolean value) {
        showCompass.set(value);
    }
    
    public boolean isHardwareAccel() {
        return hardwareAccel.get();
    }
    
    public void setHardwareAccel(boolean value) {
        hardwareAccel.set(value);
    }
    
    public String getLanguage() {
        return language.get();
    }
    
    public void setLanguage(String value) {
        language.set(value);
    }
    
    public String getTheme() {
        return theme.get();
    }
    
    public void setTheme(String value) {
        theme.set(value);
    }
    
    public void clearCache() {
        System.out.println("清除缓存...");
    }
    
    public void reset() {
        antialiasing.set(true);
        quality.set("中");
        cacheSize.set(512);
        threadCount.set(4);
        preloadEnabled.set(true);
        chartPath.set("");
        cachePath.set("");
        showGrid.set(true);
        showScaleBar.set(true);
        showCompass.set(true);
        hardwareAccel.set(true);
        language.set("zh_CN");
        theme.set("default");
    }
}
