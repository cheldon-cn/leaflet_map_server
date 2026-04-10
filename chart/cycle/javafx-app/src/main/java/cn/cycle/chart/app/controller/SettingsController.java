package cn.cycle.chart.app.controller;

import cn.cycle.chart.app.model.SettingsModel;
import cn.cycle.app.util.I18nManager;
import javafx.stage.DirectoryChooser;
import javafx.stage.Stage;

import java.io.File;
import java.util.Locale;

public class SettingsController {

    private SettingsModel settingsModel;
    private boolean hasUnsavedChanges = false;
    
    public SettingsController(SettingsModel model) {
        this.settingsModel = model;
    }

    public SettingsModel getSettingsModel() {
        return settingsModel;
    }
    
    public boolean hasUnsavedChanges() {
        return hasUnsavedChanges;
    }
    
    public void markAsChanged() {
        hasUnsavedChanges = true;
    }
    
    public void markAsSaved() {
        hasUnsavedChanges = false;
    }

    public void browseChartPath() {
        DirectoryChooser chooser = new DirectoryChooser();
        chooser.setTitle(I18nManager.tr("settings.chart.path.browse"));
        Stage stage = new Stage();
        File dir = chooser.showDialog(stage);
        if (dir != null) {
            settingsModel.setChartPath(dir.getAbsolutePath());
            markAsChanged();
        }
    }

    public void browseCachePath() {
        DirectoryChooser chooser = new DirectoryChooser();
        chooser.setTitle(I18nManager.tr("settings.cache.path.browse"));
        Stage stage = new Stage();
        File dir = chooser.showDialog(stage);
        if (dir != null) {
            settingsModel.setCachePath(dir.getAbsolutePath());
            markAsChanged();
        }
    }

    public void saveSettings() {
        applySettings();
        markAsSaved();
        System.out.println("设置已保存");
    }

    public void resetSettings() {
        settingsModel.reset();
        markAsChanged();
    }
    
    public void applySettings() {
        String language = settingsModel.getLanguage();
        Locale locale = parseLocale(language);
        I18nManager.getInstance().setLocale(locale);
        
        System.out.println("应用设置:");
        System.out.println("  - 抗锯齿: " + settingsModel.isAntialiasing());
        System.out.println("  - 质量: " + settingsModel.getQuality());
        System.out.println("  - 缓存大小: " + settingsModel.getCacheSize());
        System.out.println("  - 线程数: " + settingsModel.getThreadCount());
        System.out.println("  - 预加载: " + settingsModel.isPreloadEnabled());
        System.out.println("  - 显示网格: " + settingsModel.isShowGrid());
        System.out.println("  - 显示比例尺: " + settingsModel.isShowScaleBar());
        System.out.println("  - 显示指北针: " + settingsModel.isShowCompass());
        System.out.println("  - 硬件加速: " + settingsModel.isHardwareAccel());
        System.out.println("  - 语言: " + settingsModel.getLanguage());
        System.out.println("  - 主题: " + settingsModel.getTheme());
    }

    public void cancelSettings() {
        resetSettings();
    }
    
    public void setQuality(String quality) {
        settingsModel.setQuality(quality);
        markAsChanged();
    }
    
    public void setLanguage(String language) {
        settingsModel.setLanguage(language);
        markAsChanged();
        
        Locale locale = parseLocale(language);
        I18nManager.getInstance().setLocale(locale);
    }
    
    public void setTheme(String theme) {
        settingsModel.setTheme(theme);
        markAsChanged();
    }
    
    public void clearCache() {
        settingsModel.clearCache();
    }
    
    private Locale parseLocale(String localeStr) {
        if (localeStr == null || localeStr.isEmpty()) {
            return Locale.getDefault();
        }
        
        String[] parts = localeStr.split("_");
        if (parts.length == 1) {
            return new Locale(parts[0]);
        } else if (parts.length == 2) {
            return new Locale(parts[0], parts[1]);
        } else if (parts.length == 3) {
            return new Locale(parts[0], parts[1], parts[2]);
        }
        return Locale.getDefault();
    }
}
