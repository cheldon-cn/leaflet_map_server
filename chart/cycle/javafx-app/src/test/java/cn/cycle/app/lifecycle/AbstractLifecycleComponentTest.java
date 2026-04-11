package cn.cycle.app.lifecycle;

import org.junit.Test;
import static org.junit.Assert.*;

public class AbstractLifecycleComponentTest {
    
    private static class TestComponent extends AbstractLifecycleComponent {
        private boolean initialized = false;
        private boolean activated = false;
        
        @Override
        protected void doInitialize() {
            initialized = true;
        }
        
        @Override
        protected void doActivate() {
            activated = true;
        }
        
        @Override
        protected void doDeactivate() {
            activated = false;
        }
        
        @Override
        protected void doDispose() {
            initialized = false;
            activated = false;
        }
        
        public boolean isInitializedFlag() {
            return initialized;
        }
        
        public boolean isActivatedFlag() {
            return activated;
        }
    }
    
    @Test
    public void testInitialize() {
        TestComponent component = new TestComponent();
        
        assertFalse(component.isInitialized());
        assertFalse(component.isInitializedFlag());
        
        component.initialize();
        
        assertTrue(component.isInitialized());
        assertTrue(component.isInitializedFlag());
    }
    
    @Test
    public void testActivate() {
        TestComponent component = new TestComponent();
        component.initialize();
        
        assertFalse(component.isActive());
        assertFalse(component.isActivatedFlag());
        
        component.activate();
        
        assertTrue(component.isActive());
        assertTrue(component.isActivatedFlag());
    }
    
    @Test
    public void testDeactivate() {
        TestComponent component = new TestComponent();
        component.initialize();
        component.activate();
        
        component.deactivate();
        
        assertFalse(component.isActive());
        assertFalse(component.isActivatedFlag());
    }
    
    @Test
    public void testDispose() {
        TestComponent component = new TestComponent();
        component.initialize();
        component.activate();
        
        component.dispose();
        
        assertFalse(component.isInitialized());
        assertFalse(component.isActive());
        assertFalse(component.isInitializedFlag());
        assertFalse(component.isActivatedFlag());
    }
    
    @Test
    public void testIdempotentInitialize() {
        TestComponent component = new TestComponent();
        
        component.initialize();
        component.initialize();
        
        assertTrue(component.isInitialized());
    }
}
