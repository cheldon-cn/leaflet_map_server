package cn.cycle.echart.theme;

/**
 * 浅色主题。
 * 
 * <p>默认的浅色主题实现。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class LightTheme extends AbstractTheme {

    public LightTheme() {
        super("light", "浅色主题", "默认浅色主题，适合日间使用", 
              "1.0.0", "Cycle Team", false);
        setStyleSheet("/themes/light.css");
        setIconPath("/themes/icons/light.png");
    }

    @Override
    protected void initializeColors() {
        setColor("background", "#FFFFFF");
        setColor("background.secondary", "#F5F5F5");
        setColor("background.tertiary", "#E8E8E8");
        
        setColor("text.primary", "#212121");
        setColor("text.secondary", "#757575");
        setColor("text.disabled", "#BDBDBD");
        
        setColor("accent.primary", "#1976D2");
        setColor("accent.secondary", "#42A5F5");
        setColor("accent.tertiary", "#BBDEFB");
        
        setColor("border", "#E0E0E0");
        setColor("border.light", "#F0F0F0");
        setColor("border.dark", "#BDBDBD");
        
        setColor("success", "#4CAF50");
        setColor("warning", "#FF9800");
        setColor("error", "#F44336");
        setColor("info", "#2196F3");
        
        setColor("water", "#B3E5FC");
        setColor("land", "#C8E6C9");
        setColor("depth.shallow", "#81D4FA");
        setColor("depth.deep", "#0277BD");
        
        setColor("vessel.own", "#1976D2");
        setColor("vessel.other", "#757575");
        setColor("ais.active", "#4CAF50");
        setColor("ais.sleep", "#9E9E9E");
        
        setColor("alarm.critical", "#D32F2F");
        setColor("alarm.warning", "#F57C00");
        setColor("alarm.caution", "#FFA000");
        setColor("alarm.info", "#388E3C");
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
