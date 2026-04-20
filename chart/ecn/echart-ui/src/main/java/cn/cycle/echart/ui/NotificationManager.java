package cn.cycle.echart.ui;

import javafx.geometry.Pos;
import javafx.scene.Node;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.util.Duration;

import org.controlsfx.control.Notifications;

import java.util.Objects;

/**
 * 预警通知管理器。
 * 
 * <p>使用ControlsFX的Notifications控件实现预警通知管理。</p>
 * <p>支持不同级别的通知显示，包括信息、警告、错误、确认等。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class NotificationManager {

    private static final Duration DEFAULT_HIDE_DELAY = Duration.seconds(5);
    private static final Duration WARNING_HIDE_DELAY = Duration.seconds(8);
    private static final Duration ERROR_HIDE_DELAY = Duration.seconds(10);

    private static NotificationManager instance;
    
    private Pos defaultPosition = Pos.BOTTOM_RIGHT;
    private boolean darkStyle = false;
    private Node owner = null;
    private NotificationClickListener clickListener;

    private NotificationManager() {
    }

    public static synchronized NotificationManager getInstance() {
        if (instance == null) {
            instance = new NotificationManager();
        }
        return instance;
    }

    public void showInfo(String title, String message) {
        showInfo(title, message, null);
    }

    public void showInfo(String title, String message, NotificationActionHandler handler) {
        Notifications notification = createBaseNotification(title, message)
                .hideAfter(DEFAULT_HIDE_DELAY);
        
        if (handler != null) {
            notification.onAction(e -> handler.onAction());
        }
        
        notification.showInformation();
    }

    public void showWarning(String title, String message) {
        showWarning(title, message, null);
    }

    public void showWarning(String title, String message, NotificationActionHandler handler) {
        Notifications notification = createBaseNotification(title, message)
                .hideAfter(WARNING_HIDE_DELAY);
        
        if (handler != null) {
            notification.onAction(e -> handler.onAction());
        }
        
        notification.showWarning();
    }

    public void showError(String title, String message) {
        showError(title, message, null);
    }

    public void showError(String title, String message, NotificationActionHandler handler) {
        Notifications notification = createBaseNotification(title, message)
                .hideAfter(ERROR_HIDE_DELAY);
        
        if (handler != null) {
            notification.onAction(e -> handler.onAction());
        }
        
        notification.showError();
    }

    public void showConfirm(String title, String message) {
        showConfirm(title, message, null);
    }

    public void showConfirm(String title, String message, NotificationActionHandler handler) {
        Notifications notification = createBaseNotification(title, message)
                .hideAfter(DEFAULT_HIDE_DELAY);
        
        if (handler != null) {
            notification.onAction(e -> handler.onAction());
        }
        
        notification.showConfirm();
    }

    public void showCustom(String title, String message, Node graphic) {
        showCustom(title, message, graphic, DEFAULT_HIDE_DELAY, null);
    }

    public void showCustom(String title, String message, Node graphic, 
                          Duration hideDelay, NotificationActionHandler handler) {
        Notifications notification = createBaseNotification(title, message)
                .graphic(graphic)
                .hideAfter(hideDelay != null ? hideDelay : DEFAULT_HIDE_DELAY);
        
        if (handler != null) {
            notification.onAction(e -> handler.onAction());
        }
        
        notification.show();
    }

    public void showAlarmNotification(String alarmId, String alarmType, 
                                     String severity, String message) {
        Objects.requireNonNull(alarmId, "alarmId cannot be null");
        Objects.requireNonNull(alarmType, "alarmType cannot be null");
        
        String title = "[" + severity + "] " + alarmType;
        
        NotificationActionHandler handler = () -> {
            if (clickListener != null) {
                clickListener.onAlarmClicked(alarmId);
            }
        };
        
        switch (severity.toUpperCase()) {
            case "CRITICAL":
            case "DANGER":
                showError(title, message, handler);
                break;
            case "WARNING":
            case "CAUTION":
                showWarning(title, message, handler);
                break;
            default:
                showInfo(title, message, handler);
                break;
        }
    }

    public void showAlarmNotification(String alarmId, String alarmType, 
                                     String severity, String message,
                                     Node customGraphic) {
        Objects.requireNonNull(alarmId, "alarmId cannot be null");
        Objects.requireNonNull(alarmType, "alarmType cannot be null");
        
        String title = "[" + severity + "] " + alarmType;
        Duration delay = getDelayForSeverity(severity);
        
        NotificationActionHandler handler = () -> {
            if (clickListener != null) {
                clickListener.onAlarmClicked(alarmId);
            }
        };
        
        showCustom(title, message, customGraphic, delay, handler);
    }

    private Notifications createBaseNotification(String title, String message) {
        Notifications notification = Notifications.create()
                .title(title != null ? title : "")
                .text(message != null ? message : "")
                .position(defaultPosition);
        
        if (owner != null) {
            notification.owner(owner);
        }
        
        if (darkStyle) {
            notification.darkStyle();
        }
        
        return notification;
    }

    private Duration getDelayForSeverity(String severity) {
        if (severity == null) {
            return DEFAULT_HIDE_DELAY;
        }
        
        switch (severity.toUpperCase()) {
            case "CRITICAL":
            case "DANGER":
                return ERROR_HIDE_DELAY;
            case "WARNING":
            case "CAUTION":
                return WARNING_HIDE_DELAY;
            default:
                return DEFAULT_HIDE_DELAY;
        }
    }

    public void setDefaultPosition(Pos position) {
        this.defaultPosition = position;
    }

    public Pos getDefaultPosition() {
        return defaultPosition;
    }

    public void setDarkStyle(boolean darkStyle) {
        this.darkStyle = darkStyle;
    }

    public boolean isDarkStyle() {
        return darkStyle;
    }

    public void setOwner(Node owner) {
        this.owner = owner;
    }

    public Node getOwner() {
        return owner;
    }

    public void setClickListener(NotificationClickListener listener) {
        this.clickListener = listener;
    }

    public NotificationClickListener getClickListener() {
        return clickListener;
    }

    public static ImageView createIcon(String iconPath, int size) {
        try {
            Image image = new Image(NotificationManager.class.getResourceAsStream(iconPath));
            if (image != null && !image.isError()) {
                ImageView imageView = new ImageView(image);
                imageView.setFitWidth(size);
                imageView.setFitHeight(size);
                return imageView;
            }
        } catch (Exception e) {
        }
        return null;
    }

    public interface NotificationActionHandler {
        void onAction();
    }

    public interface NotificationClickListener {
        void onAlarmClicked(String alarmId);
    }
}
