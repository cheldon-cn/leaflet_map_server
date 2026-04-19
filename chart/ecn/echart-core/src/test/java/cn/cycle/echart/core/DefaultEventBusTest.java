package cn.cycle.echart.core;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;

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
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "test-data");
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
        EventHandler<String> handler = (event, data) -> {
            counter.incrementAndGet();
        };
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, handler);
        eventBus.unsubscribe(AppEventType.ALARM_TRIGGERED, handler);
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED);
        eventBus.publish(event);
        
        assertEquals(0, counter.get());
    }

    @Test
    public void testSubscribeAll() {
        AtomicReference<AppEventType> receivedType = new AtomicReference<>();
        
        eventBus.subscribeAll((event, data) -> {
            receivedType.set(event.getType());
        });
        
        eventBus.publish(new AppEvent(this, AppEventType.ALARM_TRIGGERED));
        assertEquals(AppEventType.ALARM_TRIGGERED, receivedType.get());
        
        eventBus.publish(new AppEvent(this, AppEventType.CHART_LOADED));
        assertEquals(AppEventType.CHART_LOADED, receivedType.get());
    }

    @Test
    public void testDataRetrieval() {
        AtomicReference<String> receivedData = new AtomicReference<>();
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            if (data != null) {
                receivedData.set(data.toString());
            }
        });
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "alarm-data");
        eventBus.publish(event);
        
        assertEquals("alarm-data", receivedData.get());
    }

    @Test
    public void testGetSubscriberCount() {
        assertEquals(0, eventBus.getSubscriberCount(AppEventType.ALARM_TRIGGERED));
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {});
        assertEquals(1, eventBus.getSubscriberCount(AppEventType.ALARM_TRIGGERED));
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {});
        assertEquals(2, eventBus.getSubscriberCount(AppEventType.ALARM_TRIGGERED));
    }

    @Test
    public void testGetTotalSubscriberCount() {
        assertEquals(0, eventBus.getTotalSubscriberCount());
        
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {});
        eventBus.subscribe(AppEventType.CHART_LOADED, (event, data) -> {});
        eventBus.subscribeAll((event, data) -> {});
        
        assertEquals(3, eventBus.getTotalSubscriberCount());
    }

    @Test
    public void testClear() {
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {});
        eventBus.subscribeAll((event, data) -> {});
        
        eventBus.clear();
        
        assertEquals(0, eventBus.getTotalSubscriberCount());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testPublishNullEvent() {
        eventBus.publish(null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSubscribeNullType() {
        eventBus.subscribe(null, (event, data) -> {});
    }

    @Test(expected = IllegalArgumentException.class)
    public void testSubscribeNullHandler() {
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, null);
    }

    @Test
    public void testAsyncMode() {
        DefaultEventBus asyncBus = new DefaultEventBus(true);
        assertTrue(asyncBus.isAsyncMode());
        
        AtomicInteger counter = new AtomicInteger(0);
        asyncBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            counter.incrementAndGet();
        });
        
        asyncBus.publish(new AppEvent(this, AppEventType.ALARM_TRIGGERED));
        
        try {
            Thread.sleep(100);
        } catch (InterruptedException e) {
            Thread.currentThread().interrupt();
        }
        
        assertEquals(1, counter.get());
        asyncBus.shutdown();
    }
}
