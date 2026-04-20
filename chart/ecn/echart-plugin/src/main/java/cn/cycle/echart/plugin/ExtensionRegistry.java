package cn.cycle.echart.plugin;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 扩展注册表。
 * 
 * <p>管理所有扩展点的注册和扩展的查找。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ExtensionRegistry {

    private static ExtensionRegistry instance;
    
    private final Map<String, ExtensionPoint<?>> extensionPoints;
    private final Map<String, List<ExtensionRegistration>> extensionsByPlugin;
    private final List<RegistryListener> listeners;

    private ExtensionRegistry() {
        this.extensionPoints = new ConcurrentHashMap<>();
        this.extensionsByPlugin = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
    }

    public static synchronized ExtensionRegistry getInstance() {
        if (instance == null) {
            instance = new ExtensionRegistry();
        }
        return instance;
    }

    public <T> void registerExtensionPoint(ExtensionPoint<T> extensionPoint) {
        Objects.requireNonNull(extensionPoint, "extensionPoint cannot be null");
        
        String id = extensionPoint.getId();
        if (extensionPoints.containsKey(id)) {
            throw new IllegalArgumentException("Extension point already registered: " + id);
        }
        
        extensionPoints.put(id, extensionPoint);
        notifyExtensionPointRegistered(extensionPoint);
    }

    public <T> void unregisterExtensionPoint(String extensionPointId) {
        ExtensionPoint<?> point = extensionPoints.remove(extensionPointId);
        if (point != null) {
            notifyExtensionPointUnregistered(point);
        }
    }

    @SuppressWarnings("unchecked")
    public <T> ExtensionPoint<T> getExtensionPoint(String extensionPointId) {
        return (ExtensionPoint<T>) extensionPoints.get(extensionPointId);
    }

    public List<ExtensionPoint<?>> getAllExtensionPoints() {
        return new ArrayList<>(extensionPoints.values());
    }

    public boolean hasExtensionPoint(String extensionPointId) {
        return extensionPoints.containsKey(extensionPointId);
    }

    public <T> void registerExtension(String extensionPointId, T extension, String pluginId) {
        Objects.requireNonNull(extension, "extension cannot be null");
        Objects.requireNonNull(pluginId, "pluginId cannot be null");

        @SuppressWarnings("unchecked")
        ExtensionPoint<T> point = (ExtensionPoint<T>) extensionPoints.get(extensionPointId);
        if (point == null) {
            throw new IllegalArgumentException("Extension point not found: " + extensionPointId);
        }

        point.registerExtension(extension, pluginId);

        ExtensionRegistration registration = new ExtensionRegistration(
                extensionPointId, extension, pluginId);
        
        extensionsByPlugin.computeIfAbsent(pluginId, k -> new ArrayList<>())
                .add(registration);

        notifyExtensionRegistered(extensionPointId, extension, pluginId);
    }

    public <T> void unregisterExtension(String extensionPointId, T extension) {
        @SuppressWarnings("unchecked")
        ExtensionPoint<T> point = (ExtensionPoint<T>) extensionPoints.get(extensionPointId);
        if (point != null) {
            point.unregisterExtension(extension);
            notifyExtensionUnregistered(extensionPointId, extension);
        }
    }

    public void unregisterAllExtensions(String pluginId) {
        List<ExtensionRegistration> registrations = extensionsByPlugin.remove(pluginId);
        if (registrations != null) {
            for (ExtensionRegistration reg : registrations) {
                unregisterExtension(reg.getExtensionPointId(), reg.getExtension());
            }
        }
    }

    @SuppressWarnings("unchecked")
    public <T> List<T> getExtensions(String extensionPointId) {
        ExtensionPoint<T> point = (ExtensionPoint<T>) extensionPoints.get(extensionPointId);
        if (point != null) {
            return point.getExtensions();
        }
        return new ArrayList<>();
    }

    @SuppressWarnings("unchecked")
    public <T> List<T> getExtensions(String extensionPointId, String pluginId) {
        ExtensionPoint<T> point = (ExtensionPoint<T>) extensionPoints.get(extensionPointId);
        if (point != null) {
            return point.getExtensions(pluginId);
        }
        return new ArrayList<>();
    }

    public List<ExtensionRegistration> getPluginExtensions(String pluginId) {
        List<ExtensionRegistration> registrations = extensionsByPlugin.get(pluginId);
        return registrations != null ? new ArrayList<>(registrations) : new ArrayList<>();
    }

    public void clear() {
        extensionPoints.clear();
        extensionsByPlugin.clear();
    }

    public void addListener(RegistryListener listener) {
        listeners.add(listener);
    }

    public void removeListener(RegistryListener listener) {
        listeners.remove(listener);
    }

    private void notifyExtensionPointRegistered(ExtensionPoint<?> point) {
        for (RegistryListener listener : listeners) {
            listener.onExtensionPointRegistered(point);
        }
    }

    private void notifyExtensionPointUnregistered(ExtensionPoint<?> point) {
        for (RegistryListener listener : listeners) {
            listener.onExtensionPointUnregistered(point);
        }
    }

    private void notifyExtensionRegistered(String pointId, Object extension, String pluginId) {
        for (RegistryListener listener : listeners) {
            listener.onExtensionRegistered(pointId, extension, pluginId);
        }
    }

    private void notifyExtensionUnregistered(String pointId, Object extension) {
        for (RegistryListener listener : listeners) {
            listener.onExtensionUnregistered(pointId, extension);
        }
    }

    public interface RegistryListener {
        void onExtensionPointRegistered(ExtensionPoint<?> point);
        void onExtensionPointUnregistered(ExtensionPoint<?> point);
        void onExtensionRegistered(String pointId, Object extension, String pluginId);
        void onExtensionUnregistered(String pointId, Object extension);
    }

    public static class ExtensionRegistration {
        private final String extensionPointId;
        private final Object extension;
        private final String pluginId;

        public ExtensionRegistration(String extensionPointId, Object extension, String pluginId) {
            this.extensionPointId = extensionPointId;
            this.extension = extension;
            this.pluginId = pluginId;
        }

        public String getExtensionPointId() {
            return extensionPointId;
        }

        public Object getExtension() {
            return extension;
        }

        public String getPluginId() {
            return pluginId;
        }
    }
}
