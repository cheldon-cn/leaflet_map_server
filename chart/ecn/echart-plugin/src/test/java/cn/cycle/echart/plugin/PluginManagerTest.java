package cn.cycle.echart.plugin;

import cn.cycle.echart.core.ServiceLocator;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

public class PluginManagerTest {

    private PluginManager manager;
    private ServiceLocator serviceLocator;

    @Before
    public void setUp() {
        serviceLocator = new ServiceLocator();
        manager = new PluginManager(serviceLocator);
    }

    @Test
    public void testRegisterPlugin() throws PluginException {
        Plugin plugin = new TestPlugin("test-1", "Test Plugin");
        manager.registerPlugin(plugin);
        
        assertTrue(manager.hasPlugin("test-1"));
        assertEquals(1, manager.getPluginCount());
    }

    @Test(expected = PluginException.class)
    public void testRegisterDuplicatePlugin() throws PluginException {
        Plugin plugin1 = new TestPlugin("test-1", "Test Plugin");
        Plugin plugin2 = new TestPlugin("test-1", "Another Plugin");
        
        manager.registerPlugin(plugin1);
        manager.registerPlugin(plugin2);
    }

    @Test
    public void testUnregisterPlugin() throws PluginException {
        Plugin plugin = new TestPlugin("test-1", "Test Plugin");
        manager.registerPlugin(plugin);
        
        manager.unregisterPlugin("test-1");
        
        assertFalse(manager.hasPlugin("test-1"));
    }

    @Test
    public void testInitializePlugin() throws PluginException {
        Plugin plugin = new TestPlugin("test-1", "Test Plugin");
        manager.registerPlugin(plugin);
        
        manager.initializePlugin("test-1");
        
        assertEquals(PluginState.INITIALIZED, plugin.getState());
    }

    @Test
    public void testStartPlugin() throws PluginException {
        Plugin plugin = new TestPlugin("test-1", "Test Plugin");
        manager.registerPlugin(plugin);
        manager.initializePlugin("test-1");
        
        manager.startPlugin("test-1");
        
        assertEquals(PluginState.STARTED, plugin.getState());
    }

    @Test
    public void testStopPlugin() throws PluginException {
        Plugin plugin = new TestPlugin("test-1", "Test Plugin");
        manager.registerPlugin(plugin);
        manager.initializePlugin("test-1");
        manager.startPlugin("test-1");
        
        manager.stopPlugin("test-1");
        
        assertEquals(PluginState.STOPPED, plugin.getState());
    }

    @Test
    public void testGetPlugins() throws PluginException {
        Plugin plugin1 = new TestPlugin("test-1", "Test Plugin 1");
        Plugin plugin2 = new TestPlugin("test-2", "Test Plugin 2");
        
        manager.registerPlugin(plugin1);
        manager.registerPlugin(plugin2);
        
        assertEquals(2, manager.getPlugins().size());
    }

    @Test
    public void testGetProperty() {
        manager.setProperty("test.key", "test.value");
        
        assertEquals("test.value", manager.getProperty("test.key"));
    }

    @Test
    public void testGetPropertyWithDefault() {
        Object value = manager.getProperty("nonexistent", "default");
        
        assertEquals("default", value);
    }

    private static class TestPlugin extends AbstractPlugin {
        TestPlugin(String id, String name) {
            super(id, name, "1.0.0", "Test plugin", "Test", null);
        }
    }
}
