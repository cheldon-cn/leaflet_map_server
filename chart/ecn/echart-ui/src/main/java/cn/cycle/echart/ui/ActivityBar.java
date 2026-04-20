package cn.cycle.echart.ui;

import javafx.scene.control.ContentDisplay;
import javafx.scene.control.ToggleButton;
import javafx.scene.control.ToggleGroup;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.VBox;

import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 活动栏。
 * 
 * <p>提供快速访问常用功能的垂直工具栏，使用VBox+ToggleButton布局。</p>
 * <p>宽度固定为48px，图标按钮对应侧边栏面板。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ActivityBar extends VBox {

    private static final double BAR_WIDTH = 48.0;
    private static final double BUTTON_SIZE = 40.0;
    private static final double ICON_SIZE = 24.0;

    private final ToggleGroup toggleGroup;
    private final Map<String, ToggleButton> buttons;
    private final Map<String, ActivityBarItem> items;
    private ActivityBarSelectionHandler selectionHandler;

    public ActivityBar() {
        this.toggleGroup = new ToggleGroup();
        this.buttons = new HashMap<>();
        this.items = new HashMap<>();
        
        initializeLayout();
        initializeDefaultItems();
    }

    private void initializeLayout() {
        setPrefWidth(BAR_WIDTH);
        setMinWidth(BAR_WIDTH);
        setMaxWidth(BAR_WIDTH);
        setSpacing(4);
        setPadding(new javafx.geometry.Insets(4, 4, 4, 4));
        getStyleClass().add("activity-bar");
    }

    private void initializeDefaultItems() {
        addItem("layers", "图层", "/icons/layers.png", "图层管理面板");
        addItem("search", "搜索", "/icons/search.png", "搜索面板");
        addItem("route", "航线", "/icons/route.png", "航线规划面板");
        addItem("alarm", "预警", "/icons/alarm.png", "预警管理面板");
        addItem("ais", "AIS", "/icons/ais.png", "AIS目标面板");
        addItem("measure", "测量", "/icons/measure.png", "测量工具面板");
        addItem("settings", "设置", "/icons/settings.png", "设置面板");
    }

    public void addItem(String id, String text, String iconPath, String tooltip) {
        Objects.requireNonNull(id, "id cannot be null");
        Objects.requireNonNull(text, "text cannot be null");
        
        ToggleButton button = createToggleButton(id, text, iconPath, tooltip);
        buttons.put(id, button);
        items.put(id, new ActivityBarItem(id, text, iconPath, tooltip));
        
        getChildren().add(button);
    }

    public void insertItem(String id, String text, String iconPath, String tooltip, int index) {
        Objects.requireNonNull(id, "id cannot be null");
        Objects.requireNonNull(text, "text cannot be null");
        
        ToggleButton button = createToggleButton(id, text, iconPath, tooltip);
        buttons.put(id, button);
        items.put(id, new ActivityBarItem(id, text, iconPath, tooltip));
        
        getChildren().add(index, button);
    }

    private ToggleButton createToggleButton(String id, String text, String iconPath, String tooltip) {
        ToggleButton button = new ToggleButton();
        button.setId(id);
        button.setToggleGroup(toggleGroup);
        button.setPrefSize(BUTTON_SIZE, BUTTON_SIZE);
        button.setMinSize(BUTTON_SIZE, BUTTON_SIZE);
        button.setMaxSize(BUTTON_SIZE, BUTTON_SIZE);
        button.setContentDisplay(ContentDisplay.GRAPHIC_ONLY);
        button.getStyleClass().add("activity-button");
        
        if (tooltip != null && !tooltip.isEmpty()) {
            javafx.scene.control.Tooltip tip = new javafx.scene.control.Tooltip(tooltip);
            button.setTooltip(tip);
        }
        
        if (iconPath != null && !iconPath.isEmpty()) {
            try {
                Image image = new Image(getClass().getResourceAsStream(iconPath));
                if (image != null && !image.isError()) {
                    ImageView imageView = new ImageView(image);
                    imageView.setFitWidth(ICON_SIZE);
                    imageView.setFitHeight(ICON_SIZE);
                    imageView.setPreserveRatio(true);
                    button.setGraphic(imageView);
                }
            } catch (Exception e) {
                button.setText(text.substring(0, 1));
            }
        } else {
            button.setText(text.substring(0, 1));
        }
        
        button.selectedProperty().addListener((obs, oldVal, newVal) -> {
            if (selectionHandler != null) {
                selectionHandler.onSelectionChanged(id, newVal);
            }
        });
        
        return button;
    }

    public void removeItem(String id) {
        ToggleButton button = buttons.remove(id);
        if (button != null) {
            getChildren().remove(button);
            items.remove(id);
        }
    }

    public void selectItem(String id) {
        ToggleButton button = buttons.get(id);
        if (button != null) {
            button.setSelected(true);
        }
    }

    public void deselectItem(String id) {
        ToggleButton button = buttons.get(id);
        if (button != null) {
            button.setSelected(false);
        }
    }

    public String getSelectedItemId() {
        ToggleButton selected = (ToggleButton) toggleGroup.getSelectedToggle();
        return selected != null ? selected.getId() : null;
    }

    public void setSelectionHandler(ActivityBarSelectionHandler handler) {
        this.selectionHandler = handler;
    }

    public ToggleButton getButton(String id) {
        return buttons.get(id);
    }

    public ActivityBarItem getItem(String id) {
        return items.get(id);
    }

    public Map<String, ActivityBarItem> getItems() {
        return new HashMap<>(items);
    }

    public ToggleGroup getToggleGroup() {
        return toggleGroup;
    }

    public static class ActivityBarItem {
        private final String id;
        private final String text;
        private final String iconPath;
        private final String tooltip;

        public ActivityBarItem(String id, String text, String iconPath, String tooltip) {
            this.id = id;
            this.text = text;
            this.iconPath = iconPath;
            this.tooltip = tooltip;
        }

        public String getId() {
            return id;
        }

        public String getText() {
            return text;
        }

        public String getIconPath() {
            return iconPath;
        }

        public String getTooltip() {
            return tooltip;
        }
    }

    public interface ActivityBarSelectionHandler {
        void onSelectionChanged(String itemId, boolean selected);
    }
}
