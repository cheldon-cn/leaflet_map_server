package cn.cycle.echart.workspace;

import java.util.ArrayList;
import java.util.List;

/**
 * 面板组。
 * 
 * <p>将多个面板组合在一起进行统一管理。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PanelGroup {

    private final String id;
    private String name;
    private final List<String> panelIds;
    private PanelManager.PanelPosition position;
    private double width;
    private double height;
    private boolean collapsed;

    public PanelGroup(String id, String name) {
        this.id = id;
        this.name = name;
        this.panelIds = new ArrayList<>();
        this.position = PanelManager.PanelPosition.RIGHT;
        this.width = 300;
        this.height = 600;
        this.collapsed = false;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public List<String> getPanelIds() {
        return new ArrayList<>(panelIds);
    }

    public int getPanelCount() {
        return panelIds.size();
    }

    public void addPanel(String panelId) {
        if (!panelIds.contains(panelId)) {
            panelIds.add(panelId);
        }
    }

    public void removePanel(String panelId) {
        panelIds.remove(panelId);
    }

    public boolean containsPanel(String panelId) {
        return panelIds.contains(panelId);
    }

    public void movePanel(String panelId, int newIndex) {
        int oldIndex = panelIds.indexOf(panelId);
        if (oldIndex >= 0 && newIndex >= 0 && newIndex < panelIds.size()) {
            panelIds.remove(oldIndex);
            panelIds.add(newIndex, panelId);
        }
    }

    public PanelManager.PanelPosition getPosition() {
        return position;
    }

    public void setPosition(PanelManager.PanelPosition position) {
        this.position = position;
    }

    public double getWidth() {
        return width;
    }

    public void setWidth(double width) {
        this.width = width;
    }

    public double getHeight() {
        return height;
    }

    public void setHeight(double height) {
        this.height = height;
    }

    public boolean isCollapsed() {
        return collapsed;
    }

    public void setCollapsed(boolean collapsed) {
        this.collapsed = collapsed;
    }

    public void toggleCollapsed() {
        this.collapsed = !this.collapsed;
    }

    public void clear() {
        panelIds.clear();
    }
}
