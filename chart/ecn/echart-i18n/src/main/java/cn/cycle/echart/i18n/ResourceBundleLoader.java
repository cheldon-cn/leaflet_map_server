package cn.cycle.echart.i18n;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.net.URLConnection;
import java.security.AccessController;
import java.security.PrivilegedActionException;
import java.security.PrivilegedExceptionAction;
import java.util.Locale;
import java.util.Objects;
import java.util.PropertyResourceBundle;
import java.util.ResourceBundle;

/**
 * 资源包加载器。
 * 
 * <p>支持UTF-8编码的资源文件加载。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ResourceBundleLoader {

    private static final String BUNDLE_BASE_NAME = "i18n.messages";
    private static final String CHARSET = "UTF-8";

    private final String baseName;
    private final ClassLoader classLoader;

    public ResourceBundleLoader() {
        this(BUNDLE_BASE_NAME);
    }

    public ResourceBundleLoader(String baseName) {
        this(baseName, ResourceBundleLoader.class.getClassLoader());
    }

    public ResourceBundleLoader(String baseName, ClassLoader classLoader) {
        this.baseName = Objects.requireNonNull(baseName, "baseName cannot be null");
        this.classLoader = Objects.requireNonNull(classLoader, "classLoader cannot be null");
    }

    /**
     * 加载指定区域设置的资源包。
     * 
     * @param locale 区域设置
     * @return 资源包
     */
    public ResourceBundle loadBundle(Locale locale) {
        Objects.requireNonNull(locale, "locale cannot be null");
        
        ResourceBundle.Control control = new ResourceBundle.Control() {
            @Override
            public ResourceBundle newBundle(String baseName, Locale locale, 
                    String format, ClassLoader loader, boolean reload)
                    throws IllegalAccessException, InstantiationException, IOException {
                
                String bundleName = toBundleName(baseName, locale);
                String resourceName = toResourceName(bundleName, "properties");
                
                URL url = loader.getResource(resourceName);
                if (url == null) {
                    return null;
                }
                
                URLConnection connection = url.openConnection();
                if (reload) {
                    connection.setUseCaches(false);
                }
                
                try (InputStream stream = connection.getInputStream()) {
                    return new PropertyResourceBundle(stream);
                }
            }
            
            @Override
            public Locale getFallbackLocale(String baseName, Locale locale) {
                return locale.equals(Locale.ROOT) ? null : Locale.ROOT;
            }
        };
        
        try {
            return ResourceBundle.getBundle(baseName, locale, classLoader, control);
        } catch (Exception e) {
            return null;
        }
    }

    /**
     * 获取基础名称。
     * 
     * @return 基础名称
     */
    public String getBaseName() {
        return baseName;
    }

    /**
     * 检查指定区域设置的资源包是否存在。
     * 
     * @param locale 区域设置
     * @return 是否存在
     */
    public boolean bundleExists(Locale locale) {
        String bundleName = baseName + "_" + locale.toString();
        String resourceName = bundleName.replace('.', '/') + ".properties";
        return classLoader.getResource(resourceName) != null;
    }
}
