package cn.cycle.echart.ui.render;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class BearingMeasurementTest {

    private BearingMeasurement measurement;

    @Before
    public void setUp() {
        measurement = new BearingMeasurement();
    }

    @Test
    public void testModeIsBearing() {
        assertEquals(MeasurementTool.MeasurementMode.BEARING, measurement.getMode());
    }

    @Test
    public void testBearingCalculationEast() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        
        assertEquals(90.0, measurement.getBearingInDegrees(), 0.001);
    }

    @Test
    public void testBearingCalculationNorth() {
        measurement.addPoint(0, 0);
        measurement.addPoint(0, 10);
        
        assertEquals(0.0, measurement.getBearingInDegrees(), 0.001);
    }

    @Test
    public void testBearingCalculationSouth() {
        measurement.addPoint(0, 0);
        measurement.addPoint(0, -10);
        
        assertEquals(180.0, measurement.getBearingInDegrees(), 0.001);
    }

    @Test
    public void testBearingCalculationWest() {
        measurement.addPoint(0, 0);
        measurement.addPoint(-10, 0);
        
        assertEquals(270.0, measurement.getBearingInDegrees(), 0.001);
    }

    @Test
    public void testGetCompassDirection() {
        assertEquals("N", measurement.getCompassDirection(0));
        assertEquals("E", measurement.getCompassDirection(90));
        assertEquals("S", measurement.getCompassDirection(180));
        assertEquals("W", measurement.getCompassDirection(270));
    }

    @Test
    public void testGetReciprocalBearing() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        
        assertEquals(270.0, measurement.getReciprocalBearing(), 0.001);
    }

    @Test
    public void testGetRelativeBearing() {
        measurement.addPoint(0, 0);
        measurement.addPoint(10, 0);
        
        double relative = measurement.getRelativeBearing(45);
        assertEquals(45.0, relative, 0.001);
    }

    @Test
    public void testGetQuadrant() {
        measurement.addPoint(0, 0);
        
        measurement.addPoint(10, 10);
        assertEquals(1, measurement.getQuadrant());
        
        measurement.removeLastPoint();
        measurement.addPoint(-10, 10);
        assertEquals(2, measurement.getQuadrant());
        
        measurement.removeLastPoint();
        measurement.addPoint(-10, -10);
        assertEquals(3, measurement.getQuadrant());
        
        measurement.removeLastPoint();
        measurement.addPoint(10, -10);
        assertEquals(4, measurement.getQuadrant());
    }
}
