package cn.cycle.echart.workspace;

import java.util.HashMap;
import java.util.Map;

/**
 * 面板布局。
 * 
 * <p>保存和恢复面板的布局配置。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PanelLayout {

    private final String name;
    private final Map<String, Map<String, Object>> panelStates;
    private Map<String, Object> globalState;

    public PanelLayout(String name) {
        this.name = name;
        this.panelStates = new HashMap<>();
        this.globalState = new HashMap<>();
    }

    public String getName() {
        return name;
    }

    public void addPanelState(String panelId, Map<String, Object> state) {
        panelStates.put(panelId, state);
    }

    public void removePanelState(String panelId) {
        panelStates.remove(panelId);
    }

    public Map<String, Object> getPanelState(String panelId) {
        return panelStates.get(panelId);
    }

    public Map<String, Map<String, Object>> getAllPanelStates() {
        return new HashMap<>(panelStates);
    }

    public int getPanelCount() {
        return panelStates.size();
    }

    public void setGlobalState(String key, Object value) {
        globalState.put(key, value);
    }

    public Object getGlobalState(String key) {
        return globalState.get(key);
    }

    @SuppressWarnings("unchecked")
    public <T> T getGlobalState(String key, Class<T> type) {
        Object value = globalState.get(key);
        if (value != null && type.isInstance(value)) {
            return (T) value;
        }
        return null;
    }

    public Map<String, Object> getGlobalState() {
        return new HashMap<>(globalState);
    }

    public void clear() {
        panelStates.clear();
        globalState.clear();
    }

    public PanelLayout copy() {
        PanelLayout copy = new PanelLayout(this.name);
        
        for (Map.Entry<String, Map<String, Object>> entry : panelStates.entrySet()) {
            copy.panelStates.put(entry.getKey(), new HashMap<>(entry.getValue()));
        }
        
        copy.globalState = new HashMap<>(this.globalState);
        
        return copy;
    }
}
