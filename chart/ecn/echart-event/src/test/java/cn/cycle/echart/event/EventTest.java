package cn.cycle.echart.event;

import org.junit.Test;

import static org.junit.Assert.*;

public class EventTest {

    @Test
    public void testCreateEvent() {
        Event<String> event = new Event<>(this, EventType.AIS_DATA_RECEIVED, "test data");
        
        assertEquals(EventType.AIS_DATA_RECEIVED, event.getType());
        assertEquals("test data", event.getData());
        assertEquals(this, event.getSource());
        assertFalse(event.isConsumed());
    }

    @Test
    public void testCreateEventWithoutData() {
        Event<Void> event = new Event<>(this, EventType.RENDER_FRAME_START);
        
        assertEquals(EventType.RENDER_FRAME_START, event.getType());
        assertNull(event.getData());
    }

    @Test
    public void testConsumeEvent() {
        Event<String> event = new Event<>(this, EventType.ALARM_CONFIG_CHANGED, "config");
        
        assertFalse(event.isConsumed());
        
        event.consume();
        
        assertTrue(event.isConsumed());
    }

    @Test
    public void testGetTimestamp() {
        long before = System.currentTimeMillis();
        Event<String> event = new Event<>(this, EventType.DATA_SOURCE_CONNECTED, "source");
        long after = System.currentTimeMillis();
        
        assertTrue(event.getTimestamp() >= before);
        assertTrue(event.getTimestamp() <= after);
    }

    @Test
    public void testToString() {
        Event<String> event = new Event<>(this, EventType.ROUTE_CALCULATED, "route");
        String str = event.toString();
        
        assertTrue(str.contains("ROUTE_CALCULATED"));
        assertTrue(str.contains("route"));
    }

    @Test(expected = IllegalArgumentException.class)
    public void testCreateEventWithNullType() {
        new Event<>(this, null, "data");
    }
}
