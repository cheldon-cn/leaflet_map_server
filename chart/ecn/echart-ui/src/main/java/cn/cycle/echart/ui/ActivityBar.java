package cn.cycle.echart.ui;

import javafx.scene.control.Button;
import javafx.scene.control.ToolBar;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

/**
 * 活动栏。
 * 
 * <p>提供快速访问常用功能的工具栏。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ActivityBar extends ToolBar {

    private final Button newButton;
    private final Button openButton;
    private final Button saveButton;
    private final Button zoomInButton;
    private final Button zoomOutButton;
    private final Button fitButton;
    private final Button measureButton;
    private final Button routeButton;
    private final Button alarmButton;

    public ActivityBar() {
        this.newButton = createButton("新建", "/icons/new.png");
        this.openButton = createButton("打开", "/icons/open.png");
        this.saveButton = createButton("保存", "/icons/save.png");
        this.zoomInButton = createButton("放大", "/icons/zoom_in.png");
        this.zoomOutButton = createButton("缩小", "/icons/zoom_out.png");
        this.fitButton = createButton("适应", "/icons/fit.png");
        this.measureButton = createButton("测量", "/icons/measure.png");
        this.routeButton = createButton("航线", "/icons/route.png");
        this.alarmButton = createButton("预警", "/icons/alarm.png");
        
        initializeLayout();
    }

    private void initializeLayout() {
        getItems().addAll(
                newButton, openButton, saveButton,
                createSeparator(),
                zoomInButton, zoomOutButton, fitButton,
                createSeparator(),
                measureButton, routeButton, alarmButton
        );
        
        getStyleClass().add("activity-bar");
    }

    private Button createButton(String text, String iconPath) {
        Button button = new Button(text);
        
        try {
            Image image = new Image(getClass().getResourceAsStream(iconPath));
            if (image != null) {
                button.setGraphic(new ImageView(image));
            }
        } catch (Exception e) {
        }
        
        button.getStyleClass().add("activity-button");
        return button;
    }

    private javafx.scene.control.Separator createSeparator() {
        return new javafx.scene.control.Separator();
    }

    public Button getNewButton() {
        return newButton;
    }

    public Button getOpenButton() {
        return openButton;
    }

    public Button getSaveButton() {
        return saveButton;
    }

    public Button getZoomInButton() {
        return zoomInButton;
    }

    public Button getZoomOutButton() {
        return zoomOutButton;
    }

    public Button getFitButton() {
        return fitButton;
    }

    public Button getMeasureButton() {
        return measureButton;
    }

    public Button getRouteButton() {
        return routeButton;
    }

    public Button getAlarmButton() {
        return alarmButton;
    }
}
