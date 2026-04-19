package cn.cycle.echart.theme;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Properties;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 主题管理器。
 * 
 * <p>负责主题的注册、切换和持久化。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ThemeManager {

    private static final String DEFAULT_THEME = "light";
    
    private final Map<String, Theme> themes;
    private final List<ThemeChangeListener> listeners;
    private Theme currentTheme;
    private Theme defaultTheme;

    public ThemeManager() {
        this.themes = new ConcurrentHashMap<>();
        this.listeners = new CopyOnWriteArrayList<>();
        this.currentTheme = null;
        this.defaultTheme = null;
        
        registerBuiltInThemes();
    }

    protected void registerBuiltInThemes() {
        LightTheme lightTheme = new LightTheme();
        DarkTheme darkTheme = new DarkTheme();
        
        registerTheme(lightTheme);
        registerTheme(darkTheme);
        
        setDefaultTheme(lightTheme);
        setCurrentTheme(lightTheme);
    }

    public void registerTheme(Theme theme) {
        Objects.requireNonNull(theme, "theme cannot be null");
        themes.put(theme.getName(), theme);
    }

    public void unregisterTheme(String name) {
        Objects.requireNonNull(name, "name cannot be null");
        themes.remove(name);
    }

    public Theme getTheme(String name) {
        Objects.requireNonNull(name, "name cannot be null");
        return themes.get(name);
    }

    public List<Theme> getAvailableThemes() {
        return Collections.unmodifiableList(new ArrayList<>(themes.values()));
    }

    public Theme getCurrentTheme() {
        return currentTheme;
    }

    public void setCurrentTheme(Theme theme) {
        Objects.requireNonNull(theme, "theme cannot be null");
        
        Theme oldTheme = this.currentTheme;
        this.currentTheme = theme;
        
        if (!Objects.equals(oldTheme, theme)) {
            notifyThemeChanged(oldTheme, theme);
        }
    }

    public void setCurrentTheme(String name) {
        Theme theme = getTheme(name);
        if (theme == null) {
            throw new IllegalArgumentException("Theme not found: " + name);
        }
        setCurrentTheme(theme);
    }

    public Theme getDefaultTheme() {
        return defaultTheme;
    }

    public void setDefaultTheme(Theme theme) {
        Objects.requireNonNull(theme, "theme cannot be null");
        this.defaultTheme = theme;
    }

    public void resetToDefault() {
        if (defaultTheme != null) {
            setCurrentTheme(defaultTheme);
        }
    }

    public boolean hasTheme(String name) {
        return themes.containsKey(name);
    }

    public int getThemeCount() {
        return themes.size();
    }

    public void addThemeChangeListener(ThemeChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.add(listener);
    }

    public void removeThemeChangeListener(ThemeChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.remove(listener);
    }

    protected void notifyThemeChanged(Theme oldTheme, Theme newTheme) {
        for (ThemeChangeListener listener : listeners) {
            try {
                listener.onThemeChanged(oldTheme, newTheme);
            } catch (Exception e) {
                System.err.println("Error notifying theme change listener: " + e.getMessage());
            }
        }
    }

    public void loadThemeFromProperties(Properties props) {
        Objects.requireNonNull(props, "props cannot be null");
        
        String themeName = props.getProperty("theme.current", DEFAULT_THEME);
        if (hasTheme(themeName)) {
            setCurrentTheme(themeName);
        }
    }

    public void saveThemeToProperties(Properties props) {
        Objects.requireNonNull(props, "props cannot be null");
        
        if (currentTheme != null) {
            props.setProperty("theme.current", currentTheme.getName());
        }
    }

    public interface ThemeChangeListener {
        void onThemeChanged(Theme oldTheme, Theme newTheme);
    }
}
