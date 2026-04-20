package cn.cycle.echart.ui;

import javafx.scene.control.Button;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.HBox;
import javafx.stage.Stage;
import javafx.stage.Screen;
import javafx.geometry.Rectangle2D;

/**
 * 窗口控制按钮组。
 * 
 * <p>提供设置、最小化、最大化/还原、关闭按钮。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WindowControls extends HBox {

    private final Stage stage;
    private final Button settingsButton;
    private final Button minimizeButton;
    private final Button maximizeButton;
    private final Button closeButton;
    
    private Runnable onSettingsAction;
    private double prevX;
    private double prevY;
    private double prevWidth;
    private double prevHeight;
    private boolean isMaximized = false;

    public WindowControls(Stage stage) {
        this.stage = stage;
        this.settingsButton = createButton("⚙", "设置", this::onSettings);
        this.minimizeButton = createButton("─", "最小化", this::onMinimize);
        this.maximizeButton = createButton("□", "最大化", this::onMaximize);
        this.closeButton = createButton("✕", "关闭", this::onClose);
        
        initializeLayout();
    }

    private void initializeLayout() {
        setSpacing(0);
        getStyleClass().add("window-controls");
        
        settingsButton.getStyleClass().add("window-control-settings");
        minimizeButton.getStyleClass().add("window-control-minimize");
        maximizeButton.getStyleClass().add("window-control-maximize");
        closeButton.getStyleClass().add("window-control-close");
        
        getChildren().addAll(settingsButton, minimizeButton, maximizeButton, closeButton);
        
        updateMaximizeButton();
    }

    private Button createButton(String text, String tooltip, Runnable action) {
        Button button = new Button(text);
        button.setTooltip(new Tooltip(tooltip));
        button.setFocusTraversable(false);
        button.setOnAction(e -> {
            if (action != null) {
                action.run();
            }
        });
        return button;
    }

    private void onSettings() {
        if (onSettingsAction != null) {
            onSettingsAction.run();
        }
    }

    private void onMinimize() {
        if (stage != null) {
            stage.setIconified(true);
        }
    }

    private void onMaximize() {
        if (stage == null) {
            return;
        }
        
        if (isMaximized) {
            stage.setX(prevX);
            stage.setY(prevY);
            stage.setWidth(prevWidth);
            stage.setHeight(prevHeight);
            isMaximized = false;
        } else {
            prevX = stage.getX();
            prevY = stage.getY();
            prevWidth = stage.getWidth();
            prevHeight = stage.getHeight();
            
            Rectangle2D screenBounds = Screen.getPrimary().getVisualBounds();
            stage.setX(screenBounds.getMinX());
            stage.setY(screenBounds.getMinY());
            stage.setWidth(screenBounds.getWidth());
            stage.setHeight(screenBounds.getHeight());
            isMaximized = true;
        }
        
        updateMaximizeButton();
    }

    private void onClose() {
        if (stage != null) {
            stage.close();
        }
    }

    private void updateMaximizeButton() {
        if (isMaximized) {
            maximizeButton.setText("❐");
            maximizeButton.getTooltip().setText("还原");
        } else {
            maximizeButton.setText("□");
            maximizeButton.getTooltip().setText("最大化");
        }
    }
    
    public boolean isMaximized() {
        return isMaximized;
    }

    public void setOnSettingsAction(Runnable action) {
        this.onSettingsAction = action;
    }

    public Button getSettingsButton() {
        return settingsButton;
    }

    public Button getMinimizeButton() {
        return minimizeButton;
    }

    public Button getMaximizeButton() {
        return maximizeButton;
    }

    public Button getCloseButton() {
        return closeButton;
    }
}
