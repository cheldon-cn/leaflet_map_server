package cn.cycle.echart.plugin;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 菜单扩展点。
 * 
 * <p>允许插件向应用程序菜单添加自定义菜单项。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class MenuExtensionPoint implements ExtensionPoint<MenuExtensionPoint.MenuItem> {

    public static final String ID = "echart.menu";
    
    private final Map<String, List<MenuItem>> extensions;
    private final List<MenuItem> allItems;

    public MenuExtensionPoint() {
        this.extensions = new ConcurrentHashMap<>();
        this.allItems = new ArrayList<>();
    }

    @Override
    public String getId() {
        return ID;
    }

    @Override
    public String getName() {
        return "菜单扩展点";
    }

    @Override
    public String getDescription() {
        return "允许插件向应用程序菜单添加自定义菜单项";
    }

    @Override
    public Class<MenuItem> getExtensionType() {
        return MenuItem.class;
    }

    @Override
    public void registerExtension(MenuItem extension, String pluginId) {
        extensions.computeIfAbsent(pluginId, k -> new ArrayList<>()).add(extension);
        allItems.add(extension);
    }

    @Override
    public void unregisterExtension(MenuItem extension) {
        allItems.remove(extension);
        for (List<MenuItem> items : extensions.values()) {
            items.remove(extension);
        }
    }

    @Override
    public List<MenuItem> getExtensions() {
        return new ArrayList<>(allItems);
    }

    @Override
    public List<MenuItem> getExtensions(String pluginId) {
        List<MenuItem> items = extensions.get(pluginId);
        return items != null ? new ArrayList<>(items) : new ArrayList<>();
    }

    public List<MenuItem> getItemsByParent(String parentId) {
        List<MenuItem> result = new ArrayList<>();
        for (MenuItem item : allItems) {
            if (parentId == null && item.getParentId() == null) {
                result.add(item);
            } else if (parentId != null && parentId.equals(item.getParentId())) {
                result.add(item);
            }
        }
        return result;
    }

    public static class MenuItem {
        private final String id;
        private String text;
        private String parentId;
        private int order;
        private String iconPath;
        private String shortcut;
        private boolean enabled;
        private boolean visible;
        private Runnable action;
        private Map<String, Object> data;

        public MenuItem(String id, String text) {
            this.id = id;
            this.text = text;
            this.order = 0;
            this.enabled = true;
            this.visible = true;
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

        public String getParentId() {
            return parentId;
        }

        public void setParentId(String parentId) {
            this.parentId = parentId;
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

        public String getShortcut() {
            return shortcut;
        }

        public void setShortcut(String shortcut) {
            this.shortcut = shortcut;
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
                action.run();
            }
        }
    }
}
