package cn.cycle.echart.ui;

import cn.cycle.echart.core.PlatformAdapter;

import javafx.application.Platform;
import javafx.scene.media.AudioClip;

import java.io.File;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.HashMap;
import java.util.Map;

/**
 * JavaFX平台适配器实现。
 * 
 * <p>提供JavaFX平台的UI线程调度和资源管理。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FxPlatformAdapter implements PlatformAdapter {

    private final Map<String, AudioClip> soundCache;
    private AudioClip currentSound;

    public FxPlatformAdapter() {
        this.soundCache = new HashMap<>();
    }

    @Override
    public String getPlatformName() {
        return "JavaFX";
    }

    @Override
    public void runOnUiThread(Runnable task) {
        if (Platform.isFxApplicationThread()) {
            task.run();
        } else {
            Platform.runLater(task);
        }
    }

    @Override
    public boolean isUiThread() {
        return Platform.isFxApplicationThread();
    }

    @Override
    public void playSound(String soundPath) {
        stopSound();
        
        AudioClip clip = soundCache.get(soundPath);
        if (clip == null) {
            String fullPath = getResourcePath(soundPath);
            if (fullPath != null) {
                clip = new AudioClip(new File(fullPath).toURI().toString());
                soundCache.put(soundPath, clip);
            }
        }
        
        if (clip != null) {
            currentSound = clip;
            clip.play();
        }
    }

    @Override
    public void stopSound() {
        if (currentSound != null) {
            currentSound.stop();
            currentSound = null;
        }
    }

    @Override
    public void showNotification(String title, String message, String type) {
        runOnUiThread(() -> {
            javafx.scene.control.Alert.AlertType alertType;
            switch (type.toLowerCase()) {
                case "error":
                    alertType = javafx.scene.control.Alert.AlertType.ERROR;
                    break;
                case "warning":
                    alertType = javafx.scene.control.Alert.AlertType.WARNING;
                    break;
                default:
                    alertType = javafx.scene.control.Alert.AlertType.INFORMATION;
                    break;
            }
            
            javafx.scene.control.Alert alert = new javafx.scene.control.Alert(alertType);
            alert.setTitle(title);
            alert.setHeaderText(null);
            alert.setContentText(message);
            alert.show();
        });
    }

    @Override
    public double getScreenDpi() {
        return javafx.stage.Screen.getPrimary().getDpi();
    }

    @Override
    public double getScreenScale() {
        return 1.0;
    }

    @Override
    public String getResourcePath(String relativePath) {
        String userDir = System.getProperty("user.dir");
        Path path = Paths.get(userDir, "resources", relativePath);
        if (path.toFile().exists()) {
            return path.toString();
        }
        
        path = Paths.get(userDir, relativePath);
        if (path.toFile().exists()) {
            return path.toString();
        }
        
        return null;
    }

    @Override
    public String getConfigDirectory() {
        String userHome = System.getProperty("user.home");
        Path configDir = Paths.get(userHome, ".echart", "config");
        if (!configDir.toFile().exists()) {
            configDir.toFile().mkdirs();
        }
        return configDir.toString();
    }

    @Override
    public String getLogDirectory() {
        String userHome = System.getProperty("user.home");
        Path logDir = Paths.get(userHome, ".echart", "logs");
        if (!logDir.toFile().exists()) {
            logDir.toFile().mkdirs();
        }
        return logDir.toString();
    }

    @Override
    public String getCacheDirectory() {
        String userHome = System.getProperty("user.home");
        Path cacheDir = Paths.get(userHome, ".echart", "cache");
        if (!cacheDir.toFile().exists()) {
            cacheDir.toFile().mkdirs();
        }
        return cacheDir.toString();
    }
}
