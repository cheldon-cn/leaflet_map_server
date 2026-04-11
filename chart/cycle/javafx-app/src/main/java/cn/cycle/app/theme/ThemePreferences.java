package cn.cycle.app.theme;

import cn.cycle.app.state.StatePersistable;

import java.util.HashMap;
import java.util.Map;
import java.util.prefs.Preferences;

public class ThemePreferences implements StatePersistable {
    
    private static final String PREFS_NODE = "cn/cycle/app/theme";
    private static final String KEY_CURRENT_THEME = "currentTheme";
    private static final String KEY_FONT_SIZE = "fontSize";
    private static final String KEY_HIGH_CONTRAST = "highContrast";
    
    private final Preferences prefs;
    private String currentTheme = "light";
    private double fontSize = 12.0;
    private boolean highContrast = false;
    
    public ThemePreferences() {
        prefs = Preferences.userRoot().node(PREFS_NODE);
        loadFromPreferences();
    }
    
    private void loadFromPreferences() {
        currentTheme = prefs.get(KEY_CURRENT_THEME, "light");
        fontSize = prefs.getDouble(KEY_FONT_SIZE, 12.0);
        highContrast = prefs.getBoolean(KEY_HIGH_CONTRAST, false);
    }
    
    public String getCurrentTheme() {
        return currentTheme;
    }
    
    public void setCurrentTheme(String theme) {
        this.currentTheme = theme;
        prefs.put(KEY_CURRENT_THEME, theme);
    }
    
    public double getFontSize() {
        return fontSize;
    }
    
    public void setFontSize(double size) {
        this.fontSize = size;
        prefs.putDouble(KEY_FONT_SIZE, size);
    }
    
    public boolean isHighContrast() {
        return highContrast;
    }
    
    public void setHighContrast(boolean enabled) {
        this.highContrast = enabled;
        prefs.putBoolean(KEY_HIGH_CONTRAST, enabled);
    }
    
    @Override
    public Map<String, Object> saveState() {
        Map<String, Object> state = new HashMap<>();
        state.put("currentTheme", currentTheme);
        state.put("fontSize", fontSize);
        state.put("highContrast", highContrast);
        return state;
    }
    
    @Override
    public void restoreState(Map<String, Object> state) {
        if (state == null) {
            return;
        }
        
        Object theme = state.get("currentTheme");
        if (theme instanceof String) {
            setCurrentTheme((String) theme);
        }
        
        Object size = state.get("fontSize");
        if (size instanceof Number) {
            setFontSize(((Number) size).doubleValue());
        }
        
        Object contrast = state.get("highContrast");
        if (contrast instanceof Boolean) {
            setHighContrast((Boolean) contrast);
        }
    }
    
    @Override
    public String getStateKey() {
        return "theme.preferences";
    }
    
    public void reset() {
        setCurrentTheme("light");
        setFontSize(12.0);
        setHighContrast(false);
    }
}
