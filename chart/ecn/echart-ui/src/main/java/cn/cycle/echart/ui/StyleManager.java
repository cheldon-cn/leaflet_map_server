package cn.cycle.echart.ui;

import javafx.scene.Scene;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 样式管理器。
 * 
 * <p>管理应用主题和样式。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class StyleManager {

    private static StyleManager instance;
    
    private final List<String> stylesheets;
    private String currentTheme;
    private Scene scene;

    private StyleManager() {
        this.stylesheets = new ArrayList<>();
        this.currentTheme = "default";
    }

    public static synchronized StyleManager getInstance() {
        if (instance == null) {
            instance = new StyleManager();
        }
        return instance;
    }

    public void setScene(Scene scene) {
        this.scene = Objects.requireNonNull(scene, "scene cannot be null");
        applyStyles();
    }

    public void addStylesheet(String path) {
        Objects.requireNonNull(path, "path cannot be null");
        
        if (!stylesheets.contains(path)) {
            stylesheets.add(path);
            applyStyles();
        }
    }

    public void removeStylesheet(String path) {
        if (stylesheets.remove(path) && scene != null) {
            scene.getStylesheets().remove(path);
        }
    }

    public void setTheme(String themeName) {
        Objects.requireNonNull(themeName, "themeName cannot be null");
        
        this.currentTheme = themeName;
        applyStyles();
    }

    public String getCurrentTheme() {
        return currentTheme;
    }

    private void applyStyles() {
        if (scene == null) {
            return;
        }
        
        scene.getStylesheets().clear();
        
        String baseStyle = "/styles/base.css";
        URL baseResource = getClass().getResource(baseStyle);
        if (baseResource != null) {
            scene.getStylesheets().add(baseResource.toExternalForm());
        }
        
        String themeStyle = "/styles/themes/" + currentTheme + ".css";
        URL themeResource = getClass().getResource(themeStyle);
        if (themeResource != null) {
            scene.getStylesheets().add(themeResource.toExternalForm());
        }
        
        for (String path : stylesheets) {
            URL resource = getClass().getResource(path);
            if (resource != null) {
                scene.getStylesheets().add(resource.toExternalForm());
            }
        }
    }

    public String loadStyle(String path) {
        try (InputStream is = getClass().getResourceAsStream(path)) {
            if (is != null) {
                byte[] bytes = new byte[is.available()];
                is.read(bytes);
                return new String(bytes, "UTF-8");
            }
        } catch (IOException e) {
        }
        return null;
    }

    public List<String> getAvailableThemes() {
        List<String> themes = new ArrayList<>();
        themes.add("default");
        themes.add("dark");
        themes.add("light");
        return themes;
    }

    public void reset() {
        stylesheets.clear();
        currentTheme = "default";
        applyStyles();
    }
}
