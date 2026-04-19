package cn.cycle.echart.ui;

import javafx.scene.control.Accordion;
import javafx.scene.control.TitledPane;
import javafx.scene.layout.VBox;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 侧边栏管理器。
 * 
 * <p>管理侧边栏面板的显示和切换。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class SideBarManager extends VBox {

    private final Accordion accordion;
    private final Map<String, FxSideBarPanel> panels;
    private FxSideBarPanel activePanel;

    public SideBarManager() {
        this.accordion = new Accordion();
        this.panels = new HashMap<>();
        this.activePanel = null;
        
        initializeLayout();
    }

    private void initializeLayout() {
        getChildren().add(accordion);
        accordion.prefWidthProperty().bind(widthProperty());
        accordion.prefHeightProperty().bind(heightProperty());
        
        getStyleClass().add("side-bar-manager");
        setPrefWidth(250);
        setMinWidth(150);
    }

    public void registerPanel(FxSideBarPanel panel) {
        Objects.requireNonNull(panel, "panel cannot be null");
        
        String panelId = panel.getId();
        panels.put(panelId, panel);
        
        TitledPane titledPane = new TitledPane();
        titledPane.setText(panel.getTitle());
        titledPane.setContent(panel.getContent());
        titledPane.setGraphic(null);
        
        if (panel.getIconImage() != null) {
            javafx.scene.image.ImageView iconView = new javafx.scene.image.ImageView(panel.getIconImage());
            iconView.setFitWidth(16);
            iconView.setFitHeight(16);
            titledPane.setGraphic(iconView);
        }
        
        titledPane.expandedProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal) {
                activatePanel(panel);
            } else if (activePanel == panel) {
                deactivatePanel(panel);
            }
        });
        
        accordion.getPanes().add(titledPane);
    }

    public void unregisterPanel(String panelId) {
        FxSideBarPanel panel = panels.remove(panelId);
        if (panel != null) {
            accordion.getPanes().removeIf(pane -> 
                pane.getText().equals(panel.getTitle()));
            
            if (activePanel == panel) {
                deactivatePanel(panel);
                activePanel = null;
            }
        }
    }

    public FxSideBarPanel getPanel(String panelId) {
        return panels.get(panelId);
    }

    public void showPanel(String panelId) {
        FxSideBarPanel panel = panels.get(panelId);
        if (panel != null) {
            for (TitledPane pane : accordion.getPanes()) {
                if (pane.getText().equals(panel.getTitle())) {
                    accordion.setExpandedPane(pane);
                    break;
                }
            }
        }
    }

    public void hidePanel(String panelId) {
        FxSideBarPanel panel = panels.get(panelId);
        if (panel != null) {
            for (TitledPane pane : accordion.getPanes()) {
                if (pane.getText().equals(panel.getTitle())) {
                    pane.setExpanded(false);
                    break;
                }
            }
        }
    }

    private void activatePanel(FxSideBarPanel panel) {
        if (activePanel != null && activePanel != panel) {
            deactivatePanel(activePanel);
        }
        activePanel = panel;
        panel.onActivate();
    }

    private void deactivatePanel(FxSideBarPanel panel) {
        if (activePanel == panel) {
            panel.onDeactivate();
            activePanel = null;
        }
    }

    public FxSideBarPanel getActivePanel() {
        return activePanel;
    }

    public Map<String, FxSideBarPanel> getPanels() {
        return new HashMap<>(panels);
    }
}
