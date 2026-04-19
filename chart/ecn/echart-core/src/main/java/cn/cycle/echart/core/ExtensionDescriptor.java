package cn.cycle.echart.core;

/**
 * 扩展点描述符接口。
 * 
 * <p>定义扩展点的元数据，用于插件系统。</p>
 * 
 * <h2>扩展点类型</h2>
 * <ul>
 *   <li>panel - 面板扩展</li>
 *   <li>tab - 标签页扩展</li>
 *   <li>menu - 菜单扩展</li>
 *   <li>toolbar - 工具栏扩展</li>
 *   <li>renderer - 渲染器扩展</li>
 *   <li>parser - 解析器扩展</li>
 * </ul>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface ExtensionDescriptor {

    /**
     * 获取扩展点ID。
     * 
     * <p>扩展点ID在系统中必须唯一。</p>
     * 
     * @return 扩展点ID，不为null
     */
    String getId();

    /**
     * 获取扩展点名称。
     * 
     * @return 扩展点名称，用于显示
     */
    String getName();

    /**
     * 获取扩展点类型。
     * 
     * @return 扩展点类型
     */
    String getType();

    /**
     * 获取扩展点描述。
     * 
     * @return 扩展点描述
     */
    String getDescription();

    /**
     * 获取扩展点接口类名。
     * 
     * <p>扩展实现必须实现此接口。</p>
     * 
     * @return 接口类名
     */
    String getInterfaceClass();

    /**
     * 获取扩展点优先级。
     * 
     * @return 优先级值
     */
    int getPriority();

    /**
     * 检查扩展点是否启用。
     * 
     * @return 如果启用返回true
     */
    boolean isEnabled();

    /**
     * 获取扩展点依赖的其他扩展点ID。
     * 
     * @return 依赖的扩展点ID数组，如果没有依赖返回空数组
     */
    String[] getDependencies();

    /**
     * 获取扩展点配置。
     * 
     * @param key 配置键
     * @return 配置值，如果不存在返回null
     */
    String getConfig(String key);

    /**
     * 获取扩展点配置。
     * 
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值，如果不存在返回默认值
     */
    String getConfig(String key, String defaultValue);
}
