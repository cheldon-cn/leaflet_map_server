package cn.cycle.echart.theme;

/**
 * 深色主题。
 * 
 * <p>深色主题实现，适合夜间使用。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DarkTheme extends AbstractTheme {

    public DarkTheme() {
        super("dark", "深色主题", "深色主题，适合夜间使用", 
              "1.0.0", "Cycle Team", true);
        setStyleSheet("/themes/dark.css");
        setIconPath("/themes/icons/dark.png");
    }

    @Override
    protected void initializeColors() {
        setColor("background", "#1E1E1E");
        setColor("background.secondary", "#252526");
        setColor("background.tertiary", "#2D2D30");
        
        setColor("text.primary", "#FFFFFF");
        setColor("text.secondary", "#B0B0B0");
        setColor("text.disabled", "#606060");
        
        setColor("accent.primary", "#0288D1");
        setColor("accent.secondary", "#03A9F4");
        setColor("accent.tertiary", "#01579B");
        
        setColor("border", "#3C3C3C");
        setColor("border.light", "#4A4A4A");
        setColor("border.dark", "#2A2A2A");
        
        setColor("success", "#66BB6A");
        setColor("warning", "#FFA726");
        setColor("error", "#EF5350");
        setColor("info", "#42A5F5");
        
        setColor("water", "#0D47A1");
        setColor("land", "#1B5E20");
        setColor("depth.shallow", "#1565C0");
        setColor("depth.deep", "#0D47A1");
        
        setColor("vessel.own", "#42A5F5");
        setColor("vessel.other", "#9E9E9E");
        setColor("ais.active", "#66BB6A");
        setColor("ais.sleep", "#616161");
        
        setColor("alarm.critical", "#EF5350");
        setColor("alarm.warning", "#FF7043");
        setColor("alarm.caution", "#FFA726");
        setColor("alarm.info", "#66BB6A");
    }

    @Override
    protected void initializeFonts() {
        setFont("font.primary", "Microsoft YaHei, Arial, sans-serif");
        setFont("font.secondary", "Arial, sans-serif");
        setFont("font.mono", "Consolas, monospace");
        
        setFont("font.size.small", "12");
        setFont("font.size.normal", "14");
        setFont("font.size.large", "16");
        setFont("font.size.title", "20");
        setFont("font.size.header", "24");
    }
}
