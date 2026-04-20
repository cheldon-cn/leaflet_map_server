package cn.cycle.echart.event;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;

import java.util.EventObject;
import java.util.Objects;

/**
 * 通用事件类。
 * 
 * <p><strong>已废弃</strong>：请使用 {@link cn.cycle.echart.core.AppEvent} 代替。</p>
 * 
 * <p>此类已被 AppEvent 替代，建议直接使用 AppEvent。</p>
 * 
 * @param <T> 事件数据类型
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 * @deprecated 使用 {@link cn.cycle.echart.core.AppEvent} 代替
 * @see cn.cycle.echart.core.AppEvent
 */
@Deprecated
public class Event<T> extends EventObject {

    private static final long serialVersionUID = 1L;

    private final EventType type;
    private final T data;
    private final long timestamp;
    private boolean consumed;

    public Event(Object source, EventType type, T data) {
        super(source);
        this.type = Objects.requireNonNull(type, "type cannot be null");
        this.data = data;
        this.timestamp = System.currentTimeMillis();
        this.consumed = false;
    }

    public Event(Object source, EventType type) {
        this(source, type, null);
    }

    /**
     * 获取事件类型。
     * 
     * @return 事件类型
     * @deprecated 使用 {@link AppEvent#getType()} 代替
     */
    @Deprecated
    public EventType getType() {
        return type;
    }

    /**
     * 获取事件数据。
     * 
     * @return 事件数据
     * @deprecated 使用 {@link AppEvent#getData()} 代替
     */
    @Deprecated
    public T getData() {
        return data;
    }

    /**
     * 获取事件时间戳。
     * 
     * @return 时间戳（毫秒）
     * @deprecated 使用 {@link AppEvent#getTimestamp()} 代替
     */
    @Deprecated
    public long getTimestamp() {
        return timestamp;
    }

    /**
     * 检查事件是否已被消费。
     * 
     * @return 是否已被消费
     */
    public boolean isConsumed() {
        return consumed;
    }

    /**
     * 消费事件。
     * 
     * <p>标记事件为已消费，后续处理器可跳过处理。</p>
     */
    public void consume() {
        this.consumed = true;
    }

    /**
     * 转换为 AppEvent。
     * 
     * <p>提供从废弃类型到新类型的转换。</p>
     * 
     * @return 对应的 AppEvent 实例
     */
    public AppEvent toAppEvent() {
        AppEventType appType = type.toAppEventType();
        if (appType == null) {
            throw new IllegalStateException("Cannot convert EventType to AppEventType: " + type);
        }
        return new AppEvent(source, appType, data);
    }

    /**
     * 从 AppEvent 转换。
     * 
     * @param appEvent AppEvent 实例
     * @param <T> 数据类型
     * @return 对应的 Event 实例
     */
    @SuppressWarnings("unchecked")
    public static <T> Event<T> fromAppEvent(AppEvent appEvent) {
        if (appEvent == null) {
            return null;
        }
        EventType eventType = EventType.fromAppEventType(appEvent.getType());
        if (eventType == null) {
            return null;
        }
        Event<T> event = new Event<>(appEvent.getSource(), eventType, (T) appEvent.getData());
        return event;
    }

    @Override
    public String toString() {
        return "Event{" +
               "type=" + type +
               ", source=" + source.getClass().getSimpleName() +
               ", timestamp=" + timestamp +
               ", consumed=" + consumed +
               ", data=" + data +
               '}';
    }
}
