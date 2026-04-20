package cn.cycle.echart.theme;

import javafx.scene.image.Image;
import javafx.scene.image.ImageView;

import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 图标资源适配器。
 * 
 * <p>根据DPI缩放因子自动选择合适尺寸的图标资源。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class IconResourceAdapter {

    private static final String[] SIZE_SUFFIXES = {"", "@1.25x", "@1.5x", "@1.75x", "@2x", "@2.5x", "@3x"};
    private static final double[] SIZE_SCALES = {1.0, 1.25, 1.5, 1.75, 2.0, 2.5, 3.0};

    private final HighDPIScaler scaler;
    private final Map<String, Image> iconCache;
    private String iconBasePath;
    private boolean cacheEnabled;

    public IconResourceAdapter(HighDPIScaler scaler) {
        this.scaler = scaler;
        this.iconCache = new HashMap<>();
        this.iconBasePath = "/icons/";
        this.cacheEnabled = true;
    }

    public ImageView getIcon(String iconName) {
        return getIcon(iconName, -1, -1);
    }

    public ImageView getIcon(String iconName, double width, double height) {
        Objects.requireNonNull(iconName, "iconName cannot be null");
        
        Image image = loadImage(iconName);
        if (image == null) {
            return null;
        }
        
        ImageView imageView = new ImageView(image);
        
        if (width > 0 && height > 0) {
            imageView.setFitWidth(scaler.scaleValue(width));
            imageView.setFitHeight(scaler.scaleValue(height));
        } else {
            double scale = scaler.getScale();
            imageView.setFitWidth(image.getWidth() * scale);
            imageView.setFitHeight(image.getHeight() * scale);
        }
        
        imageView.setPreserveRatio(true);
        imageView.setSmooth(true);
        
        return imageView;
    }

    public Image loadImage(String iconName) {
        Objects.requireNonNull(iconName, "iconName cannot be null");
        
        String cacheKey = getCacheKey(iconName);
        
        if (cacheEnabled && iconCache.containsKey(cacheKey)) {
            return iconCache.get(cacheKey);
        }
        
        String iconPath = resolveIconPath(iconName);
        Image image = loadIconFromPath(iconPath);
        
        if (image == null) {
            image = loadIconFromPath(iconBasePath + iconName);
        }
        
        if (image != null && cacheEnabled) {
            iconCache.put(cacheKey, image);
        }
        
        return image;
    }

    private String resolveIconPath(String iconName) {
        double scale = scaler.getScale();
        String bestSuffix = "";
        double bestScale = 1.0;
        
        for (int i = 0; i < SIZE_SCALES.length; i++) {
            if (SIZE_SCALES[i] <= scale && SIZE_SCALES[i] >= bestScale) {
                bestScale = SIZE_SCALES[i];
                bestSuffix = SIZE_SUFFIXES[i];
            }
        }
        
        int dotIndex = iconName.lastIndexOf('.');
        if (dotIndex > 0) {
            String baseName = iconName.substring(0, dotIndex);
            String extension = iconName.substring(dotIndex);
            return iconBasePath + baseName + bestSuffix + extension;
        } else {
            return iconBasePath + iconName + bestSuffix;
        }
    }

    private Image loadIconFromPath(String path) {
        try {
            InputStream stream = getClass().getResourceAsStream(path);
            if (stream != null) {
                return new Image(stream);
            }
        } catch (Exception e) {
        }
        return null;
    }

    private String getCacheKey(String iconName) {
        double scale = scaler.getScale();
        return iconName + "@" + scale;
    }

    public void setIconBasePath(String basePath) {
        Objects.requireNonNull(basePath, "basePath cannot be null");
        this.iconBasePath = basePath;
        if (!this.iconBasePath.endsWith("/")) {
            this.iconBasePath += "/";
        }
    }

    public String getIconBasePath() {
        return iconBasePath;
    }

    public void setCacheEnabled(boolean cacheEnabled) {
        this.cacheEnabled = cacheEnabled;
    }

    public boolean isCacheEnabled() {
        return cacheEnabled;
    }

    public void clearCache() {
        iconCache.clear();
    }

    public void preloadIcon(String iconName) {
        loadImage(iconName);
    }

    public void preloadIcons(String... iconNames) {
        for (String iconName : iconNames) {
            preloadIcon(iconName);
        }
    }

    public static String[] getSizeSuffixes() {
        return SIZE_SUFFIXES.clone();
    }

    public static double[] getSizeScales() {
        return SIZE_SCALES.clone();
    }
}
