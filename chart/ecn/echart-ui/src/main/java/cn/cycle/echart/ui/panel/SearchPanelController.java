package cn.cycle.echart.ui.panel;

import javafx.beans.property.SimpleStringProperty;
import javafx.beans.property.StringProperty;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;

import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.ResourceBundle;
import java.util.function.Consumer;

/**
 * 搜索面板控制器。
 * 
 * @author Cycle Team
 * @version 1.0.0
 */
public class SearchPanelController implements Initializable {

    @FXML private TextField searchField;
    @FXML private Button searchButton;
    @FXML private Button clearButton;
    @FXML private Button hideButton;
    @FXML private ComboBox<SearchType> searchTypeComboBox;
    @FXML private ListView<SearchResult> resultListView;
    @FXML private Label resultCountLabel;
    @FXML private VBox emptyPlaceholder;
    @FXML private CheckBox caseSensitiveCheckBox;
    @FXML private CheckBox wholeWordCheckBox;
    @FXML private CheckBox regexCheckBox;
    
    private ObservableList<SearchResult> searchResults;
    private Consumer<SearchResult> onSearchResultSelected;
    private Runnable onHidePanel;
    private SearchType currentSearchType = SearchType.ALL;
    
    public enum SearchType {
        ALL("全部"),
        CHART("海图数据"),
        ROUTE("航线数据"),
        AIS("AIS数据"),
        ALARM("预警区域"),
        OVERLAY("叠加图层");
        
        private final String displayName;
        
        SearchType(String displayName) {
            this.displayName = displayName;
        }
        
        @Override
        public String toString() {
            return displayName;
        }
    }
    
    public static class SearchResult {
        private final String id;
        private final String name;
        private final SearchType type;
        private final String description;
        private final String location;
        
        public SearchResult(String id, String name, SearchType type, String description, String location) {
            this.id = id;
            this.name = name;
            this.type = type;
            this.description = description;
            this.location = location;
        }
        
        public String getId() { return id; }
        public String getName() { return name; }
        public SearchType getType() { return type; }
        public String getDescription() { return description; }
        public String getLocation() { return location; }
        
        @Override
        public String toString() {
            return name;
        }
    }
    
    @Override
    public void initialize(URL location, ResourceBundle resources) {
        searchResults = FXCollections.observableArrayList();
        initializeControls();
        initializeListeners();
        showEmptyState();
    }
    
    private void initializeControls() {
        searchTypeComboBox.setItems(FXCollections.observableArrayList(SearchType.values()));
        searchTypeComboBox.setValue(SearchType.ALL);
        
        resultListView.setItems(searchResults);
        resultListView.setCellFactory(list -> new SearchResultListCell());
        
        setButtonIcon(searchButton, "/icons/search_32x32.png");
        setButtonIcon(clearButton, "/icons/close_32x32.png");
        setButtonIcon(hideButton, "/icons/sidebar_32x321.png");
    }
    
    private void setButtonIcon(Button button, String iconPath) {
        if (button != null && button.getGraphic() instanceof ImageView) {
            try {
                Image image = new Image(getClass().getResourceAsStream(iconPath));
                if (image != null) {
                    ((ImageView) button.getGraphic()).setImage(image);
                }
            } catch (Exception e) {
            }
        }
    }
    
    private void initializeListeners() {
        searchField.textProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal == null || newVal.trim().isEmpty()) {
                clearButton.setDisable(true);
            } else {
                clearButton.setDisable(false);
            }
        });
        
        searchField.setOnAction(e -> onSearch());
        
        searchTypeComboBox.valueProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal != null) {
                currentSearchType = newVal;
            }
        });
        
        resultListView.getSelectionModel().selectedItemProperty().addListener((obs, oldVal, newVal) -> {
            if (newVal != null && onSearchResultSelected != null) {
                onSearchResultSelected.accept(newVal);
            }
        });
        
        resultListView.setOnMouseClicked(event -> {
            if (event.getClickCount() == 2) {
                SearchResult selected = resultListView.getSelectionModel().getSelectedItem();
                if (selected != null && onSearchResultSelected != null) {
                    onSearchResultSelected.accept(selected);
                }
            }
        });
    }
    
    @FXML
    private void onSearch() {
        String keyword = searchField.getText();
        if (keyword == null || keyword.trim().isEmpty()) {
            return;
        }
        
        performSearch(keyword.trim());
    }
    
    private void performSearch(String keyword) {
        searchResults.clear();
        
        boolean caseSensitive = caseSensitiveCheckBox.isSelected();
        boolean wholeWord = wholeWordCheckBox.isSelected();
        boolean useRegex = regexCheckBox.isSelected();
        
        List<SearchResult> results = new ArrayList<>();
        
        for (int i = 1; i <= 5; i++) {
            String name = "示例结果 " + i;
            if (matchesKeyword(name, keyword, caseSensitive, wholeWord, useRegex)) {
                results.add(new SearchResult(
                    "result-" + i,
                    name,
                    currentSearchType,
                    "这是搜索结果描述 " + i,
                    "位置: " + (i * 10) + ", " + (i * 20)
                ));
            }
        }
        
        searchResults.addAll(results);
        updateResultCount();
        
        if (searchResults.isEmpty()) {
            showEmptyState();
        } else {
            showResultsState();
        }
    }
    
    private boolean matchesKeyword(String text, String keyword, boolean caseSensitive, boolean wholeWord, boolean useRegex) {
        if (useRegex) {
            try {
                String flags = caseSensitive ? "" : "(?i)";
                return text.matches(flags + keyword);
            } catch (Exception e) {
                return false;
            }
        }
        
        String searchText = caseSensitive ? text : text.toLowerCase();
        String searchKeyword = caseSensitive ? keyword : keyword.toLowerCase();
        
        if (wholeWord) {
            String[] words = searchText.split("\\s+");
            for (String word : words) {
                if (word.equals(searchKeyword)) {
                    return true;
                }
            }
            return false;
        }
        
        return searchText.contains(searchKeyword);
    }
    
    @FXML
    private void onClear() {
        searchField.clear();
        clearResults();
    }
    
    @FXML
    private void onHide() {
        if (onHidePanel != null) {
            onHidePanel.run();
        }
    }
    
    public void search(String keyword) {
        searchField.setText(keyword);
        onSearch();
    }
    
    public void setSearchType(SearchType type) {
        searchTypeComboBox.setValue(type);
        currentSearchType = type;
    }
    
    public void clearResults() {
        searchResults.clear();
        updateResultCount();
        showEmptyState();
    }
    
    public void addSearchResult(SearchResult result) {
        searchResults.add(result);
        updateResultCount();
        showResultsState();
    }
    
    public void setSearchResults(List<SearchResult> results) {
        searchResults.clear();
        if (results != null) {
            searchResults.addAll(results);
        }
        updateResultCount();
        
        if (searchResults.isEmpty()) {
            showEmptyState();
        } else {
            showResultsState();
        }
    }
    
    private void updateResultCount() {
        resultCountLabel.setText("找到 " + searchResults.size() + " 个结果");
    }
    
    private void showEmptyState() {
        emptyPlaceholder.setVisible(true);
        emptyPlaceholder.setManaged(true);
        resultListView.setVisible(false);
        resultListView.setManaged(false);
    }
    
    private void showResultsState() {
        emptyPlaceholder.setVisible(false);
        emptyPlaceholder.setManaged(false);
        resultListView.setVisible(true);
        resultListView.setManaged(true);
    }
    
    public void onActivate() {
        if (searchField != null) {
            searchField.requestFocus();
        }
    }
    
    public void onDeactivate() {
    }
    
    public void setOnSearchResultSelected(Consumer<SearchResult> callback) {
        this.onSearchResultSelected = callback;
    }
    
    public void setOnHidePanel(Runnable callback) {
        this.onHidePanel = callback;
    }
    
    private static class SearchResultListCell extends ListCell<SearchResult> {
        @Override
        protected void updateItem(SearchResult item, boolean empty) {
            super.updateItem(item, empty);
            
            if (empty || item == null) {
                setGraphic(null);
                setText(null);
                return;
            }
            
            VBox container = new VBox(4);
            container.setPadding(new Insets(6, 8, 6, 8));
            
            HBox header = new HBox(8);
            header.setAlignment(javafx.geometry.Pos.CENTER_LEFT);
            
            Label typeLabel = new Label(item.getType().toString());
            typeLabel.setStyle("-fx-background-color: derive(-fx-accent, 80%); -fx-background-radius: 3; " +
                             "-fx-padding: 2 6 2 6; -fx-font-size: 10px; -fx-text-fill: -fx-text-background-color;");
            
            Label nameLabel = new Label(item.getName());
            nameLabel.setStyle("-fx-font-weight: bold; -fx-font-size: 12px; -fx-text-fill: -fx-text-background-color;");
            
            header.getChildren().addAll(typeLabel, nameLabel);
            
            Label descLabel = new Label(item.getDescription());
            descLabel.setStyle("-fx-font-size: 11px; -fx-text-fill: derive(-fx-text-background-color, -20%);");
            descLabel.setWrapText(true);
            
            Label locationLabel = new Label(item.getLocation());
            locationLabel.setStyle("-fx-font-size: 10px; -fx-text-fill: derive(-fx-text-background-color, -40%);");
            
            container.getChildren().addAll(header, descLabel, locationLabel);
            
            setGraphic(container);
            setText(null);
        }
    }
}
