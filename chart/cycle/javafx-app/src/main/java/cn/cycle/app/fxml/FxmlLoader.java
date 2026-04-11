package cn.cycle.app.fxml;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import javafx.fxml.FXMLLoader;

import java.io.IOException;
import java.net.URL;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

public class FxmlLoader extends AbstractLifecycleComponent {
    
    private static final FxmlLoader INSTANCE = new FxmlLoader();
    private static final String FXML_PATH = "/fxml/";
    
    private final Map<String, Object> controllerCache = new ConcurrentHashMap<>();
    
    private FxmlLoader() {
    }
    
    public static FxmlLoader getInstance() {
        return INSTANCE;
    }
    
    public <T> T load(String fxmlFile) throws IOException {
        FXMLLoader loader = new FXMLLoader();
        URL location = FxmlLoader.class.getResource(FXML_PATH + fxmlFile);
        if (location == null) {
            throw new IOException("FXML file not found: " + fxmlFile);
        }
        loader.setLocation(location);
        return loader.load();
    }
    
    public <T> T loadWithController(String fxmlFile, Object controller) throws IOException {
        FXMLLoader loader = new FXMLLoader();
        URL location = FxmlLoader.class.getResource(FXML_PATH + fxmlFile);
        if (location == null) {
            throw new IOException("FXML file not found: " + fxmlFile);
        }
        loader.setLocation(location);
        loader.setController(controller);
        return loader.load();
    }
    
    public FXMLLoader getLoader(String fxmlFile) {
        FXMLLoader loader = new FXMLLoader();
        URL location = FxmlLoader.class.getResource(FXML_PATH + fxmlFile);
        loader.setLocation(location);
        return loader;
    }
    
    public FXMLLoader getLoaderWithController(String fxmlFile, Object controller) {
        FXMLLoader loader = getLoader(fxmlFile);
        loader.setController(controller);
        return loader;
    }
    
    public <T> T loadCached(String fxmlFile) throws IOException {
        Object cached = controllerCache.get(fxmlFile);
        if (cached != null) {
            @SuppressWarnings("unchecked")
            T result = (T) cached;
            return result;
        }
        
        T loaded = load(fxmlFile);
        return loaded;
    }
    
    public void cacheController(String fxmlFile, Object controller) {
        controllerCache.put(fxmlFile, controller);
    }
    
    public Object getCachedController(String fxmlFile) {
        return controllerCache.get(fxmlFile);
    }
    
    public void clearCache() {
        controllerCache.clear();
    }
    
    public void clearCache(String fxmlFile) {
        controllerCache.remove(fxmlFile);
    }
    
    public URL getResource(String fxmlFile) {
        return FxmlLoader.class.getResource(FXML_PATH + fxmlFile);
    }
    
    public boolean exists(String fxmlFile) {
        return getResource(fxmlFile) != null;
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        controllerCache.clear();
    }
}
