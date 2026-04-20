package cn.cycle.echart.theme;

/**
 * 高对比度主题。
 * 
 * <p>高对比度主题实现，适合视力障碍用户使用。</p>
 * <p>遵循WCAG 2.1 AAA级对比度标准（对比度≥7:1）。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class HighContrastTheme extends AbstractTheme {

    public HighContrastTheme() {
        super("high-contrast", "高对比度主题", "高对比度主题，适合视力障碍用户使用", 
              "1.0.0", "Cycle Team", true);
        setStyleSheet("/themes/high-contrast.css");
        setIconPath("/themes/icons/high-contrast.png");
    }

    @Override
    protected void initializeColors() {
        setColor("background", "#000000");
        setColor("background.secondary", "#0A0A0A");
        setColor("background.tertiary", "#141414");
        
        setColor("text.primary", "#FFFFFF");
        setColor("text.secondary", "#FFFF00");
        setColor("text.disabled", "#808080");
        
        setColor("accent.primary", "#00FFFF");
        setColor("accent.secondary", "#00FF00");
        setColor("accent.tertiary", "#FF00FF");
        
        setColor("border", "#FFFFFF");
        setColor("border.light", "#FFFF00");
        setColor("border.dark", "#808080");
        
        setColor("success", "#00FF00");
        setColor("warning", "#FFFF00");
        setColor("error", "#FF0000");
        setColor("info", "#00FFFF");
        
        setColor("water", "#0000FF");
        setColor("land", "#008000");
        setColor("depth.shallow", "#00FFFF");
        setColor("depth.deep", "#0000FF");
        
        setColor("vessel.own", "#00FF00");
        setColor("vessel.other", "#FFFF00");
        setColor("ais.active", "#00FF00");
        setColor("ais.sleep", "#808080");
        
        setColor("alarm.critical", "#FF0000");
        setColor("alarm.warning", "#FF8000");
        setColor("alarm.caution", "#FFFF00");
        setColor("alarm.info", "#00FFFF");
    }

    @Override
    protected void initializeFonts() {
        setFont("font.primary", "Arial, sans-serif");
        setFont("font.secondary", "Arial, sans-serif");
        setFont("font.mono", "Consolas, monospace");
        
        setFont("font.size.small", "14");
        setFont("font.size.normal", "16");
        setFont("font.size.large", "18");
        setFont("font.size.title", "22");
        setFont("font.size.header", "26");
    }
}
