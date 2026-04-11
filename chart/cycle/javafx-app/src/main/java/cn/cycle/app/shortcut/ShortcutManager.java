package cn.cycle.app.shortcut;

import cn.cycle.app.context.AppContext;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.Scene;
import javafx.scene.input.KeyCode;
import javafx.scene.input.KeyCodeCombination;
import javafx.scene.input.KeyCombination;
import javafx.stage.Stage;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class ShortcutManager extends AbstractLifecycleComponent {
    
    private static final ShortcutManager INSTANCE = new ShortcutManager();
    
    private Scene scene;
    private final Map<KeyCombination, Runnable> shortcuts = new ConcurrentHashMap<>();
    private final Map<String, KeyCombination> namedShortcuts = new HashMap<>();
    
    private ShortcutManager() {
        initDefaultShortcuts();
    }
    
    public static ShortcutManager getInstance() {
        return INSTANCE;
    }
    
    private void initDefaultShortcuts() {
        register("file.open", new KeyCodeCombination(KeyCode.O, KeyCombination.CONTROL_DOWN), 
            () -> executeIfReady(() -> AppContext.getInstance().getController().openChart()));
        
        register("file.save", new KeyCodeCombination(KeyCode.S, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().saveChart()));
        
        register("file.close", new KeyCodeCombination(KeyCode.W, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().closeChart()));
        
        register("file.exit", new KeyCodeCombination(KeyCode.Q, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().exitApplication()));
        
        register("view.refresh", new KeyCodeCombination(KeyCode.F5),
            () -> executeIfReady(() -> {
            }));
        
        register("view.fullscreen", new KeyCodeCombination(KeyCode.F11),
            () -> executeIfReady(this::toggleFullScreen));
        
        register("view.zoomIn", new KeyCodeCombination(KeyCode.PLUS, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().zoomIn()));
        
        register("view.zoomOut", new KeyCodeCombination(KeyCode.MINUS, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().zoomOut()));
        
        register("view.fitWindow", new KeyCodeCombination(KeyCode.DIGIT0, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().fitToWindow()));
        
        register("edit.undo", new KeyCodeCombination(KeyCode.Z, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().undo()));
        
        register("edit.redo", new KeyCodeCombination(KeyCode.Y, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> AppContext.getInstance().getController().redo()));
        
        register("edit.find", new KeyCodeCombination(KeyCode.F, KeyCombination.CONTROL_DOWN),
            () -> executeIfReady(() -> {
            }));
    }
    
    public void register(String name, KeyCombination combination, Runnable action) {
        shortcuts.put(combination, action);
        namedShortcuts.put(name, combination);
    }
    
    public void unregister(String name) {
        KeyCombination combination = namedShortcuts.remove(name);
        if (combination != null) {
            shortcuts.remove(combination);
        }
    }
    
    public void attachToScene(Scene scene) {
        if (this.scene != null) {
            detachFromScene();
        }
        this.scene = scene;
        
        Map<KeyCombination, Runnable> accelerators = new HashMap<>();
        shortcuts.forEach((key, action) -> accelerators.put(key, action));
        scene.getAccelerators().putAll(accelerators);
    }
    
    public void detachFromScene() {
        if (scene != null) {
            scene.getAccelerators().clear();
            scene = null;
        }
    }
    
    private void toggleFullScreen() {
        if (AppContext.getInstance().isInitialized()) {
            Stage stage = AppContext.getInstance().getPrimaryStage();
            stage.setFullScreen(!stage.isFullScreen());
        }
    }
    
    private void executeIfReady(Runnable action) {
        if (AppContext.getInstance().isInitialized()) {
            try {
                action.run();
            } catch (Exception e) {
                System.err.println("Shortcut action failed: " + e.getMessage());
            }
        }
    }
    
    public KeyCombination getShortcut(String name) {
        return namedShortcuts.get(name);
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        detachFromScene();
        shortcuts.clear();
        namedShortcuts.clear();
    }
}
