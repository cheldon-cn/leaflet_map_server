package cn.cycle.echart.plugin;

/**
 * 插件异常。
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PluginException extends Exception {

    private static final long serialVersionUID = 1L;
    
    private final String pluginId;

    public PluginException(String message) {
        super(message);
        this.pluginId = null;
    }

    public PluginException(String message, Throwable cause) {
        super(message, cause);
        this.pluginId = null;
    }

    public PluginException(String pluginId, String message) {
        super(message);
        this.pluginId = pluginId;
    }

    public PluginException(String pluginId, String message, Throwable cause) {
        super(message, cause);
        this.pluginId = pluginId;
    }

    public String getPluginId() {
        return pluginId;
    }
}
