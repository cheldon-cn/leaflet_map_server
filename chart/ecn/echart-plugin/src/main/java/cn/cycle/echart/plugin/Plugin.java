package cn.cycle.echart.plugin;

import java.net.URLClassLoader;
import java.util.Map;

/**
 * 插件接口。
 * 
 * <p>定义插件的基本属性和生命周期方法。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface Plugin {

    /**
     * 获取插件ID。
     * 
     * @return 插件ID
     */
    String getId();

    /**
     * 获取插件名称。
     * 
     * @return 插件名称
     */
    String getName();

    /**
     * 获取插件版本。
     * 
     * @return 插件版本
     */
    String getVersion();

    /**
     * 获取插件描述。
     * 
     * @return 插件描述
     */
    String getDescription();

    /**
     * 获取插件作者。
     * 
     * @return 插件作者
     */
    String getAuthor();

    /**
     * 获取插件依赖。
     * 
     * @return 依赖插件ID列表
     */
    String[] getDependencies();

    /**
     * 获取插件配置。
     * 
     * @return 配置属性映射
     */
    Map<String, Object> getConfiguration();

    /**
     * 初始化插件。
     * 
     * @param context 插件上下文
     * @throws PluginException 初始化失败时抛出
     */
    void initialize(PluginContext context) throws PluginException;

    /**
     * 启动插件。
     * 
     * @throws PluginException 启动失败时抛出
     */
    void start() throws PluginException;

    /**
     * 停止插件。
     * 
     * @throws PluginException 停止失败时抛出
     */
    void stop() throws PluginException;

    /**
     * 销毁插件。
     */
    void destroy();

    /**
     * 获取插件状态。
     * 
     * @return 插件状态
     */
    PluginState getState();

    /**
     * 获取插件的类加载器。
     * 
     * @return 类加载器
     */
    URLClassLoader getClassLoader();

    /**
     * 设置插件的类加载器。
     * 
     * @param classLoader 类加载器
     */
    void setClassLoader(URLClassLoader classLoader);
}
