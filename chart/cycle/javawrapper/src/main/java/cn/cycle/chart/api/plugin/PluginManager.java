package cn.cycle.chart.api.plugin;

import cn.cycle.chart.api.core.ChartViewer;

import java.util.*;

public class PluginManager {

    private final ChartViewer chartViewer;
    private final Map<String, ChartPlugin> plugins;
    private final List<PluginListener> listeners;

    public PluginManager(ChartViewer chartViewer) {
        this.chartViewer = chartViewer;
        this.plugins = new LinkedHashMap<>();
        this.listeners = new ArrayList<>();
    }

    public void registerPlugin(ChartPlugin plugin) {
        if (plugin == null) {
            return;
        }
        String name = plugin.getName();
        if (plugins.containsKey(name)) {
            throw new IllegalStateException("Plugin already registered: " + name);
        }
        plugins.put(name, plugin);
        plugin.initialize(chartViewer);
        firePluginRegistered(plugin);
    }

    public void unregisterPlugin(String name) {
        ChartPlugin plugin = plugins.remove(name);
        if (plugin != null) {
            plugin.shutdown();
            firePluginUnregistered(plugin);
        }
    }

    public ChartPlugin getPlugin(String name) {
        return plugins.get(name);
    }

    public Collection<ChartPlugin> getPlugins() {
        return Collections.unmodifiableCollection(plugins.values());
    }

    public List<ChartPlugin> getEnabledPlugins() {
        List<ChartPlugin> enabled = new ArrayList<>();
        for (ChartPlugin plugin : plugins.values()) {
            if (plugin.isEnabled()) {
                enabled.add(plugin);
            }
        }
        return enabled;
    }

    public void enablePlugin(String name) {
        ChartPlugin plugin = plugins.get(name);
        if (plugin != null) {
            plugin.setEnabled(true);
            firePluginStateChanged(plugin);
        }
    }

    public void disablePlugin(String name) {
        ChartPlugin plugin = plugins.get(name);
        if (plugin != null) {
            plugin.setEnabled(false);
            firePluginStateChanged(plugin);
        }
    }

    public void shutdownAll() {
        for (ChartPlugin plugin : plugins.values()) {
            try {
                plugin.shutdown();
            } catch (Exception e) {
                System.err.println("Error shutting down plugin: " + plugin.getName());
            }
        }
        plugins.clear();
    }

    public void addListener(PluginListener listener) {
        if (listener != null && !listeners.contains(listener)) {
            listeners.add(listener);
        }
    }

    public void removeListener(PluginListener listener) {
        listeners.remove(listener);
    }

    private void firePluginRegistered(ChartPlugin plugin) {
        for (PluginListener listener : listeners) {
            listener.onPluginRegistered(plugin);
        }
    }

    private void firePluginUnregistered(ChartPlugin plugin) {
        for (PluginListener listener : listeners) {
            listener.onPluginUnregistered(plugin);
        }
    }

    private void firePluginStateChanged(ChartPlugin plugin) {
        for (PluginListener listener : listeners) {
            listener.onPluginStateChanged(plugin);
        }
    }

    public interface PluginListener {
        void onPluginRegistered(ChartPlugin plugin);
        void onPluginUnregistered(ChartPlugin plugin);
        void onPluginStateChanged(ChartPlugin plugin);
    }
}
