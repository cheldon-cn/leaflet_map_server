package cn.cycle.app.integration;

import cn.cycle.app.sidebar.*;
import org.junit.Test;
import static org.junit.Assert.*;

public class SideBarIntegrationTest {
    
    @Test
    public void testAllPanelsCreation() {
        DataCatalogPanel catalogPanel = new DataCatalogPanel();
        DataConvertPanel convertPanel = new DataConvertPanel();
        QueryPanel queryPanel = new QueryPanel();
        StylePanel stylePanel = new StylePanel();
        
        assertNotNull(catalogPanel);
        assertNotNull(convertPanel);
        assertNotNull(queryPanel);
        assertNotNull(stylePanel);
    }
    
    @Test
    public void testPanelIds() {
        assertEquals("data-catalog", new DataCatalogPanel().getId());
        assertEquals("data-convert", new DataConvertPanel().getId());
        assertEquals("query", new QueryPanel().getId());
        assertEquals("style", new StylePanel().getId());
    }
    
    @Test
    public void testPanelTitles() {
        assertEquals("数据目录", new DataCatalogPanel().getTitle());
        assertEquals("数据转换", new DataConvertPanel().getTitle());
        assertEquals("数据查询", new QueryPanel().getTitle());
        assertEquals("符号样式", new StylePanel().getTitle());
    }
    
    @Test
    public void testPanelIcons() {
        DataCatalogPanel catalogPanel = new DataCatalogPanel();
        DataConvertPanel convertPanel = new DataConvertPanel();
        QueryPanel queryPanel = new QueryPanel();
        StylePanel stylePanel = new StylePanel();
        
        assertNotNull(catalogPanel.getIcon());
        assertNotNull(convertPanel.getIcon());
        assertNotNull(queryPanel.getIcon());
        assertNotNull(stylePanel.getIcon());
    }
    
    @Test
    public void testPanelContentCreation() {
        DataCatalogPanel catalogPanel = new DataCatalogPanel();
        DataConvertPanel convertPanel = new DataConvertPanel();
        QueryPanel queryPanel = new QueryPanel();
        StylePanel stylePanel = new StylePanel();
        
        assertNotNull(catalogPanel.createContent());
        assertNotNull(convertPanel.createContent());
        assertNotNull(queryPanel.createContent());
        assertNotNull(stylePanel.createContent());
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
        
        String stateKey = panel.getStateKey();
        assertEquals("sidebar.panel.data-catalog", stateKey);
        
        assertNotNull(panel.saveState());
        
        panel.restoreState(null);
    }
    
    @Test
    public void testMultiplePanelsLifecycle() {
        SideBarPanel[] panels = {
            new DataCatalogPanel(),
            new DataConvertPanel(),
            new QueryPanel(),
            new StylePanel()
        };
        
        for (SideBarPanel panel : panels) {
            panel.initialize();
            assertTrue(panel.isInitialized());
        }
        
        for (SideBarPanel panel : panels) {
            panel.activate();
            assertTrue(panel.isActive());
        }
        
        for (SideBarPanel panel : panels) {
            panel.deactivate();
            assertFalse(panel.isActive());
        }
        
        for (SideBarPanel panel : panels) {
            panel.dispose();
            assertFalse(panel.isInitialized());
        }
    }
}
