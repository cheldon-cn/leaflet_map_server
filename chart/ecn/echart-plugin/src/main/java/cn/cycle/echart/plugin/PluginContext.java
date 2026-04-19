package cn.cycle.echart.plugin;

import cn.cycle.echart.core.ServiceLocator;

import java.util.Map;

/**
 * 插件上下文。
 * 
 * <p>提供插件运行时环境和访问服务的能力。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface PluginContext {

    /**
     * 获取服务定位器。
     * 
     * @return 服务定位器
     */
    ServiceLocator getServiceLocator();

    /**
     * 获取服务。
     * 
     * @param <T> 服务类型
     * @param serviceType 服务类型
     * @return 服务实例
     */
    <T> T getService(Class<T> serviceType);

    /**
     * 注册服务。
     * 
     * @param <T> 服务类型
     * @param serviceType 服务类型
     * @param service 服务实例
     */
    <T> void registerService(Class<T> serviceType, T service);

    /**
     * 获取配置属性。
     * 
     * @param key 属性键
     * @return 属性值
     */
    Object getProperty(String key);

    /**
     * 获取配置属性。
     * 
     * @param key 属性键
     * @param defaultValue 默认值
     * @return 属性值
     */
    Object getProperty(String key, Object defaultValue);

    /**
     * 设置配置属性。
     * 
     * @param key 属性键
     * @param value 属性值
     */
    void setProperty(String key, Object value);

    /**
     * 获取所有配置属性。
     * 
     * @return 配置属性映射
     */
    Map<String, Object> getProperties();

    /**
     * 获取插件数据目录。
     * 
     * @return 数据目录路径
     */
    String getDataDirectory();

    /**
     * 获取插件配置目录。
     * 
     * @return 配置目录路径
     */
    String getConfigDirectory();

    /**
     * 获取插件管理器。
     * 
     * @return 插件管理器
     */
    PluginManager getPluginManager();

    /**
     * 记录日志。
     * 
     * @param level 日志级别
     * @param message 日志消息
     */
    void log(LogLevel level, String message);

    /**
     * 记录日志。
     * 
     * @param level 日志级别
     * @param message 日志消息
     * @param throwable 异常
     */
    void log(LogLevel level, String message, Throwable throwable);

    /**
     * 日志级别枚举。
     */
    enum LogLevel {
        DEBUG,
        INFO,
        WARN,
        ERROR
    }
}
