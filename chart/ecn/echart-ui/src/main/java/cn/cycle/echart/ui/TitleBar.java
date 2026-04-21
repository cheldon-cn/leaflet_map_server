package cn.cycle.echart.ui;

import javafx.geometry.Insets;
import javafx.scene.control.Label;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.stage.Stage;

/**
 * 标题栏。
 * 
 * <p>包含应用标题和窗口控制按钮，支持拖拽移动窗口。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class TitleBar extends HBox {

    private static final boolean DEBUG = false;
    
    private final Stage stage;
    private final Label titleLabel;
    private final WindowControls windowControls;
    
    private double xOffset = 0;
    private double yOffset = 0;
    private boolean dragging = false;

    public TitleBar(Stage stage) {
        this.stage = stage;
        this.titleLabel = new Label("E-Chart Display and Alarm Application");
        this.windowControls = new WindowControls(stage);
        
        initializeLayout();
        setupDragHandler();
    }

    private void initializeLayout() {
        setPadding(new Insets(0, 0, 0, 10));
        setSpacing(10);
        setStyle("-fx-background-color: derive(-fx-base, -10%); -fx-border-color: derive(-fx-base, -20%); -fx-border-width: 0 0 1 0;");
        getStyleClass().add("title-bar");
        
        titleLabel.setStyle("-fx-font-size: 12px; -fx-font-weight: bold;");
        
        HBox spacer = new HBox();
        HBox.setHgrow(spacer, Priority.ALWAYS);
        
        getChildren().addAll(titleLabel, spacer, windowControls);
        setHeight(32);
        setMinHeight(32);
        setMaxHeight(32);
    }

    private void setupDragHandler() {
        setOnMousePressed(event -> {
            if (event.isConsumed()) {
                if (DEBUG) System.out.println("[TitleBar] MousePressed: event already consumed, skipping");
                return;
            }
            xOffset = event.getSceneX();
            yOffset = event.getSceneY();
            dragging = true;
            if (DEBUG) {
                System.out.println("[TitleBar] MousePressed: offset=(" + xOffset + ", " + yOffset + ")");
            }
        });
        
        setOnMouseDragged(event -> {
            if (event.isConsumed() || !dragging) {
                if (DEBUG && event.isConsumed()) {
                    System.out.println("[TitleBar] MouseDragged: event consumed, skipping");
                }
                return;
            }
            if (stage != null && !stage.isMaximized()) {
                double newX = event.getScreenX() - xOffset;
                double newY = event.getScreenY() - yOffset;
                if (DEBUG) {
                    System.out.println("[TitleBar] MouseDragged: moving window to (" + newX + ", " + newY + ")");
                }
                stage.setX(newX);
                stage.setY(newY);
            }
        });
        
        setOnMouseReleased(event -> {
            if (DEBUG) {
                System.out.println("[TitleBar] MouseReleased: dragging=" + dragging);
            }
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
}
