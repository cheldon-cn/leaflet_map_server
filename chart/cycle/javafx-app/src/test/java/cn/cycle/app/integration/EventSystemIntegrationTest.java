package cn.cycle.app.integration;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;

public class EventSystemIntegrationTest {
    
    @Before
    public void setUp() {
    }
    
    @Test
    public void testEventPublishSubscribeFlow() {
        AtomicInteger receivedCount = new AtomicInteger(0);
        AtomicReference<AppEvent> receivedEvent = new AtomicReference<>();
        
        Consumer<AppEvent> handler = event -> {
            receivedCount.incrementAndGet();
            receivedEvent.set(event);
        };
        
        AppEventBus.getInstance().subscribe(AppEventType.CHART_LOADED, handler);
        
        AppEvent event = new AppEvent(AppEventType.CHART_LOADED, this)
            .withData("fileName", "test.000")
            .withData("fileSize", 1024L);
        
        AppEventBus.getInstance().publish(event);
        
        assertEquals(1, receivedCount.get());
        assertNotNull(receivedEvent.get());
        assertEquals("test.000", receivedEvent.get().getData("fileName"));
        assertEquals(Long.valueOf(1024L), receivedEvent.get().getData("fileSize"));
        
        AppEventBus.getInstance().unsubscribe(AppEventType.CHART_LOADED, handler);
    }
    
    @Test
    public void testMultipleEventTypes() {
        AtomicInteger chartCount = new AtomicInteger(0);
        AtomicInteger layerCount = new AtomicInteger(0);
        AtomicInteger themeCount = new AtomicInteger(0);
        
        Consumer<AppEvent> chartHandler = e -> chartCount.incrementAndGet();
        Consumer<AppEvent> layerHandler = e -> layerCount.incrementAndGet();
        Consumer<AppEvent> themeHandler = e -> themeCount.incrementAndGet();
        
        AppEventBus.getInstance().subscribe(AppEventType.CHART_LOADED, chartHandler);
        AppEventBus.getInstance().subscribe(AppEventType.LAYER_ADDED, layerHandler);
        AppEventBus.getInstance().subscribe(AppEventType.THEME_CHANGED, themeHandler);
        
        AppEventBus.getInstance().publish(new AppEvent(AppEventType.CHART_LOADED, this));
        AppEventBus.getInstance().publish(new AppEvent(AppEventType.LAYER_ADDED, this));
        AppEventBus.getInstance().publish(new AppEvent(AppEventType.THEME_CHANGED, this));
        AppEventBus.getInstance().publish(new AppEvent(AppEventType.CHART_LOADED, this));
        
        assertEquals(2, chartCount.get());
        assertEquals(1, layerCount.get());
        assertEquals(1, themeCount.get());
        
        AppEventBus.getInstance().unsubscribe(AppEventType.CHART_LOADED, chartHandler);
        AppEventBus.getInstance().unsubscribe(AppEventType.LAYER_ADDED, layerHandler);
        AppEventBus.getInstance().unsubscribe(AppEventType.THEME_CHANGED, themeHandler);
    }
    
    @Test
    public void testEventChain() {
        AtomicInteger counter = new AtomicInteger(0);
        
        Consumer<AppEvent> handler1 = e -> {
            counter.incrementAndGet();
            if (e.hasData("chain")) {
                AppEventBus.getInstance().publish(
                    new AppEvent(AppEventType.LAYER_ADDED, this)
                        .withData("chained", true)
                );
            }
        };
        
        Consumer<AppEvent> handler2 = e -> {
            if (e.hasData("chained")) {
                counter.addAndGet(10);
            }
        };
        
        AppEventBus.getInstance().subscribe(AppEventType.CHART_LOADED, handler1);
        AppEventBus.getInstance().subscribe(AppEventType.LAYER_ADDED, handler2);
        
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.CHART_LOADED, this)
                .withData("chain", true)
        );
        
        assertEquals(11, counter.get());
        
        AppEventBus.getInstance().unsubscribe(AppEventType.CHART_LOADED, handler1);
        AppEventBus.getInstance().unsubscribe(AppEventType.LAYER_ADDED, handler2);
    }
    
    @Test
    public void testLifecycleComponentIntegration() {
        TestComponent component = new TestComponent();
        
        assertFalse(component.isInitialized());
        assertFalse(component.isActive());
        
        component.initialize();
        assertTrue(component.isInitialized());
        
        component.activate();
        assertTrue(component.isActive());
        
        component.deactivate();
        assertFalse(component.isActive());
        
        component.dispose();
        assertFalse(component.isInitialized());
    }
    
    private static class TestComponent extends AbstractLifecycleComponent {
        @Override
        protected void doInitialize() {}
        @Override
        protected void doActivate() {}
        @Override
        protected void doDeactivate() {}
        @Override
        protected void doDispose() {}
    }
}
