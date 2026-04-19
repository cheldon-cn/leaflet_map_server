package cn.cycle.echart.core;

import org.junit.Before;
import org.junit.After;
import org.junit.Test;
import static org.junit.Assert.*;

public class ServiceLocatorTest {

    @Before
    public void setUp() {
        ServiceLocator.clear();
    }

    @After
    public void tearDown() {
        ServiceLocator.clear();
    }

    @Test
    public void testRegisterAndGetService() {
        TestService service = new TestServiceImpl();
        ServiceLocator.register(TestService.class, service);
        
        TestService retrieved = ServiceLocator.getService(TestService.class);
        assertSame(service, retrieved);
    }

    @Test
    public void testRegisterFactory() {
        ServiceLocator.registerFactory(TestService.class, TestServiceImpl::new);
        
        TestService service1 = ServiceLocator.getService(TestService.class);
        TestService service2 = ServiceLocator.getService(TestService.class);
        
        assertNotNull(service1);
        assertNotNull(service2);
        assertNotSame(service1, service2);
    }

    @Test
    public void testTryGetService() {
        TestService service = ServiceLocator.tryGetService(TestService.class);
        assertNull(service);
        
        ServiceLocator.register(TestService.class, new TestServiceImpl());
        service = ServiceLocator.tryGetService(TestService.class);
        assertNotNull(service);
    }

    @Test
    public void testIsRegistered() {
        assertFalse(ServiceLocator.isRegistered(TestService.class));
        
        ServiceLocator.register(TestService.class, new TestServiceImpl());
        assertTrue(ServiceLocator.isRegistered(TestService.class));
    }

    @Test
    public void testUnregister() {
        ServiceLocator.register(TestService.class, new TestServiceImpl());
        assertTrue(ServiceLocator.isRegistered(TestService.class));
        
        ServiceLocator.unregister(TestService.class);
        assertFalse(ServiceLocator.isRegistered(TestService.class));
    }

    @Test
    public void testClear() {
        ServiceLocator.register(TestService.class, new TestServiceImpl());
        ServiceLocator.registerFactory(String.class, () -> "test");
        
        ServiceLocator.clear();
        
        assertFalse(ServiceLocator.isRegistered(TestService.class));
        assertFalse(ServiceLocator.isRegistered(String.class));
    }

    @Test(expected = IllegalArgumentException.class)
    public void testGetUnregisteredService() {
        ServiceLocator.getService(TestService.class);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testRegisterNullType() {
        ServiceLocator.register(null, new TestServiceImpl());
    }

    @Test(expected = IllegalArgumentException.class)
    public void testRegisterNullInstance() {
        ServiceLocator.register(TestService.class, null);
    }

    @Test(expected = IllegalArgumentException.class)
    public void testGetServiceNullType() {
        ServiceLocator.getService(null);
    }

    private interface TestService {
        String getName();
    }

    private static class TestServiceImpl implements TestService {
        @Override
        public String getName() {
            return "TestServiceImpl";
        }
    }
}
