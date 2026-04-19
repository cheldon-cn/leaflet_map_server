package cn.cycle.echart.i18n;

import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.ResourceBundle;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 国际化管理器。
 * 
 * <p>提供多语言资源管理和消息获取功能。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * I18nManager i18n = I18nManager.getInstance();
 * i18n.setLocale(Locale.SIMPLIFIED_CHINESE);
 * String message = i18n.getMessage("app.title");
 * String formatted = i18n.getMessage("app.greeting", "World");
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class I18nManager implements LocaleProvider {

    private static volatile I18nManager instance;

    private final ResourceBundleLoader bundleLoader;
    private final MessageFormatter formatter;
    private final Map<Locale, ResourceBundle> bundleCache;
    private final List<LocaleChangeListener> listeners;
    
    private volatile Locale currentLocale;
    private volatile ResourceBundle currentBundle;

    private I18nManager() {
        this.bundleLoader = new ResourceBundleLoader();
        this.formatter = new MessageFormatter();
        this.bundleCache = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        this.currentLocale = Locale.getDefault();
        this.currentBundle = loadBundle(currentLocale);
    }

    public static I18nManager getInstance() {
        if (instance == null) {
            synchronized (I18nManager.class) {
                if (instance == null) {
                    instance = new I18nManager();
                }
            }
        }
        return instance;
    }

    /**
     * 获取消息。
     * 
     * @param key 消息键
     * @return 消息值，如果未找到返回键
     */
    public String getMessage(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        
        if (currentBundle == null) {
            return key;
        }
        
        try {
            return currentBundle.getString(key);
        } catch (Exception e) {
            return key;
        }
    }

    /**
     * 获取消息并格式化。
     * 
     * @param key 消息键
     * @param args 参数
     * @return 格式化后的消息
     */
    public String getMessage(String key, Object... args) {
        String pattern = getMessage(key);
        if (args == null || args.length == 0) {
            return pattern;
        }
        return formatter.format(pattern, args);
    }

    /**
     * 检查消息键是否存在。
     * 
     * @param key 消息键
     * @return 是否存在
     */
    public boolean hasMessage(String key) {
        if (currentBundle == null) {
            return false;
        }
        return currentBundle.containsKey(key);
    }

    @Override
    public Locale getLocale() {
        return currentLocale;
    }

    @Override
    public void setLocale(Locale locale) {
        Objects.requireNonNull(locale, "locale cannot be null");
        
        Locale oldLocale = this.currentLocale;
        if (oldLocale.equals(locale)) {
            return;
        }
        
        this.currentLocale = locale;
        this.currentBundle = loadBundle(locale);
        
        notifyLocaleChanged(oldLocale, locale);
    }

    @Override
    public Locale getDefaultLocale() {
        return Locale.getDefault();
    }

    @Override
    public void addLocaleChangeListener(LocaleChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        synchronized (listeners) {
            listeners.add(listener);
        }
    }

    @Override
    public void removeLocaleChangeListener(LocaleChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        synchronized (listeners) {
            listeners.remove(listener);
        }
    }

    /**
     * 清除缓存。
     */
    public void clearCache() {
        bundleCache.clear();
    }

    /**
     * 重置为默认区域设置。
     */
    public void reset() {
        setLocale(getDefaultLocale());
    }

    private ResourceBundle loadBundle(Locale locale) {
        return bundleCache.computeIfAbsent(locale, bundleLoader::loadBundle);
    }

    private void notifyLocaleChanged(Locale oldLocale, Locale newLocale) {
        List<LocaleChangeListener> listenersCopy;
        synchronized (listeners) {
            listenersCopy = new ArrayList<>(listeners);
        }
        
        for (LocaleChangeListener listener : listenersCopy) {
            try {
                listener.onLocaleChanged(oldLocale, newLocale);
            } catch (Exception e) {
                System.err.println("Error notifying locale change: " + e.getMessage());
            }
        }
    }
}
