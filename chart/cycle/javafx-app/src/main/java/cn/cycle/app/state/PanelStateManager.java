package cn.cycle.app.state;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import cn.cycle.app.sidebar.SideBarManager;
import cn.cycle.app.panel.RightTabManager;

import java.util.HashMap;
import java.util.Map;
import java.util.prefs.Preferences;

public class PanelStateManager extends AbstractLifecycleComponent {
    
    private static final PanelStateManager INSTANCE = new PanelStateManager();
    private static final String PREFS_KEY = "panel.states";
    
    private final Preferences prefs;
    private final Map<String, StatePersistable> persistableComponents = new HashMap<>();
    
    private SideBarManager sideBarManager;
    private RightTabManager rightTabManager;
    
    private PanelStateManager() {
        prefs = Preferences.userNodeForPackage(PanelStateManager.class);
    }
    
    public static PanelStateManager getInstance() {
        return INSTANCE;
    }
    
    public void setSideBarManager(SideBarManager manager) {
        this.sideBarManager = manager;
        if (manager != null) {
            registerPersistable(manager);
        }
    }
    
    public void setRightTabManager(RightTabManager manager) {
        this.rightTabManager = manager;
        if (manager != null) {
            registerPersistable(manager);
        }
    }
    
    public void registerPersistable(StatePersistable component) {
        if (component != null && component.getStateKey() != null) {
            persistableComponents.put(component.getStateKey(), component);
        }
    }
    
    public void unregisterPersistable(String key) {
        persistableComponents.remove(key);
    }
    
    public void saveAllStates() {
        Map<String, Object> allStates = new HashMap<>();
        
        for (Map.Entry<String, StatePersistable> entry : persistableComponents.entrySet()) {
            Map<String, Object> componentState = entry.getValue().saveState();
            if (componentState != null && !componentState.isEmpty()) {
                allStates.put(entry.getKey(), componentState);
            }
        }
        
        prefs.put(PREFS_KEY, mapToString(allStates));
    }
    
    public void restoreAllStates() {
        String saved = prefs.get(PREFS_KEY, null);
        if (saved == null) {
            return;
        }
        
        Map<String, Object> allStates = stringToMap(saved);
        
        for (Map.Entry<String, Object> entry : allStates.entrySet()) {
            String key = entry.getKey();
            StatePersistable component = persistableComponents.get(key);
            if (component != null) {
                @SuppressWarnings("unchecked")
                Map<String, Object> componentState = (Map<String, Object>) entry.getValue();
                component.restoreState(componentState);
            }
        }
    }
    
    public void saveComponentState(String key) {
        StatePersistable component = persistableComponents.get(key);
        if (component == null) {
            return;
        }
        
        String saved = prefs.get(PREFS_KEY, "{}");
        Map<String, Object> allStates = stringToMap(saved);
        
        Map<String, Object> componentState = component.saveState();
        if (componentState != null) {
            allStates.put(key, componentState);
        }
        
        prefs.put(PREFS_KEY, mapToString(allStates));
    }
    
    public void clearAllStates() {
        prefs.remove(PREFS_KEY);
    }
    
    private String mapToString(Map<String, Object> map) {
        StringBuilder sb = new StringBuilder();
        sb.append("{");
        boolean first = true;
        for (Map.Entry<String, Object> entry : map.entrySet()) {
            if (!first) {
                sb.append(",");
            }
            first = false;
            sb.append("\"").append(entry.getKey()).append("\":");
            Object value = entry.getValue();
            if (value instanceof Map) {
                @SuppressWarnings("unchecked")
                Map<String, Object> nestedMap = (Map<String, Object>) value;
                sb.append(mapToString(nestedMap));
            } else if (value instanceof String) {
                sb.append("\"").append(value).append("\"");
            } else if (value instanceof Number || value instanceof Boolean) {
                sb.append(value);
            } else {
                sb.append("\"").append(value.toString()).append("\"");
            }
        }
        sb.append("}");
        return sb.toString();
    }
    
    @SuppressWarnings("unchecked")
    private Map<String, Object> stringToMap(String str) {
        Map<String, Object> result = new HashMap<>();
        if (str == null || str.isEmpty() || !str.startsWith("{") || !str.endsWith("}")) {
            return result;
        }
        
        str = str.substring(1, str.length() - 1);
        if (str.isEmpty()) {
            return result;
        }
        
        int depth = 0;
        StringBuilder key = new StringBuilder();
        StringBuilder value = new StringBuilder();
        boolean inKey = true;
        boolean inString = false;
        
        for (int i = 0; i < str.length(); i++) {
            char c = str.charAt(i);
            
            if (c == '"' && (i == 0 || str.charAt(i - 1) != '\\')) {
                inString = !inString;
                continue;
            }
            
            if (!inString) {
                if (c == '{') {
                    depth++;
                } else if (c == '}') {
                    depth--;
                } else if (c == ':' && depth == 0 && inKey) {
                    inKey = false;
                    continue;
                } else if (c == ',' && depth == 0) {
                    addToMap(result, key.toString().trim(), value.toString().trim());
                    key = new StringBuilder();
                    value = new StringBuilder();
                    inKey = true;
                    continue;
                }
            }
            
            if (inKey) {
                key.append(c);
            } else {
                value.append(c);
            }
        }
        
        if (key.length() > 0) {
            addToMap(result, key.toString().trim(), value.toString().trim());
        }
        
        return result;
    }
    
    private void addToMap(Map<String, Object> map, String key, String value) {
        key = key.replace("\"", "");
        
        if (value.startsWith("{")) {
            map.put(key, stringToMap(value));
        } else if (value.startsWith("\"") && value.endsWith("\"")) {
            map.put(key, value.substring(1, value.length() - 1));
        } else if ("true".equalsIgnoreCase(value)) {
            map.put(key, Boolean.TRUE);
        } else if ("false".equalsIgnoreCase(value)) {
            map.put(key, Boolean.FALSE);
        } else {
            try {
                if (value.contains(".")) {
                    map.put(key, Double.parseDouble(value));
                } else {
                    map.put(key, Long.parseLong(value));
                }
            } catch (NumberFormatException e) {
                map.put(key, value);
            }
        }
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        persistableComponents.clear();
    }
}
