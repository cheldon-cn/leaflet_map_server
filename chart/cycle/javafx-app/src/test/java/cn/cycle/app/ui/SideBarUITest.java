package cn.cycle.app.ui;

import cn.cycle.app.sidebar.*;
import org.junit.Before;
import org.junit.Test;
import static org.junit.Assert.*;
import javafx.scene.Node;
import javafx.scene.layout.BorderPane;

public class SideBarUITest {
    
    @Before
    public void setUp() {
    }
    
    @Test
    public void testSideBarPanelIconSize() {
        DataCatalogPanel panel = new DataCatalogPanel();
        Node icon = panel.getIcon();
        
        assertNotNull(icon);
        assertTrue(icon.prefWidth(-1) > 0);
        assertTrue(icon.prefHeight(-1) > 0);
    }
    
    @Test
    public void testSideBarPanelContentSize() {
        DataCatalogPanel panel = new DataCatalogPanel();
        Node content = panel.createContent();
        
        assertNotNull(content);
        assertTrue(content instanceof BorderPane);
    }
    
    @Test
    public void testAllPanelsHaveValidIcons() {
        SideBarPanel[] panels = {
            new DataCatalogPanel(),
            new DataConvertPanel(),
            new QueryPanel(),
            new StylePanel()
        };
        
        for (SideBarPanel panel : panels) {
            Node icon = panel.getIcon();
            assertNotNull("Panel " + panel.getId() + " should have icon", icon);
        }
    }
    
    @Test
    public void testAllPanelsHaveValidContent() {
        SideBarPanel[] panels = {
            new DataCatalogPanel(),
            new DataConvertPanel(),
            new QueryPanel(),
            new StylePanel()
        };
        
        for (SideBarPanel panel : panels) {
            Node content = panel.createContent();
            assertNotNull("Panel " + panel.getId() + " should have content", content);
        }
    }
    
    @Test
    public void testPanelContentReuse() {
        DataCatalogPanel panel = new DataCatalogPanel();
        
        Node content1 = panel.createContent();
        Node content2 = panel.createContent();
        
        assertSame("Content should be cached", content1, content2);
    }
}
