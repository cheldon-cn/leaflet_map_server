package cn.cycle.echart.plugin;

import org.junit.Test;
import static org.junit.Assert.*;

public class PluginStateTest {

    @Test
    public void testIsRunning() {
        assertTrue(PluginState.STARTED.isRunning());
        assertFalse(PluginState.STOPPED.isRunning());
        assertFalse(PluginState.INITIALIZED.isRunning());
    }

    @Test
    public void testIsStopped() {
        assertTrue(PluginState.STOPPED.isStopped());
        assertTrue(PluginState.DESTROYED.isStopped());
        assertTrue(PluginState.ERROR.isStopped());
        assertFalse(PluginState.STARTED.isStopped());
    }

    @Test
    public void testCanStart() {
        assertTrue(PluginState.INITIALIZED.canStart());
        assertTrue(PluginState.STOPPED.canStart());
        assertFalse(PluginState.STARTED.canStart());
        assertFalse(PluginState.CREATED.canStart());
    }

    @Test
    public void testCanStop() {
        assertTrue(PluginState.STARTED.canStop());
        assertFalse(PluginState.STOPPED.canStop());
        assertFalse(PluginState.INITIALIZED.canStop());
    }

    @Test
    public void testGetDisplayName() {
        assertEquals("已创建", PluginState.CREATED.getDisplayName());
        assertEquals("已启动", PluginState.STARTED.getDisplayName());
    }
}
