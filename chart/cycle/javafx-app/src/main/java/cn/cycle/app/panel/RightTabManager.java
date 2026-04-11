package cn.cycle.app.panel;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import cn.cycle.app.state.StatePersistable;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

public class RightTabManager extends AbstractLifecycleComponent implements StatePersistable {
    
    private final TabPane tabPane;
    private final Map<String, RightTabPanel> panels = new LinkedHashMap<>();
    private final Map<String, Tab> tabs = new HashMap<>();
    private String activeTabId = null;
    private double width = 300;
    
    public RightTabManager(TabPane tabPane) {
        this.tabPane = tabPane;
    }
    
    public void registerPanel(RightTabPanel panel) {
        panels.put(panel.getId(), panel);
        Tab tab = panel.createTab();
        tabs.put(panel.getId(), tab);
        tabPane.getTabs().add(tab);
        
        tab.setOnSelectionChanged(e -> {
            if (tab.isSelected()) {
                onTabSelected(panel.getId());
            }
        });
    }
    
    public void unregisterPanel(String panelId) {
        RightTabPanel panel = panels.remove(panelId);
        Tab tab = tabs.remove(panelId);
        if (panel != null && tab != null) {
            panel.dispose();
            tabPane.getTabs().remove(tab);
        }
    }
    
    private void onTabSelected(String panelId) {
        if (activeTabId != null && !activeTabId.equals(panelId)) {
            RightTabPanel oldPanel = panels.get(activeTabId);
            if (oldPanel != null) {
                oldPanel.deactivate();
            }
        }
        
        RightTabPanel panel = panels.get(panelId);
        if (panel != null) {
            panel.activate();
            activeTabId = panelId;
            
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.RIGHT_TAB_CHANGED, this)
                    .withData("tabId", panelId)
            );
        }
    }
    
    public void selectTab(String panelId) {
        Tab tab = tabs.get(panelId);
        if (tab != null) {
            tabPane.getSelectionModel().select(tab);
        }
    }
    
    public String getActiveTabId() {
        return activeTabId;
    }
    
    public RightTabPanel getPanel(String panelId) {
        return panels.get(panelId);
    }
    
    public double getWidth() {
        return width;
    }
    
    public void setWidth(double width) {
        this.width = width;
        if (tabPane != null) {
            tabPane.setPrefWidth(width);
        }
    }
    
    public void showAll() {
        tabPane.setVisible(true);
        tabPane.setManaged(true);
    }
    
    public void hideAll() {
        tabPane.setVisible(false);
        tabPane.setManaged(false);
    }
    
    @Override
    protected void doInitialize() {
        for (RightTabPanel panel : panels.values()) {
            panel.initialize();
        }
    }
    
    @Override
    protected void doActivate() {
        if (activeTabId != null) {
            RightTabPanel panel = panels.get(activeTabId);
            if (panel != null) {
                panel.activate();
            }
        }
    }
    
    @Override
    protected void doDeactivate() {
        if (activeTabId != null) {
            RightTabPanel panel = panels.get(activeTabId);
            if (panel != null) {
                panel.deactivate();
            }
        }
    }
    
    @Override
    protected void doDispose() {
        for (RightTabPanel panel : panels.values()) {
            panel.dispose();
        }
        panels.clear();
        tabs.clear();
    }
    
    @Override
    public Map<String, Object> saveState() {
        Map<String, Object> state = new HashMap<>();
        state.put("activeTabId", activeTabId);
        state.put("width", width);
        return state;
    }
    
    @Override
    public void restoreState(Map<String, Object> state) {
        if (state == null) {
            return;
        }
        
        Object w = state.get("width");
        if (w instanceof Number) {
            width = ((Number) w).doubleValue();
            setWidth(width);
        }
        
        Object activeId = state.get("activeTabId");
        if (activeId instanceof String) {
            String tabId = (String) activeId;
            if (panels.containsKey(tabId)) {
                selectTab(tabId);
            }
        }
    }
    
    @Override
    public String getStateKey() {
        return "righttab.manager";
    }
}
