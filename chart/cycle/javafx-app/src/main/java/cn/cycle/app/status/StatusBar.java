package cn.cycle.app.status;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Label;
import javafx.scene.control.Tooltip;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.Region;
import javafx.scene.paint.Color;

import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class StatusBar extends AbstractLifecycleComponent {
    
    public enum StatusLevel {
        NORMAL,
        WARNING,
        ERROR
    }
    
    private final HBox container;
    private final HBox leftSection;
    private final HBox rightSection;
    private final Map<String, StatusItem> items = new ConcurrentHashMap<>();
    
    private boolean compactMode = false;
    
    public StatusBar() {
        container = new HBox();
        container.setAlignment(Pos.CENTER_LEFT);
        container.setPadding(new Insets(4, 8, 4, 8));
        container.setSpacing(0);
        container.setStyle("-fx-background-color: #f0f0f0; -fx-border-color: #e0e0e0; -fx-border-width: 1 0 0 0;");
        
        leftSection = new HBox();
        leftSection.setAlignment(Pos.CENTER_LEFT);
        leftSection.setSpacing(16);
        
        rightSection = new HBox();
        rightSection.setAlignment(Pos.CENTER_RIGHT);
        rightSection.setSpacing(16);
        HBox.setHgrow(rightSection, Priority.ALWAYS);
        
        container.getChildren().addAll(leftSection, rightSection);
        
        initDefaultItems();
    }
    
    private void initDefaultItems() {
        addLeftItem("service", "服务状态", "● 已连接");
        addLeftItem("message", "提示信息", "就绪");
        
        addRightItem("position", "鼠标位置", "-");
        addRightItem("zoom", "缩放比例", "1:-");
    }
    
    public void addLeftItem(String id, String tooltip, String defaultValue) {
        Label label = new Label(defaultValue);
        label.setTooltip(new Tooltip(tooltip));
        
        StatusItem item = new StatusItem(id, label, tooltip, true);
        items.put(id, item);
        
        if (!compactMode || isEssentialItem(id)) {
            leftSection.getChildren().add(label);
        }
    }
    
    public void addRightItem(String id, String tooltip, String defaultValue) {
        Label label = new Label(defaultValue);
        label.setTooltip(new Tooltip(tooltip));
        
        StatusItem item = new StatusItem(id, label, tooltip, false);
        items.put(id, item);
        
        if (!compactMode || isEssentialItem(id)) {
            rightSection.getChildren().add(label);
        }
    }
    
    public void addItem(String id, String tooltip, String defaultValue) {
        addLeftItem(id, tooltip, defaultValue);
    }
    
    private boolean isEssentialItem(String id) {
        return "service".equals(id) || "message".equals(id);
    }
    
    public void updateItem(String id, String value) {
        StatusItem item = items.get(id);
        if (item != null) {
            item.getLabel().setText(value);
        }
    }
    
    public void updateItem(String id, String value, StatusLevel level) {
        StatusItem item = items.get(id);
        if (item != null) {
            Label label = item.getLabel();
            label.setText(value);
            
            switch (level) {
                case WARNING:
                    label.setTextFill(Color.ORANGE);
                    break;
                case ERROR:
                    label.setTextFill(Color.RED);
                    break;
                case NORMAL:
                default:
                    label.setTextFill(Color.BLACK);
                    break;
            }
        }
    }
    
    public void setServiceStatus(String status, StatusLevel level) {
        String prefix;
        switch (level) {
            case ERROR:
                prefix = "○ ";
                break;
            case WARNING:
                prefix = "◐ ";
                break;
            default:
                prefix = "● ";
                break;
        }
        updateItem("service", prefix + status, level);
    }
    
    public void setRegion(String region) {
        updateItem("region", region);
    }
    
    public void setPosition(double longitude, double latitude) {
        String pos = String.format("%.4f°%s %.4f°%s", 
            Math.abs(longitude), longitude >= 0 ? "E" : "W",
            Math.abs(latitude), latitude >= 0 ? "N" : "S");
        updateItem("position", pos);
    }
    
    public void setPosition(String position) {
        updateItem("position", position);
    }
    
    public void setTileLevel(int level) {
        updateItem("tileLevel", "Level " + level);
    }
    
    public void setZoomRatio(double ratio) {
        updateItem("zoom", String.format("1:%.0f", ratio));
    }
    
    public void setMessage(String message) {
        updateItem("message", message);
    }
    
    public void setCompactMode(boolean compact) {
        if (this.compactMode == compact) {
            return;
        }
        
        this.compactMode = compact;
        
        leftSection.getChildren().clear();
        rightSection.getChildren().clear();
        
        if (compact) {
            for (Map.Entry<String, StatusItem> entry : items.entrySet()) {
                if (isEssentialItem(entry.getKey())) {
                    if (entry.getValue().isLeftSection()) {
                        leftSection.getChildren().add(entry.getValue().getLabel());
                    } else {
                        rightSection.getChildren().add(entry.getValue().getLabel());
                    }
                }
            }
        } else {
            for (StatusItem item : items.values()) {
                if (item.isLeftSection()) {
                    leftSection.getChildren().add(item.getLabel());
                } else {
                    rightSection.getChildren().add(item.getLabel());
                }
            }
        }
    }
    
    public HBox getContainer() {
        return container;
    }
    
    public void addSpacer() {
        Region spacer = new Region();
        HBox.setHgrow(spacer, Priority.ALWAYS);
        leftSection.getChildren().add(spacer);
    }
    
    @Override
    protected void doInitialize() {
        AppEventBus.getInstance().subscribe(AppEventType.STATUS_MESSAGE, this::handleStatusMessage);
        AppEventBus.getInstance().subscribe(AppEventType.VIEW_CHANGED, this::handleViewChanged);
    }
    
    @Override
    protected void doDispose() {
        AppEventBus.getInstance().unsubscribe(AppEventType.STATUS_MESSAGE, this::handleStatusMessage);
        AppEventBus.getInstance().unsubscribe(AppEventType.VIEW_CHANGED, this::handleViewChanged);
        items.clear();
    }
    
    private void handleStatusMessage(AppEvent event) {
        String message = event.getData("message");
        if (message != null) {
            setMessage(message);
        }
    }
    
    private void handleViewChanged(AppEvent event) {
        Object zoom = event.getData("zoom");
        if (zoom instanceof Number) {
            setZoomRatio(((Number) zoom).doubleValue());
        }
        
        Object level = event.getData("tileLevel");
        if (level instanceof Number) {
            setTileLevel(((Number) level).intValue());
        }
    }
    
    private static class StatusItem {
        private final String id;
        private final Label label;
        private final String tooltip;
        private final boolean leftSection;
        
        StatusItem(String id, Label label, String tooltip, boolean leftSection) {
            this.id = id;
            this.label = label;
            this.tooltip = tooltip;
            this.leftSection = leftSection;
        }
        
        public String getId() {
            return id;
        }
        
        public Label getLabel() {
            return label;
        }
        
        public String getTooltip() {
            return tooltip;
        }
        
        public boolean isLeftSection() {
            return leftSection;
        }
    }
}
