package cn.cycle.echart.plugin;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 工具栏扩展点。
 * 
 * <p>允许插件向应用程序工具栏添加自定义工具按钮。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ToolbarExtensionPoint implements ExtensionPoint<ToolbarExtensionPoint.ToolbarItem> {

    public static final String ID = "echart.toolbar";
    
    private final Map<String, List<ToolbarItem>> extensions;
    private final List<ToolbarItem> allItems;

    public ToolbarExtensionPoint() {
        this.extensions = new ConcurrentHashMap<>();
        this.allItems = new ArrayList<>();
    }

    @Override
    public String getId() {
        return ID;
    }

    @Override
    public String getName() {
        return "工具栏扩展点";
    }

    @Override
    public String getDescription() {
        return "允许插件向应用程序工具栏添加自定义工具按钮";
    }

    @Override
    public Class<ToolbarItem> getExtensionType() {
        return ToolbarItem.class;
    }

    @Override
    public void registerExtension(ToolbarItem extension, String pluginId) {
        extensions.computeIfAbsent(pluginId, k -> new ArrayList<>()).add(extension);
        allItems.add(extension);
    }

    @Override
    public void unregisterExtension(ToolbarItem extension) {
        allItems.remove(extension);
        for (List<ToolbarItem> items : extensions.values()) {
            items.remove(extension);
        }
    }

    @Override
    public List<ToolbarItem> getExtensions() {
        return new ArrayList<>(allItems);
    }

    @Override
    public List<ToolbarItem> getExtensions(String pluginId) {
        List<ToolbarItem> items = extensions.get(pluginId);
        return items != null ? new ArrayList<>(items) : new ArrayList<>();
    }

    public List<ToolbarItem> getItemsByGroup(String groupId) {
        List<ToolbarItem> result = new ArrayList<>();
        for (ToolbarItem item : allItems) {
            if (groupId == null && item.getGroupId() == null) {
                result.add(item);
            } else if (groupId != null && groupId.equals(item.getGroupId())) {
                result.add(item);
            }
        }
        result.sort((a, b) -> Integer.compare(a.getOrder(), b.getOrder()));
        return result;
    }

    public static class ToolbarItem {
        private final String id;
        private String text;
        private String tooltip;
        private String groupId;
        private int order;
        private String iconPath;
        private boolean enabled;
        private boolean visible;
        private boolean toggleable;
        private boolean selected;
        private Runnable action;
        private Map<String, Object> data;

        public ToolbarItem(String id, String text) {
            this.id = id;
            this.text = text;
            this.order = 0;
            this.enabled = true;
            this.visible = true;
            this.toggleable = false;
            this.selected = false;
            this.data = new HashMap<>();
        }

        public String getId() {
            return id;
        }

        public String getText() {
            return text;
        }

        public void setText(String text) {
            this.text = text;
        }

        public String getTooltip() {
            return tooltip;
        }

        public void setTooltip(String tooltip) {
            this.tooltip = tooltip;
        }

        public String getGroupId() {
            return groupId;
        }

        public void setGroupId(String groupId) {
            this.groupId = groupId;
        }

        public int getOrder() {
            return order;
        }

        public void setOrder(int order) {
            this.order = order;
        }

        public String getIconPath() {
            return iconPath;
        }

        public void setIconPath(String iconPath) {
            this.iconPath = iconPath;
        }

        public boolean isEnabled() {
            return enabled;
        }

        public void setEnabled(boolean enabled) {
            this.enabled = enabled;
        }

        public boolean isVisible() {
            return visible;
        }

        public void setVisible(boolean visible) {
            this.visible = visible;
        }

        public boolean isToggleable() {
            return toggleable;
        }

        public void setToggleable(boolean toggleable) {
            this.toggleable = toggleable;
        }

        public boolean isSelected() {
            return selected;
        }

        public void setSelected(boolean selected) {
            this.selected = selected;
        }

        public Runnable getAction() {
            return action;
        }

        public void setAction(Runnable action) {
            this.action = action;
        }

        public Map<String, Object> getData() {
            return data;
        }

        public void setData(String key, Object value) {
            data.put(key, value);
        }

        public Object getData(String key) {
            return data.get(key);
        }

        public void execute() {
            if (enabled && action != null) {
                if (toggleable) {
                    selected = !selected;
                }
                action.run();
            }
        }
    }
}
