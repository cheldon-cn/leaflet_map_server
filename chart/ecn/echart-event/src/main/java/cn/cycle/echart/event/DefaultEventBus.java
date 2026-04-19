package cn.cycle.echart.event;

import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventHandler;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.Objects;

/**
 * 默认事件总线实现。
 * 
 * <p>提供线程安全的事件发布/订阅机制。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DefaultEventBus implements EventBus {

    private final Map<AppEventType, List<EventHandler<?>>> subscribers;
    private final List<EventHandler<Object>> globalSubscribers;

    public DefaultEventBus() {
        this.subscribers = new ConcurrentHashMap<>();
        this.globalSubscribers = new CopyOnWriteArrayList<>();
    }

    @Override
    public void publish(AppEvent event) {
        Objects.requireNonNull(event, "event cannot be null");
        
        List<EventHandler<?>> handlers = subscribers.get(event.getType());
        if (handlers != null) {
            for (EventHandler<?> handler : handlers) {
                try {
                    dispatchEvent(handler, event);
                } catch (Exception e) {
                    System.err.println("Error dispatching event: " + e.getMessage());
                }
            }
        }
        
        for (EventHandler<Object> globalHandler : globalSubscribers) {
            try {
                globalHandler.onEvent(event, event.getData());
            } catch (Exception e) {
                System.err.println("Error in global handler: " + e.getMessage());
            }
        }
    }

    @SuppressWarnings("unchecked")
    private <T> void dispatchEvent(EventHandler<?> handler, AppEvent event) {
        EventHandler<T> typedHandler = (EventHandler<T>) handler;
        T data = event.getData() != null ? (T) event.getData() : null;
        typedHandler.onEvent(event, data);
    }

    @Override
    public <T> void subscribe(AppEventType eventType, EventHandler<T> handler) {
        Objects.requireNonNull(eventType, "eventType cannot be null");
        Objects.requireNonNull(handler, "handler cannot be null");
        
        subscribers.computeIfAbsent(eventType, k -> new CopyOnWriteArrayList<>())
                  .add(handler);
    }

    @Override
    public void unsubscribe(AppEventType eventType, EventHandler<?> handler) {
        Objects.requireNonNull(eventType, "eventType cannot be null");
        Objects.requireNonNull(handler, "handler cannot be null");
        
        List<EventHandler<?>> handlers = subscribers.get(eventType);
        if (handlers != null) {
            handlers.remove(handler);
        }
    }

    @Override
    public void subscribeAll(EventHandler<Object> handler) {
        Objects.requireNonNull(handler, "handler cannot be null");
        globalSubscribers.add(handler);
    }

    @Override
    public void unsubscribeAll(EventHandler<Object> handler) {
        Objects.requireNonNull(handler, "handler cannot be null");
        globalSubscribers.remove(handler);
    }

    @Override
    public void clear() {
        subscribers.clear();
        globalSubscribers.clear();
    }

    @Override
    public int getSubscriberCount(AppEventType eventType) {
        Objects.requireNonNull(eventType, "eventType cannot be null");
        List<EventHandler<?>> handlers = subscribers.get(eventType);
        return handlers != null ? handlers.size() : 0;
    }

    @Override
    public int getTotalSubscriberCount() {
        int count = globalSubscribers.size();
        for (List<EventHandler<?>> handlers : subscribers.values()) {
            count += handlers.size();
        }
        return count;
    }
}
