package cn.cycle.app.dpi;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

import java.io.InputStream;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class IconLoader extends AbstractLifecycleComponent {
    
    private static final IconLoader INSTANCE = new IconLoader();
    private static final String ICON_BASE_PATH = "/icons/";
    
    private final Map<String, Image> iconCache = new ConcurrentHashMap<>();
    private DPIScaler dpiScaler;
    
    private IconLoader() {
    }
    
    public static IconLoader getInstance() {
        return INSTANCE;
    }
    
    public void setDpiScaler(DPIScaler dpiScaler) {
        this.dpiScaler = dpiScaler;
    }
    
    public Image loadIcon(String name) {
        String cacheKey = getCacheKey(name);
        
        Image cached = iconCache.get(cacheKey);
        if (cached != null) {
            return cached;
        }
        
        Image icon = loadIconFromDisk(name);
        if (icon != null) {
            iconCache.put(cacheKey, icon);
        }
        
        return icon;
    }
    
    public ImageView loadIconView(String name) {
        Image icon = loadIcon(name);
        if (icon != null) {
            return new ImageView(icon);
        }
        return null;
    }
    
    public ImageView loadIconView(String name, double size) {
        ImageView view = loadIconView(name);
        if (view != null) {
            view.setFitWidth(size);
            view.setFitHeight(size);
        }
        return view;
    }
    
    public Image loadIcon(String name, String sizeDir) {
        String cacheKey = sizeDir + "/" + name;
        
        Image cached = iconCache.get(cacheKey);
        if (cached != null) {
            return cached;
        }
        
        Image icon = loadIconFromDisk(name, sizeDir);
        if (icon != null) {
            iconCache.put(cacheKey, icon);
        }
        
        return icon;
    }
    
    private Image loadIconFromDisk(String name) {
        String sizeDir = getSizeDir();
        return loadIconFromDisk(name, sizeDir);
    }
    
    private Image loadIconFromDisk(String name, String sizeDir) {
        String path = ICON_BASE_PATH + sizeDir + "/" + name;
        if (!name.endsWith(".png") && !name.endsWith(".svg")) {
            path += ".png";
        }
        
        try {
            InputStream is = IconLoader.class.getResourceAsStream(path);
            if (is != null) {
                return new Image(is);
            }
            
            return tryFallbackSizes(name, sizeDir);
        } catch (Exception e) {
            System.err.println("Failed to load icon: " + path + ", " + e.getMessage());
            return null;
        }
    }
    
    private Image tryFallbackSizes(String name, String failedSizeDir) {
        String[] sizeDirs = {"32x32", "24x24", "20x20", "16x16"};
        
        for (String sizeDir : sizeDirs) {
            if (sizeDir.equals(failedSizeDir)) {
                continue;
            }
            
            String path = ICON_BASE_PATH + sizeDir + "/" + name;
            if (!name.endsWith(".png") && !name.endsWith(".svg")) {
                path += ".png";
            }
            
            try {
                InputStream is = IconLoader.class.getResourceAsStream(path);
                if (is != null) {
                    return new Image(is);
                }
            } catch (Exception ignored) {
            }
        }
        
        return null;
    }
    
    private String getSizeDir() {
        if (dpiScaler != null) {
            return dpiScaler.getIconSizeDir();
        }
        return "16x16";
    }
    
    private String getCacheKey(String name) {
        return getSizeDir() + "/" + name;
    }
    
    public void clearCache() {
        iconCache.clear();
    }
    
    public void preloadIcons(String[] names) {
        for (String name : names) {
            loadIcon(name);
        }
    }
    
    @Override
    protected void doInitialize() {
        if (dpiScaler == null) {
            dpiScaler = DPIScaler.getInstance();
        }
    }
    
    @Override
    protected void doDispose() {
        iconCache.clear();
    }
}
