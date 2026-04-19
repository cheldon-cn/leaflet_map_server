package cn.cycle.echart.ui.render;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class DistanceMeasurementTest {

    private DistanceMeasurement measurement;

    @Before
    public void setUp() {
        measurement = new DistanceMeasurement();
    }

    @Test
    public void testModeIsDistance() {
        assertEquals(MeasurementTool.MeasurementMode.DISTANCE, measurement.getMode());
    }

    @Test
    public void testDistanceCalculation() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        
        assertEquals(10.0, measurement.getTotalDistance(), 0.001);
    }

    @Test
    public void testMultiplePointsDistance() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        measurement.addPoint(10, 10);
        
        assertEquals(20.0, measurement.getTotalDistance(), 0.001);
    }

    @Test
    public void testGetDistanceInKilometers() {
        measurement.addPoint(0, 0);
        measurement.addPoint(1, 0);
        
        double km = measurement.getDistanceInKilometers();
        assertEquals(1.852, km, 0.001);
    }

    @Test
    public void testGetDistanceInMeters() {
        measurement.addPoint(0, 0);
        measurement.addPoint(1, 0);
        
        double meters = measurement.getDistanceInMeters();
        assertEquals(1852.0, meters, 0.001);
    }

    @Test
    public void testClearPoints() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        
        measurement.clearPoints();
        assertEquals(0, measurement.getTotalDistance(), 0.001);
    }
}
