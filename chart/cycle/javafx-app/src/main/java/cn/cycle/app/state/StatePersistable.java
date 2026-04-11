package cn.cycle.app.state;

import java.util.Map;

public interface StatePersistable {
    
    Map<String, Object> saveState();
    
    void restoreState(Map<String, Object> state);
    
    String getStateKey();
}
