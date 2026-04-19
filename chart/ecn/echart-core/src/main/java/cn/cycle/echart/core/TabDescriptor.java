package cn.cycle.echart.core;

/**
 * 标签页描述符接口。
 * 
 * <p>定义标签页的元数据，用于标签页注册和管理。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface TabDescriptor {

    /**
     * 获取标签页ID。
     * 
     * <p>标签页ID在系统中必须唯一。</p>
     * 
     * @return 标签页ID，不为null
     */
    String getId();

    /**
     * 获取标签页名称。
     * 
     * @return 标签页名称，用于显示
     */
    String getName();

    /**
     * 获取标签页图标路径。
     * 
     * @return 图标路径，可以为null
     */
    String getIcon();

    /**
     * 获取所属面板ID。
     * 
     * @return 面板ID，如果为null表示独立标签页
     */
    String getParentPanelId();

    /**
     * 获取标签页优先级。
     * 
     * <p>优先级越高，标签页越靠前显示。</p>
     * 
     * @return 优先级值
     */
    int getPriority();

    /**
     * 检查标签页是否可关闭。
     * 
     * @return 如果可关闭返回true
     */
    boolean isClosable();

    /**
     * 检查标签页是否可拖拽。
     * 
     * @return 如果可拖拽返回true
     */
    boolean isDraggable();

    /**
     * 检查标签页是否可分离。
     * 
     * @return 如果可分离返回true
     */
    boolean isDetachable();

    /**
     * 获取标签页关联的内容类型。
     * 
     * @return 内容类型，如"chart"、"text"、"image"
     */
    String getContentType();
}
