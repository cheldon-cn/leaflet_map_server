package cn.cycle.app.event;

import javafx.application.Platform;

import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;
import java.util.function.Consumer;

public class AppEventBus {
    
    private static final AppEventBus INSTANCE = new AppEventBus();
    private final Map<AppEventType, List<Consumer<AppEvent>>> subscribers = new ConcurrentHashMap<>();
    
    private AppEventBus() {
    }
    
    public static AppEventBus getInstance() {
        return INSTANCE;
    }
    
    public void subscribe(AppEventType type, Consumer<AppEvent> handler) {
        subscribers.computeIfAbsent(type, k -> new CopyOnWriteArrayList<>()).add(handler);
    }
    
    public void unsubscribe(AppEventType type, Consumer<AppEvent> handler) {
        List<Consumer<AppEvent>> handlers = subscribers.get(type);
        if (handlers != null) {
            handlers.remove(handler);
        }
    }
    
    public void publish(AppEvent event) {
        List<Consumer<AppEvent>> handlers = subscribers.get(event.getType());
        if (handlers != null) {
            if (Platform.isFxApplicationThread()) {
                for (Consumer<AppEvent> handler : handlers) {
                    try {
                        handler.accept(event);
                    } catch (Exception e) {
                        System.err.println("Error handling event: " + event.getType() + ", " + e.getMessage());
                    }
                }
            } else {
                Platform.runLater(() -> {
                    for (Consumer<AppEvent> handler : handlers) {
                        try {
                            handler.accept(event);
                        } catch (Exception e) {
                            System.err.println("Error handling event: " + event.getType() + ", " + e.getMessage());
                        }
                    }
                });
            }
        }
    }
    
    public void clearAllSubscribers() {
        subscribers.clear();
    }
    
    public int getSubscriberCount(AppEventType type) {
        List<Consumer<AppEvent>> handlers = subscribers.get(type);
        return handlers != null ? handlers.size() : 0;
    }
}
