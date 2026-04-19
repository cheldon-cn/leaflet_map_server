package cn.cycle.echart.ui.render;

import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;

import javafx.scene.canvas.Canvas;

public class FxRenderContextTest {

    private Canvas canvas;
    private FxRenderContext context;

    @Before
    public void setUp() {
        canvas = new Canvas(800, 600);
        context = new FxRenderContext(canvas);
    }

    @Test
    public void testGetWidth() {
        assertEquals(800, context.getWidth());
    }

    @Test
    public void testGetHeight() {
        assertEquals(600, context.getHeight());
    }

    @Test
    public void testGetZoom() {
        assertEquals(1.0, context.getZoom(), 0.001);
    }

    @Test
    public void testSetZoom() {
        context.setZoom(2.0);
        assertEquals(2.0, context.getZoom(), 0.001);
    }

    @Test
    public void testSetZoomClamped() {
        context.setZoom(0.05);
        assertEquals(0.1, context.getZoom(), 0.001);
        
        context.setZoom(150.0);
        assertEquals(100.0, context.getZoom(), 0.001);
    }

    @Test
    public void testNeedsRender() {
        assertTrue(context.needsRender());
    }

    @Test
    public void testClearDirty() {
        context.clearDirty();
        assertFalse(context.needsRender());
    }

    @Test
    public void testMarkDirty() {
        context.clearDirty();
        context.markDirty();
        assertTrue(context.needsRender());
    }

    @Test
    public void testGetDevicePixelRatio() {
        assertEquals(1.0, context.getDevicePixelRatio(), 0.001);
    }

    @Test
    public void testSetDevicePixelRatio() {
        context.setDevicePixelRatio(2.0);
        assertEquals(2.0, context.getDevicePixelRatio(), 0.001);
    }

    @Test
    public void testGetGraphicsContext() {
        assertNotNull(context.getGraphicsContext());
    }

    @Test
    public void testGetCanvas() {
        assertSame(canvas, context.getCanvas());
    }

    @Test
    public void testResize() {
        context.resize(1024, 768);
        assertEquals(1024, context.getWidth());
        assertEquals(768, context.getHeight());
    }
}
