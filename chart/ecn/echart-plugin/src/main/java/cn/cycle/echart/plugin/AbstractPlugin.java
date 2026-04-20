package cn.cycle.echart.plugin;

import java.net.URLClassLoader;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 抽象插件基类。
 * 
 * <p>提供插件的基本实现。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public abstract class AbstractPlugin implements Plugin {

    private final String id;
    private final String name;
    private final String version;
    private final String description;
    private final String author;
    private final String[] dependencies;
    
    protected PluginContext context;
    protected PluginState state;
    protected Map<String, Object> configuration;
    protected URLClassLoader classLoader;

    protected AbstractPlugin(String id, String name, String version, 
                            String description, String author, String[] dependencies) {
        this.id = Objects.requireNonNull(id, "id cannot be null");
        this.name = name != null ? name : id;
        this.version = version != null ? version : "1.0.0";
        this.description = description != null ? description : "";
        this.author = author != null ? author : "Unknown";
        this.dependencies = dependencies != null ? dependencies : new String[0];
        this.state = PluginState.CREATED;
        this.configuration = new HashMap<>();
    }

    @Override
    public String getId() {
        return id;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public String getVersion() {
        return version;
    }

    @Override
    public String getDescription() {
        return description;
    }

    @Override
    public String getAuthor() {
        return author;
    }

    @Override
    public String[] getDependencies() {
        return dependencies.clone();
    }

    @Override
    public Map<String, Object> getConfiguration() {
        return Collections.unmodifiableMap(configuration);
    }

    @Override
    public PluginState getState() {
        return state;
    }

    @Override
    public void initialize(PluginContext context) throws PluginException {
        Objects.requireNonNull(context, "context cannot be null");
        
        if (state != PluginState.CREATED) {
            throw new PluginException(id, "Plugin already initialized");
        }
        
        this.state = PluginState.INITIALIZING;
        this.context = context;
        
        try {
            doInitialize();
            this.state = PluginState.INITIALIZED;
        } catch (Exception e) {
            this.state = PluginState.ERROR;
            throw new PluginException(id, "Failed to initialize plugin", e);
        }
    }

    @Override
    public void start() throws PluginException {
        if (!state.canStart()) {
            throw new PluginException(id, "Cannot start plugin in state: " + state);
        }
        
        this.state = PluginState.STARTING;
        
        try {
            doStart();
            this.state = PluginState.STARTED;
        } catch (Exception e) {
            this.state = PluginState.ERROR;
            throw new PluginException(id, "Failed to start plugin", e);
        }
    }

    @Override
    public void stop() throws PluginException {
        if (!state.canStop()) {
            throw new PluginException(id, "Cannot stop plugin in state: " + state);
        }
        
        this.state = PluginState.STOPPING;
        
        try {
            doStop();
            this.state = PluginState.STOPPED;
        } catch (Exception e) {
            this.state = PluginState.ERROR;
            throw new PluginException(id, "Failed to stop plugin", e);
        }
    }

    @Override
    public void destroy() {
        if (state == PluginState.DESTROYED) {
            return;
        }
        
        try {
            doDestroy();
        } finally {
            this.state = PluginState.DESTROYED;
            this.context = null;
        }
    }

    protected void doInitialize() throws PluginException {
    }

    protected void doStart() throws PluginException {
    }

    protected void doStop() throws PluginException {
    }

    protected void doDestroy() {
    }

    protected void setConfiguration(String key, Object value) {
        Objects.requireNonNull(key, "key cannot be null");
        configuration.put(key, value);
    }

    protected Object getConfiguration(String key) {
        return configuration.get(key);
    }

    public URLClassLoader getClassLoader() {
        return classLoader;
    }

    public void setClassLoader(URLClassLoader classLoader) {
        this.classLoader = classLoader;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) {
            return true;
        }
        if (obj == null || getClass() != obj.getClass()) {
            return false;
        }
        Plugin other = (Plugin) obj;
        return id.equals(other.getId());
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    @Override
    public String toString() {
        return String.format("Plugin[id=%s, name=%s, version=%s, state=%s]",
                id, name, version, state);
    }
}
