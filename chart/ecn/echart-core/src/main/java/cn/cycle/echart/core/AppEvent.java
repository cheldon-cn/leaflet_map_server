package cn.cycle.echart.core;

import java.util.EventObject;
import java.util.HashMap;
import java.util.Map;
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
 * // 使用链式调用添加多个数据
 * AppEvent event = new AppEvent(this, AppEventType.ZOOM_CHANGED)
 *     .withData("zoomLevel", 12)
 *     .withData("centerX", 120.5)
 *     .withData("centerY", 31.2);
 * 
 * // 处理事件
 * public void onEvent(AppEvent event) {
 *     if (event.getType() == AppEventType.ALARM_TRIGGERED) {
 *         Alarm alarm = event.getData(Alarm.class);
 *         // 处理预警
 *     }
 *     // 获取键值对数据
 *     Double zoom = event.getData("zoomLevel", Double.class);
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
    private Map<String, Object> dataMap;

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
     * 添加键值对数据（链式调用）。
     * 
     * <p>设计文档7.2要求的链式调用方法。</p>
     * 
     * @param key 数据键，不能为null
     * @param value 数据值
     * @return 当前事件实例，支持链式调用
     * @throws IllegalArgumentException 如果key为null
     */
    public AppEvent withData(String key, Object value) {
        Objects.requireNonNull(key, "key cannot be null");
        if (dataMap == null) {
            dataMap = new HashMap<>();
        }
        dataMap.put(key, value);
        return this;
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
     * 根据键获取数据（泛型方法）。
     * 
     * <p>设计文档7.2要求的泛型方法。</p>
     * 
     * @param <T> 数据类型
     * @param key 数据键，不能为null
     * @param dataType 数据类型，不能为null
     * @return 数据值，如果键不存在或类型不匹配返回null
     * @throws IllegalArgumentException 如果key或dataType为null
     */
    @SuppressWarnings("unchecked")
    public <T> T getData(String key, Class<T> dataType) {
        Objects.requireNonNull(key, "key cannot be null");
        Objects.requireNonNull(dataType, "dataType cannot be null");
        if (dataMap == null) {
            return null;
        }
        Object value = dataMap.get(key);
        if (value != null && dataType.isInstance(value)) {
            return (T) value;
        }
        return null;
    }

    /**
     * 根据键获取数据（Object类型）。
     * 
     * @param key 数据键，不能为null
     * @return 数据值，如果键不存在返回null
     * @throws IllegalArgumentException 如果key为null
     */
    public Object getData(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        if (dataMap == null) {
            return null;
        }
        return dataMap.get(key);
    }

    /**
     * 检查是否包含指定键的数据。
     * 
     * @param key 数据键，不能为null
     * @return 如果包含返回true
     * @throws IllegalArgumentException 如果key为null
     */
    public boolean hasData(String key) {
        Objects.requireNonNull(key, "key cannot be null");
        return dataMap != null && dataMap.containsKey(key);
    }

    /**
     * 获取所有键值对数据。
     * 
     * @return 数据Map的只读副本，如果无数据返回空Map
     */
    public Map<String, Object> getDataMap() {
        if (dataMap == null) {
            return new HashMap<>();
        }
        return new HashMap<>(dataMap);
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
               ", dataMap=" + dataMap +
               '}';
    }
}
