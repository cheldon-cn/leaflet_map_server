package cn.cycle.echart.ui;

import cn.cycle.echart.core.TabDescriptor;
import javafx.scene.Node;
import javafx.scene.control.Tab;

/**
 * JavaFX右侧标签页接口。
 * 
 * <p>定义右侧标签页的基本行为。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface FxRightTabPanel extends TabDescriptor {

    /**
     * 获取标签页标识符。
     * 
     * @return 标签页标识符
     */
    String getTabId();

    /**
     * 获取标签页。
     * 
     * @return JavaFX Tab对象
     */
    Tab getTab();

    /**
     * 获取标签页内容。
     * 
     * @return 内容节点
     */
    Node getContent();

    /**
     * 标签页被选中时调用。
     */
    void onSelected();

    /**
     * 标签页被取消选中时调用。
     */
    void onDeselected();

    /**
     * 关闭标签页。
     * 
     * @return 是否允许关闭
     */
    default boolean onClose() {
        return true;
    }

    /**
     * 更新标签页内容。
     */
    default void refresh() {
    }

    @Override
    default String getId() {
        return getTabId();
    }

    @Override
    default String getName() {
        return getTitle();
    }

    default String getTitle() {
        return getTab().getText();
    }

    @Override
    default String getParentPanelId() {
        return null;
    }

    @Override
    default int getPriority() {
        return 100;
    }

    @Override
    default boolean isClosable() {
        return false;
    }

    @Override
    default boolean isDraggable() {
        return true;
    }

    @Override
    default boolean isDetachable() {
        return false;
    }

    @Override
    default String getContentType() {
        return "panel";
    }
}
