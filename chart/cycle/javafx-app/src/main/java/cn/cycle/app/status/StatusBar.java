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
    private final Map<String, StatusItem> items = new ConcurrentHashMap<>();
    
    private boolean compactMode = false;
    
    public StatusBar() {
        container = new HBox();
        container.setAlignment(Pos.CENTER_LEFT);
        container.setPadding(new Insets(4, 8, 4, 8));
        container.setSpacing(16);
        container.setStyle("-fx-background-color: #f0f0f0; -fx-border-color: #e0e0e0; -fx-border-width: 1 0 0 0;");
        
        initDefaultItems();
    }
    
    private void initDefaultItems() {
        addItem("service", "服务状态", "● 已连接");
        addItem("region", "显示区域", "-");
        addItem("position", "鼠标位置", "-");
        addItem("tileLevel", "瓦片级数", "Level -");
        addItem("zoom", "缩放比例", "1:-");
        addItem("message", "提示信息", "就绪");
    }
    
    public void addItem(String id, String tooltip, String defaultValue) {
        Label label = new Label(defaultValue);
        label.setTooltip(new Tooltip(tooltip));
        
        StatusItem item = new StatusItem(id, label, tooltip);
        items.put(id, item);
        
        if (!compactMode || isEssentialItem(id)) {
            container.getChildren().add(label);
        }
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
        
        container.getChildren().clear();
        
        if (compact) {
            for (Map.Entry<String, StatusItem> entry : items.entrySet()) {
                if (isEssentialItem(entry.getKey())) {
                    container.getChildren().add(entry.getValue().getLabel());
                }
            }
        } else {
            for (StatusItem item : items.values()) {
                container.getChildren().add(item.getLabel());
            }
        }
    }
    
    public HBox getContainer() {
        return container;
    }
    
    public void addSpacer() {
        Region spacer = new Region();
        HBox.setHgrow(spacer, Priority.ALWAYS);
        container.getChildren().add(spacer);
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
        
        StatusItem(String id, Label label, String tooltip) {
            this.id = id;
            this.label = label;
            this.tooltip = tooltip;
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
    }
}
