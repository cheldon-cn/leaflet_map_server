package cn.cycle.app.event;

import java.util.HashMap;
import java.util.Map;

public class AppEvent {
    private final AppEventType type;
    private final Object source;
    private final Map<String, Object> data;
    
    public AppEvent(AppEventType type, Object source) {
        this.type = type;
        this.source = source;
        this.data = new HashMap<>();
    }
    
    public AppEvent withData(String key, Object value) {
        data.put(key, value);
        return this;
    }
    
    public AppEventType getType() {
        return type;
    }
    
    public Object getSource() {
        return source;
    }
    
    public Map<String, Object> getData() {
        return data;
    }
    
    @SuppressWarnings("unchecked")
    public <T> T getData(String key) {
        return (T) data.get(key);
    }
    
    public boolean hasData(String key) {
        return data.containsKey(key);
    }
}
