package cn.cycle.app.accessibility;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.state.StatePersistable;
import cn.cycle.app.theme.ThemeManager;
import javafx.scene.Scene;
import javafx.scene.Parent;

import java.util.HashMap;
import java.util.Map;

public class HighContrastMode implements StatePersistable {
    
    private static final HighContrastMode INSTANCE = new HighContrastMode();
    
    private static final String HIGH_CONTRAST_THEME = "high-contrast";
    private static final String HIGH_CONTRAST_CSS = "/css/theme-high-contrast.css";
    
    private boolean enabled = false;
    private String previousTheme = null;
    private Scene scene;
    
    private HighContrastMode() {
    }
    
    public static HighContrastMode getInstance() {
        return INSTANCE;
    }
    
    public void setScene(Scene scene) {
        this.scene = scene;
    }
    
    public void enable() {
        if (enabled) {
            return;
        }
        
        previousTheme = ThemeManager.getInstance().getCurrentTheme();
        enabled = true;
        
        if (scene != null) {
            applyHighContrastStyles();
        }
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.THEME_CHANGED, this)
                .withData("highContrast", true)
        );
    }
    
    public void disable() {
        if (!enabled) {
            return;
        }
        
        enabled = false;
        
        if (scene != null && previousTheme != null) {
            ThemeManager.getInstance().setTheme(previousTheme);
            removeHighContrastStyles();
        }
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.THEME_CHANGED, this)
                .withData("highContrast", false)
        );
    }
    
    public void toggle() {
        if (enabled) {
            disable();
        } else {
            enable();
        }
    }
    
    public boolean isEnabled() {
        return enabled;
    }
    
    private void applyHighContrastStyles() {
        if (scene == null) {
            return;
        }
        
        scene.getStylesheets().add(HIGH_CONTRAST_CSS);
        
        Parent root = scene.getRoot();
        if (root != null) {
            root.setStyle("-fx-base: black; -fx-background: black; -fx-text-fill: white;");
        }
    }
    
    private void removeHighContrastStyles() {
        if (scene == null) {
            return;
        }
        
        scene.getStylesheets().remove(HIGH_CONTRAST_CSS);
        
        Parent root = scene.getRoot();
        if (root != null) {
            root.setStyle("");
        }
    }
    
    @Override
    public Map<String, Object> saveState() {
        Map<String, Object> state = new HashMap<>();
        state.put("enabled", enabled);
        state.put("previousTheme", previousTheme);
        return state;
    }
    
    @Override
    public void restoreState(Map<String, Object> state) {
        if (state == null) {
            return;
        }
        
        Object enabledObj = state.get("enabled");
        if (enabledObj instanceof Boolean) {
            if ((Boolean) enabledObj) {
                enable();
            }
        }
        
        Object themeObj = state.get("previousTheme");
        if (themeObj instanceof String) {
            previousTheme = (String) themeObj;
        }
    }
    
    @Override
    public String getStateKey() {
        return "accessibility.highContrast";
    }
}
