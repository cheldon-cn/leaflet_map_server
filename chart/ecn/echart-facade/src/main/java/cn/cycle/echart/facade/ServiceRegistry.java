package cn.cycle.echart.facade;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 服务注册中心。
 * 
 * <p>管理应用程序中所有服务的注册和查找。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ServiceRegistry {

    private static ServiceRegistry instance;
    
    private final Map<Class<?>, Object> services;
    private final Map<Class<?>, List<ServiceListener<?>>> listeners;
    private final Map<Class<?>, ServiceFactory<?>> factories;

    private ServiceRegistry() {
        this.services = new ConcurrentHashMap<>();
        this.listeners = new ConcurrentHashMap<>();
        this.factories = new ConcurrentHashMap<>();
    }

    public static synchronized ServiceRegistry getInstance() {
        if (instance == null) {
            instance = new ServiceRegistry();
        }
        return instance;
    }

    public <T> void register(Class<T> serviceType, T service) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        Objects.requireNonNull(service, "service cannot be null");

        if (!serviceType.isInstance(service)) {
            throw new IllegalArgumentException("Service is not an instance of " + serviceType.getName());
        }

        Object existing = services.put(serviceType, service);
        
        if (existing == null) {
            notifyServiceRegistered(serviceType, service);
        } else {
            notifyServiceReplaced(serviceType, existing, service);
        }
    }

    public <T> void registerFactory(Class<T> serviceType, ServiceFactory<T> factory) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        Objects.requireNonNull(factory, "factory cannot be null");
        
        factories.put(serviceType, factory);
    }

    @SuppressWarnings("unchecked")
    public <T> T get(Class<T> serviceType) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");

        Object service = services.get(serviceType);
        
        if (service == null) {
            ServiceFactory<?> factory = factories.get(serviceType);
            if (factory != null) {
                service = factory.create();
                if (service != null) {
                    services.put(serviceType, service);
                    notifyServiceRegistered(serviceType, service);
                }
            }
        }

        if (service == null) {
            throw new IllegalStateException("Service not found: " + serviceType.getName());
        }

        return (T) service;
    }

    public <T> T getOrNull(Class<T> serviceType) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        
        try {
            return get(serviceType);
        } catch (IllegalStateException e) {
            return null;
        }
    }

    public <T> boolean hasService(Class<T> serviceType) {
        return services.containsKey(serviceType) || factories.containsKey(serviceType);
    }

    @SuppressWarnings("unchecked")
    public <T> T unregister(Class<T> serviceType) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");

        Object service = services.remove(serviceType);
        
        if (service != null) {
            notifyServiceUnregistered(serviceType, (T) service);
        }

        return (T) service;
    }

    public List<Class<?>> getRegisteredServices() {
        return new ArrayList<>(services.keySet());
    }

    public void clear() {
        for (Class<?> serviceType : new ArrayList<>(services.keySet())) {
            unregister(serviceType);
        }
        factories.clear();
    }

    public <T> void addListener(Class<T> serviceType, ServiceListener<T> listener) {
        listeners.computeIfAbsent(serviceType, k -> new ArrayList<>()).add(listener);
    }

    public <T> void removeListener(Class<T> serviceType, ServiceListener<T> listener) {
        List<ServiceListener<?>> typeListeners = listeners.get(serviceType);
        if (typeListeners != null) {
            typeListeners.remove(listener);
        }
    }

    @SuppressWarnings("unchecked")
    private <T> void notifyServiceRegistered(Class<T> serviceType, Object service) {
        List<ServiceListener<?>> typeListeners = listeners.get(serviceType);
        if (typeListeners != null) {
            for (ServiceListener<?> listener : typeListeners) {
                ((ServiceListener<T>) listener).onServiceRegistered((T) service);
            }
        }
    }

    @SuppressWarnings("unchecked")
    private <T> void notifyServiceReplaced(Class<T> serviceType, Object oldService, Object newService) {
        List<ServiceListener<?>> typeListeners = listeners.get(serviceType);
        if (typeListeners != null) {
            for (ServiceListener<?> listener : typeListeners) {
                ((ServiceListener<T>) listener).onServiceReplaced((T) oldService, (T) newService);
            }
        }
    }

    @SuppressWarnings("unchecked")
    private <T> void notifyServiceUnregistered(Class<T> serviceType, T service) {
        List<ServiceListener<?>> typeListeners = listeners.get(serviceType);
        if (typeListeners != null) {
            for (ServiceListener<?> listener : typeListeners) {
                ((ServiceListener<T>) listener).onServiceUnregistered(service);
            }
        }
    }

    @FunctionalInterface
    public interface ServiceFactory<T> {
        T create();
    }

    public interface ServiceListener<T> {
        void onServiceRegistered(T service);
        void onServiceReplaced(T oldService, T newService);
        void onServiceUnregistered(T service);
    }
}
