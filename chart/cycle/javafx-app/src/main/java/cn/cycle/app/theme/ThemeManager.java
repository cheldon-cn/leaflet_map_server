package cn.cycle.app.theme;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.Scene;

import java.util.HashMap;
import java.util.Map;
import java.util.prefs.Preferences;

public class ThemeManager extends AbstractLifecycleComponent {
    
    private static final ThemeManager INSTANCE = new ThemeManager();
    private static final String PREFS_THEME_KEY = "app.theme.current";
    private static final String DEFAULT_THEME = "light";
    
    private final Map<String, String> themeStylesheets = new HashMap<>();
    private final Preferences prefs = Preferences.userNodeForPackage(ThemeManager.class);
    private String currentTheme = DEFAULT_THEME;
    private Scene scene;
    
    private ThemeManager() {
        registerTheme("light", getClass().getResource("/css/theme-light.css").toExternalForm());
        registerTheme("dark", getClass().getResource("/css/theme-dark.css").toExternalForm());
        registerTheme("blue", getClass().getResource("/css/theme-blue.css").toExternalForm());
    }
    
    public static ThemeManager getInstance() {
        return INSTANCE;
    }
    
    public void registerTheme(String themeName, String stylesheetPath) {
        if (themeName != null && stylesheetPath != null) {
            themeStylesheets.put(themeName.toLowerCase(), stylesheetPath);
        }
    }
    
    public void setScene(Scene scene) {
        this.scene = scene;
        applyCurrentTheme();
    }
    
    public void setTheme(String themeName) {
        if (themeName == null || !themeStylesheets.containsKey(themeName.toLowerCase())) {
            themeName = DEFAULT_THEME;
        }
        
        String normalizedTheme = themeName.toLowerCase();
        if (normalizedTheme.equals(currentTheme)) {
            return;
        }
        
        String oldStylesheet = themeStylesheets.get(currentTheme);
        String newStylesheet = themeStylesheets.get(normalizedTheme);
        
        if (scene != null) {
            if (oldStylesheet != null) {
                scene.getStylesheets().remove(oldStylesheet);
            }
            if (newStylesheet != null) {
                scene.getStylesheets().add(newStylesheet);
            }
        }
        
        currentTheme = normalizedTheme;
        saveThemePreference(normalizedTheme);
    }
    
    public String getCurrentTheme() {
        return currentTheme;
    }
    
    public String[] getAvailableThemes() {
        return themeStylesheets.keySet().toArray(new String[0]);
    }
    
    public boolean hasTheme(String themeName) {
        return themeName != null && themeStylesheets.containsKey(themeName.toLowerCase());
    }
    
    private void applyCurrentTheme() {
        if (scene == null) {
            return;
        }
        
        String stylesheet = themeStylesheets.get(currentTheme);
        if (stylesheet != null && !scene.getStylesheets().contains(stylesheet)) {
            scene.getStylesheets().add(stylesheet);
        }
    }
    
    private void saveThemePreference(String theme) {
        prefs.put(PREFS_THEME_KEY, theme);
    }
    
    private String loadThemePreference() {
        return prefs.get(PREFS_THEME_KEY, DEFAULT_THEME);
    }
    
    @Override
    protected void doInitialize() {
        currentTheme = loadThemePreference();
    }
    
    @Override
    protected void doDispose() {
        themeStylesheets.clear();
    }
}
