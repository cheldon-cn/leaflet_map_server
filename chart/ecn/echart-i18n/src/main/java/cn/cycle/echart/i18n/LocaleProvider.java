package cn.cycle.echart.i18n;

import java.util.Locale;
import java.util.Objects;

/**
 * 区域设置提供者接口。
 * 
 * <p>提供当前区域设置的获取和设置功能。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface LocaleProvider {

    /**
     * 获取当前区域设置。
     * 
     * @return 当前区域设置，不为null
     */
    Locale getLocale();

    /**
     * 设置当前区域设置。
     * 
     * @param locale 区域设置，不能为null
     * @throws IllegalArgumentException 如果locale为null
     */
    void setLocale(Locale locale);

    /**
     * 获取默认区域设置。
     * 
     * @return 默认区域设置
     */
    Locale getDefaultLocale();

    /**
     * 添加区域设置变更监听器。
     * 
     * @param listener 监听器
     */
    void addLocaleChangeListener(LocaleChangeListener listener);

    /**
     * 移除区域设置变更监听器。
     * 
     * @param listener 监听器
     */
    void removeLocaleChangeListener(LocaleChangeListener listener);

    /**
     * 区域设置变更监听器接口。
     */
    interface LocaleChangeListener {
        void onLocaleChanged(Locale oldLocale, Locale newLocale);
    }
}
