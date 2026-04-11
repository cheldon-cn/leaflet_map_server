package cn.cycle.app.event;

import org.junit.Test;
import static org.junit.Assert.*;

public class AppEventTest {
    
    @Test
    public void testCreateEvent() {
        AppEvent event = new AppEvent(AppEventType.CHART_LOADED, this);
        
        assertEquals(AppEventType.CHART_LOADED, event.getType());
        assertEquals(this, event.getSource());
    }
    
    @Test
    public void testEventWithData() {
        AppEvent event = new AppEvent(AppEventType.CHART_LOADED, this)
            .withData("fileName", "test.000")
            .withData("size", Long.valueOf(1024L));
        
        assertEquals("test.000", event.getData("fileName"));
        assertEquals(Long.valueOf(1024L), event.getData("size"));
    }
    
    @Test
    public void testEventWithNullData() {
        AppEvent event = new AppEvent(AppEventType.CHART_LOADED, this);
        
        assertNull(event.getData("nonexistent"));
    }
    
    @Test
    public void testHasData() {
        AppEvent event = new AppEvent(AppEventType.CHART_LOADED, this)
            .withData("fileName", "test.000");
        
        assertTrue(event.hasData("fileName"));
        assertFalse(event.hasData("nonexistent"));
    }
    
    @Test
    public void testGetDataMap() {
        AppEvent event = new AppEvent(AppEventType.CHART_LOADED, this)
            .withData("key1", "value1");
        
        assertNotNull(event.getData());
        assertTrue(event.getData().containsKey("key1"));
    }
}
