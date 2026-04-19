package cn.cycle.echart.core;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Objects;

/**
 * 服务定位器。
 * 
 * <p>提供服务注册和查找功能，实现简单的依赖注入。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * // 注册服务
 * ServiceLocator.register(PlatformAdapter.class, new FxPlatformAdapter());
 * ServiceLocator.register(EventBus.class, new DefaultEventBus());
 * 
 * // 获取服务
 * PlatformAdapter platform = ServiceLocator.getService(PlatformAdapter.class);
 * EventBus eventBus = ServiceLocator.getService(EventBus.class);
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public final class ServiceLocator {

    private static final Map<Class<?>, Object> services = new ConcurrentHashMap<>();
    private static final Map<Class<?>, Factory<?>> factories = new ConcurrentHashMap<>();

    private ServiceLocator() {
    }

    /**
     * 注册服务实例。
     * 
     * @param <T> 服务类型
     * @param serviceType 服务类型，不能为null
     * @param instance 服务实例，不能为null
     * @throws IllegalArgumentException 如果参数为null
     */
    public static <T> void register(Class<T> serviceType, T instance) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        Objects.requireNonNull(instance, "instance cannot be null");
        services.put(serviceType, instance);
    }

    /**
     * 注册服务工厂。
     * 
     * @param <T> 服务类型
     * @param serviceType 服务类型，不能为null
     * @param factory 服务工厂，不能为null
     * @throws IllegalArgumentException 如果参数为null
     */
    public static <T> void registerFactory(Class<T> serviceType, Factory<T> factory) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        Objects.requireNonNull(factory, "factory cannot be null");
        factories.put(serviceType, factory);
    }

    /**
     * 获取服务实例。
     * 
     * @param <T> 服务类型
     * @param serviceType 服务类型，不能为null
     * @return 服务实例
     * @throws IllegalArgumentException 如果服务未注册
     */
    @SuppressWarnings("unchecked")
    public static <T> T getService(Class<T> serviceType) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        
        Object instance = services.get(serviceType);
        if (instance != null) {
            return (T) instance;
        }
        
        Factory<?> factory = factories.get(serviceType);
        if (factory != null) {
            return (T) factory.create();
        }
        
        throw new IllegalArgumentException(
            "Service not registered: " + serviceType.getName());
    }

    /**
     * 尝试获取服务实例。
     * 
     * @param <T> 服务类型
     * @param serviceType 服务类型，不能为null
     * @return 服务实例，如果未注册返回null
     */
    @SuppressWarnings("unchecked")
    public static <T> T tryGetService(Class<T> serviceType) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        
        Object instance = services.get(serviceType);
        if (instance != null) {
            return (T) instance;
        }
        
        Factory<?> factory = factories.get(serviceType);
        if (factory != null) {
            return (T) factory.create();
        }
        
        return null;
    }

    /**
     * 检查服务是否已注册。
     * 
     * @param serviceType 服务类型，不能为null
     * @return 如果已注册返回true
     */
    public static boolean isRegistered(Class<?> serviceType) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        return services.containsKey(serviceType) || factories.containsKey(serviceType);
    }

    /**
     * 注销服务。
     * 
     * @param serviceType 服务类型，不能为null
     */
    public static void unregister(Class<?> serviceType) {
        Objects.requireNonNull(serviceType, "serviceType cannot be null");
        services.remove(serviceType);
        factories.remove(serviceType);
    }

    /**
     * 清除所有服务。
     */
    public static void clear() {
        services.clear();
        factories.clear();
    }

    /**
     * 服务工厂接口。
     * 
     * @param <T> 服务类型
     */
    @FunctionalInterface
    public interface Factory<T> {
        /**
         * 创建服务实例。
         * 
         * @return 服务实例
         */
        T create();
    }
}
