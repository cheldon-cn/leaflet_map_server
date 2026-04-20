package cn.cycle.echart.workspace;

import java.util.HashMap;
import java.util.Map;

/**
 * 面板数据模型。
 * 
 * <p>表示工作区中的一个面板。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class Panel {

    private final String id;
    private String name;
    private PanelManager.PanelType type;
    private String groupId;
    private boolean visible;
    private boolean pinned;
    private double width;
    private double height;
    private double minWidth;
    private double minHeight;
    private PanelManager.PanelPosition position;
    private int zIndex;
    private Map<String, Object> state;

    public Panel(String id, String name, PanelManager.PanelType type) {
        this.id = id;
        this.name = name;
        this.type = type;
        this.visible = true;
        this.pinned = false;
        this.width = 300;
        this.height = 400;
        this.minWidth = 100;
        this.minHeight = 100;
        this.position = PanelManager.PanelPosition.RIGHT;
        this.zIndex = 0;
        this.state = new HashMap<>();
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

    public PanelManager.PanelType getType() {
        return type;
    }

    public void setType(PanelManager.PanelType type) {
        this.type = type;
    }

    public String getGroupId() {
        return groupId;
    }

    public void setGroupId(String groupId) {
        this.groupId = groupId;
    }

    public boolean isVisible() {
        return visible;
    }

    public void setVisible(boolean visible) {
        this.visible = visible;
    }

    public boolean isPinned() {
        return pinned;
    }

    public void setPinned(boolean pinned) {
        this.pinned = pinned;
    }

    public double getWidth() {
        return width;
    }

    public void setWidth(double width) {
        this.width = Math.max(minWidth, width);
    }

    public double getHeight() {
        return height;
    }

    public void setHeight(double height) {
        this.height = Math.max(minHeight, height);
    }

    public double getMinWidth() {
        return minWidth;
    }

    public void setMinWidth(double minWidth) {
        this.minWidth = minWidth;
    }

    public double getMinHeight() {
        return minHeight;
    }

    public void setMinHeight(double minHeight) {
        this.minHeight = minHeight;
    }

    public PanelManager.PanelPosition getPosition() {
        return position;
    }

    public void setPosition(PanelManager.PanelPosition position) {
        this.position = position;
    }

    public int getZIndex() {
        return zIndex;
    }

    public void setZIndex(int zIndex) {
        this.zIndex = zIndex;
    }

    public Map<String, Object> getState() {
        return state;
    }

    public void setState(String key, Object value) {
        state.put(key, value);
    }

    public Object getState(String key) {
        return state.get(key);
    }

    @SuppressWarnings("unchecked")
    public <T> T getState(String key, Class<T> type) {
        Object value = state.get(key);
        if (value != null && type.isInstance(value)) {
            return (T) value;
        }
        return null;
    }

    public Map<String, Object> saveState() {
        Map<String, Object> savedState = new HashMap<>();
        savedState.put("id", id);
        savedState.put("name", name);
        savedState.put("visible", visible);
        savedState.put("pinned", pinned);
        savedState.put("width", width);
        savedState.put("height", height);
        savedState.put("position", position.name());
        savedState.put("zIndex", zIndex);
        savedState.put("customState", new HashMap<>(state));
        return savedState;
    }

    @SuppressWarnings("unchecked")
    public void restoreState(Map<String, Object> savedState) {
        if (savedState == null) return;
        
        if (savedState.containsKey("name")) {
            this.name = (String) savedState.get("name");
        }
        if (savedState.containsKey("visible")) {
            this.visible = (Boolean) savedState.get("visible");
        }
        if (savedState.containsKey("pinned")) {
            this.pinned = (Boolean) savedState.get("pinned");
        }
        if (savedState.containsKey("width")) {
            this.width = ((Number) savedState.get("width")).doubleValue();
        }
        if (savedState.containsKey("height")) {
            this.height = ((Number) savedState.get("height")).doubleValue();
        }
        if (savedState.containsKey("position")) {
            this.position = PanelManager.PanelPosition.valueOf((String) savedState.get("position"));
        }
        if (savedState.containsKey("zIndex")) {
            this.zIndex = ((Number) savedState.get("zIndex")).intValue();
        }
        if (savedState.containsKey("customState")) {
            this.state = new HashMap<>((Map<String, Object>) savedState.get("customState"));
        }
    }
}
