package cn.cycle.app.i18n;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;

import java.util.Locale;
import java.util.MissingResourceException;
import java.util.ResourceBundle;
import java.util.concurrent.ConcurrentHashMap;

public class I18nManager extends AbstractLifecycleComponent {
    
    private static final I18nManager INSTANCE = new I18nManager();
    private static final String BUNDLE_BASE_NAME = "i18n.messages";
    
    private Locale currentLocale = Locale.getDefault();
    private ResourceBundle resourceBundle;
    private final ConcurrentHashMap<String, String> cache = new ConcurrentHashMap<>();
    
    private I18nManager() {
    }
    
    public static I18nManager getInstance() {
        return INSTANCE;
    }
    
    public void setLocale(Locale locale) {
        if (locale == null) {
            locale = Locale.getDefault();
        }
        if (locale.equals(this.currentLocale)) {
            return;
        }
        this.currentLocale = locale;
        loadBundle();
        cache.clear();
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.LOCALE_CHANGED, this)
                .withData("locale", locale)
        );
    }
    
    public Locale getLocale() {
        return currentLocale;
    }
    
    public String get(String key) {
        if (key == null) {
            return "";
        }
        
        String cached = cache.get(key);
        if (cached != null) {
            return cached;
        }
        
        String value;
        if (resourceBundle != null) {
            try {
                value = resourceBundle.getString(key);
            } catch (MissingResourceException e) {
                value = key;
            }
        } else {
            value = key;
        }
        
        cache.put(key, value);
        return value;
    }
    
    public String get(String key, Object... args) {
        String pattern = get(key);
        if (args == null || args.length == 0) {
            return pattern;
        }
        return String.format(pattern, args);
    }
    
    public String getOrDefault(String key, String defaultValue) {
        String value = get(key);
        return key.equals(value) ? defaultValue : value;
    }
    
    public boolean hasKey(String key) {
        if (key == null || resourceBundle == null) {
            return false;
        }
        return resourceBundle.containsKey(key);
    }
    
    private void loadBundle() {
        try {
            resourceBundle = ResourceBundle.getBundle(BUNDLE_BASE_NAME, currentLocale);
        } catch (MissingResourceException e) {
            resourceBundle = null;
        }
    }
    
    public void clearCache() {
        cache.clear();
    }
    
    @Override
    protected void doInitialize() {
        loadBundle();
    }
    
    @Override
    protected void doDispose() {
        cache.clear();
    }
}
