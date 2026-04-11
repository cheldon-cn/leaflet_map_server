package cn.cycle.app.exception;

import cn.cycle.app.context.AppContext;
import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.application.Platform;
import javafx.scene.control.*;
import javafx.scene.control.ButtonBar.ButtonData;
import javafx.scene.paint.Color;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.canvas.Canvas;
import javafx.scene.Scene;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.stage.Stage;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.Map;
import java.util.function.Consumer;

public class ExceptionHandler extends AbstractLifecycleComponent {
    
    private static final ExceptionHandler INSTANCE = new ExceptionHandler();
    
    private final Map<Class<? extends Throwable>, Consumer<Throwable>> handlers = new ConcurrentHashMap<>();
    private Consumer<Throwable> defaultHandler;
    private boolean showDialogs = true;
    
    private ExceptionHandler() {
        initDefaultHandlers();
    }
    
    public static ExceptionHandler getInstance() {
        return INSTANCE;
    }
    
    private void initDefaultHandlers() {
        defaultHandler = this::showDefaultError;
    }
    
    public void handle(Throwable throwable) {
        if (Platform.isFxApplicationThread()) {
            handleInternal(throwable);
        } else {
            Platform.runLater(() -> handleInternal(throwable));
        }
    }
    
    private void handleInternal(Throwable throwable) {
        Consumer<Throwable> handler = findHandler(throwable.getClass());
        if (handler != null) {
            handler.accept(throwable);
        } else {
            defaultHandler.accept(throwable);
        }
        
        publishErrorEvent(throwable);
    }
    
    private Consumer<Throwable> findHandler(Class<? extends Throwable> throwableClass) {
        Class<?> current = throwableClass;
        while (current != null && Throwable.class.isAssignableFrom(current)) {
            @SuppressWarnings("unchecked")
            Consumer<Throwable> handler = handlers.get(current);
            if (handler != null) {
                return handler;
            }
            current = current.getSuperclass();
        }
        return null;
    }
    
    public void registerHandler(Class<? extends Throwable> throwableClass, Consumer<Throwable> handler) {
        handlers.put(throwableClass, handler);
    }
    
    public void unregisterHandler(Class<? extends Throwable> throwableClass) {
        handlers.remove(throwableClass);
    }
    
    private void showDefaultError(Throwable throwable) {
        if (!showDialogs) {
            logError(throwable);
            return;
        }
        
        Alert alert = new Alert(Alert.AlertType.ERROR);
        alert.setTitle("错误");
        alert.setHeaderText("发生错误");
        alert.setContentText(throwable.getMessage());
        
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        throwable.printStackTrace(pw);
        String stackTrace = sw.toString();
        
        TextArea textArea = new TextArea(stackTrace);
        textArea.setEditable(false);
        textArea.setWrapText(true);
        textArea.setMaxWidth(Double.MAX_VALUE);
        textArea.setMaxHeight(Double.MAX_VALUE);
        
        alert.getDialogPane().setExpandableContent(textArea);
        
        alert.showAndWait();
    }
    
    public void handleDataLoadError(Object item, Exception e) {
        logError("数据加载失败: " + e.getMessage(), e);
        
        if (showDialogs) {
            Platform.runLater(() -> {
                Alert alert = new Alert(Alert.AlertType.WARNING);
                alert.setTitle("加载失败");
                alert.setHeaderText("数据加载失败");
                alert.setContentText(e.getMessage());
                alert.showAndWait();
            });
        }
    }
    
    public void handleServiceDisconnect() {
        logError("服务连接断开", null);
        
        if (!showDialogs) {
            return;
        }
        
        Platform.runLater(() -> {
            Alert alert = new Alert(Alert.AlertType.WARNING);
            alert.setTitle("服务断开");
            alert.setHeaderText("数据服务连接已断开");
            alert.setContentText("请检查网络连接后重试");
            
            ButtonType reconnectBtn = new ButtonType("重新连接");
            ButtonType closeBtn = new ButtonType("关闭", ButtonData.CANCEL_CLOSE);
            alert.getButtonTypes().setAll(reconnectBtn, closeBtn);
            
            Optional<ButtonType> result = alert.showAndWait();
            if (result.isPresent() && result.get() == reconnectBtn) {
                if (AppContext.getInstance().isInitialized()) {
                }
            }
        });
    }
    
    public void handleRenderError(Canvas canvas, Exception e) {
        logError("渲染异常: " + e.getMessage(), e);
        
        if (canvas != null) {
            GraphicsContext gc = canvas.getGraphicsContext2D();
            gc.clearRect(0, 0, canvas.getWidth(), canvas.getHeight());
            
            gc.setFill(Color.LIGHTGRAY);
            gc.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());
            
            gc.setFill(Color.GRAY);
            gc.fillText("渲染异常，请刷新重试", 
                canvas.getWidth() / 2 - 70, canvas.getHeight() / 2);
        }
    }
    
    private void publishErrorEvent(Throwable throwable) {
        AppEventBus.getInstance().publish(
            new AppEvent(AppEventType.SERVICE_ERROR, this)
                .withData("error", throwable)
        );
    }
    
    private void logError(Throwable throwable) {
        logError(throwable.getMessage(), throwable);
    }
    
    private void logError(String message, Throwable throwable) {
        System.err.println("[ERROR] " + message);
        if (throwable != null) {
            throwable.printStackTrace();
        }
    }
    
    public void setShowDialogs(boolean showDialogs) {
        this.showDialogs = showDialogs;
    }
    
    public void setDefaultHandler(Consumer<Throwable> handler) {
        this.defaultHandler = handler;
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        handlers.clear();
    }
}
