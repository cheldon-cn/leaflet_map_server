package cn.cycle.echart.core;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class AbstractLifecycleComponentTest {

    private TestComponent component;

    @Before
    public void setUp() {
        component = new TestComponent("test-component");
    }

    @Test
    public void testInitialState() {
        assertEquals("test-component", component.getName());
        assertEquals(LifecycleComponent.State.CREATED, component.getState());
        assertFalse(component.isInitialized());
        assertFalse(component.isRunning());
        assertFalse(component.isDisposed());
    }

    @Test
    public void testInitialize() {
        component.initialize();
        assertEquals(LifecycleComponent.State.INITIALIZED, component.getState());
        assertTrue(component.isInitialized());
        assertFalse(component.isRunning());
        assertEquals(1, component.getInitializeCount());
    }

    @Test
    public void testStart() {
        component.initialize();
        component.start();
        assertEquals(LifecycleComponent.State.STARTED, component.getState());
        assertTrue(component.isInitialized());
        assertTrue(component.isRunning());
        assertEquals(1, component.getStartCount());
    }

    @Test
    public void testStop() {
        component.initialize();
        component.start();
        component.stop();
        assertEquals(LifecycleComponent.State.STOPPED, component.getState());
        assertTrue(component.isInitialized());
        assertFalse(component.isRunning());
        assertEquals(1, component.getStopCount());
    }

    @Test
    public void testDispose() {
        component.initialize();
        component.start();
        component.dispose();
        assertEquals(LifecycleComponent.State.DISPOSED, component.getState());
        assertTrue(component.isDisposed());
        assertEquals(1, component.getDisposeCount());
    }

    @Test
    public void testDisposeFromCreated() {
        component.dispose();
        assertEquals(LifecycleComponent.State.DISPOSED, component.getState());
        assertTrue(component.isDisposed());
    }

    @Test
    public void testDisposeFromStarted() {
        component.initialize();
        component.start();
        component.dispose();
        assertEquals(LifecycleComponent.State.DISPOSED, component.getState());
        assertTrue(component.isDisposed());
        assertEquals(1, component.getStopCount());
        assertEquals(1, component.getDisposeCount());
    }

    @Test(expected = IllegalStateException.class)
    public void testStartWithoutInitialize() {
        component.start();
    }

    @Test(expected = IllegalStateException.class)
    public void testStopWithoutStart() {
        component.initialize();
        component.stop();
    }

    @Test(expected = IllegalArgumentException.class)
    public void testNullName() {
        new TestComponent(null);
    }

    @Test
    public void testRestart() {
        component.initialize();
        component.start();
        component.stop();
        component.start();
        assertEquals(LifecycleComponent.State.STARTED, component.getState());
        assertTrue(component.isRunning());
        assertEquals(2, component.getStartCount());
    }

    private static class TestComponent extends AbstractLifecycleComponent {
        private int initializeCount = 0;
        private int startCount = 0;
        private int stopCount = 0;
        private int disposeCount = 0;

        TestComponent(String name) {
            super(name);
        }

        @Override
        protected void doInitialize() {
            initializeCount++;
        }

        @Override
        protected void doStart() {
            startCount++;
        }

        @Override
        protected void doStop() {
            stopCount++;
        }

        @Override
        protected void doDispose() {
            disposeCount++;
        }

        public int getInitializeCount() {
            return initializeCount;
        }

        public int getStartCount() {
            return startCount;
        }

        public int getStopCount() {
            return stopCount;
        }

        public int getDisposeCount() {
            return disposeCount;
        }
    }
}
