package cn.cycle.echart.event;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventHandler;
import org.junit.Before;
import org.junit.Test;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;

import static org.junit.Assert.*;

public class DefaultEventBusTest {

    private DefaultEventBus eventBus;

    @Before
    public void setUp() {
        eventBus = new DefaultEventBus();
    }

    @Test
    public void testPublishAndSubscribe() {
        AtomicInteger counter = new AtomicInteger(0);
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            counter.incrementAndGet();
        });
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "test data");
        eventBus.publish(event);
        
        assertEquals(1, counter.get());
    }

    @Test
    public void testMultipleSubscribers() {
        AtomicInteger counter = new AtomicInteger(0);
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            counter.incrementAndGet();
        });
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            counter.incrementAndGet();
        });
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED);
        eventBus.publish(event);
        
        assertEquals(2, counter.get());
    }

    @Test
    public void testUnsubscribe() {
        AtomicInteger counter = new AtomicInteger(0);
        EventHandler<String> handler = (event, data) -> counter.incrementAndGet();
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, handler);
        eventBus.unsubscribe(AppEventType.ALARM_TRIGGERED, handler);
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED);
        eventBus.publish(event);
        
        assertEquals(0, counter.get());
    }

    @Test
    public void testSubscribeAll() {
        AtomicReference<Object> receivedData = new AtomicReference<>();
        
        eventBus.subscribeAll((event, data) -> {
            receivedData.set(data);
        });
        
        AppEvent event1 = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "alarm");
        AppEvent event2 = new AppEvent(this, AppEventType.CHART_LOADED, "chart");
        
        eventBus.publish(event1);
        assertEquals("alarm", receivedData.get());
        
        eventBus.publish(event2);
        assertEquals("chart", receivedData.get());
    }

    @Test
    public void testGetSubscriberCount() {
        EventHandler<String> handler1 = (event, data) -> {};
        EventHandler<String> handler2 = (event, data) -> {};
        
        assertEquals(0, eventBus.getSubscriberCount(AppEventType.ALARM_TRIGGERED));
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, handler1);
        assertEquals(1, eventBus.getSubscriberCount(AppEventType.ALARM_TRIGGERED));
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, handler2);
        assertEquals(2, eventBus.getSubscriberCount(AppEventType.ALARM_TRIGGERED));
    }

    @Test
    public void testGetTotalSubscriberCount() {
        EventHandler<String> handler1 = (event, data) -> {};
        EventHandler<String> handler2 = (event, data) -> {};
        
        assertEquals(0, eventBus.getTotalSubscriberCount());
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, handler1);
        eventBus.subscribe(AppEventType.CHART_LOADED, handler2);
        eventBus.subscribeAll((event, data) -> {});
        
        assertEquals(3, eventBus.getTotalSubscriberCount());
    }

    @Test
    public void testClear() {
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {});
        eventBus.subscribeAll((event, data) -> {});
        
        assertEquals(2, eventBus.getTotalSubscriberCount());
        
        eventBus.clear();
        
        assertEquals(0, eventBus.getTotalSubscriberCount());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testPublishNullEvent() {
        eventBus.publish(null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSubscribeNullEventType() {
        eventBus.subscribe(null, (event, data) -> {});
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSubscribeNullHandler() {
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, null);
    }
}
