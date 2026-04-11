package cn.cycle.app.panel;

import cn.cycle.app.lifecycle.LifecycleComponent;
import cn.cycle.app.state.StatePersistable;
import javafx.scene.Node;
import javafx.scene.control.Tab;

import java.util.Map;

public interface RightTabPanel extends LifecycleComponent, StatePersistable {
    
    String getId();
    
    String getTitle();
    
    Node getIcon();
    
    Tab createTab();
    
    @Override
    default void initialize() {
    }
    
    @Override
    default void activate() {
        onActivate();
    }
    
    @Override
    default void deactivate() {
        onDeactivate();
    }
    
    @Override
    default void dispose() {
        onDestroy();
    }
    
    @Override
    default boolean isInitialized() {
        return true;
    }
    
    @Override
    default boolean isActive() {
        return false;
    }
    
    default void onActivate() {
    }
    
    default void onDeactivate() {
    }
    
    default void onDestroy() {
    }
    
    @Override
    default Map<String, Object> saveState() {
        return null;
    }
    
    @Override
    default void restoreState(Map<String, Object> state) {
    }
    
    @Override
    default String getStateKey() {
        return "righttab.panel." + getId();
    }
}
