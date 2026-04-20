package cn.cycle.echart.plugin;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.net.URLClassLoader;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.Objects;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

/**
 * 插件加载器。
 * 
 * <p>负责从文件系统加载插件JAR包并实例化插件类。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class PluginLoader {

    private final PluginSecurityManager securityManager;
    private final List<LoadListener> listeners;
    private Path pluginDirectory;
    private boolean validateSignatures;

    public PluginLoader(PluginSecurityManager securityManager) {
        this.securityManager = Objects.requireNonNull(securityManager, "securityManager cannot be null");
        this.listeners = new ArrayList<>();
        this.pluginDirectory = Paths.get("plugins");
        this.validateSignatures = false;
    }

    public List<PluginDescriptor> scanPlugins() throws IOException {
        List<PluginDescriptor> descriptors = new ArrayList<>();
        
        File pluginDir = pluginDirectory.toFile();
        if (!pluginDir.exists() || !pluginDir.isDirectory()) {
            return descriptors;
        }

        File[] jarFiles = pluginDir.listFiles((dir, name) -> name.endsWith(".jar"));
        if (jarFiles == null) {
            return descriptors;
        }

        for (File jarFile : jarFiles) {
            try {
                PluginDescriptor descriptor = scanJarFile(jarFile);
                if (descriptor != null) {
                    descriptors.add(descriptor);
                }
            } catch (Exception e) {
                notifyScanFailed(jarFile.toPath(), e);
            }
        }

        return descriptors;
    }

    private PluginDescriptor scanJarFile(File jarFile) throws IOException {
        try (JarFile jar = new JarFile(jarFile)) {
            String pluginClass = findPluginClass(jar);
            if (pluginClass == null) {
                return null;
            }

            PluginDescriptor descriptor = new PluginDescriptor();
            descriptor.setJarPath(jarFile.toPath());
            descriptor.setPluginClassName(pluginClass);

            JarEntry manifestEntry = jar.getJarEntry("plugin.properties");
            if (manifestEntry != null) {
                java.util.Properties props = new java.util.Properties();
                props.load(jar.getInputStream(manifestEntry));
                
                descriptor.setId(props.getProperty("plugin.id", "unknown"));
                descriptor.setName(props.getProperty("plugin.name", "Unknown Plugin"));
                descriptor.setVersion(props.getProperty("plugin.version", "1.0.0"));
                descriptor.setDescription(props.getProperty("plugin.description", ""));
                descriptor.setAuthor(props.getProperty("plugin.author", ""));
            }

            return descriptor;
        }
    }

    private String findPluginClass(JarFile jar) {
        Enumeration<JarEntry> entries = jar.entries();
        while (entries.hasMoreElements()) {
            JarEntry entry = entries.nextElement();
            String name = entry.getName();
            
            if (name.endsWith(".class")) {
                String className = name.substring(0, name.length() - 6)
                        .replace('/', '.');
                
                if (isPluginClass(jar, entry, className)) {
                    return className;
                }
            }
        }
        return null;
    }

    private boolean isPluginClass(JarFile jar, JarEntry entry, String className) {
        return className.endsWith("Plugin") || 
               className.contains(".plugin.") ||
               className.endsWith("Extension");
    }

    public Plugin loadPlugin(PluginDescriptor descriptor) throws PluginException {
        Objects.requireNonNull(descriptor, "descriptor cannot be null");

        Path jarPath = descriptor.getJarPath();
        if (!Files.exists(jarPath)) {
            throw new PluginException("Plugin JAR not found: " + jarPath);
        }

        notifyLoadStarted(descriptor);

        try {
            if (validateSignatures && !securityManager.validateSignature(jarPath)) {
                throw new PluginException("Plugin signature validation failed: " + jarPath);
            }

            URL jarUrl = jarPath.toUri().toURL();
            URLClassLoader classLoader = new URLClassLoader(
                    new URL[]{jarUrl},
                    getClass().getClassLoader()
            );

            Class<?> pluginClass = classLoader.loadClass(descriptor.getPluginClassName());
            
            if (!Plugin.class.isAssignableFrom(pluginClass)) {
                throw new PluginException("Class does not implement Plugin interface: " + 
                        descriptor.getPluginClassName());
            }

            @SuppressWarnings("unchecked")
            Class<? extends Plugin> castedClass = (Class<? extends Plugin>) pluginClass;
            Plugin plugin = castedClass.getDeclaredConstructor().newInstance();
            
            plugin.setClassLoader(classLoader);

            notifyLoadCompleted(descriptor, plugin);
            return plugin;

        } catch (Exception e) {
            notifyLoadFailed(descriptor, e);
            throw new PluginException("Failed to load plugin: " + descriptor.getId(), e);
        }
    }

    public void unloadPlugin(Plugin plugin) {
        if (plugin == null) {
            return;
        }

        notifyUnloadStarted(plugin);

        try {
            plugin.stop();
            
            ClassLoader classLoader = plugin.getClassLoader();
            if (classLoader instanceof URLClassLoader) {
                ((URLClassLoader) classLoader).close();
            }

            notifyUnloadCompleted(plugin);
        } catch (Exception e) {
            notifyUnloadFailed(plugin, e);
        }
    }

    public void setPluginDirectory(Path directory) {
        this.pluginDirectory = Objects.requireNonNull(directory);
    }

    public Path getPluginDirectory() {
        return pluginDirectory;
    }

    public void setValidateSignatures(boolean validate) {
        this.validateSignatures = validate;
    }

    public boolean isValidateSignatures() {
        return validateSignatures;
    }

    public void addListener(LoadListener listener) {
        listeners.add(listener);
    }

    public void removeListener(LoadListener listener) {
        listeners.remove(listener);
    }

    private void notifyScanFailed(Path jarPath, Exception e) {
        for (LoadListener listener : listeners) {
            listener.onScanFailed(jarPath, e);
        }
    }

    private void notifyLoadStarted(PluginDescriptor descriptor) {
        for (LoadListener listener : listeners) {
            listener.onLoadStarted(descriptor);
        }
    }

    private void notifyLoadCompleted(PluginDescriptor descriptor, Plugin plugin) {
        for (LoadListener listener : listeners) {
            listener.onLoadCompleted(descriptor, plugin);
        }
    }

    private void notifyLoadFailed(PluginDescriptor descriptor, Exception e) {
        for (LoadListener listener : listeners) {
            listener.onLoadFailed(descriptor, e);
        }
    }

    private void notifyUnloadStarted(Plugin plugin) {
        for (LoadListener listener : listeners) {
            listener.onUnloadStarted(plugin);
        }
    }

    private void notifyUnloadCompleted(Plugin plugin) {
        for (LoadListener listener : listeners) {
            listener.onUnloadCompleted(plugin);
        }
    }

    private void notifyUnloadFailed(Plugin plugin, Exception e) {
        for (LoadListener listener : listeners) {
            listener.onUnloadFailed(plugin, e);
        }
    }

    public interface LoadListener {
        void onScanFailed(Path jarPath, Exception e);
        void onLoadStarted(PluginDescriptor descriptor);
        void onLoadCompleted(PluginDescriptor descriptor, Plugin plugin);
        void onLoadFailed(PluginDescriptor descriptor, Exception e);
        void onUnloadStarted(Plugin plugin);
        void onUnloadCompleted(Plugin plugin);
        void onUnloadFailed(Plugin plugin, Exception e);
    }

    public static class PluginDescriptor {
        private String id;
        private String name;
        private String version;
        private String description;
        private String author;
        private Path jarPath;
        private String pluginClassName;

        public String getId() {
            return id;
        }

        public void setId(String id) {
            this.id = id;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public String getVersion() {
            return version;
        }

        public void setVersion(String version) {
            this.version = version;
        }

        public String getDescription() {
            return description;
        }

        public void setDescription(String description) {
            this.description = description;
        }

        public String getAuthor() {
            return author;
        }

        public void setAuthor(String author) {
            this.author = author;
        }

        public Path getJarPath() {
            return jarPath;
        }

        public void setJarPath(Path jarPath) {
            this.jarPath = jarPath;
        }

        public String getPluginClassName() {
            return pluginClassName;
        }

        public void setPluginClassName(String pluginClassName) {
            this.pluginClassName = pluginClassName;
        }
    }
}
