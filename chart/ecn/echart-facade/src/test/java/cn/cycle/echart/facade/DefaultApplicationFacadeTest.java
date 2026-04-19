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
    public void testGetChartFileManager() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getChartFileManager());
    }

    @Test
    public void testGetAlarmManager() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getAlarmManager());
    }

    @Test
    public void testGetAisTargetManager() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getAisTargetManager());
    }

    @Test
    public void testGetRouteManager() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getRouteManager());
    }

    @Test
    public void testGetWorkspaceManager() throws FacadeException {
        facade.initialize();
        
        assertNotNull(facade.getWorkspaceManager());
    }

    @Test(expected = FacadeException.class)
    public void testStartWithoutInitialize() throws FacadeException {
        facade.start();
    }
}
