package cn.cycle.echart.plugin;

import cn.cycle.echart.core.ServiceLocator;

import java.io.File;
import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 插件管理器。
 * 
 * <p>负责插件的加载、卸载、启动和停止。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PluginManager implements PluginContext {

    private final ServiceLocator serviceLocator;
    private final Map<String, Plugin> plugins;
    private final Map<String, Object> properties;
    private final String dataDirectory;
    private final String configDirectory;
    private final List<PluginLifecycleListener> listeners;

    public PluginManager(ServiceLocator serviceLocator) {
        this.serviceLocator = serviceLocator;
        this.plugins = new ConcurrentHashMap<>();
        this.properties = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        
        String userHome = System.getProperty("user.home");
        this.dataDirectory = userHome + File.separator + ".echart" + File.separator + "plugins" + File.separator + "data";
        this.configDirectory = userHome + File.separator + ".echart" + File.separator + "plugins" + File.separator + "config";
        
        initializeDirectories();
    }

    private void initializeDirectories() {
        new File(dataDirectory).mkdirs();
        new File(configDirectory).mkdirs();
    }

    public void registerPlugin(Plugin plugin) throws PluginException {
        Objects.requireNonNull(plugin, "plugin cannot be null");
        
        if (plugins.containsKey(plugin.getId())) {
            throw new PluginException(plugin.getId(), "Plugin already registered: " + plugin.getId());
        }
        
        checkDependencies(plugin);
        
        plugins.put(plugin.getId(), plugin);
        
        log(LogLevel.INFO, "Plugin registered: " + plugin.getId());
    }

    public void unregisterPlugin(String pluginId) throws PluginException {
        Objects.requireNonNull(pluginId, "pluginId cannot be null");
        
        Plugin plugin = plugins.get(pluginId);
        if (plugin == null) {
            throw new PluginException(pluginId, "Plugin not found: " + pluginId);
        }
        
        if (plugin.getState().isRunning()) {
            plugin.stop();
        }
        
        if (plugin.getState() != PluginState.DESTROYED) {
            plugin.destroy();
        }
        
        plugins.remove(pluginId);
        
        log(LogLevel.INFO, "Plugin unregistered: " + pluginId);
    }

    private void checkDependencies(Plugin plugin) throws PluginException {
        String[] dependencies = plugin.getDependencies();
        if (dependencies == null || dependencies.length == 0) {
            return;
        }
        
        for (String depId : dependencies) {
            if (!plugins.containsKey(depId)) {
                throw new PluginException(plugin.getId(), 
                        "Missing dependency: " + depId + " for plugin: " + plugin.getId());
            }
        }
    }

    public void initializePlugin(String pluginId) throws PluginException {
        Plugin plugin = getPlugin(pluginId);
        if (plugin == null) {
            throw new PluginException(pluginId, "Plugin not found: " + pluginId);
        }
        
        notifyLifecycleEvent(plugin, PluginLifecycleEvent.BEFORE_INITIALIZE);
        
        plugin.initialize(this);
        
        notifyLifecycleEvent(plugin, PluginLifecycleEvent.AFTER_INITIALIZE);
        
        log(LogLevel.INFO, "Plugin initialized: " + pluginId);
    }

    public void startPlugin(String pluginId) throws PluginException {
        Plugin plugin = getPlugin(pluginId);
        if (plugin == null) {
            throw new PluginException(pluginId, "Plugin not found: " + pluginId);
        }
        
        if (!plugin.getState().canStart()) {
            throw new PluginException(pluginId, 
                    "Cannot start plugin in state: " + plugin.getState());
        }
        
        notifyLifecycleEvent(plugin, PluginLifecycleEvent.BEFORE_START);
        
        plugin.start();
        
        notifyLifecycleEvent(plugin, PluginLifecycleEvent.AFTER_START);
        
        log(LogLevel.INFO, "Plugin started: " + pluginId);
    }

    public void stopPlugin(String pluginId) throws PluginException {
        Plugin plugin = getPlugin(pluginId);
        if (plugin == null) {
            throw new PluginException(pluginId, "Plugin not found: " + pluginId);
        }
        
        if (!plugin.getState().canStop()) {
            throw new PluginException(pluginId, 
                    "Cannot stop plugin in state: " + plugin.getState());
        }
        
        notifyLifecycleEvent(plugin, PluginLifecycleEvent.BEFORE_STOP);
        
        plugin.stop();
        
        notifyLifecycleEvent(plugin, PluginLifecycleEvent.AFTER_STOP);
        
        log(LogLevel.INFO, "Plugin stopped: " + pluginId);
    }

    public Plugin getPlugin(String pluginId) {
        return plugins.get(pluginId);
    }

    public List<Plugin> getPlugins() {
        return new ArrayList<>(plugins.values());
    }

    public List<Plugin> getPluginsByState(PluginState state) {
        List<Plugin> result = new ArrayList<>();
        for (Plugin plugin : plugins.values()) {
            if (plugin.getState() == state) {
                result.add(plugin);
            }
        }
        return result;
    }

    public boolean hasPlugin(String pluginId) {
        return plugins.containsKey(pluginId);
    }

    public int getPluginCount() {
        return plugins.size();
    }

    public void startAll() throws PluginException {
        for (Plugin plugin : plugins.values()) {
            if (plugin.getState().canStart()) {
                startPlugin(plugin.getId());
            }
        }
    }

    public void stopAll() throws PluginException {
        for (Plugin plugin : plugins.values()) {
            if (plugin.getState().canStop()) {
                stopPlugin(plugin.getId());
            }
        }
    }

    public void addLifecycleListener(PluginLifecycleListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.add(listener);
    }

    public void removeLifecycleListener(PluginLifecycleListener listener) {
        listeners.remove(listener);
    }

    private void notifyLifecycleEvent(Plugin plugin, PluginLifecycleEvent event) {
        for (PluginLifecycleListener listener : listeners) {
            try {
                listener.onPluginLifecycle(plugin, event);
            } catch (Exception e) {
                log(LogLevel.ERROR, "Error notifying lifecycle listener: " + e.getMessage(), e);
            }
        }
    }

    @Override
    public ServiceLocator getServiceLocator() {
        return serviceLocator;
    }

    @Override
    public <T> T getService(Class<T> serviceType) {
        return serviceLocator.getService(serviceType);
    }

    @Override
    public <T> void registerService(Class<T> serviceType, T service) {
        ServiceLocator.register(serviceType, service);
    }

    @Override
    public Object getProperty(String key) {
        return properties.get(key);
    }

    @Override
    public Object getProperty(String key, Object defaultValue) {
        return properties.getOrDefault(key, defaultValue);
    }

    @Override
    public void setProperty(String key, Object value) {
        properties.put(key, value);
    }

    @Override
    public Map<String, Object> getProperties() {
        return new HashMap<>(properties);
    }

    @Override
    public String getDataDirectory() {
        return dataDirectory;
    }

    @Override
    public String getConfigDirectory() {
        return configDirectory;
    }

    @Override
    public PluginManager getPluginManager() {
        return this;
    }

    @Override
    public void log(LogLevel level, String message) {
        System.out.println("[" + level + "] " + message);
    }

    @Override
    public void log(LogLevel level, String message, Throwable throwable) {
        System.out.println("[" + level + "] " + message);
        throwable.printStackTrace(System.out);
    }

    public enum PluginLifecycleEvent {
        BEFORE_INITIALIZE,
        AFTER_INITIALIZE,
        BEFORE_START,
        AFTER_START,
        BEFORE_STOP,
        AFTER_STOP,
        BEFORE_DESTROY,
        AFTER_DESTROY
    }

    public interface PluginLifecycleListener {
        void onPluginLifecycle(Plugin plugin, PluginLifecycleEvent event);
    }
}
