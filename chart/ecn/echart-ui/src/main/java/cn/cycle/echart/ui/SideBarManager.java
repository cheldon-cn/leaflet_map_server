package cn.cycle.echart.ui;

import javafx.animation.TranslateTransition;
import javafx.animation.Timeline;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.util.Duration;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 侧边栏管理器。
 * 
 * <p>使用ToggleButton+StackPane模式管理侧边栏面板的显示和切换。</p>
 * <p>支持面板折叠/展开动画。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class SideBarManager extends HBox {

    private static final double SIDEBAR_WIDTH = 280.0;
    private static final double COLLAPSED_WIDTH = 0.0;
    private static final Duration ANIMATION_DURATION = Duration.millis(200);
    private static final double ICON_SIZE = 20.0;

    private final VBox buttonBar;
    private final StackPane contentPane;
    private final ToggleGroup toggleGroup;
    private final Map<String, SideBarPanel> panels;
    private final Map<String, ToggleButton> buttons;
    
    private SideBarPanel activePanel;
    private boolean isExpanded = false;
    private double expandedWidth = SIDEBAR_WIDTH;

    public SideBarManager() {
        this.buttonBar = new VBox();
        this.contentPane = new StackPane();
        this.toggleGroup = new ToggleGroup();
        this.panels = new HashMap<>();
        this.buttons = new HashMap<>();
        
        initializeLayout();
    }

    private void initializeLayout() {
        buttonBar.setSpacing(2);
        buttonBar.setPadding(new javafx.geometry.Insets(4, 2, 4, 2));
        buttonBar.getStyleClass().add("sidebar-button-bar");
        buttonBar.setPrefWidth(40);
        buttonBar.setMinWidth(40);
        buttonBar.setMaxWidth(40);
        
        contentPane.setPrefWidth(COLLAPSED_WIDTH);
        contentPane.setMinWidth(0);
        contentPane.getStyleClass().add("sidebar-content-pane");
        contentPane.setVisible(false);
        contentPane.setManaged(false);
        HBox.setHgrow(contentPane, Priority.NEVER);
        
        getChildren().addAll(buttonBar, contentPane);
        getStyleClass().add("side-bar-manager");
        setPrefWidth(40);
        setMinWidth(40);
    }

    public void registerPanel(SideBarPanel panel) {
        Objects.requireNonNull(panel, "panel cannot be null");
        
        String panelId = panel.getId();
        panels.put(panelId, panel);
        
        ToggleButton button = createToggleButton(panel);
        buttons.put(panelId, button);
        buttonBar.getChildren().add(button);
        
        contentPane.getChildren().add(panel.getContent());
        panel.getContent().setVisible(false);
        panel.getContent().setManaged(false);
    }

    public void registerPanel(SideBarPanel panel, int index) {
        Objects.requireNonNull(panel, "panel cannot be null");
        
        String panelId = panel.getId();
        panels.put(panelId, panel);
        
        ToggleButton button = createToggleButton(panel);
        buttons.put(panelId, button);
        buttonBar.getChildren().add(index, button);
        
        contentPane.getChildren().add(panel.getContent());
        panel.getContent().setVisible(false);
        panel.getContent().setManaged(false);
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
        
        button.selectedProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal) {
                showPanel(panel.getId());
            } else if (activePanel == panel) {
                collapsePanel();
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
        SideBarPanel panel = panels.get(panelId);
        if (panel == null) {
            return;
        }
        
        if (activePanel != null && activePanel != panel) {
            activePanel.getContent().setVisible(false);
            activePanel.getContent().setManaged(false);
            activePanel.onDeactivate();
        }
        
        activePanel = panel;
        panel.getContent().setVisible(true);
        panel.getContent().setManaged(true);
        panel.onActivate();
        
        if (!isExpanded) {
            expandPanel();
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
        if (isExpanded) {
            return;
        }
        
        isExpanded = true;
        animateWidth(COLLAPSED_WIDTH, expandedWidth);
    }

    public void collapsePanel() {
        if (!isExpanded) {
            return;
        }
        
        isExpanded = false;
        animateWidth(expandedWidth, COLLAPSED_WIDTH);
        
        if (activePanel != null) {
            activePanel.getContent().setVisible(false);
            activePanel.getContent().setManaged(false);
            activePanel.onDeactivate();
            activePanel = null;
        }
        
        toggleGroup.selectToggle(null);
    }

    private void animateWidth(double fromWidth, double toWidth) {
        TranslateTransition transition = new TranslateTransition(ANIMATION_DURATION, contentPane);
        transition.setFromX(fromWidth > toWidth ? 0 : -expandedWidth);
        transition.setToX(fromWidth > toWidth ? -expandedWidth : 0);
        transition.play();
        
        Timeline timeline = new Timeline();
        javafx.animation.KeyValue keyValue = new javafx.animation.KeyValue(
                contentPane.prefWidthProperty(), toWidth);
        javafx.animation.KeyFrame keyFrame = new javafx.animation.KeyFrame(ANIMATION_DURATION, keyValue);
        timeline.getKeyFrames().add(keyFrame);
        timeline.play();
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
        this.expandedWidth = width;
        if (isExpanded) {
            contentPane.setPrefWidth(width);
        }
    }

    public double getExpandedWidth() {
        return expandedWidth;
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
