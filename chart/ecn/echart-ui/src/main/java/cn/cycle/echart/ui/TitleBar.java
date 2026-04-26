package cn.cycle.echart.ui;

import cn.cycle.echart.core.LogUtil;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.TextField;
import javafx.scene.control.Tooltip;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.stage.Stage;

/**
 * 标题栏。
 * 
 * <p>包含应用logo、用户登录、设置按钮、标题、搜索框、侧边栏切换按钮和窗口控制按钮。</p>
 * 
 * @author Cycle Team
 * @version 1.2.0
 * @since 1.0.0
 */
public class TitleBar extends HBox {

    private static final String TAG = "TitleBar";
    
    private final Stage stage;
    private final Label titleLabel;
    private final WindowControls windowControls;
    
    private final ImageView logoView;
    private final Button loginButton;
    private final Button settingsButton;
    private final TextField searchField;
    private final Button toggleLeftBarButton;
    private final Button toggleRightBarButton;
    
    private double xOffset = 0;
    private double yOffset = 0;
    private boolean dragging = false;
    
    private Runnable onLoginAction;
    private Runnable onSettingsAction;
    private Runnable onToggleLeftBar;
    private Runnable onToggleRightBar;
    private Runnable onSearch;

    public TitleBar(Stage stage) {
        this.stage = stage;
        this.titleLabel = new Label("E-Chart Display and Alarm Application");
        this.windowControls = new WindowControls(stage, false);
        
        this.logoView = createLogoView();
        this.loginButton = createIconButton("👤", "用户登录", this::onLogin);
        this.settingsButton = createIconButton("⚙", "设置", this::onSettings);
        this.searchField = createSearchField();
        this.toggleLeftBarButton = createIconButton("☰", "显示左侧栏", this::onToggleLeftBar);
        this.toggleRightBarButton = createIconButton("⊳", "显示右侧栏", this::onToggleRightBar);
        
        initializeLayout();
        setupDragHandler();
    }
    
    private ImageView createLogoView() {
        ImageView view = new ImageView();
        view.setFitHeight(20);
        view.setFitWidth(20);
        view.setPreserveRatio(true);
        try {
            Image logo = new Image(getClass().getResourceAsStream("/icons/logo.png"));
            if (logo != null && !logo.isError()) {
                view.setImage(logo);
            }
        } catch (Exception e) {
            LogUtil.debug(TAG, "Logo not found, using placeholder");
        }
        return view;
    }
    
    private Button createIconButton(String text, String tooltip, Runnable action) {
        Button button = new Button(text);
        button.setTooltip(new Tooltip(tooltip));
        button.setFocusTraversable(false);
        button.getStyleClass().add("title-bar-button");
        button.setOnAction(e -> {
            if (action != null) {
                action.run();
            }
        });
        return button;
    }
    
    private TextField createSearchField() {
        TextField field = new TextField();
        field.setPromptText("搜索...");
        field.setPrefWidth(150);
        field.setMaxWidth(200);
        field.getStyleClass().add("title-bar-search");
        field.setOnAction(e -> {
            if (onSearch != null) {
                onSearch.run();
            }
        });
        return field;
    }

    private void initializeLayout() {
        setPadding(new Insets(0, 5, 0, 10));
        setSpacing(8);
        getStyleClass().add("title-bar");
        
        titleLabel.getStyleClass().add("title-bar-title");
        
        HBox leftSection = new HBox(8);
        leftSection.setAlignment(Pos.CENTER_LEFT);
        leftSection.getChildren().addAll(logoView, loginButton, settingsButton);
        
        HBox centerSection = new HBox();
        centerSection.setAlignment(Pos.CENTER);
        centerSection.getChildren().add(titleLabel);
        
        HBox rightSection = new HBox(5);
        rightSection.setAlignment(Pos.CENTER_RIGHT);
        rightSection.getChildren().addAll(searchField, toggleLeftBarButton, toggleRightBarButton, windowControls);
        
        HBox spacerLeft = new HBox();
        HBox.setHgrow(spacerLeft, Priority.ALWAYS);
        
        HBox spacerRight = new HBox();
        HBox.setHgrow(spacerRight, Priority.ALWAYS);
        
        getChildren().addAll(leftSection, spacerLeft, centerSection, spacerRight, rightSection);
        setHeight(32);
        setMinHeight(32);
        setMaxHeight(32);
        
        widthProperty().addListener((obs, oldVal, newVal) -> {
            LogUtil.debug("TitleBar", "widthProperty: %s -> %s, windowControls.width=%s, stage.width=%s", 
                    oldVal, newVal, windowControls.getWidth(), stage != null ? stage.getWidth() : "null");
        });
    }
    
    private void onLogin() {
        if (onLoginAction != null) {
            onLoginAction.run();
        }
    }
    
    private void onSettings() {
        if (onSettingsAction != null) {
            onSettingsAction.run();
        }
    }
    
    private void onToggleLeftBar() {
        if (onToggleLeftBar != null) {
            onToggleLeftBar.run();
        }
    }
    
    private void onToggleRightBar() {
        if (onToggleRightBar != null) {
            onToggleRightBar.run();
        }
    }

    private void setupDragHandler() {
        setOnMousePressed(event -> {
            if (event.isConsumed()) {
                return;
            }
            xOffset = event.getSceneX();
            yOffset = event.getSceneY();
            dragging = true;
        });
        
        setOnMouseDragged(event -> {
            if (event.isConsumed() || !dragging) {
                return;
            }
            if (stage != null && !stage.isMaximized() && !windowControls.isMaximized()) {
                double newX = event.getScreenX() - xOffset;
                double newY = event.getScreenY() - yOffset;
                stage.setX(newX);
                stage.setY(newY);
            }
        });
        
        setOnMouseReleased(event -> {
            LogUtil.debug(TAG, "MouseReleased: dragging=%s", dragging);
            dragging = false;
        });
    }

    public void setTitle(String title) {
        titleLabel.setText(title);
    }

    public String getTitle() {
        return titleLabel.getText();
    }

    public WindowControls getWindowControls() {
        return windowControls;
    }
    
    public void setOnLoginAction(Runnable action) {
        this.onLoginAction = action;
    }
    
    public void setOnSettingsAction(Runnable action) {
        this.onSettingsAction = action;
    }
    
    public void setOnToggleLeftBar(Runnable action) {
        this.onToggleLeftBar = action;
    }
    
    public void setOnToggleRightBar(Runnable action) {
        this.onToggleRightBar = action;
    }
    
    public void setOnSearch(Runnable action) {
        this.onSearch = action;
    }
    
    public String getSearchText() {
        return searchField.getText();
    }
    
    public void setSearchText(String text) {
        searchField.setText(text);
    }
}
