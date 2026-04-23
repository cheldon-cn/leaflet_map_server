package cn.cycle.echart.ui.handler;

import cn.cycle.echart.theme.Theme;
import cn.cycle.echart.theme.ThemeManager;
import cn.cycle.echart.ui.StatusBar;
import cn.cycle.echart.ui.UIConfig;
import cn.cycle.echart.ui.dialog.SettingsDialog;
import cn.cycle.echart.ui.dialog.ThemeDialog;
import javafx.stage.Stage;

public class ToolsHandler {

    private final Stage stage;
    private final StatusBar statusBar;
    private final MessageCallback messageCallback;
    
    private ThemeManager themeManager;
    private UIConfig uiConfig;

    public ToolsHandler(Stage stage, StatusBar statusBar, MessageCallback messageCallback) {
        this.stage = stage;
        this.statusBar = statusBar;
        this.messageCallback = messageCallback;
    }

    public void setThemeManager(ThemeManager themeManager) {
        this.themeManager = themeManager;
    }

    public void setUiConfig(UIConfig uiConfig) {
        this.uiConfig = uiConfig;
    }

    public void onOptions() {
        showSettingsDialog();
    }

    public void onThemeSettings() {
        showThemeDialog();
    }

    private void showSettingsDialog() {
        if (uiConfig == null) {
            uiConfig = new UIConfig();
        }
        
        SettingsDialog dialog = new SettingsDialog(
                uiConfig.isAutoSaveEnabled(),
                uiConfig.getAutoSaveInterval(),
                uiConfig.getChartCacheSize(),
                uiConfig.isAlarmSoundEnabled(),
                uiConfig.getAisUpdateInterval(),
                uiConfig.getLocale()
        );
        
        if (stage != null) {
            dialog.setOwner(stage);
        }
        
        dialog.showAndWait().ifPresent(result -> {
            if (result == javafx.scene.control.ButtonType.OK) {
                uiConfig.setBoolean("workspace.autoSave", dialog.isAutoSaveEnabled());
                uiConfig.setInt("workspace.autoSaveInterval", dialog.getAutoSaveInterval());
                uiConfig.setInt("chart.cacheSize", dialog.getCacheSize());
                uiConfig.setBoolean("alarm.soundEnabled", dialog.isAlarmSoundEnabled());
                uiConfig.setInt("ais.updateInterval", dialog.getAisUpdateInterval());
                uiConfig.setLocale(dialog.getLocale());
                uiConfig.save();
                
                statusBar.showMessage("设置已保存");
            }
        });
    }

    private void showThemeDialog() {
        if (themeManager == null) {
            messageCallback.showInfo("主题设置", "主题管理器未初始化");
            return;
        }
        
        ThemeDialog dialog = new ThemeDialog(themeManager);
        
        if (stage != null) {
            dialog.setOwner(stage);
        }
        
        dialog.showAndWait().ifPresent(result -> {
            if (result == javafx.scene.control.ButtonType.OK) {
                Theme selectedTheme = dialog.getSelectedTheme();
                if (selectedTheme != null && selectedTheme != themeManager.getCurrentTheme()) {
                    themeManager.setCurrentTheme(selectedTheme);
                    
                    if (uiConfig != null) {
                        uiConfig.setTheme(selectedTheme.getName());
                        uiConfig.save();
                    }
                    
                    statusBar.showMessage("主题已切换为: " + selectedTheme.getDisplayName());
                }
            }
        });
    }
}
