package cn.cycle.echart.ui;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Properties;

public class UIConfig {

    private static final String CONFIG_FILE = "echart.properties";
    private static final String USER_HOME = System.getProperty("user.home");
    private static final String CONFIG_DIR = USER_HOME + File.separator + ".echart";
    
    private final Properties properties;
    private final File configFile;

    public UIConfig() {
        this.properties = new Properties();
        this.configFile = new File(CONFIG_DIR, CONFIG_FILE);
        
        loadDefaults();
        loadFromFile();
    }

    protected void loadDefaults() {
        properties.setProperty("workspace.autoSave", "true");
        properties.setProperty("workspace.autoSaveInterval", "300");
        properties.setProperty("chart.cacheSize", "100");
        properties.setProperty("alarm.soundEnabled", "true");
        properties.setProperty("ais.updateInterval", "1000");
        properties.setProperty("locale", "zh_CN");
        properties.setProperty("theme", "light");
    }

    protected void loadFromFile() {
        if (configFile.exists()) {
            try (FileInputStream fis = new FileInputStream(configFile)) {
                properties.load(fis);
            } catch (IOException e) {
                System.err.println("Failed to load config file: " + e.getMessage());
            }
        }
    }

    public void save() {
        File dir = configFile.getParentFile();
        if (!dir.exists()) {
            dir.mkdirs();
        }
        
        try (FileOutputStream fos = new FileOutputStream(configFile)) {
            properties.store(fos, "E-Chart UI Configuration");
        } catch (IOException e) {
            System.err.println("Failed to save config file: " + e.getMessage());
        }
    }

    public String getString(String key) {
        return properties.getProperty(key);
    }

    public String getString(String key, String defaultValue) {
        return properties.getProperty(key, defaultValue);
    }

    public int getInt(String key) {
        return getInt(key, 0);
    }

    public int getInt(String key, int defaultValue) {
        String value = properties.getProperty(key);
        if (value == null) {
            return defaultValue;
        }
        try {
            return Integer.parseInt(value);
        } catch (NumberFormatException e) {
            return defaultValue;
        }
    }

    public boolean getBoolean(String key) {
        return getBoolean(key, false);
    }

    public boolean getBoolean(String key, boolean defaultValue) {
        String value = properties.getProperty(key);
        if (value == null) {
            return defaultValue;
        }
        return Boolean.parseBoolean(value);
    }

    public void setString(String key, String value) {
        properties.setProperty(key, value);
    }

    public void setInt(String key, int value) {
        properties.setProperty(key, String.valueOf(value));
    }

    public void setBoolean(String key, boolean value) {
        properties.setProperty(key, String.valueOf(value));
    }

    public boolean isAutoSaveEnabled() {
        return getBoolean("workspace.autoSave", true);
    }

    public int getAutoSaveInterval() {
        return getInt("workspace.autoSaveInterval", 300);
    }

    public int getChartCacheSize() {
        return getInt("chart.cacheSize", 100);
    }

    public boolean isAlarmSoundEnabled() {
        return getBoolean("alarm.soundEnabled", true);
    }

    public int getAisUpdateInterval() {
        return getInt("ais.updateInterval", 1000);
    }

    public String getLocale() {
        return getString("locale", "zh_CN");
    }

    public void setLocale(String locale) {
        setString("locale", locale);
    }

    public String getTheme() {
        return getString("theme", "light");
    }

    public void setTheme(String theme) {
        setString("theme", theme);
    }
}
