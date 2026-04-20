package cn.cycle.echart.i18n;

import java.util.ArrayList;
import java.util.HashMap;
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
 * 
 * // 按模块获取消息
 * String coreMsg = i18n.getModuleMessage("core", "core.system.startup");
 * String renderMsg = i18n.getModuleMessage("render", "render.frame.start");
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class I18nManager implements LocaleProvider {

    private static volatile I18nManager instance;

    private static final String[] MODULE_NAMES = {"core", "render", "data", "ui"};

    private final ResourceBundleLoader bundleLoader;
    private final MessageFormatter formatter;
    private final Map<Locale, ResourceBundle> bundleCache;
    private final Map<String, Map<Locale, ResourceBundle>> moduleBundleCache;
    private final List<LocaleChangeListener> listeners;
    
    private volatile Locale currentLocale;
    private volatile ResourceBundle currentBundle;
    private volatile Map<String, ResourceBundle> currentModuleBundles;

    private I18nManager() {
        this.bundleLoader = new ResourceBundleLoader();
        this.formatter = new MessageFormatter();
        this.bundleCache = new ConcurrentHashMap<>();
        this.moduleBundleCache = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        this.currentLocale = Locale.getDefault();
        this.currentBundle = loadBundle(currentLocale);
        this.currentModuleBundles = new ConcurrentHashMap<>();
        loadModuleBundles(currentLocale);
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
        
        String message = tryGetFromModuleBundles(key);
        if (message != null) {
            return message;
        }
        
        if (currentBundle == null) {
            return key;
        }
        
        try {
            return currentBundle.getString(key);
        } catch (Exception e) {
            return key;
        }
    }

    private String tryGetFromModuleBundles(String key) {
        for (Map.Entry<String, ResourceBundle> entry : currentModuleBundles.entrySet()) {
            ResourceBundle bundle = entry.getValue();
            if (bundle != null && bundle.containsKey(key)) {
                return bundle.getString(key);
            }
        }
        return null;
    }

    /**
     * 获取指定模块的消息。
     * 
     * <p>从模块专属资源文件中获取消息。</p>
     * 
     * @param moduleName 模块名称（core, render, data, ui）
     * @param key 消息键
     * @return 消息值，如果未找到返回键
     */
    public String getModuleMessage(String moduleName, String key) {
        Objects.requireNonNull(moduleName, "moduleName cannot be null");
        Objects.requireNonNull(key, "key cannot be null");
        
        ResourceBundle moduleBundle = currentModuleBundles.get(moduleName);
        if (moduleBundle != null) {
            try {
                return moduleBundle.getString(key);
            } catch (Exception e) {
                return key;
            }
        }
        return getMessage(key);
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
     * 获取指定模块的消息并格式化。
     * 
     * @param moduleName 模块名称
     * @param key 消息键
     * @param args 参数
     * @return 格式化后的消息
     */
    public String getModuleMessage(String moduleName, String key, Object... args) {
        String pattern = getModuleMessage(moduleName, key);
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
        loadModuleBundles(locale);
        
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
        moduleBundleCache.clear();
        currentModuleBundles.clear();
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

    private void loadModuleBundles(Locale locale) {
        Map<String, ResourceBundle> newModuleBundles = new ConcurrentHashMap<>();
        for (String moduleName : MODULE_NAMES) {
            try {
                ResourceBundle moduleBundle = loadModuleBundle(moduleName, locale);
                if (moduleBundle != null) {
                    newModuleBundles.put(moduleName, moduleBundle);
                }
            } catch (Exception e) {
                System.err.println("Error loading module bundle: " + moduleName + ", error: " + e.getMessage());
            }
        }
        this.currentModuleBundles = newModuleBundles;
    }

    private ResourceBundle loadModuleBundle(String moduleName, Locale locale) {
        String baseName = "i18n." + moduleName + ".messages";
        Map<Locale, ResourceBundle> localeBundles = moduleBundleCache.computeIfAbsent(
            moduleName, k -> new ConcurrentHashMap<>());
        return localeBundles.computeIfAbsent(locale, l -> {
            ResourceBundleLoader moduleLoader = new ResourceBundleLoader(baseName);
            return moduleLoader.loadBundle(l);
        });
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
