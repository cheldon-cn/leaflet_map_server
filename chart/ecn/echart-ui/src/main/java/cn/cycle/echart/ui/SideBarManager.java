package cn.cycle.echart.ui;

import javafx.beans.property.ReadOnlyBooleanProperty;
import javafx.beans.property.ReadOnlyBooleanWrapper;
import javafx.beans.property.ReadOnlyDoubleProperty;
import javafx.beans.property.ReadOnlyDoubleWrapper;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;

import cn.cycle.echart.core.LogUtil;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * 侧边栏管理器。
 * 
 * <p>使用ToggleButton+StackPane模式管理侧边栏面板的显示和切换。</p>
 * <p>内部包含按钮栏和内容面板，之间无分割条。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class SideBarManager extends HBox {

    public static final double SIDEBAR_WIDTH = 200.0;
    private static final double COLLAPSED_WIDTH = 0.0;
    private static final double ICON_SIZE = 20.0;
    public static final double SIDEBAR_BUTTON_WIDTH = 40.0;
    public static final double MIN_EXPANDED_WIDTH = 100.0;

    private final VBox buttonBar;
    private final StackPane contentPane;
    private final ToggleGroup toggleGroup;
    private final Map<String, SideBarPanel> panels;
    private final Map<String, ToggleButton> buttons;
    private final List<String> panelOrder;
    
    private SideBarPanel activePanel;
    private boolean isExpanded = false;
    private double expandedWidth = SIDEBAR_WIDTH;
    private double lastExpandedWidth = SIDEBAR_WIDTH;
    
    private final ReadOnlyBooleanWrapper expandedProperty = new ReadOnlyBooleanWrapper(false);
    private final ReadOnlyDoubleWrapper currentWidthProperty = new ReadOnlyDoubleWrapper(SIDEBAR_BUTTON_WIDTH);
    
    private Runnable onBeforeExpand;
    private Runnable onAfterExpand;
    private Runnable onBeforeCollapse;
    private Runnable onAfterCollapse;

    public SideBarManager() {
        this.buttonBar = new VBox();
        this.contentPane = new StackPane();
        this.toggleGroup = new ToggleGroup();
        this.panels = new HashMap<>();
        this.buttons = new HashMap<>();
        this.panelOrder = new ArrayList<>();
        
        initializeLayout();
    }

    private void initializeLayout() {
        buttonBar.setSpacing(2);
        buttonBar.setPadding(new javafx.geometry.Insets(4, 2, 4, 2));
        buttonBar.getStyleClass().add("sidebar-button-bar");
        buttonBar.setPrefWidth(SIDEBAR_BUTTON_WIDTH);
        buttonBar.setMinWidth(SIDEBAR_BUTTON_WIDTH);
        buttonBar.setMaxWidth(SIDEBAR_BUTTON_WIDTH);
        
        contentPane.setPrefWidth(COLLAPSED_WIDTH);
        contentPane.setMinWidth(0);
        contentPane.setMaxWidth(0);
        contentPane.getStyleClass().add("sidebar-content-pane");
        contentPane.setVisible(false);
        
        getChildren().addAll(buttonBar, contentPane);
        
        setMinWidth(SIDEBAR_BUTTON_WIDTH);
        setPrefWidth(SIDEBAR_BUTTON_WIDTH);
        //setMaxWidth(SIDEBAR_BUTTON_WIDTH);
        
        getStyleClass().add("side-bar-manager");
    }

    public void registerPanel(SideBarPanel panel) {
        Objects.requireNonNull(panel, "panel cannot be null");
        
        String panelId = panel.getId();
        panels.put(panelId, panel);
        panelOrder.add(panelId);
        
        ToggleButton button = createToggleButton(panel);
        buttons.put(panelId, button);
        buttonBar.getChildren().add(button);
        
        javafx.scene.Node content = panel.getContent();
        content.setVisible(false);
        StackPane.setAlignment(content, javafx.geometry.Pos.TOP_LEFT);
        contentPane.getChildren().add(content);
    }

    public void registerPanel(SideBarPanel panel, int index) {
        Objects.requireNonNull(panel, "panel cannot be null");
        
        String panelId = panel.getId();
        panels.put(panelId, panel);
        panelOrder.add(index, panelId);
        
        ToggleButton button = createToggleButton(panel);
        buttons.put(panelId, button);
        buttonBar.getChildren().add(index, button);
        
        javafx.scene.Node content = panel.getContent();
        content.setVisible(false);
        StackPane.setAlignment(content, javafx.geometry.Pos.TOP_LEFT);
        contentPane.getChildren().add(content);
    }

    private ToggleButton createToggleButton(SideBarPanel panel) {
        ToggleButton button = new ToggleButton();
        button.setId(panel.getId());
        button.setToggleGroup(toggleGroup);
        button.setPrefSize(36, 36);
        button.setMinSize(36, 36);
        button.setMaxSize(36, 36);
        button.getStyleClass().add("sidebar-toggle-button");
        
        if (panel.getTooltip() != null && !panel.getTooltip().isEmpty()) {
            javafx.scene.control.Tooltip tooltip = new javafx.scene.control.Tooltip(panel.getTooltip());
            button.setTooltip(tooltip);
        }
        
        if (panel.getIconPath() != null && !panel.getIconPath().isEmpty()) {
            try {
                Image image = new Image(getClass().getResourceAsStream(panel.getIconPath()));
                if (image != null && !image.isError()) {
                    ImageView imageView = new ImageView(image);
                    imageView.setFitWidth(ICON_SIZE);
                    imageView.setFitHeight(ICON_SIZE);
                    imageView.setPreserveRatio(true);
                    button.setGraphic(imageView);
                }
            } catch (Exception e) {
                button.setText(panel.getTitle().substring(0, 1));
            }
        } else {
            button.setText(panel.getTitle().substring(0, 1));
        }
        
        button.setOnAction(e -> {
            LogUtil.debug("SideBarManager", "Button clicked: panelId=%s, isExpanded=%s, activePanel=%s",
                    panel.getId(), isExpanded, activePanel != null ? activePanel.getId() : "null");
            if (isExpanded && activePanel != null && activePanel.getId().equals(panel.getId())) {
                collapsePanel();
            } else {
                showPanel(panel.getId());
                button.setSelected(true);
            }
        });
        
        return button;
    }

    public void unregisterPanel(String panelId) {
        SideBarPanel panel = panels.remove(panelId);
        if (panel != null) {
            ToggleButton button = buttons.remove(panelId);
            if (button != null) {
                buttonBar.getChildren().remove(button);
            }
            
            contentPane.getChildren().remove(panel.getContent());
            
            if (activePanel == panel) {
                activePanel = null;
                collapsePanel();
            }
        }
    }

    public void showPanel(String panelId) {
        showPanel(panelId, -1);
    }
    
    public void showPanel(String panelId, double width) {
        SideBarPanel panel = panels.get(panelId);
        if (panel == null) {
            LogUtil.debug("SideBarManager", "showPanel: panel not found for id=%s, availablePanels=%s", panelId, panels.keySet());
            return;
        }
         LogUtil.debug("SideBarManager", "showPanel: panelId=%s, availablePanels=%s", panelId, panels.keySet());
        if (activePanel != null && activePanel != panel) {
            activePanel.getContent().setVisible(false);
            activePanel.onDeactivate();
        }
        
        activePanel = panel;
        panel.getContent().setVisible(true);
        panel.getContent().toFront();
        panel.onActivate();
        
        if (!isExpanded) {
            if (width > 0) {
                expandPanel(width);
            } else {
                expandPanel();
            }
        } else {
            double targetWidth = width > 0 ? width : lastExpandedWidth;
            if (targetWidth <= 0) {
                targetWidth = SIDEBAR_WIDTH;
            }
            setExpandedWidth(targetWidth);
        }
        
        ToggleButton button = buttons.get(panelId);
        if (button != null && !button.isSelected()) {
            button.setSelected(true);
        }
    }

    public void hidePanel(String panelId) {
        SideBarPanel panel = panels.get(panelId);
        if (panel == null) {
            return;
        }
        
        if (activePanel == panel) {
            collapsePanel();
        }
        
        ToggleButton button = buttons.get(panelId);
        if (button != null) {
            button.setSelected(false);
        }
    }

    public void togglePanel(String panelId) {
        SideBarPanel panel = panels.get(panelId);
        if (panel == null) {
            return;
        }
        
        if (activePanel == panel && isExpanded) {
            collapsePanel();
        } else {
            showPanel(panelId);
        }
    }

    public void expandPanel() {
        expandPanel(lastExpandedWidth > 0 ? lastExpandedWidth : SIDEBAR_WIDTH);
    }
    
    public void expandPanel(double width) {
        if (isExpanded) {
            return;
        }
        
        LogUtil.debug("SideBarManager", "[EXPAND BEFORE] buttonBar=%s, contentPane=%s, total=%s", 
                SIDEBAR_BUTTON_WIDTH, contentPane.getPrefWidth(), getPrefWidth());
        
        if (onBeforeExpand != null) {
            onBeforeExpand.run();
        }
        
        isExpanded = true;
        
        expandedWidth = Math.max(width, MIN_EXPANDED_WIDTH);
        
        double totalWidth = SIDEBAR_BUTTON_WIDTH + expandedWidth;
        
        contentPane.setPrefWidth(expandedWidth);
        contentPane.setMinWidth(MIN_EXPANDED_WIDTH);
        contentPane.setMaxWidth(Double.MAX_VALUE);
        contentPane.setVisible(true);
        
        //setMinWidth(totalWidth);
        setPrefWidth(totalWidth);
        //setMaxWidth(totalWidth);
        
        currentWidthProperty.set(totalWidth);
        expandedProperty.set(true);
        
        LogUtil.debug("SideBarManager", "[EXPAND AFTER] buttonBar=%s, contentPane=%s, total=%s", 
                SIDEBAR_BUTTON_WIDTH, contentPane.getPrefWidth(), getPrefWidth());
        
        if (onAfterExpand != null) {
            javafx.application.Platform.runLater(onAfterExpand);
        }
    }

    public void collapsePanel() {
        if (!isExpanded) {
            return;
        }
        
        LogUtil.debug("SideBarManager", "[COLLAPSE BEFORE] buttonBar=%s, contentPane=%s, total=%s", 
                SIDEBAR_BUTTON_WIDTH, contentPane.getPrefWidth(), getPrefWidth());
        
        if (onBeforeCollapse != null) {
            onBeforeCollapse.run();
        }
        
        lastExpandedWidth = expandedWidth;
        
        isExpanded = false;
        contentPane.setVisible(false);
        contentPane.setPrefWidth(COLLAPSED_WIDTH);
        contentPane.setMinWidth(0);
        contentPane.setMaxWidth(0);
        
        //setMinWidth(SIDEBAR_BUTTON_WIDTH);
        setPrefWidth(SIDEBAR_BUTTON_WIDTH);
        //setMaxWidth(SIDEBAR_BUTTON_WIDTH);
        
        currentWidthProperty.set(SIDEBAR_BUTTON_WIDTH);
        
        if (activePanel != null) {
            activePanel.getContent().setVisible(false);
            activePanel.onDeactivate();
            activePanel = null;
        }
        
        expandedProperty.set(false);
        toggleGroup.selectToggle(null);
        
        LogUtil.debug("SideBarManager", "[COLLAPSE AFTER] buttonBar=%s, contentPane=0, total=%s", 
                SIDEBAR_BUTTON_WIDTH, SIDEBAR_BUTTON_WIDTH);
        
        if (onAfterCollapse != null) {
            javafx.application.Platform.runLater(onAfterCollapse);
        }
    }
    
    public String getFirstPanelId() {
        String layersPanelId = "layers";
        if (panels.containsKey(layersPanelId)) {
            SideBarPanel panel = panels.get(layersPanelId);
            if (panel != null && panel.getContent() != null) {
                return layersPanelId;
            }
        }
        
        for (String panelId : panelOrder) {
            SideBarPanel panel = panels.get(panelId);
            if (panel != null && panel.getContent() != null) {
                return panelId;
            }
        }
        
        return null;
    }

    public SideBarPanel getPanel(String panelId) {
        return panels.get(panelId);
    }

    public SideBarPanel getActivePanel() {
        return activePanel;
    }

    public boolean isExpanded() {
        return isExpanded;
    }

    public void setExpandedWidth(double width) {
        double actualWidth = Math.max(width, MIN_EXPANDED_WIDTH);
        this.expandedWidth = actualWidth;
        this.lastExpandedWidth = actualWidth;
        
        if (isExpanded) {
            double totalWidth = SIDEBAR_BUTTON_WIDTH + actualWidth;
            contentPane.setPrefWidth(actualWidth);
            contentPane.setMinWidth(MIN_EXPANDED_WIDTH);
            
            //setMinWidth(totalWidth);
            setPrefWidth(totalWidth);
            //setMaxWidth(totalWidth);
            
            currentWidthProperty.set(totalWidth);
        }
    }

    public double getExpandedWidth() {
        return expandedWidth;
    }
    
    public double getLastExpandedWidth() {
        return lastExpandedWidth;
    }

    public Map<String, SideBarPanel> getPanels() {
        return new HashMap<>(panels);
    }

    public VBox getButtonBar() {
        return buttonBar;
    }

    public StackPane getContentPane() {
        return contentPane;
    }

    public ToggleGroup getToggleGroup() {
        return toggleGroup;
    }
    
    public ReadOnlyBooleanProperty expandedProperty() {
        return expandedProperty.getReadOnlyProperty();
    }
    
    public ReadOnlyDoubleProperty currentWidthProperty() {
        return currentWidthProperty.getReadOnlyProperty();
    }
    
    public double getCurrentWidth() {
        return currentWidthProperty.get();
    }
    
    public void setOnBeforeExpand(Runnable callback) {
        this.onBeforeExpand = callback;
    }
    
    public void setOnAfterExpand(Runnable callback) {
        this.onAfterExpand = callback;
    }
    
    public void setOnBeforeCollapse(Runnable callback) {
        this.onBeforeCollapse = callback;
    }
    
    public void setOnAfterCollapse(Runnable callback) {
        this.onAfterCollapse = callback;
    }

    public static class SideBarPanel {
        private final String id;
        private final String title;
        private final String iconPath;
        private final String tooltip;
        private final javafx.scene.Node content;

        public SideBarPanel(String id, String title, String iconPath, String tooltip, 
                           javafx.scene.Node content) {
            this.id = id;
            this.title = title;
            this.iconPath = iconPath;
            this.tooltip = tooltip;
            this.content = content;
        }

        public String getId() {
            return id;
        }

        public String getTitle() {
            return title;
        }

        public String getIconPath() {
            return iconPath;
        }

        public String getTooltip() {
            return tooltip;
        }

        public javafx.scene.Node getContent() {
            return content;
        }

        public void onActivate() {
        }

        public void onDeactivate() {
        }
    }
}
