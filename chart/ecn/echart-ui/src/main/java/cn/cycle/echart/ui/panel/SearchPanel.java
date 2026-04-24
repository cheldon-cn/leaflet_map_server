package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxSideBarPanel;
import javafx.fxml.FXMLLoader;
import javafx.scene.Node;
import javafx.scene.image.Image;

import java.io.IOException;
import java.util.List;
import java.util.Map;
import java.util.function.Consumer;

/**
 * 搜索面板。
 * 
 * <p>提供海图数据、航线、AIS等多种搜索功能。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class SearchPanel implements FxSideBarPanel {

    private static final String PANEL_ID = "search-panel";
    
    private Node content;
    private SearchPanelController controller;
    
    public SearchPanel() {
        loadFXML();
    }
    
    private void loadFXML() {
        try {
            FXMLLoader loader = new FXMLLoader(getClass().getResource("/fxml/SearchPanel.fxml"));
            content = loader.load();
            controller = loader.getController();
        } catch (IOException e) {
            throw new RuntimeException("Failed to load SearchPanel.fxml", e);
        }
    }

    @Override
    public String getPanelId() {
        return PANEL_ID;
    }

    @Override
    public String getTitle() {
        return "搜索";
    }

    @Override
    public Image getIconImage() {
        return null;
    }

    @Override
    public Node getContent() {
        return content;
    }

    @Override
    public void onActivate() {
        if (controller != null) {
            controller.onActivate();
        }
    }

    @Override
    public void onDeactivate() {
        if (controller != null) {
            controller.onDeactivate();
        }
    }
    
    public SearchPanelController getController() {
        return controller;
    }
    
    public void search(String keyword) {
        if (controller != null) {
            controller.search(keyword);
        }
    }
    
    public void setSearchType(SearchPanelController.SearchType type) {
        if (controller != null) {
            controller.setSearchType(type);
        }
    }
    
    public void clearResults() {
        if (controller != null) {
            controller.clearResults();
        }
    }
    
    public void setOnSearchResultSelected(Consumer<SearchPanelController.SearchResult> callback) {
        if (controller != null) {
            controller.setOnSearchResultSelected(callback);
        }
    }
    
    public void setOnHidePanel(Runnable callback) {
        if (controller != null) {
            controller.setOnHidePanel(callback);
        }
    }
    
    public void addSearchResult(SearchPanelController.SearchResult result) {
        if (controller != null) {
            controller.addSearchResult(result);
        }
    }
    
    public void setSearchResults(List<SearchPanelController.SearchResult> results) {
        if (controller != null) {
            controller.setSearchResults(results);
        }
    }
}
