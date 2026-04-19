package cn.cycle.echart.event;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.EventHandler;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.TimeUnit;
import java.util.Objects;

/**
 * 事件分发器。
 * 
 * <p>提供同步和异步事件分发能力。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class EventDispatcher {

    private final DefaultEventBus eventBus;
    private final ExecutorService asyncExecutor;
    private volatile boolean shutdown;

    public EventDispatcher(DefaultEventBus eventBus) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.asyncExecutor = Executors.newSingleThreadExecutor(r -> {
            Thread t = new Thread(r, "EventDispatcher-Async");
            t.setDaemon(true);
            return t;
        });
        this.shutdown = false;
    }

    /**
     * 同步分发事件。
     * 
     * @param event 应用事件
     */
    public void dispatch(AppEvent event) {
        Objects.requireNonNull(event, "event cannot be null");
        checkShutdown();
        eventBus.publish(event);
    }

    /**
     * 异步分发事件。
     * 
     * @param event 应用事件
     */
    public void dispatchAsync(AppEvent event) {
        Objects.requireNonNull(event, "event cannot be null");
        checkShutdown();
        asyncExecutor.submit(() -> eventBus.publish(event));
    }

    /**
     * 分发事件并等待所有处理器完成。
     * 
     * @param event 应用事件
     * @param timeout 超时时间（毫秒）
     * @return 是否在超时前完成
     */
    public boolean dispatchAndWait(AppEvent event, long timeout) {
        Objects.requireNonNull(event, "event cannot be null");
        checkShutdown();
        
        try {
            return asyncExecutor.submit(() -> {
                eventBus.publish(event);
                return true;
            }).get(timeout, TimeUnit.MILLISECONDS);
        } catch (Exception e) {
            return false;
        }
    }

    /**
     * 关闭分发器。
     */
    public void shutdown() {
        shutdown = true;
        asyncExecutor.shutdown();
        try {
            if (!asyncExecutor.awaitTermination(5, TimeUnit.SECONDS)) {
                asyncExecutor.shutdownNow();
            }
        } catch (InterruptedException e) {
            asyncExecutor.shutdownNow();
            Thread.currentThread().interrupt();
        }
    }

    /**
     * 检查分发器是否已关闭。
     * 
     * @return 是否已关闭
     */
    public boolean isShutdown() {
        return shutdown;
    }

    private void checkShutdown() {
        if (shutdown) {
            throw new IllegalStateException("EventDispatcher has been shutdown");
        }
    }
}
