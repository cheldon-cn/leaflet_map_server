package cn.cycle.echart.app;

import javafx.animation.Animation;
import javafx.animation.KeyFrame;
import javafx.animation.KeyValue;
import javafx.animation.Timeline;
import javafx.animation.TranslateTransition;
import javafx.scene.Node;
import javafx.scene.layout.Pane;
import javafx.util.Duration;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * 面板折叠管理器。
 * 
 * <p>管理侧边栏和面板的折叠/展开动画。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PanelCollapseManager {

    private static final Duration DEFAULT_ANIMATION_DURATION = Duration.millis(200);

    private final Map<String, PanelState> panelStates;
    private final Map<String, PanelConfig> panelConfigs;
    private final List<PanelStateChangeListener> listeners;
    
    private Duration animationDuration;
    private boolean animationEnabled;

    public PanelCollapseManager() {
        this.panelStates = new HashMap<>();
        this.panelConfigs = new HashMap<>();
        this.listeners = new ArrayList<>();
        this.animationDuration = DEFAULT_ANIMATION_DURATION;
        this.animationEnabled = true;
    }

    public void registerPanel(String panelId, Node panel, double expandedWidth) {
        Objects.requireNonNull(panelId, "panelId cannot be null");
        Objects.requireNonNull(panel, "panel cannot be null");
        
        PanelConfig config = new PanelConfig(panel, expandedWidth);
        panelConfigs.put(panelId, config);
        panelStates.put(panelId, PanelState.EXPANDED);
    }

    public void togglePanel(String panelId) {
        Objects.requireNonNull(panelId, "panelId cannot be null");
        
        PanelState currentState = panelStates.get(panelId);
        if (currentState == null) {
            return;
        }
        
        if (currentState == PanelState.EXPANDED) {
            collapsePanel(panelId);
        } else {
            expandPanel(panelId);
        }
    }

    public void collapsePanel(String panelId) {
        Objects.requireNonNull(panelId, "panelId cannot be null");
        
        PanelConfig config = panelConfigs.get(panelId);
        if (config == null) {
            return;
        }
        
        PanelState oldState = panelStates.get(panelId);
        if (oldState == PanelState.COLLAPSED) {
            return;
        }
        
        Node panel = config.getPanel();
        
        if (animationEnabled) {
            animateCollapse(panel, config.getExpandedWidth());
        } else {
            panel.setVisible(false);
            panel.setManaged(false);
            if (panel instanceof Pane) {
                ((Pane) panel).setPrefWidth(0);
            }
        }
        
        panelStates.put(panelId, PanelState.COLLAPSED);
        notifyListeners(panelId, oldState, PanelState.COLLAPSED);
    }

    public void expandPanel(String panelId) {
        Objects.requireNonNull(panelId, "panelId cannot be null");
        
        PanelConfig config = panelConfigs.get(panelId);
        if (config == null) {
            return;
        }
        
        PanelState oldState = panelStates.get(panelId);
        if (oldState == PanelState.EXPANDED) {
            return;
        }
        
        Node panel = config.getPanel();
        
        if (animationEnabled) {
            animateExpand(panel, config.getExpandedWidth());
        } else {
            panel.setVisible(true);
            panel.setManaged(true);
            if (panel instanceof Pane) {
                ((Pane) panel).setPrefWidth(config.getExpandedWidth());
            }
        }
        
        panelStates.put(panelId, PanelState.EXPANDED);
        notifyListeners(panelId, oldState, PanelState.EXPANDED);
    }

    private void animateCollapse(Node panel, double expandedWidth) {
        panel.setVisible(true);
        panel.setManaged(true);
        
        if (panel instanceof Pane) {
            Pane pane = (Pane) panel;
            
            Timeline timeline = new Timeline();
            KeyValue keyValue = new KeyValue(pane.prefWidthProperty(), 0);
            KeyFrame keyFrame = new KeyFrame(animationDuration, keyValue);
            timeline.getKeyFrames().add(keyFrame);
            
            timeline.setOnFinished(event -> {
                panel.setVisible(false);
                panel.setManaged(false);
            });
            
            timeline.play();
        } else {
            TranslateTransition transition = new TranslateTransition(animationDuration, panel);
            transition.setFromX(0);
            transition.setToX(-expandedWidth);
            transition.setOnFinished(event -> {
                panel.setVisible(false);
                panel.setManaged(false);
            });
            transition.play();
        }
    }

    private void animateExpand(Node panel, double expandedWidth) {
        panel.setVisible(true);
        panel.setManaged(true);
        
        if (panel instanceof Pane) {
            Pane pane = (Pane) panel;
            pane.setPrefWidth(0);
            
            Timeline timeline = new Timeline();
            KeyValue keyValue = new KeyValue(pane.prefWidthProperty(), expandedWidth);
            KeyFrame keyFrame = new KeyFrame(animationDuration, keyValue);
            timeline.getKeyFrames().add(keyFrame);
            
            timeline.play();
        } else {
            panel.setTranslateX(-expandedWidth);
            
            TranslateTransition transition = new TranslateTransition(animationDuration, panel);
            transition.setFromX(-expandedWidth);
            transition.setToX(0);
            transition.play();
        }
    }

    public void collapseAll() {
        for (String panelId : panelConfigs.keySet()) {
            collapsePanel(panelId);
        }
    }

    public void expandAll() {
        for (String panelId : panelConfigs.keySet()) {
            expandPanel(panelId);
        }
    }

    public PanelState getPanelState(String panelId) {
        return panelStates.get(panelId);
    }

    public boolean isExpanded(String panelId) {
        PanelState state = panelStates.get(panelId);
        return state == PanelState.EXPANDED;
    }

    public boolean isCollapsed(String panelId) {
        PanelState state = panelStates.get(panelId);
        return state == PanelState.COLLAPSED;
    }

    public void setExpandedWidth(String panelId, double width) {
        PanelConfig config = panelConfigs.get(panelId);
        if (config != null) {
            config.setExpandedWidth(width);
        }
    }

    public double getExpandedWidth(String panelId) {
        PanelConfig config = panelConfigs.get(panelId);
        return config != null ? config.getExpandedWidth() : 0;
    }

    public void setAnimationDuration(Duration duration) {
        Objects.requireNonNull(duration, "duration cannot be null");
        this.animationDuration = duration;
    }

    public Duration getAnimationDuration() {
        return animationDuration;
    }

    public void setAnimationEnabled(boolean enabled) {
        this.animationEnabled = enabled;
    }

    public boolean isAnimationEnabled() {
        return animationEnabled;
    }

    public void addPanelStateChangeListener(PanelStateChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.add(listener);
    }

    public void removePanelStateChangeListener(PanelStateChangeListener listener) {
        listeners.remove(listener);
    }

    private void notifyListeners(String panelId, PanelState oldState, PanelState newState) {
        for (PanelStateChangeListener listener : listeners) {
            listener.onPanelStateChanged(panelId, oldState, newState);
        }
    }

    public void unregisterPanel(String panelId) {
        panelConfigs.remove(panelId);
        panelStates.remove(panelId);
    }

    public void clear() {
        panelConfigs.clear();
        panelStates.clear();
    }

    public enum PanelState {
        EXPANDED,
        COLLAPSED
    }

    public interface PanelStateChangeListener {
        void onPanelStateChanged(String panelId, PanelState oldState, PanelState newState);
    }

    private static class PanelConfig {
        private final Node panel;
        private double expandedWidth;

        public PanelConfig(Node panel, double expandedWidth) {
            this.panel = panel;
            this.expandedWidth = expandedWidth;
        }

        public Node getPanel() {
            return panel;
        }

        public double getExpandedWidth() {
            return expandedWidth;
        }

        public void setExpandedWidth(double expandedWidth) {
            this.expandedWidth = expandedWidth;
        }
    }
}
