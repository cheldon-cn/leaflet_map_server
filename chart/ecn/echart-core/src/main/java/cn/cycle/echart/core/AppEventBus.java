package cn.cycle.echart.core;

import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * 应用事件总线（单例模式）。
 * 
 * <p>设计文档7.1要求的单例模式+BlockingQueue+后台线程+Platform.runLater调度。</p>
 * 
 * <h2>架构说明</h2>
 * <pre>
 * 发布者线程 ──→ BlockingQueue ──→ 后台消费者线程
 *                                    │
 *                                    ↓
 *                           PlatformAdapter.runOnUiThread()
 *                                    │
 *                                    ↓
 *                              UI线程执行
 * </pre>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * // 获取单例实例
 * AppEventBus bus = AppEventBus.getInstance();
 * 
 * // 设置平台适配器（通常在应用启动时设置）
 * AppEventBus.setPlatformAdapter(new FxPlatformAdapter());
 * 
 * // 发布事件（任何线程都可以调用）
 * bus.publish(new AppEvent(this, AppEventType.ALARM_TRIGGERED, alarm));
 * 
 * // 订阅事件（处理器会在UI线程执行）
 * bus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
 *     // 这里已经在UI线程，可以直接更新UI
 *     alarmPanel.updateAlarm((Alarm) data);
 * });
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AppEventBus implements EventBus {

    private static AppEventBus instance;
    private static PlatformAdapter platformAdapter;

    private final BlockingQueue<AppEvent> eventQueue;
    private final Map<AppEventType, List<EventHandler<?>>> subscribers;
    private final List<EventHandler<Object>> globalSubscribers;
    private final ExecutorService executor;
    private final AtomicBoolean running;
    private Thread consumerThread;

    private AppEventBus() {
        this.eventQueue = new LinkedBlockingQueue<>();
        this.subscribers = new ConcurrentHashMap<>();
        this.globalSubscribers = new CopyOnWriteArrayList<>();
        this.executor = Executors.newCachedThreadPool();
        this.running = new AtomicBoolean(true);
        startConsumerThread();
    }

    /**
     * 获取单例实例。
     * 
     * @return 事件总线实例
     */
    public static synchronized AppEventBus getInstance() {
        if (instance == null) {
            instance = new AppEventBus();
        }
        return instance;
    }

    /**
     * 设置平台适配器。
     * 
     * <p>用于UI线程调度，应在应用启动时设置。</p>
     * 
     * @param adapter 平台适配器
     */
    public static void setPlatformAdapter(PlatformAdapter adapter) {
        platformAdapter = adapter;
    }

    /**
     * 获取平台适配器。
     * 
     * @return 平台适配器，可能为null
     */
    public static PlatformAdapter getPlatformAdapter() {
        return platformAdapter;
    }

    private void startConsumerThread() {
        consumerThread = new Thread(this::consumeEvents, "AppEventBus-Consumer");
        consumerThread.setDaemon(true);
        consumerThread.start();
    }

    private void consumeEvents() {
        while (running.get()) {
            try {
                AppEvent event = eventQueue.take();
                dispatchEvent(event);
            } catch (InterruptedException e) {
                Thread.currentThread().interrupt();
                break;
            } catch (Exception e) {
                System.err.println("Error consuming event: " + e.getMessage());
            }
        }
    }

    @SuppressWarnings("unchecked")
    private void dispatchEvent(AppEvent event) {
        Runnable dispatchTask = () -> {
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
        };

        if (platformAdapter != null) {
            platformAdapter.runOnUiThread(dispatchTask);
        } else {
            dispatchTask.run();
        }
    }

    @Override
    public void publish(AppEvent event) {
        Objects.requireNonNull(event, "event cannot be null");
        if (!running.get()) {
            throw new IllegalStateException("AppEventBus has been shutdown");
        }
        eventQueue.offer(event);
    }

    /**
     * 同步发布事件（立即执行，不经过队列）。
     * 
     * <p>用于需要立即处理的事件，会阻塞当前线程。</p>
     * 
     * @param event 事件，不能为null
     */
    public void publishSync(AppEvent event) {
        Objects.requireNonNull(event, "event cannot be null");
        if (!running.get()) {
            throw new IllegalStateException("AppEventBus has been shutdown");
        }
        dispatchEvent(event);
    }

    /**
     * 异步发布事件（使用线程池执行，不经过队列）。
     * 
     * <p>用于不需要UI线程调度的事件。</p>
     * 
     * @param event 事件，不能为null
     */
    public void publishAsync(AppEvent event) {
        Objects.requireNonNull(event, "event cannot be null");
        if (!running.get()) {
            throw new IllegalStateException("AppEventBus has been shutdown");
        }
        executor.submit(() -> dispatchEvent(event));
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
        eventQueue.clear();
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
     * 获取队列中待处理的事件数量。
     * 
     * @return 待处理事件数量
     */
    public int getPendingEventCount() {
        return eventQueue.size();
    }

    /**
     * 获取已订阅的事件类型集合。
     * 
     * @return 事件类型集合
     */
    public Set<AppEventType> getSubscribedEventTypes() {
        return subscribers.keySet();
    }

    /**
     * 关闭事件总线，释放资源。
     */
    public void shutdown() {
        running.set(false);
        if (consumerThread != null) {
            consumerThread.interrupt();
        }
        executor.shutdown();
        clear();
    }

    /**
     * 检查事件总线是否正在运行。
     * 
     * @return 如果正在运行返回true
     */
    public boolean isRunning() {
        return running.get();
    }

    /**
     * 重置单例实例（仅用于测试）。
     */
    static synchronized void resetInstance() {
        if (instance != null) {
            instance.shutdown();
            instance = null;
        }
    }
}
