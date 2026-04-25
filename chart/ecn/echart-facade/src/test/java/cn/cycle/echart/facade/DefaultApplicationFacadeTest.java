package cn.cycle.echart.facade;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class DefaultApplicationFacadeTest {

    private DefaultApplicationFacade facade;

    @Before
    public void setUp() {
        facade = new DefaultApplicationFacade();
    }

    @Test
    public void testInitialize() throws FacadeException {
        assertFalse(facade.isInitialized());
        
        facade.initialize();
        
        assertTrue(facade.isInitialized());
    }

    @Test
    public void testStart() throws FacadeException {
        facade.initialize();
        
        assertFalse(facade.isRunning());
        
        facade.start();
        
        assertTrue(facade.isRunning());
    }

    @Test
    public void testStop() throws FacadeException {
        facade.initialize();
        facade.start();
        
        facade.stop();
        
        assertFalse(facade.isRunning());
    }

    @Test
    public void testDestroy() throws FacadeException {
        facade.initialize();
        facade.start();
        
        facade.destroy();
        
        assertFalse(facade.isInitialized());
        assertFalse(facade.isRunning());
    }

    @Test
    public void testGetAlarmFacade() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getAlarmFacade());
    }

    @Test
    public void testGetRouteFacade() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getRouteFacade());
    }

    @Test
    public void testGetWorkspaceFacade() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getWorkspaceFacade());
    }

    @Test
    public void testGetAisFacade() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getAisFacade());
    }

    @Test
    public void testGetLoadedCharts() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getLoadedCharts());
    }

    @Test
    public void testGetLoadedChartsBeforeInit() {
        assertNotNull(facade.getLoadedCharts());
        assertTrue(facade.getLoadedCharts().isEmpty());
    }

    @Test(expected = FacadeException.class)
    public void testStartWithoutInitialize() throws FacadeException {
        facade.start();
    }
}
