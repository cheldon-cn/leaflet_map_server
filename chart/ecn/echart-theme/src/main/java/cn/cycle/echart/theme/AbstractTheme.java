package cn.cycle.echart.theme;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 抽象主题基类。
 * 
 * <p>提供主题的基本实现。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public abstract class AbstractTheme implements Theme {

    private final String name;
    private final String displayName;
    private final String description;
    private final String version;
    private final String author;
    private final boolean dark;
    
    protected final Map<String, String> colors;
    protected final Map<String, String> fonts;
    protected String styleSheet;
    protected String iconPath;

    protected AbstractTheme(String name, String displayName, String description,
                           String version, String author, boolean dark) {
        this.name = Objects.requireNonNull(name, "name cannot be null");
        this.displayName = displayName != null ? displayName : name;
        this.description = description != null ? description : "";
        this.version = version != null ? version : "1.0.0";
        this.author = author != null ? author : "Unknown";
        this.dark = dark;
        this.colors = new HashMap<>();
        this.fonts = new HashMap<>();
        this.styleSheet = null;
        this.iconPath = null;
        
        initializeColors();
        initializeFonts();
    }

    protected abstract void initializeColors();

    protected abstract void initializeFonts();

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getDisplayName() {
        return displayName;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public String getVersion() {
        return version;
    }

    @Override
    public String getAuthor() {
        return author;
    }

    @Override
    public boolean isDark() {
        return dark;
    }

    @Override
    public Map<String, String> getColors() {
        return Collections.unmodifiableMap(colors);
    }

    @Override
    public String getColor(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        return colors.get(key);
    }

    @Override
    public Map<String, String> getFonts() {
        return Collections.unmodifiableMap(fonts);
    }

    @Override
    public String getFont(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        return fonts.get(key);
    }

    @Override
    public String getStyleSheet() {
        return styleSheet;
    }

    @Override
    public String getIconPath() {
        return iconPath;
    }

    protected void setColor(String key, String value) {
        Objects.requireNonNull(key, "key cannot be null");
        Objects.requireNonNull(value, "value cannot be null");
        colors.put(key, value);
    }

    protected void setFont(String key, String value) {
        Objects.requireNonNull(key, "key cannot be null");
        Objects.requireNonNull(value, "value cannot be null");
        fonts.put(key, value);
    }

    protected void setStyleSheet(String path) {
        this.styleSheet = path;
    }

    protected void setIconPath(String path) {
        this.iconPath = path;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }
        Theme other = (Theme) obj;
        return name.equals(other.getName());
    }

    @Override
    public int hashCode() {
        return name.hashCode();
    }

    @Override
    public String toString() {
        return String.format("Theme[name=%s, displayName=%s, dark=%s]",
                name, displayName, dark);
    }
}
