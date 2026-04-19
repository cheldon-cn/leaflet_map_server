package cn.cycle.echart.ui.render;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class AreaMeasurementTest {

    private AreaMeasurement measurement;

    @Before
    public void setUp() {
        measurement = new AreaMeasurement();
    }

    @Test
    public void testModeIsArea() {
        assertEquals(MeasurementTool.MeasurementMode.AREA, measurement.getMode());
    }

    @Test
    public void testAreaCalculation() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        measurement.addPoint(10, 10);
        measurement.addPoint(0, 10);
        
        assertEquals(100.0, measurement.getTotalArea(), 0.001);
    }

    @Test
    public void testTriangleArea() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        measurement.addPoint(5, 10);
        
        assertEquals(50.0, measurement.getTotalArea(), 0.001);
    }

    @Test
    public void testGetAreaInSquareKilometers() {
        measurement.addPoint(0, 0);
        measurement.addPoint(1, 0);
        measurement.addPoint(1, 1);
        measurement.addPoint(0, 1);
        
        double km2 = measurement.getAreaInSquareKilometers();
        assertEquals(3.429904, km2, 0.001);
    }

    @Test
    public void testGetPerimeter() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        measurement.addPoint(10, 10);
        measurement.addPoint(0, 10);
        
        assertEquals(40.0, measurement.getPerimeter(), 0.001);
    }

    @Test
    public void testGetCentroid() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        measurement.addPoint(10, 10);
        measurement.addPoint(0, 10);
        
        MeasurementTool.Point centroid = measurement.getCentroid();
        assertEquals(5.0, centroid.x, 0.001);
        assertEquals(5.0, centroid.y, 0.001);
    }

    @Test
    public void testIsPointInside() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        measurement.addPoint(10, 10);
        measurement.addPoint(0, 10);
        
        assertTrue(measurement.isPointInside(5, 5));
        assertFalse(measurement.isPointInside(15, 15));
    }
}
