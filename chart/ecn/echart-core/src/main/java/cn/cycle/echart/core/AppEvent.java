package cn.cycle.echart.core;

import java.util.EventObject;
import java.util.Objects;

/**
 * 应用事件。
 * 
 * <p>表示系统中发生的各种事件，用于模块间通信。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * // 创建并发送事件
 * AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, alarm);
 * eventBus.publish(event);
 * 
 * // 处理事件
 * public void onEvent(AppEvent event) {
 *     if (event.getType() == AppEventType.ALARM_TRIGGERED) {
 *         Alarm alarm = event.getData(Alarm.class);
 *         // 处理预警
 *     }
 * }
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AppEvent extends EventObject {

    private static final long serialVersionUID = 1L;

    private final AppEventType type;
    private final Object data;
    private final long timestamp;
    private final String sourceComponent;

    /**
     * 创建事件实例。
     * 
     * @param source 事件源，不能为null
     * @param type 事件类型，不能为null
     * @param data 事件数据，可以为null
     * @throws IllegalArgumentException 如果source或type为null
     */
    public AppEvent(Object source, AppEventType type, Object data) {
        super(source);
        this.type = Objects.requireNonNull(type, "type cannot be null");
        this.data = data;
        this.timestamp = System.currentTimeMillis();
        this.sourceComponent = source.getClass().getSimpleName();
    }

    /**
     * 创建无数据的事件实例。
     * 
     * @param source 事件源，不能为null
     * @param type 事件类型，不能为null
     * @throws IllegalArgumentException 如果source或type为null
     */
    public AppEvent(Object source, AppEventType type) {
        this(source, type, null);
    }

    /**
     * 获取事件类型。
     * 
     * @return 事件类型，不为null
     */
    public AppEventType getType() {
        return type;
    }

    /**
     * 获取事件数据。
     * 
     * @return 事件数据，可能为null
     */
    public Object getData() {
        return data;
    }

    /**
     * 获取指定类型的事件数据。
     * 
     * @param <T> 数据类型
     * @param dataType 数据类型，不能为null
     * @return 事件数据，如果类型不匹配返回null
     * @throws IllegalArgumentException 如果dataType为null
     */
    @SuppressWarnings("unchecked")
    public <T> T getData(Class<T> dataType) {
        Objects.requireNonNull(dataType, "dataType cannot be null");
        if (data != null && dataType.isInstance(data)) {
            return (T) data;
        }
        return null;
    }

    /**
     * 获取事件时间戳。
     * 
     * @return 时间戳（毫秒）
     */
    public long getTimestamp() {
        return timestamp;
    }

    /**
     * 获取源组件名称。
     * 
     * @return 源组件名称
     */
    public String getSourceComponent() {
        return sourceComponent;
    }

    @Override
    public String toString() {
        return "AppEvent{" +
               "type=" + type +
               ", sourceComponent='" + sourceComponent + '\'' +
               ", timestamp=" + timestamp +
               ", data=" + data +
               '}';
    }
}
