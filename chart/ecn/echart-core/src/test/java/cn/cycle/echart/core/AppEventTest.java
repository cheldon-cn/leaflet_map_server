package cn.cycle.echart.core;

import org.junit.Test;
import static org.junit.Assert.*;

public class AppEventTest {

    @Test
    public void testCreateEvent() {
        Object source = new Object();
        AppEvent event = new AppEvent(source, AppEventType.ALARM_TRIGGERED, "test-data");
        
        assertSame(source, event.getSource());
        assertEquals(AppEventType.ALARM_TRIGGERED, event.getType());
        assertEquals("test-data", event.getData());
        assertTrue(event.getTimestamp() > 0);
        assertNotNull(event.getSourceComponent());
    }

    @Test
    public void testCreateEventWithoutData() {
        Object source = new Object();
        AppEvent event = new AppEvent(source, AppEventType.CHART_LOADED);
        
        assertSame(source, event.getSource());
        assertEquals(AppEventType.CHART_LOADED, event.getType());
        assertNull(event.getData());
    }

    @Test
    public void testGetDataWithType() {
        String testData = "test-string";
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, testData);
        
        String data = event.getData(String.class);
        assertEquals("test-string", data);
    }

    @Test
    public void testGetDataWithWrongType() {
        String testData = "test-string";
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, testData);
        
        Integer data = event.getData(Integer.class);
        assertNull(data);
    }

    @Test
    public void testGetDataWithNullData() {
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED);
        
        String data = event.getData(String.class);
        assertNull(data);
    }

    @Test
    public void testGetSourceComponent() {
        TestSource source = new TestSource();
        AppEvent event = new AppEvent(source, AppEventType.ALARM_TRIGGERED);
        
        assertEquals("TestSource", event.getSourceComponent());
    }

    @Test
    public void testToString() {
        AppEvent event = new AppEvent(this, AppEventType.ALARM_TRIGGERED, "data");
        String str = event.toString();
        
        assertTrue(str.contains("ALARM_TRIGGERED"));
        assertTrue(str.contains("AppEventTest"));
    }

    @Test(expected = IllegalArgumentException.class)
    public void testNullType() {
        new AppEvent(this, null);
    }

    private static class TestSource {
    }
}
