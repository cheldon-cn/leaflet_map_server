package cn.cycle.app.event;

import org.junit.Test;
import static org.junit.Assert.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;

public class AppEventBusTest {
    
    @Test
    public void testSubscribeAndPublish() {
        AppEventBus bus = AppEventBus.getInstance();
        AtomicInteger counter = new AtomicInteger(0);
        
        Consumer<AppEvent> handler = event -> counter.incrementAndGet();
        bus.subscribe(AppEventType.CHART_LOADED, handler);
        
        bus.publish(new AppEvent(AppEventType.CHART_LOADED, this));
        
        assertEquals(1, counter.get());
        bus.unsubscribe(AppEventType.CHART_LOADED, handler);
    }
    
    @Test
    public void testUnsubscribe() {
        AppEventBus bus = AppEventBus.getInstance();
        AtomicInteger counter = new AtomicInteger(0);
        
        Consumer<AppEvent> handler = event -> counter.incrementAndGet();
        bus.subscribe(AppEventType.CHART_LOADED, handler);
        bus.unsubscribe(AppEventType.CHART_LOADED, handler);
        
        bus.publish(new AppEvent(AppEventType.CHART_LOADED, this));
        
        assertEquals(0, counter.get());
    }
    
    @Test
    public void testMultipleSubscribers() {
        AppEventBus bus = AppEventBus.getInstance();
        AtomicInteger counter = new AtomicInteger(0);
        
        Consumer<AppEvent> handler1 = event -> counter.incrementAndGet();
        Consumer<AppEvent> handler2 = event -> counter.incrementAndGet();
        
        bus.subscribe(AppEventType.THEME_CHANGED, handler1);
        bus.subscribe(AppEventType.THEME_CHANGED, handler2);
        
        bus.publish(new AppEvent(AppEventType.THEME_CHANGED, this));
        
        assertEquals(2, counter.get());
        bus.unsubscribe(AppEventType.THEME_CHANGED, handler1);
        bus.unsubscribe(AppEventType.THEME_CHANGED, handler2);
    }
    
    @Test
    public void testEventData() {
        AppEventBus bus = AppEventBus.getInstance();
        AtomicReference<Object> receivedData = new AtomicReference<>();
        
        Consumer<AppEvent> handler = event -> receivedData.set(event.getData("fileName"));
        bus.subscribe(AppEventType.CHART_LOADED, handler);
        
        AppEvent event = new AppEvent(AppEventType.CHART_LOADED, this)
            .withData("fileName", "test.000");
        bus.publish(event);
        
        assertEquals("test.000", receivedData.get());
        bus.unsubscribe(AppEventType.CHART_LOADED, handler);
    }
    
    @Test
    public void testGetInstance() {
        AppEventBus bus1 = AppEventBus.getInstance();
        AppEventBus bus2 = AppEventBus.getInstance();
        
        assertSame(bus1, bus2);
    }
    
    @Test
    public void testGetSubscriberCount() {
        AppEventBus bus = AppEventBus.getInstance();
        
        Consumer<AppEvent> handler = event -> {};
        bus.subscribe(AppEventType.VIEW_CHANGED, handler);
        
        assertTrue(bus.getSubscriberCount(AppEventType.VIEW_CHANGED) >= 1);
        bus.unsubscribe(AppEventType.VIEW_CHANGED, handler);
    }
}
