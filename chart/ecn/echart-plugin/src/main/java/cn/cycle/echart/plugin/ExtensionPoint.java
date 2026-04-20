package cn.cycle.echart.plugin;

/**
 * 扩展点接口。
 * 
 * <p>定义插件可以扩展的功能点。</p>
 * 
 * @param <T> 扩展类型
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface ExtensionPoint<T> {

    /**
     * 获取扩展点ID。
     * 
     * @return 扩展点ID
     */
    String getId();

    /**
     * 获取扩展点名称。
     * 
     * @return 扩展点名称
     */
    String getName();

    /**
     * 获取扩展点描述。
     * 
     * @return 扩展点描述
     */
    String getDescription();

    /**
     * 获取扩展类型。
     * 
     * @return 扩展类型的Class对象
     */
    Class<T> getExtensionType();

    /**
     * 注册扩展。
     * 
     * @param extension 扩展实例
     * @param pluginId 插件ID
     */
    void registerExtension(T extension, String pluginId);

    /**
     * 注销扩展。
     * 
     * @param extension 扩展实例
     */
    void unregisterExtension(T extension);

    /**
     * 获取所有已注册的扩展。
     * 
     * @return 扩展列表
     */
    java.util.List<T> getExtensions();

    /**
     * 获取指定插件的扩展。
     * 
     * @param pluginId 插件ID
     * @return 扩展列表
     */
    java.util.List<T> getExtensions(String pluginId);
}
