package cn.cycle.echart.workspace;

import cn.cycle.echart.ais.AISTarget;
import cn.cycle.echart.route.Route;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;

/**
 * 工作区数据模型。
 * 
 * <p>表示一个完整的工作区状态。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class Workspace {

    private String id;
    private String version;
    private String name;
    private String description;
    private LocalDateTime createdTime;
    private LocalDateTime modifiedTime;
    private String filePath;
    
    private WorkspaceConfig config;
    private final List<String> loadedCharts;
    private final List<Route> routes;
    private final List<String> alarmIds;
    private final List<AISTarget> aisTargets;
    private final Map<String, Object> properties;
    
    private boolean dirty;

    public Workspace() {
        this.id = UUID.randomUUID().toString();
        this.version = "1.0.0";
        this.name = "未命名工作区";
        this.description = "";
        this.createdTime = LocalDateTime.now();
        this.modifiedTime = LocalDateTime.now();
        this.filePath = null;
        this.config = new WorkspaceConfig();
        this.loadedCharts = new ArrayList<>();
        this.routes = new ArrayList<>();
        this.alarmIds = new ArrayList<>();
        this.aisTargets = new ArrayList<>();
        this.properties = new HashMap<>();
        this.dirty = false;
    }

    public Workspace(String name) {
        this();
        this.name = name != null ? name : "未命名工作区";
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id != null ? id : UUID.randomUUID().toString();
    }

    public String getVersion() {
        return version;
    }

    public void setVersion(String version) {
        this.version = version != null ? version : "1.0.0";
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name != null ? name : "未命名工作区";
        markDirty();
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description != null ? description : "";
        markDirty();
    }

    public LocalDateTime getCreatedTime() {
        return createdTime;
    }

    public void setCreatedTime(LocalDateTime createdTime) {
        this.createdTime = createdTime;
    }

    public LocalDateTime getModifiedTime() {
        return modifiedTime;
    }

    public void setModifiedTime(LocalDateTime modifiedTime) {
        this.modifiedTime = modifiedTime;
    }

    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public WorkspaceConfig getConfig() {
        return config;
    }

    public void setConfig(WorkspaceConfig config) {
        this.config = config != null ? config : new WorkspaceConfig();
        markDirty();
    }

    public List<String> getLoadedCharts() {
        return Collections.unmodifiableList(loadedCharts);
    }

    public void addChart(String chartPath) {
        Objects.requireNonNull(chartPath, "chartPath cannot be null");
        if (!loadedCharts.contains(chartPath)) {
            loadedCharts.add(chartPath);
            markDirty();
        }
    }

    public void removeChart(String chartPath) {
        if (loadedCharts.remove(chartPath)) {
            markDirty();
        }
    }

    public void clearCharts() {
        loadedCharts.clear();
        markDirty();
    }

    public List<Route> getRoutes() {
        return Collections.unmodifiableList(routes);
    }

    public void addRoute(Route route) {
        Objects.requireNonNull(route, "route cannot be null");
        routes.add(route);
        markDirty();
    }

    public void removeRoute(Route route) {
        if (routes.remove(route)) {
            markDirty();
        }
    }

    public void clearRoutes() {
        routes.clear();
        markDirty();
    }

    public List<String> getAlarmIds() {
        return Collections.unmodifiableList(alarmIds);
    }

    public void addAlarmId(String alarmId) {
        Objects.requireNonNull(alarmId, "alarmId cannot be null");
        if (!alarmIds.contains(alarmId)) {
            alarmIds.add(alarmId);
            markDirty();
        }
    }

    public void removeAlarmId(String alarmId) {
        if (alarmIds.remove(alarmId)) {
            markDirty();
        }
    }

    public void clearAlarmIds() {
        alarmIds.clear();
        markDirty();
    }

    public List<AISTarget> getAisTargets() {
        return Collections.unmodifiableList(aisTargets);
    }

    public void addAisTarget(AISTarget target) {
        Objects.requireNonNull(target, "target cannot be null");
        aisTargets.add(target);
        markDirty();
    }

    public void removeAisTarget(AISTarget target) {
        if (aisTargets.remove(target)) {
            markDirty();
        }
    }

    public void clearAisTargets() {
        aisTargets.clear();
        markDirty();
    }

    public Map<String, Object> getProperties() {
        return Collections.unmodifiableMap(properties);
    }

    public void setProperty(String key, Object value) {
        Objects.requireNonNull(key, "key cannot be null");
        properties.put(key, value);
        markDirty();
    }

    public Object getProperty(String key) {
        return properties.get(key);
    }

    @SuppressWarnings("unchecked")
    public <T> T getProperty(String key, Class<T> type) {
        Object value = properties.get(key);
        if (value != null && type.isInstance(value)) {
            return (T) value;
        }
        return null;
    }

    public void removeProperty(String key) {
        if (properties.remove(key) != null) {
            markDirty();
        }
    }

    public boolean isDirty() {
        return dirty;
    }

    public void markDirty() {
        this.dirty = true;
        this.modifiedTime = LocalDateTime.now();
    }

    public void markClean() {
        this.dirty = false;
    }

    public void setDirty(boolean dirty) {
        this.dirty = dirty;
        if (dirty) {
            this.modifiedTime = LocalDateTime.now();
        }
    }

    public void reset() {
        loadedCharts.clear();
        routes.clear();
        alarmIds.clear();
        aisTargets.clear();
        properties.clear();
        config = new WorkspaceConfig();
        dirty = false;
        modifiedTime = LocalDateTime.now();
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Workspace other = (Workspace) obj;
        return id.equals(other.id);
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    @Override
    public String toString() {
        return String.format("Workspace{id='%s', name='%s', charts=%d, routes=%d}", 
                id, name, loadedCharts.size(), routes.size());
    }
}
