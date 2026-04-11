package cn.cycle.app.sidebar;

import org.junit.Test;
import static org.junit.Assert.*;

public class SideBarPanelTest {
    
    @Test
    public void testDataCatalogPanel() {
        DataCatalogPanel panel = new DataCatalogPanel();
        
        assertEquals("data-catalog", panel.getId());
        assertEquals("数据目录", panel.getTitle());
        assertNotNull(panel.getIcon());
        assertNotNull(panel.createContent());
    }
    
    @Test
    public void testDataConvertPanel() {
        DataConvertPanel panel = new DataConvertPanel();
        
        assertEquals("data-convert", panel.getId());
        assertEquals("数据转换", panel.getTitle());
        assertNotNull(panel.getIcon());
        assertNotNull(panel.createContent());
    }
    
    @Test
    public void testQueryPanel() {
        QueryPanel panel = new QueryPanel();
        
        assertEquals("query", panel.getId());
        assertEquals("数据查询", panel.getTitle());
        assertNotNull(panel.getIcon());
        assertNotNull(panel.createContent());
    }
    
    @Test
    public void testStylePanel() {
        StylePanel panel = new StylePanel();
        
        assertEquals("style", panel.getId());
        assertEquals("符号样式", panel.getTitle());
        assertNotNull(panel.getIcon());
        assertNotNull(panel.createContent());
    }
    
    @Test
    public void testPanelLifecycle() {
        DataCatalogPanel panel = new DataCatalogPanel();
        
        assertFalse(panel.isInitialized());
        
        panel.initialize();
        assertTrue(panel.isInitialized());
        
        panel.activate();
        assertTrue(panel.isActive());
        
        panel.deactivate();
        assertFalse(panel.isActive());
        
        panel.dispose();
        assertFalse(panel.isInitialized());
    }
    
    @Test
    public void testPanelStatePersistence() {
        DataCatalogPanel panel = new DataCatalogPanel();
        
        assertEquals("sidebar.panel.data-catalog", panel.getStateKey());
        assertNotNull(panel.saveState());
        
        panel.restoreState(null);
    }
}
