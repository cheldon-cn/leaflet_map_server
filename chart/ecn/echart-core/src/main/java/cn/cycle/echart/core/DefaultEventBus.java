package cn.cycle.echart.core;

import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * 默认事件总线实现。
 * 
 * <p>提供线程安全的事件发布和订阅机制。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DefaultEventBus implements EventBus {

    private final Map<AppEventType, List<EventHandler<?>>> subscribers;
    private final List<EventHandler<Object>> globalSubscribers;
    private final ExecutorService executor;
    private final boolean asyncMode;

    /**
     * 创建同步事件总线。
     */
    public DefaultEventBus() {
        this(false);
    }

    /**
     * 创建事件总线。
     * 
     * @param asyncMode 是否使用异步模式
     */
    public DefaultEventBus(boolean asyncMode) {
        this.subscribers = new ConcurrentHashMap<>();
        this.globalSubscribers = new CopyOnWriteArrayList<>();
        this.asyncMode = asyncMode;
        if (asyncMode) {
            this.executor = Executors.newCachedThreadPool();
        } else {
            this.executor = null;
        }
    }

    @Override
    public void publish(AppEvent event) {
        Objects.requireNonNull(event, "event cannot be null");
        
        if (asyncMode && executor != null) {
            executor.submit(() -> doPublish(event));
        } else {
            doPublish(event);
        }
    }

    @SuppressWarnings("unchecked")
    private void doPublish(AppEvent event) {
        List<EventHandler<?>> handlers = subscribers.get(event.getType());
        if (handlers != null) {
            for (EventHandler<?> handler : handlers) {
                try {
                    ((EventHandler<Object>) handler).onEvent(event, event.getData());
                } catch (Exception e) {
                    System.err.println("Error handling event: " + event + 
                                      ", handler: " + handler + 
                                      ", error: " + e.getMessage());
                }
            }
        }
        
        for (EventHandler<Object> handler : globalSubscribers) {
            try {
                handler.onEvent(event, event.getData());
            } catch (Exception e) {
                System.err.println("Error handling event by global handler: " + 
                                  event + ", error: " + e.getMessage());
            }
        }
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

    /**
     * 关闭事件总线，释放资源。
     */
    public void shutdown() {
        if (executor != null) {
            executor.shutdown();
        }
        clear();
    }

    /**
     * 检查是否为异步模式。
     * 
     * @return 如果为异步模式返回true
     */
    public boolean isAsyncMode() {
        return asyncMode;
    }
}
