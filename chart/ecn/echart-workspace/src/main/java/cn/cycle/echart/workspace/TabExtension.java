package cn.cycle.echart.workspace;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 标签页扩展。
 * 
 * <p>支持面板中的标签页扩展功能。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class TabExtension {

    private final String id;
    private String title;
    private String iconPath;
    private int priority;
    private boolean closeable;
    private boolean movable;
    private final List<TabAction> actions;
    private TabExtensionListener listener;

    public TabExtension(String id, String title) {
        this.id = Objects.requireNonNull(id, "id cannot be null");
        this.title = Objects.requireNonNull(title, "title cannot be null");
        this.iconPath = null;
        this.priority = 0;
        this.closeable = true;
        this.movable = true;
        this.actions = new ArrayList<>();
    }

    public String getId() {
        return id;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
        notifyUpdated();
    }

    public String getIconPath() {
        return iconPath;
    }

    public void setIconPath(String iconPath) {
        this.iconPath = iconPath;
        notifyUpdated();
    }

    public int getPriority() {
        return priority;
    }

    public void setPriority(int priority) {
        this.priority = priority;
    }

    public boolean isCloseable() {
        return closeable;
    }

    public void setCloseable(boolean closeable) {
        this.closeable = closeable;
    }

    public boolean isMovable() {
        return movable;
    }

    public void setMovable(boolean movable) {
        this.movable = movable;
    }

    public void addAction(TabAction action) {
        actions.add(action);
    }

    public void removeAction(TabAction action) {
        actions.remove(action);
    }

    public List<TabAction> getActions() {
        return new ArrayList<>(actions);
    }

    public void clearActions() {
        actions.clear();
    }

    public void setListener(TabExtensionListener listener) {
        this.listener = listener;
    }

    public TabExtensionListener getListener() {
        return listener;
    }

    public void onSelect() {
        if (listener != null) {
            listener.onTabSelected(this);
        }
    }

    public void onDeselect() {
        if (listener != null) {
            listener.onTabDeselected(this);
        }
    }

    public boolean onClose() {
        if (listener != null) {
            return listener.onTabCloseRequested(this);
        }
        return true;
    }

    public void onMove(int oldIndex, int newIndex) {
        if (listener != null) {
            listener.onTabMoved(this, oldIndex, newIndex);
        }
    }

    private void notifyUpdated() {
        if (listener != null) {
            listener.onTabUpdated(this);
        }
    }

    public static class TabAction {
        private final String id;
        private String name;
        private String iconPath;
        private boolean enabled;
        private Runnable action;

        public TabAction(String id, String name) {
            this.id = id;
            this.name = name;
            this.enabled = true;
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

        public Runnable getAction() {
            return action;
        }

        public void setAction(Runnable action) {
            this.action = action;
        }

        public void execute() {
            if (enabled && action != null) {
                action.run();
            }
        }
    }

    public interface TabExtensionListener {
        void onTabSelected(TabExtension tab);
        void onTabDeselected(TabExtension tab);
        boolean onTabCloseRequested(TabExtension tab);
        void onTabMoved(TabExtension tab, int oldIndex, int newIndex);
        void onTabUpdated(TabExtension tab);
    }
}
