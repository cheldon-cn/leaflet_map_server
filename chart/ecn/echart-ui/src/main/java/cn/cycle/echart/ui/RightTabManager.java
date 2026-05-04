package cn.cycle.echart.ui;

import cn.cycle.echart.ui.handler.ViewHandler;
import javafx.scene.control.Tab;
import javafx.scene.control.TabPane;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 右侧标签页管理器。
 * 
 * <p>管理右侧标签页的显示和切换。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RightTabManager extends TabPane {

    private static final double MIN_PANEL_WIDTH = 200.0;
    private static final double MAX_PANEL_WIDTH = 360.0;

    private final Map<String, FxRightTabPanel> panels;
    private FxRightTabPanel selectedPanel;

    public RightTabManager() {
        this.panels = new HashMap<>();
        this.selectedPanel = null;
        
        initializeLayout();
    }

    private void initializeLayout() {
        setTabClosingPolicy(TabClosingPolicy.ALL_TABS);
        setSide(javafx.geometry.Side.TOP);
        
        getSelectionModel().selectedItemProperty().addListener((obs, oldTab, newTab) -> {
            if (oldTab != null) {
                FxRightTabPanel oldPanel = findPanelByTab(oldTab);
                if (oldPanel != null) {
                    oldPanel.onDeselected();
                }
            }
            
            if (newTab != null) {
                FxRightTabPanel newPanel = findPanelByTab(newTab);
                if (newPanel != null) {
                    newPanel.onSelected();
                    selectedPanel = newPanel;
                }
            } else {
                selectedPanel = null;
            }
        });
        
        getStyleClass().add("right-tab-manager");
        setPrefWidth(ViewHandler.DEFAULT_RIGHT_PANEL_WIDTH);
        setMinWidth(MIN_PANEL_WIDTH);
        setMaxWidth(MAX_PANEL_WIDTH);
    }

    public void registerPanel(FxRightTabPanel panel) {
        Objects.requireNonNull(panel, "panel cannot be null");
        
        panels.put(panel.getId(), panel);
        
        Tab tab = panel.getTab();
        tab.setClosable(panel.isClosable());
        
        tab.setOnCloseRequest(event -> {
            if (!panel.onClose()) {
                event.consume();
            } else {
                panels.remove(panel.getId());
            }
        });
        
        getTabs().add(tab);
    }

    public void unregisterPanel(String panelId) {
        FxRightTabPanel panel = panels.remove(panelId);
        if (panel != null) {
            getTabs().remove(panel.getTab());
            
            if (selectedPanel == panel) {
                selectedPanel = null;
            }
        }
    }

    public FxRightTabPanel getPanel(String panelId) {
        return panels.get(panelId);
    }

    public void selectPanel(String panelId) {
        FxRightTabPanel panel = panels.get(panelId);
        if (panel != null) {
            getSelectionModel().select(panel.getTab());
        }
    }

    public void closePanel(String panelId) {
        FxRightTabPanel panel = panels.get(panelId);
        if (panel != null && panel.isClosable()) {
            if (panel.onClose()) {
                unregisterPanel(panelId);
            }
        }
    }

    public void refreshPanel(String panelId) {
        FxRightTabPanel panel = panels.get(panelId);
        if (panel != null) {
            panel.refresh();
        }
    }

    public void refreshAll() {
        for (FxRightTabPanel panel : panels.values()) {
            panel.refresh();
        }
    }

    private FxRightTabPanel findPanelByTab(Tab tab) {
        for (FxRightTabPanel panel : panels.values()) {
            if (panel.getTab() == tab) {
                return panel;
            }
        }
        return null;
    }

    public FxRightTabPanel getSelectedPanel() {
        return selectedPanel;
    }

    public Map<String, FxRightTabPanel> getPanels() {
        return new HashMap<>(panels);
    }
    
    public boolean hasTabs() {
        return !getTabs().isEmpty();
    }
}
