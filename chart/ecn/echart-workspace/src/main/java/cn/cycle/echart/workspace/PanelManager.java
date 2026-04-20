package cn.cycle.echart.workspace;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 面板管理器。
 * 
 * <p>管理工作区中的面板布局和状态。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PanelManager {

    private final EventBus eventBus;
    private final Map<String, Panel> panels;
    private final Map<String, PanelGroup> groups;
    private final List<PanelManagerListener> listeners;
    private PanelLayout currentLayout;

    public PanelManager(EventBus eventBus) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.panels = new ConcurrentHashMap<>();
        this.groups = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        this.currentLayout = new PanelLayout("default");
    }

    public Panel registerPanel(String id, String name, PanelType type) {
        if (panels.containsKey(id)) {
            return panels.get(id);
        }

        Panel panel = new Panel(id, name, type);
        panels.put(id, panel);
        notifyPanelRegistered(panel);
        return panel;
    }

    public void unregisterPanel(String id) {
        Panel panel = panels.remove(id);
        if (panel != null) {
            for (PanelGroup group : groups.values()) {
                group.removePanel(id);
            }
            notifyPanelUnregistered(panel);
        }
    }

    public Panel getPanel(String id) {
        return panels.get(id);
    }

    public List<Panel> getAllPanels() {
        return new ArrayList<>(panels.values());
    }

    public List<Panel> getPanelsByType(PanelType type) {
        List<Panel> result = new ArrayList<>();
        for (Panel panel : panels.values()) {
            if (panel.getType() == type) {
                result.add(panel);
            }
        }
        return result;
    }

    public PanelGroup createGroup(String id, String name) {
        PanelGroup group = new PanelGroup(id, name);
        groups.put(id, group);
        notifyGroupCreated(group);
        return group;
    }

    public void removeGroup(String id) {
        PanelGroup group = groups.remove(id);
        if (group != null) {
            notifyGroupRemoved(group);
        }
    }

    public PanelGroup getGroup(String id) {
        return groups.get(id);
    }

    public List<PanelGroup> getAllGroups() {
        return new ArrayList<>(groups.values());
    }

    public void addPanelToGroup(String panelId, String groupId) {
        Panel panel = panels.get(panelId);
        PanelGroup group = groups.get(groupId);
        
        if (panel != null && group != null) {
            group.addPanel(panelId);
            panel.setGroupId(groupId);
            notifyPanelAddedToGroup(panel, group);
        }
    }

    public void removePanelFromGroup(String panelId, String groupId) {
        Panel panel = panels.get(panelId);
        PanelGroup group = groups.get(groupId);
        
        if (panel != null && group != null) {
            group.removePanel(panelId);
            panel.setGroupId(null);
            notifyPanelRemovedFromGroup(panel, group);
        }
    }

    public void showPanel(String id) {
        Panel panel = panels.get(id);
        if (panel != null && !panel.isVisible()) {
            panel.setVisible(true);
            notifyPanelVisibilityChanged(panel, true);
        }
    }

    public void hidePanel(String id) {
        Panel panel = panels.get(id);
        if (panel != null && panel.isVisible()) {
            panel.setVisible(false);
            notifyPanelVisibilityChanged(panel, false);
        }
    }

    public void togglePanel(String id) {
        Panel panel = panels.get(id);
        if (panel != null) {
            if (panel.isVisible()) {
                hidePanel(id);
            } else {
                showPanel(id);
            }
        }
    }

    public void setPanelPosition(String id, PanelPosition position) {
        Panel panel = panels.get(id);
        if (panel != null) {
            panel.setPosition(position);
            notifyPanelPositionChanged(panel, position);
        }
    }

    public void setPanelSize(String id, double width, double height) {
        Panel panel = panels.get(id);
        if (panel != null) {
            panel.setWidth(width);
            panel.setHeight(height);
            notifyPanelSizeChanged(panel, width, height);
        }
    }

    public PanelLayout getCurrentLayout() {
        return currentLayout;
    }

    public void applyLayout(PanelLayout layout) {
        PanelLayout oldLayout = this.currentLayout;
        this.currentLayout = layout;
        notifyLayoutChanged(oldLayout, layout);
    }

    public void saveCurrentLayout(String name) {
        PanelLayout layout = new PanelLayout(name);
        for (Panel panel : panels.values()) {
            layout.addPanelState(panel.getId(), panel.saveState());
        }
        notifyLayoutSaved(layout);
    }

    public void addListener(PanelManagerListener listener) {
        listeners.add(listener);
    }

    public void removeListener(PanelManagerListener listener) {
        listeners.remove(listener);
    }

    private void notifyPanelRegistered(Panel panel) {
        eventBus.publish(new AppEvent(this, AppEventType.PANEL_REGISTERED, panel));
        for (PanelManagerListener listener : listeners) {
            listener.onPanelRegistered(panel);
        }
    }

    private void notifyPanelUnregistered(Panel panel) {
        for (PanelManagerListener listener : listeners) {
            listener.onPanelUnregistered(panel);
        }
    }

    private void notifyGroupCreated(PanelGroup group) {
        for (PanelManagerListener listener : listeners) {
            listener.onGroupCreated(group);
        }
    }

    private void notifyGroupRemoved(PanelGroup group) {
        for (PanelManagerListener listener : listeners) {
            listener.onGroupRemoved(group);
        }
    }

    private void notifyPanelAddedToGroup(Panel panel, PanelGroup group) {
        for (PanelManagerListener listener : listeners) {
            listener.onPanelAddedToGroup(panel, group);
        }
    }

    private void notifyPanelRemovedFromGroup(Panel panel, PanelGroup group) {
        for (PanelManagerListener listener : listeners) {
            listener.onPanelRemovedFromGroup(panel, group);
        }
    }

    private void notifyPanelVisibilityChanged(Panel panel, boolean visible) {
        for (PanelManagerListener listener : listeners) {
            listener.onPanelVisibilityChanged(panel, visible);
        }
    }

    private void notifyPanelPositionChanged(Panel panel, PanelPosition position) {
        for (PanelManagerListener listener : listeners) {
            listener.onPanelPositionChanged(panel, position);
        }
    }

    private void notifyPanelSizeChanged(Panel panel, double width, double height) {
        for (PanelManagerListener listener : listeners) {
            listener.onPanelSizeChanged(panel, width, height);
        }
    }

    private void notifyLayoutChanged(PanelLayout oldLayout, PanelLayout newLayout) {
        for (PanelManagerListener listener : listeners) {
            listener.onLayoutChanged(oldLayout, newLayout);
        }
    }

    private void notifyLayoutSaved(PanelLayout layout) {
        for (PanelManagerListener listener : listeners) {
            listener.onLayoutSaved(layout);
        }
    }

    public enum PanelType {
        CHART,
        ALARM,
        AIS,
        ROUTE,
        TOOL,
        INFO,
        CUSTOM
    }

    public enum PanelPosition {
        LEFT,
        RIGHT,
        TOP,
        BOTTOM,
        CENTER,
        FLOATING
    }

    public interface PanelManagerListener {
        void onPanelRegistered(Panel panel);
        void onPanelUnregistered(Panel panel);
        void onGroupCreated(PanelGroup group);
        void onGroupRemoved(PanelGroup group);
        void onPanelAddedToGroup(Panel panel, PanelGroup group);
        void onPanelRemovedFromGroup(Panel panel, PanelGroup group);
        void onPanelVisibilityChanged(Panel panel, boolean visible);
        void onPanelPositionChanged(Panel panel, PanelPosition position);
        void onPanelSizeChanged(Panel panel, double width, double height);
        void onLayoutChanged(PanelLayout oldLayout, PanelLayout newLayout);
        void onLayoutSaved(PanelLayout layout);
    }
}
