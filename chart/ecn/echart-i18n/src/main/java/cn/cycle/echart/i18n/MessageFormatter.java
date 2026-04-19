package cn.cycle.echart.i18n;

import java.text.MessageFormat;
import java.util.Locale;
import java.util.Objects;

/**
 * 消息格式化器。
 * 
 * <p>支持带参数的消息格式化。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class MessageFormatter {

    private final Locale locale;

    public MessageFormatter() {
        this(Locale.getDefault());
    }

    public MessageFormatter(Locale locale) {
        this.locale = Objects.requireNonNull(locale, "locale cannot be null");
    }

    /**
     * 格式化消息。
     * 
     * @param pattern 消息模式
     * @param arguments 参数
     * @return 格式化后的消息
     */
    public String format(String pattern, Object... arguments) {
        if (pattern == null) {
            return "";
        }
        
        if (arguments == null || arguments.length == 0) {
            return pattern;
        }
        
        MessageFormat format = new MessageFormat(pattern, locale);
        return format.format(arguments);
    }

    /**
     * 格式化消息（静态方法）。
     * 
     * @param pattern 消息模式
     * @param locale 区域设置
     * @param arguments 参数
     * @return 格式化后的消息
     */
    public static String format(String pattern, Locale locale, Object... arguments) {
        if (pattern == null) {
            return "";
        }
        
        if (arguments == null || arguments.length == 0) {
            return pattern;
        }
        
        MessageFormat format = new MessageFormat(pattern, locale != null ? locale : Locale.getDefault());
        return format.format(arguments);
    }

    /**
     * 获取当前区域设置。
     * 
     * @return 区域设置
     */
    public Locale getLocale() {
        return locale;
    }
}
