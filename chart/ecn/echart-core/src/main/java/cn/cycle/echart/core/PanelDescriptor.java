package cn.cycle.echart.core;

/**
 * 面板描述符接口。
 * 
 * <p>定义面板的元数据，用于面板注册和管理。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * public class AlarmPanelDescriptor implements PanelDescriptor {
 *     public String getId() { return "alarm-panel"; }
 *     public String getName() { return "预警面板"; }
 *     public String getIcon() { return "alarm.png"; }
 *     public String getPosition() { return "right"; }
 *     public int getPriority() { return 100; }
 * }
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface PanelDescriptor {

    /**
     * 获取面板ID。
     * 
     * <p>面板ID在系统中必须唯一。</p>
     * 
     * @return 面板ID，不为null
     */
    String getId();

    /**
     * 获取面板名称。
     * 
     * @return 面板名称，用于显示
     */
    String getName();

    /**
     * 获取面板图标路径。
     * 
     * @return 图标路径，可以为null
     */
    String getIcon();

    /**
     * 获取面板位置。
     * 
     * <p>位置值可以是：left、right、top、bottom、center</p>
     * 
     * @return 面板位置
     */
    String getPosition();

    /**
     * 获取面板优先级。
     * 
     * <p>优先级越高，面板越靠前显示。</p>
     * 
     * @return 优先级值
     */
    int getPriority();

    /**
     * 检查面板是否可关闭。
     * 
     * @return 如果可关闭返回true
     */
    boolean isClosable();

    /**
     * 检查面板是否可移动。
     * 
     * @return 如果可移动返回true
     */
    boolean isMovable();

    /**
     * 检查面板是否可停靠。
     * 
     * @return 如果可停靠返回true
     */
    boolean isDockable();

    /**
     * 获取面板最小宽度。
     * 
     * @return 最小宽度（像素），-1表示无限制
     */
    int getPanelMinWidth();

    /**
     * 获取面板最小高度。
     * 
     * @return 最小高度（像素），-1表示无限制
     */
    int getPanelMinHeight();

    /**
     * 获取面板默认宽度。
     * 
     * @return 默认宽度（像素）
     */
    int getPanelDefaultWidth();

    /**
     * 获取面板默认高度。
     * 
     * @return 默认高度（像素）
     */
    int getPanelDefaultHeight();
}
