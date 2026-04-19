package cn.cycle.echart.ui.render;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class MeasurementToolTest {

    private MeasurementTool tool;

    @Before
    public void setUp() {
        tool = new MeasurementTool();
    }

    @Test
    public void testSetMode() {
        tool.setMode(MeasurementTool.MeasurementMode.DISTANCE);
        assertEquals(MeasurementTool.MeasurementMode.DISTANCE, tool.getMode());
    }

    @Test
    public void testAddPoint() {
        tool.setMode(MeasurementTool.MeasurementMode.DISTANCE);
        tool.addPoint(0, 0);
        tool.addPoint(10, 0);
        
        assertEquals(2, tool.getPoints().size());
    }

    @Test
    public void testRemoveLastPoint() {
        tool.setMode(MeasurementTool.MeasurementMode.DISTANCE);
        tool.addPoint(0, 0);
        tool.addPoint(10, 0);
        
        tool.removeLastPoint();
        assertEquals(1, tool.getPoints().size());
    }

    @Test
    public void testClearPoints() {
        tool.setMode(MeasurementTool.MeasurementMode.DISTANCE);
        tool.addPoint(0, 0);
        tool.addPoint(10, 0);
        
        tool.clearPoints();
        assertEquals(0, tool.getPoints().size());
    }

    @Test
    public void testSetActive() {
        tool.setActive(true);
        assertTrue(tool.isActive());
        
        tool.setActive(false);
        assertFalse(tool.isActive());
    }
}
