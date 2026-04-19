package cn.cycle.echart.core;

/**
 * 事件总线接口。
 * 
 * <p>提供发布/订阅模式的事件分发机制，实现模块间解耦通信。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * // 订阅事件
 * eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
 *     Alarm alarm = data;
 *     System.out.println("Alarm triggered: " + alarm);
 * });
 * 
 * // 发布事件
 * eventBus.publish(new AppEvent(this, AppEventType.ALARM_TRIGGERED, alarm));
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface EventBus {

    /**
     * 发布事件。
     * 
     * @param event 应用事件，不能为null
     * @throws IllegalArgumentException 如果event为null
     */
    void publish(AppEvent event);

    /**
     * 订阅事件类型。
     * 
     * @param <T> 事件数据类型
     * @param eventType 事件类型，不能为null
     * @param handler 事件处理器，不能为null
     * @throws IllegalArgumentException 如果参数为null
     */
    <T> void subscribe(AppEventType eventType, EventHandler<T> handler);

    /**
     * 取消订阅事件类型。
     * 
     * @param eventType 事件类型，不能为null
     * @param handler 事件处理器，不能为null
     * @throws IllegalArgumentException 如果参数为null
     */
    void unsubscribe(AppEventType eventType, EventHandler<?> handler);

    /**
     * 订阅所有事件类型。
     * 
     * @param handler 事件处理器，不能为null
     * @throws IllegalArgumentException 如果handler为null
     */
    void subscribeAll(EventHandler<Object> handler);

    /**
     * 取消订阅所有事件类型。
     * 
     * @param handler 事件处理器，不能为null
     * @throws IllegalArgumentException 如果handler为null
     */
    void unsubscribeAll(EventHandler<Object> handler);

    /**
     * 清除所有订阅。
     */
    void clear();

    /**
     * 获取指定事件类型的订阅者数量。
     * 
     * @param eventType 事件类型，不能为null
     * @return 订阅者数量
     */
    int getSubscriberCount(AppEventType eventType);

    /**
     * 获取总订阅者数量。
     * 
     * @return 总订阅者数量
     */
    int getTotalSubscriberCount();
}
