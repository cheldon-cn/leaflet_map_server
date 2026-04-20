package cn.cycle.echart.theme;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 高对比度检测器。
 * 
 * <p>检测系统是否启用了高对比度模式，并通知监听器。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class HighContrastDetector {

    private static HighContrastDetector instance;
    
    private boolean highContrastEnabled;
    private final List<HighContrastChangeListener> listeners;
    
    private HighContrastDetector() {
        this.listeners = new CopyOnWriteArrayList<>();
        this.highContrastEnabled = detectSystemHighContrast();
    }
    
    public static synchronized HighContrastDetector getInstance() {
        if (instance == null) {
            instance = new HighContrastDetector();
        }
        return instance;
    }
    
    public boolean isHighContrastEnabled() {
        return highContrastEnabled;
    }
    
    public void refresh() {
        boolean newValue = detectSystemHighContrast();
        if (newValue != highContrastEnabled) {
            boolean oldValue = highContrastEnabled;
            highContrastEnabled = newValue;
            notifyListeners(oldValue, newValue);
        }
    }
    
    private boolean detectSystemHighContrast() {
        String osName = System.getProperty("os.name", "").toLowerCase();
        
        if (osName.contains("win")) {
            return checkWindowsHighContrast();
        } else if (osName.contains("mac")) {
            return checkMacHighContrast();
        } else if (osName.contains("nux") || osName.contains("nix")) {
            return checkLinuxHighContrast();
        }
        
        return false;
    }
    
    private static boolean checkWindowsHighContrast() {
        try {
            String highContrast = System.getProperty("highContrast", "false");
            if ("true".equalsIgnoreCase(highContrast)) {
                return true;
            }
            
            String contrast = System.getenv("HIGHCONTRAST");
            if (contrast != null && !"off".equalsIgnoreCase(contrast)) {
                return true;
            }
            
            return false;
        } catch (SecurityException e) {
            return false;
        }
    }
    
    private static boolean checkMacHighContrast() {
        try {
            String contrast = System.getenv("NSAccessibilityEnabled");
            if (contrast != null) {
                return true;
            }
            return false;
        } catch (SecurityException e) {
            return false;
        }
    }
    
    private static boolean checkLinuxHighContrast() {
        try {
            String contrast = System.getenv("GTK_THEME");
            if (contrast != null && contrast.toLowerCase().contains("high-contrast")) {
                return true;
            }
            return false;
        } catch (SecurityException e) {
            return false;
        }
    }
    
    public void addListener(HighContrastChangeListener listener) {
        if (listener != null) {
            listeners.add(listener);
        }
    }
    
    public void removeListener(HighContrastChangeListener listener) {
        if (listener != null) {
            listeners.remove(listener);
        }
    }
    
    private void notifyListeners(boolean oldValue, boolean newValue) {
        for (HighContrastChangeListener listener : listeners) {
            try {
                listener.onHighContrastChanged(oldValue, newValue);
            } catch (Exception e) {
                System.err.println("Error notifying high contrast change listener: " + e.getMessage());
            }
        }
    }
    
    /**
     * 高对比度状态变化监听器接口。
     */
    public interface HighContrastChangeListener {
        void onHighContrastChanged(boolean oldValue, boolean newValue);
    }
}
