package cn.cycle.app.prefs;

import cn.cycle.app.state.StatePersistable;

import java.util.HashMap;
import java.util.Map;
import java.util.prefs.Preferences;

public class UserPreferences implements StatePersistable {
    
    private static final UserPreferences INSTANCE = new UserPreferences();
    private static final String PREFS_NODE = "cn/cycle/app/prefs";
    
    private final Preferences prefs;
    private final Map<String, Object> cache = new HashMap<>();
    
    private UserPreferences() {
        prefs = Preferences.userRoot().node(PREFS_NODE);
    }
    
    public static UserPreferences getInstance() {
        return INSTANCE;
    }
    
    public String getString(String key, String defaultValue) {
        String cached = (String) cache.get(key);
        if (cached != null) {
            return cached;
        }
        return prefs.get(key, defaultValue);
    }
    
    public void putString(String key, String value) {
        prefs.put(key, value);
        cache.put(key, value);
    }
    
    public int getInt(String key, int defaultValue) {
        Object cached = cache.get(key);
        if (cached instanceof Integer) {
            return (Integer) cached;
        }
        return prefs.getInt(key, defaultValue);
    }
    
    public void putInt(String key, int value) {
        prefs.putInt(key, value);
        cache.put(key, value);
    }
    
    public long getLong(String key, long defaultValue) {
        Object cached = cache.get(key);
        if (cached instanceof Long) {
            return (Long) cached;
        }
        return prefs.getLong(key, defaultValue);
    }
    
    public void putLong(String key, long value) {
        prefs.putLong(key, value);
        cache.put(key, value);
    }
    
    public double getDouble(String key, double defaultValue) {
        Object cached = cache.get(key);
        if (cached instanceof Double) {
            return (Double) cached;
        }
        return prefs.getDouble(key, defaultValue);
    }
    
    public void putDouble(String key, double value) {
        prefs.putDouble(key, value);
        cache.put(key, value);
    }
    
    public boolean getBoolean(String key, boolean defaultValue) {
        Object cached = cache.get(key);
        if (cached instanceof Boolean) {
            return (Boolean) cached;
        }
        return prefs.getBoolean(key, defaultValue);
    }
    
    public void putBoolean(String key, boolean value) {
        prefs.putBoolean(key, value);
        cache.put(key, value);
    }
    
    public void remove(String key) {
        prefs.remove(key);
        cache.remove(key);
    }
    
    public void clear() {
        try {
            prefs.clear();
            cache.clear();
        } catch (Exception e) {
        }
    }
    
    public boolean hasKey(String key) {
        return cache.containsKey(key) || prefs.get(key, null) != null;
    }
    
    @Override
    public Map<String, Object> saveState() {
        return new HashMap<>(cache);
    }
    
    @Override
    public void restoreState(Map<String, Object> state) {
        if (state == null) {
            return;
        }
        
        for (Map.Entry<String, Object> entry : state.entrySet()) {
            String key = entry.getKey();
            Object value = entry.getValue();
            
            if (value instanceof String) {
                putString(key, (String) value);
            } else if (value instanceof Integer) {
                putInt(key, (Integer) value);
            } else if (value instanceof Long) {
                putLong(key, (Long) value);
            } else if (value instanceof Double) {
                putDouble(key, (Double) value);
            } else if (value instanceof Boolean) {
                putBoolean(key, (Boolean) value);
            }
        }
    }
    
    @Override
    public String getStateKey() {
        return "user.preferences";
    }
}
