package cn.cycle.app.sidebar;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import cn.cycle.app.state.StatePersistable;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;

import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;

public class SideBarManager extends AbstractLifecycleComponent implements StatePersistable {
    
    private final VBox buttonContainer;
    private final StackPane panelContainer;
    private final Map<String, SideBarPanel> panels = new LinkedHashMap<>();
    private final Map<String, ToggleButton> buttons = new HashMap<>();
    private String activePanelId = null;
    private double panelWidth = 250;
    
    public SideBarManager(VBox buttonContainer, StackPane panelContainer) {
        this.buttonContainer = buttonContainer;
        this.panelContainer = panelContainer;
    }
    
    public void registerPanel(SideBarPanel panel) {
        panels.put(panel.getId(), panel);
        
        ToggleButton button = createButton(panel);
        buttons.put(panel.getId(), button);
        buttonContainer.getChildren().add(button);
    }
    
    public void unregisterPanel(String panelId) {
        SideBarPanel panel = panels.remove(panelId);
        if (panel != null) {
            panel.dispose();
            buttonContainer.getChildren().remove(buttons.remove(panelId));
        }
    }
    
    public void showPanel(String panelId) {
        if (activePanelId != null && activePanelId.equals(panelId)) {
            hidePanel();
            return;
        }
        
        SideBarPanel panel = panels.get(panelId);
        if (panel == null) {
            return;
        }
        
        if (activePanelId != null) {
            SideBarPanel activePanel = panels.get(activePanelId);
            if (activePanel != null) {
                activePanel.deactivate();
            }
            ToggleButton activeButton = buttons.get(activePanelId);
            if (activeButton != null) {
                activeButton.setSelected(false);
            }
        }
        
        panelContainer.getChildren().clear();
        panelContainer.getChildren().add(panel.createContent());
        panel.activate();
        ToggleButton button = buttons.get(panelId);
        if (button != null) {
            button.setSelected(true);
        }
        activePanelId = panelId;
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.SIDEBAR_PANEL_CHANGED, this)
                .withData("panelId", panelId)
        );
    }
    
    public void hidePanel() {
        if (activePanelId != null) {
            SideBarPanel panel = panels.get(activePanelId);
            if (panel != null) {
                panel.deactivate();
            }
            ToggleButton button = buttons.get(activePanelId);
            if (button != null) {
                button.setSelected(false);
            }
            panelContainer.getChildren().clear();
            activePanelId = null;
        }
    }
    
    public void togglePanel(String panelId) {
        if (activePanelId != null && activePanelId.equals(panelId)) {
            hidePanel();
        } else {
            showPanel(panelId);
        }
    }
    
    private ToggleButton createButton(SideBarPanel panel) {
        ToggleButton button = new ToggleButton();
        button.setGraphic(panel.getIcon());
        button.setTooltip(new Tooltip(panel.getTitle()));
        button.setMaxWidth(Double.MAX_VALUE);
        button.setPrefHeight(40);
        button.setStyle("-fx-padding: 8; -fx-spacing: 5;");
        button.setOnAction(e -> togglePanel(panel.getId()));
        return button;
    }
    
    public String getActivePanelId() {
        return activePanelId;
    }
    
    public boolean isPanelVisible(String panelId) {
        return panelId != null && panelId.equals(activePanelId);
    }
    
    public SideBarPanel getPanel(String panelId) {
        return panels.get(panelId);
    }
    
    public double getPanelWidth() {
        return panelWidth;
    }
    
    public void setPanelWidth(double width) {
        this.panelWidth = width;
        if (panelContainer != null) {
            panelContainer.setPrefWidth(width);
        }
    }
    
    public void collapseAll() {
        hidePanel();
    }
    
    @Override
    protected void doInitialize() {
        for (SideBarPanel panel : panels.values()) {
            panel.initialize();
        }
    }
    
    @Override
    protected void doActivate() {
        if (activePanelId != null) {
            SideBarPanel panel = panels.get(activePanelId);
            if (panel != null) {
                panel.activate();
            }
        }
    }
    
    @Override
    protected void doDeactivate() {
        if (activePanelId != null) {
            SideBarPanel panel = panels.get(activePanelId);
            if (panel != null) {
                panel.deactivate();
            }
        }
    }
    
    @Override
    protected void doDispose() {
        hidePanel();
        for (SideBarPanel panel : panels.values()) {
            panel.dispose();
        }
        panels.clear();
        buttons.clear();
    }
    
    @Override
    public Map<String, Object> saveState() {
        Map<String, Object> state = new HashMap<>();
        state.put("activePanelId", activePanelId);
        state.put("panelWidth", panelWidth);
        return state;
    }
    
    @Override
    public void restoreState(Map<String, Object> state) {
        if (state == null) {
            return;
        }
        
        Object width = state.get("panelWidth");
        if (width instanceof Number) {
            panelWidth = ((Number) width).doubleValue();
        }
        
        Object activeId = state.get("activePanelId");
        if (activeId instanceof String) {
            String panelId = (String) activeId;
            if (panels.containsKey(panelId)) {
                showPanel(panelId);
            }
        }
    }
    
    @Override
    public String getStateKey() {
        return "sidebar.manager";
    }
}
