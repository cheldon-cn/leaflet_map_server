package cn.cycle.echart.event;

import java.util.EventObject;
import java.util.Objects;

/**
 * 通用事件类。
 * 
 * <p>提供更灵活的事件数据封装。</p>
 * 
 * @param <T> 事件数据类型
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
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
     */
    public EventType getType() {
        return type;
    }

    /**
     * 获取事件数据。
     * 
     * @return 事件数据
     */
    public T getData() {
        return data;
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
