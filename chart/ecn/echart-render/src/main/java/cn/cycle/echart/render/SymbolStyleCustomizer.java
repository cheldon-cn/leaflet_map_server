package cn.cycle.echart.render;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 符号样式自定义器。
 * 
 * <p>提供S-52符号样式的自定义配置功能，支持颜色、线宽、透明度等属性调整。</p>
 * 
 * <h2>可自定义属性</h2>
 * <ul>
 *   <li>颜色 (Color)</li>
 *   <li>线宽 (Line Width)</li>
 *   <li>透明度 (Transparency)</li>
 *   <li>符号大小 (Symbol Size)</li>
 *   <li>字体样式 (Font Style)</li>
 * </ul>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class SymbolStyleCustomizer {

    private static volatile SymbolStyleCustomizer instance;

    private final Map<String, StyleOverride> styleOverrides;
    private DisplayCategory currentDisplayCategory;
    private DisplayPriority displayPriority;

    private SymbolStyleCustomizer() {
        this.styleOverrides = new HashMap<>();
        this.currentDisplayCategory = DisplayCategory.STANDARD;
        this.displayPriority = DisplayPriority.STANDARD;
        initializeDefaultStyles();
    }

    /**
     * 获取单例实例。
     * 
     * @return SymbolStyleCustomizer实例
     */
    public static synchronized SymbolStyleCustomizer getInstance() {
        if (instance == null) {
            instance = new SymbolStyleCustomizer();
        }
        return instance;
    }

    private void initializeDefaultStyles() {
        setStyleOverride("SOUNDG", StyleOverride.create()
            .setColor("#000000")
            .setFontSize(8)
            .setVisible(true));
        
        setStyleOverride("LIGHTS", StyleOverride.create()
            .setColor("#FFD700")
            .setSymbolSize(12)
            .setVisible(true));
        
        setStyleOverride("DEPARE", StyleOverride.create()
            .setColor("#ADD8E6")
            .setTransparency(0.3)
            .setVisible(true));
        
        setStyleOverride("COALNE", StyleOverride.create()
            .setColor("#8B4513")
            .setLineWidth(1.5)
            .setVisible(true));
        
        setStyleOverride("DEPCNT", StyleOverride.create()
            .setColor("#4169E1")
            .setLineWidth(0.5)
            .setVisible(true));
    }

    /**
     * 设置符号样式覆盖。
     * 
     * @param symbolCode 符号代码
     * @param override 样式覆盖配置
     */
    public void setStyleOverride(String symbolCode, StyleOverride override) {
        Objects.requireNonNull(symbolCode, "symbolCode cannot be null");
        Objects.requireNonNull(override, "override cannot be null");
        styleOverrides.put(symbolCode, override);
    }

    /**
     * 获取符号样式覆盖。
     * 
     * @param symbolCode 符号代码
     * @return 样式覆盖配置，如果未设置返回null
     */
    public StyleOverride getStyleOverride(String symbolCode) {
        Objects.requireNonNull(symbolCode, "symbolCode cannot be null");
        return styleOverrides.get(symbolCode);
    }

    /**
     * 移除符号样式覆盖。
     * 
     * @param symbolCode 符号代码
     */
    public void removeStyleOverride(String symbolCode) {
        Objects.requireNonNull(symbolCode, "symbolCode cannot be null");
        styleOverrides.remove(symbolCode);
    }

    /**
     * 设置显示类别。
     * 
     * @param category 显示类别
     */
    public void setDisplayCategory(DisplayCategory category) {
        Objects.requireNonNull(category, "category cannot be null");
        this.currentDisplayCategory = category;
    }

    /**
     * 获取当前显示类别。
     * 
     * @return 显示类别
     */
    public DisplayCategory getDisplayCategory() {
        return currentDisplayCategory;
    }

    /**
     * 设置显示优先级。
     * 
     * @param priority 显示优先级
     */
    public void setDisplayPriority(DisplayPriority priority) {
        Objects.requireNonNull(priority, "priority cannot be null");
        this.displayPriority = priority;
    }

    /**
     * 获取当前显示优先级。
     * 
     * @return 显示优先级
     */
    public DisplayPriority getDisplayPriority() {
        return displayPriority;
    }

    /**
     * 应用样式到渲染上下文。
     * 
     * @param context 渲染上下文
     * @param symbolCode 符号代码
     */
    public void applyStyle(RenderContext context, String symbolCode) {
        Objects.requireNonNull(context, "context cannot be null");
        Objects.requireNonNull(symbolCode, "symbolCode cannot be null");
        
        StyleOverride override = styleOverrides.get(symbolCode);
        if (override != null) {
            context.setSymbolStyle(symbolCode, override.toMap());
        }
    }

    /**
     * 重置为默认样式。
     */
    public void resetToDefaults() {
        styleOverrides.clear();
        currentDisplayCategory = DisplayCategory.STANDARD;
        displayPriority = DisplayPriority.STANDARD;
        initializeDefaultStyles();
    }

    /**
     * 获取所有样式覆盖。
     * 
     * @return 样式覆盖映射
     */
    public Map<String, StyleOverride> getAllStyleOverrides() {
        return new HashMap<>(styleOverrides);
    }

    /**
     * 显示类别枚举。
     */
    public enum DisplayCategory {
        DISPLAY_BASE("Display Base", "基础显示"),
        STANDARD("Standard", "标准显示"),
        ALL("All", "全部显示"),
        OTHER("Other", "其他");

        private final String englishName;
        private final String chineseName;

        DisplayCategory(String englishName, String chineseName) {
            this.englishName = englishName;
            this.chineseName = chineseName;
        }

        public String getEnglishName() {
            return englishName;
        }

        public String getChineseName() {
            return chineseName;
        }
    }

    /**
     * 显示优先级枚举。
     */
    public enum DisplayPriority {
        NO_DATA(1, "No Data"),
        NAVIGATION_AIDS(2, "Navigation Aids"),
        DANGERS(3, "Dangers"),
        DEPTHS(4, "Depths"),
        AREAS(5, "Areas"),
        LINES(6, "Lines"),
        POINTS(7, "Points"),
        STANDARD(4, "Standard");

        private final int priority;
        private final String description;

        DisplayPriority(int priority, String description) {
            this.priority = priority;
            this.description = description;
        }

        public int getPriority() {
            return priority;
        }

        public String getDescription() {
            return description;
        }
    }

    /**
     * 样式覆盖配置。
     */
    public static class StyleOverride {
        private String color;
        private Double lineWidth;
        private Double transparency;
        private Integer symbolSize;
        private Integer fontSize;
        private Boolean visible;
        private final Map<String, Object> customProperties;

        private StyleOverride() {
            this.customProperties = new HashMap<>();
        }

        public static StyleOverride create() {
            return new StyleOverride();
        }

        public StyleOverride setColor(String color) {
            this.color = color;
            return this;
        }

        public StyleOverride setLineWidth(double lineWidth) {
            this.lineWidth = lineWidth;
            return this;
        }

        public StyleOverride setTransparency(double transparency) {
            this.transparency = transparency;
            return this;
        }

        public StyleOverride setSymbolSize(int symbolSize) {
            this.symbolSize = symbolSize;
            return this;
        }

        public StyleOverride setFontSize(int fontSize) {
            this.fontSize = fontSize;
            return this;
        }

        public StyleOverride setVisible(boolean visible) {
            this.visible = visible;
            return this;
        }

        public StyleOverride setCustomProperty(String key, Object value) {
            customProperties.put(key, value);
            return this;
        }

        public String getColor() {
            return color;
        }

        public Double getLineWidth() {
            return lineWidth;
        }

        public Double getTransparency() {
            return transparency;
        }

        public Integer getSymbolSize() {
            return symbolSize;
        }

        public Integer getFontSize() {
            return fontSize;
        }

        public Boolean isVisible() {
            return visible;
        }

        public Map<String, Object> toMap() {
            Map<String, Object> map = new HashMap<>();
            if (color != null) map.put("color", color);
            if (lineWidth != null) map.put("lineWidth", lineWidth);
            if (transparency != null) map.put("transparency", transparency);
            if (symbolSize != null) map.put("symbolSize", symbolSize);
            if (fontSize != null) map.put("fontSize", fontSize);
            if (visible != null) map.put("visible", visible);
            map.putAll(customProperties);
            return map;
        }
    }
}
