package cn.cycle.echart.core;

import java.util.function.Consumer;

/**
 * 事件处理器接口。
 * 
 * <p>用于处理特定类型的事件。</p>
 * 
 * @param <T> 事件数据类型
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
@FunctionalInterface
public interface EventHandler<T> {

    /**
     * 处理事件。
     * 
     * @param event 应用事件
     * @param data 事件数据
     */
    void onEvent(AppEvent event, T data);
}
