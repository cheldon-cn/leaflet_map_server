package cn.cycle.app.util;

import java.util.Locale;
import java.util.ResourceBundle;
import java.util.MissingResourceException;
import java.util.concurrent.ConcurrentHashMap;
import javafx.beans.property.ObjectProperty;
import javafx.beans.property.SimpleObjectProperty;

public class I18nManager {
    
    private static I18nManager instance;
    private static final String BUNDLE_BASE = "cn.cycle.app.i18n.messages";
    
    private final ConcurrentHashMap<Locale, ResourceBundle> bundles = new ConcurrentHashMap<>();
    private final ObjectProperty<Locale> localeProperty = new SimpleObjectProperty<>(Locale.getDefault());
    
    private I18nManager() {}
    
    public static synchronized I18nManager getInstance() {
        if (instance == null) {
            instance = new I18nManager();
        }
        return instance;
    }
    
    public void setLocale(Locale locale) {
        localeProperty.set(locale);
        Locale.setDefault(locale);
    }
    
    public Locale getLocale() {
        return localeProperty.get();
    }
    
    public ObjectProperty<Locale> localeProperty() {
        return localeProperty;
    }
    
    public String getString(String key) {
        try {
            return getBundle().getString(key);
        } catch (MissingResourceException e) {
            return key;
        }
    }
    
    public String getString(String key, Object... args) {
        try {
            String pattern = getBundle().getString(key);
            return String.format(pattern, args);
        } catch (MissingResourceException e) {
            return key;
        }
    }
    
    public String getString(Locale locale, String key) {
        try {
            return getBundle(locale).getString(key);
        } catch (MissingResourceException e) {
            return key;
        }
    }
    
    private ResourceBundle getBundle() {
        return getBundle(localeProperty.get());
    }
    
    private ResourceBundle getBundle(Locale locale) {
        return bundles.computeIfAbsent(locale, 
            l -> {
                try {
                    return ResourceBundle.getBundle(BUNDLE_BASE, l);
                } catch (MissingResourceException e) {
                    return ResourceBundle.getBundle(BUNDLE_BASE, Locale.ENGLISH);
                }
            });
    }
    
    public void clearCache() {
        bundles.clear();
    }
    
    public static String tr(String key) {
        return getInstance().getString(key);
    }
    
    public static String tr(String key, Object... args) {
        return getInstance().getString(key, args);
    }
}
