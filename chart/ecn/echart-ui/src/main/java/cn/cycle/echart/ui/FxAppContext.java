package cn.cycle.echart.ui;

import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.core.ServiceLocator;
import cn.cycle.echart.facade.WorkspaceFacade;
import cn.cycle.echart.i18n.I18nManager;
import cn.cycle.echart.workspace.Workspace;

import java.util.Locale;
import java.util.Objects;

/**
 * JavaFX应用上下文。
 * 
 * <p>提供应用级别的服务和状态管理。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FxAppContext {

    private static FxAppContext instance;
    
    private final EventBus eventBus;
    private final I18nManager i18nManager;
    private final WorkspaceFacade workspaceFacade;
    private final ServiceLocator serviceLocator;
    private final FxPlatformAdapter platformAdapter;
    
    private Workspace currentWorkspace;
    private Locale currentLocale;

    private FxAppContext(EventBus eventBus, I18nManager i18nManager, 
            WorkspaceFacade workspaceFacade, ServiceLocator serviceLocator) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.i18nManager = Objects.requireNonNull(i18nManager, "i18nManager cannot be null");
        this.workspaceFacade = Objects.requireNonNull(workspaceFacade, "workspaceFacade cannot be null");
        this.serviceLocator = Objects.requireNonNull(serviceLocator, "serviceLocator cannot be null");
        this.platformAdapter = new FxPlatformAdapter();
        this.currentLocale = Locale.getDefault();
    }

    public static synchronized FxAppContext getInstance() {
        if (instance == null) {
            throw new IllegalStateException("FxAppContext not initialized. Call initialize() first.");
        }
        return instance;
    }

    public static synchronized void initialize(EventBus eventBus, I18nManager i18nManager,
            WorkspaceFacade workspaceFacade, ServiceLocator serviceLocator) {
        if (instance != null) {
            throw new IllegalStateException("FxAppContext already initialized.");
        }
        instance = new FxAppContext(eventBus, i18nManager, workspaceFacade, serviceLocator);
    }

    public static synchronized void shutdown() {
        instance = null;
    }

    public EventBus getEventBus() {
        return eventBus;
    }

    public I18nManager getI18nManager() {
        return i18nManager;
    }

    public WorkspaceFacade getWorkspaceFacade() {
        return workspaceFacade;
    }

    public ServiceLocator getServiceLocator() {
        return serviceLocator;
    }

    public FxPlatformAdapter getPlatformAdapter() {
        return platformAdapter;
    }

    public Workspace getCurrentWorkspace() {
        return currentWorkspace;
    }

    public void setCurrentWorkspace(Workspace workspace) {
        this.currentWorkspace = workspace;
    }

    public Locale getCurrentLocale() {
        return currentLocale;
    }

    public void setCurrentLocale(Locale locale) {
        Objects.requireNonNull(locale, "locale cannot be null");
        this.currentLocale = locale;
        i18nManager.setLocale(locale);
    }

    public String getMessage(String key) {
        return i18nManager.getMessage(key);
    }

    public String getMessage(String key, Object... args) {
        return i18nManager.getMessage(key, args);
    }

    public void runOnUiThread(Runnable action) {
        platformAdapter.runOnUiThread(action);
    }

    public boolean isUiThread() {
        return platformAdapter.isUiThread();
    }
}
