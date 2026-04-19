package cn.cycle.echart.theme;

import java.util.Map;

/**
 * 主题接口。
 * 
 * <p>定义主题的基本属性和样式。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface Theme {

    /**
     * 获取主题名称。
     * 
     * @return 主题名称
     */
    String getName();

    /**
     * 获取主题显示名称。
     * 
     * @return 主题显示名称
     */
    String getDisplayName();

    /**
     * 获取主题描述。
     * 
     * @return 主题描述
     */
    String getDescription();

    /**
     * 获取主题版本。
     * 
     * @return 主题版本
     */
    String getVersion();

    /**
     * 获取主题作者。
     * 
     * @return 主题作者
     */
    String getAuthor();

    /**
     * 检查是否为暗色主题。
     * 
     * @return 是否为暗色主题
     */
    boolean isDark();

    /**
     * 获取颜色配置。
     * 
     * @return 颜色配置映射
     */
    Map<String, String> getColors();

    /**
     * 获取指定颜色。
     * 
     * @param key 颜色键
     * @return 颜色值（十六进制格式）
     */
    String getColor(String key);

    /**
     * 获取字体配置。
     * 
     * @return 字体配置映射
     */
    Map<String, String> getFonts();

    /**
     * 获取指定字体。
     * 
     * @param key 字体键
     * @return 字体值
     */
    String getFont(String key);

    /**
     * 获取CSS样式表路径。
     * 
     * @return CSS样式表路径
     */
    String getStyleSheet();

    /**
     * 获取主题图标路径。
     * 
     * @return 主题图标路径
     */
    String getIconPath();
}
