package cn.cycle.echart.event;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import org.junit.Before;
import org.junit.After;
import org.junit.Test;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;

import static org.junit.Assert.*;

public class EventDispatcherTest {

    private DefaultEventBus eventBus;
    private EventDispatcher dispatcher;

    @Before
    public void setUp() {
        eventBus = new DefaultEventBus();
        dispatcher = new EventDispatcher(eventBus);
    }

    @After
    public void tearDown() {
        dispatcher.shutdown();
    }

    @Test
    public void testDispatch() {
        AtomicInteger counter = new AtomicInteger(0);
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            counter.incrementAndGet();
        });
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "test");
        dispatcher.dispatch(event);
        
        assertEquals(1, counter.get());
    }

    @Test
    public void testDispatchAsync() throws InterruptedException {
        AtomicBoolean received = new AtomicBoolean(false);
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            received.set(true);
        });
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "test");
        dispatcher.dispatchAsync(event);
        
        Thread.sleep(100);
        
        assertTrue(received.get());
    }

    @Test
    public void testDispatchAndWait() {
        AtomicInteger counter = new AtomicInteger(0);
        eventBus.subscribe(AppEventType.ALARM_TRIGGERED, (event, data) -> {
            counter.incrementAndGet();
        });
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "test");
        boolean completed = dispatcher.dispatchAndWait(event, 1000);
        
        assertTrue(completed);
        assertEquals(1, counter.get());
    }

    @Test
    public void testShutdown() {
        assertFalse(dispatcher.isShutdown());
        
        dispatcher.shutdown();
        
        assertTrue(dispatcher.isShutdown());
    }

    @Test(expected = IllegalStateException.class)
    public void testDispatchAfterShutdown() {
        dispatcher.shutdown();
        
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "test");
        dispatcher.dispatch(event);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testDispatchNullEvent() {
        dispatcher.dispatch(null);
    }
}
