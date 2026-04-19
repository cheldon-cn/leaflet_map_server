package cn.cycle.echart.ui;

import cn.cycle.echart.core.PanelDescriptor;
import javafx.scene.Node;
import javafx.scene.image.Image;

/**
 * JavaFX侧边栏面板接口。
 * 
 * <p>定义侧边栏面板的基本行为。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface FxSideBarPanel extends PanelDescriptor {

    /**
     * 获取面板标识符。
     * 
     * @return 面板标识符
     */
    String getPanelId();

    /**
     * 获取面板标题。
     * 
     * @return 标题
     */
    String getTitle();

    /**
     * 获取面板图标图像。
     * 
     * @return 图标
     */
    Image getIconImage();

    /**
     * 获取面板内容节点。
     * 
     * @return JavaFX节点
     */
    Node getContent();

    /**
     * 面板被激活时调用。
     */
    void onActivate();

    /**
     * 面板被停用时调用。
     */
    void onDeactivate();

    /**
     * 关闭面板。
     */
    default void close() {
    }

    @Override
    default String getIcon() {
        return null;
    }

    @Override
    default String getId() {
        return getPanelId();
    }

    @Override
    default String getName() {
        return getTitle();
    }

    @Override
    default String getPosition() {
        return "left";
    }

    @Override
    default int getPriority() {
        return 100;
    }

    @Override
    default boolean isClosable() {
        return true;
    }

    @Override
    default boolean isMovable() {
        return true;
    }

    @Override
    default boolean isDockable() {
        return true;
    }

    @Override
    default int getPanelMinWidth() {
        return 150;
    }

    @Override
    default int getPanelMinHeight() {
        return -1;
    }

    @Override
    default int getPanelDefaultWidth() {
        return 250;
    }

    @Override
    default int getPanelDefaultHeight() {
        return -1;
    }
}
