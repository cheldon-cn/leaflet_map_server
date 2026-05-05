package cn.cycle.echart.ui;

import cn.cycle.echart.core.LogUtil;
import javafx.event.EventHandler;
import javafx.scene.Cursor;
import javafx.scene.Scene;
import javafx.scene.input.MouseEvent;
import javafx.stage.Stage;

/**
 * 窗口边缘调整大小处理器。
 * 
 * <p>为UNDECORATED窗口提供边缘拖动调整大小的功能。</p>
 * <p>使用Scene级别的事件过滤器，确保捕获所有鼠标事件。</p>
 * 
 * @author Cycle Team
 * @version 1.2.0
 * @since 1.0.0
 */
public class WindowResizer {

    private static final String TAG = "WindowResizer";
    private static final int RESIZE_MARGIN = 8;
    
    private final Stage stage;
    private final Scene scene;
    
    private boolean resizing = false;
    private ResizeDirection direction = ResizeDirection.NONE;
    private double startX;
    private double startY;
    private double startWidth;
    private double startHeight;
    private double startWindowX;
    private double startWindowY;
    
    private enum ResizeDirection {
        NONE,
        N, S, E, W,
        NE, NW, SE, SW
    }

    public WindowResizer(Stage stage, Scene scene) {
        this.stage = stage;
        this.scene = scene;
        setupResizeHandler();
    }

    private void setupResizeHandler() {
        EventHandler<MouseEvent> pressHandler = new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                direction = getResizeDirection(event);
                if (direction != ResizeDirection.NONE) {
                    resizing = true;
                    startX = event.getScreenX();
                    startY = event.getScreenY();
                    startWidth = stage.getWidth();
                    startHeight = stage.getHeight();
                    startWindowX = stage.getX();
                    startWindowY = stage.getY();
                    LogUtil.debug(TAG, "=== RESIZE START ===\nDirection: %s\nMouse: (%s, %s)\nWindow: pos=(%s, %s) size=(%s, %s)\nStage actual: width=%s height=%s",
                            direction, startX, startY, startWindowX, startWindowY, startWidth, startHeight, stage.getWidth(), stage.getHeight());
                    event.consume();
                    LogUtil.debug(TAG, "After consume: event.isConsumed()=%s", event.isConsumed());
                }
            }
        };

        EventHandler<MouseEvent> dragHandler = new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                if (!resizing) {
                    updateCursor(event);
                    return;
                }
                
                double deltaX = event.getScreenX() - startX;
                double deltaY = event.getScreenY() - startY;
                
                double minWidth = stage.getMinWidth() > 0 ? stage.getMinWidth() : 400;
                double minHeight = stage.getMinHeight() > 0 ? stage.getMinHeight() : 300;
                
                double newWidth = startWidth;
                double newHeight = startHeight;
                double newX = startWindowX;
                double newY = startWindowY;
                
                LogUtil.debug(TAG, "=== RESIZE DRAG (before calc) === | startHeight=%s, deltaY=%s", startHeight, deltaY);
               
                switch (direction) {
                    case E:
                        LogUtil.debug(TAG, "calc: startWidth + deltaX = %s", (startWidth + deltaX));
                        newWidth = Math.round(Math.max(minWidth, startWidth + deltaX));
                        break;
                    case W:
                        LogUtil.debug(TAG, "calc: startWidth - deltaX = %s", (startWidth - deltaX));
                        newWidth = Math.round(Math.max(minWidth, startWidth - deltaX));
                        newX = startWindowX + startWidth - newWidth;
                        break;
                    case S:
                        LogUtil.debug(TAG, "calc: startHeight + deltaY = %s", (startHeight + deltaY));
                        newHeight = Math.round(Math.max(minHeight, startHeight + deltaY));
                        break;
                    case N:
                        LogUtil.debug(TAG, "calc: startHeight - deltaY = %s", (startHeight - deltaY));
                        newHeight = Math.round(Math.max(minHeight, startHeight - deltaY));
                        newY = startWindowY + startHeight - newHeight;
                        break;
                    case SE:
                        LogUtil.debug(TAG, "calc: startWidth + deltaX = %s, startHeight + deltaY = %s", 
                                (startWidth + deltaX), (startHeight + deltaY));
                        newWidth = Math.round(Math.max(minWidth, startWidth + deltaX));
                        newHeight = Math.round(Math.max(minHeight, startHeight + deltaY));
                        break;
                    case SW:
                        LogUtil.debug(TAG, "calc: startWidth - deltaX = %s, startHeight + deltaY = %s", 
                                (startWidth - deltaX), (startHeight + deltaY));
                        newWidth = Math.round(Math.max(minWidth, startWidth - deltaX));
                        newHeight = Math.round(Math.max(minHeight, startHeight + deltaY));
                        newX = startWindowX + startWidth - newWidth;
                        break;
                    case NE:
                        LogUtil.debug(TAG, "calc: startWidth + deltaX = %s, startHeight - deltaY = %s", 
                                (startWidth + deltaX), (startHeight - deltaY));
                        newWidth = Math.round(Math.max(minWidth, startWidth + deltaX));
                        newHeight = Math.round(Math.max(minHeight, startHeight - deltaY));
                        newY = startWindowY + startHeight - newHeight;
                        break;
                    case NW:
                        LogUtil.debug(TAG, "calc: startWidth - deltaX = %s, startHeight - deltaY = %s", 
                                (startWidth - deltaX), (startHeight - deltaY));
                        newWidth = Math.round(Math.max(minWidth, startWidth - deltaX));
                        newHeight = Math.round(Math.max(minHeight, startHeight - deltaY));
                        newX = startWindowX + startWidth - newWidth;
                        newY = startWindowY + startHeight - newHeight;
                        break;
                    default:
                        break;
                }
                
                LogUtil.debug(TAG, "=== RESIZE DRAG (after calc) === | newHeight=%s, newY=%s | New: pos=(%s, %s) size=(%s, %s) |After set: stage.getY()=%s, stage.getHeight()=%s",
                        newHeight, newY, newX, newY, newWidth, newHeight, stage.getY(), stage.getHeight());
                
                stage.setX(newX);
                stage.setY(newY);
                stage.setWidth(newWidth);
                stage.setHeight(newHeight);
                
                event.consume();
            }
        };

        EventHandler<MouseEvent> moveHandler = new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                if (resizing) {
                    return;
                }
                updateCursor(event);
            }
        };

        EventHandler<MouseEvent> releaseHandler = new EventHandler<MouseEvent>() {
            @Override
            public void handle(MouseEvent event) {
                if (resizing) {
                    LogUtil.debug(TAG, "=== RESIZE END ===");
                    resizing = false;
                    direction = ResizeDirection.NONE;
                    updateCursor(event);
                    event.consume();
                }
            }
        };

        scene.addEventFilter(MouseEvent.MOUSE_PRESSED, pressHandler);
        scene.addEventFilter(MouseEvent.MOUSE_DRAGGED, dragHandler);
        scene.addEventFilter(MouseEvent.MOUSE_MOVED, moveHandler);
        scene.addEventFilter(MouseEvent.MOUSE_RELEASED, releaseHandler);
    }

    private void updateCursor(MouseEvent event) {
        ResizeDirection dir = getResizeDirection(event);
        
        Cursor cursor;
        switch (dir) {
            case N:
            case S:
                cursor = Cursor.V_RESIZE;
                break;
            case E:
            case W:
                cursor = Cursor.H_RESIZE;
                break;
            case NE:
            case SW:
                cursor = Cursor.NE_RESIZE;
                break;
            case NW:
            case SE:
                cursor = Cursor.NW_RESIZE;
                break;
            default:
                cursor = Cursor.DEFAULT;
                break;
        }
        
        scene.setCursor(cursor);
    }

    private ResizeDirection getResizeDirection(MouseEvent event) {
        double windowX = stage.getX();
        double windowY = stage.getY();
        double windowWidth = stage.getWidth();
        double windowHeight = stage.getHeight();
        
        double screenX = event.getScreenX();
        double screenY = event.getScreenY();
        
        double relX = screenX - windowX;
        double relY = screenY - windowY;
        
        boolean onNorth = relY >= 0 && relY < RESIZE_MARGIN;
        boolean onSouth = relY > windowHeight - RESIZE_MARGIN && relY <= windowHeight;
        boolean onWest = relX >= 0 && relX < RESIZE_MARGIN;
        boolean onEast = relX > windowWidth - RESIZE_MARGIN && relX <= windowWidth;
        
        if (onNorth && onEast) return ResizeDirection.NE;
        if (onNorth && onWest) return ResizeDirection.NW;
        if (onSouth && onEast) return ResizeDirection.SE;
        if (onSouth && onWest) return ResizeDirection.SW;
        if (onNorth) return ResizeDirection.N;
        if (onSouth) return ResizeDirection.S;
        if (onEast) return ResizeDirection.E;
        if (onWest) return ResizeDirection.W;
        
        return ResizeDirection.NONE;
    }
}
