package cn.cycle.echart.facade;

import cn.cycle.echart.alarm.AlarmManager;
import cn.cycle.echart.ais.AISTargetManager;
import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.core.ServiceLocator;
import cn.cycle.echart.data.ChartFileManager;
import cn.cycle.echart.route.RouteManager;
import cn.cycle.echart.workspace.WorkspaceManager;

import java.util.Objects;

/**
 * 默认应用门面实现。
 * 
 * <p>提供统一的业务服务接口层实现。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DefaultApplicationFacade implements ApplicationFacade {

    private EventBus eventBus;
    private ChartFileManager chartFileManager;
    private AlarmManager alarmManager;
    private AISTargetManager aisTargetManager;
    private RouteManager routeManager;
    private WorkspaceManager workspaceManager;
    
    private boolean initialized;
    private boolean running;

    public DefaultApplicationFacade() {
        this.initialized = false;
        this.running = false;
    }

    @Override
    public void initialize() throws FacadeException {
        if (initialized) {
            return;
        }
        
        try {
            eventBus = new cn.cycle.echart.core.DefaultEventBus();
            initializeManagers();
            registerServices();
            
            initialized = true;
        } catch (Exception e) {
            throw new FacadeException("initialize", "Failed to initialize application", e);
        }
    }

    protected void initializeManagers() {
        chartFileManager = new ChartFileManager(eventBus);
        alarmManager = new AlarmManager(eventBus);
        aisTargetManager = new AISTargetManager(eventBus);
        routeManager = new RouteManager(eventBus);
        workspaceManager = new WorkspaceManager(eventBus);
    }

    protected void registerServices() {
        ServiceLocator.register(ChartFileManager.class, chartFileManager);
        ServiceLocator.register(AlarmManager.class, alarmManager);
        ServiceLocator.register(AISTargetManager.class, aisTargetManager);
        ServiceLocator.register(RouteManager.class, routeManager);
        ServiceLocator.register(WorkspaceManager.class, workspaceManager);
    }

    @Override
    public void start() throws FacadeException {
        if (!initialized) {
            throw new FacadeException("start", "Application not initialized");
        }
        
        if (running) {
            return;
        }
        
        running = true;
    }

    @Override
    public void stop() {
        if (!running) {
            return;
        }
        
        running = false;
    }

    @Override
    public void destroy() {
        stop();
        
        if (chartFileManager != null) {
            chartFileManager = null;
        }
        if (alarmManager != null) {
            alarmManager = null;
        }
        if (aisTargetManager != null) {
            aisTargetManager = null;
        }
        if (routeManager != null) {
            routeManager = null;
        }
        if (workspaceManager != null) {
            workspaceManager = null;
        }
        if (eventBus != null) {
            eventBus = null;
        }
        
        ServiceLocator.clear();
        initialized = false;
    }

    @Override
    public boolean isInitialized() {
        return initialized;
    }

    @Override
    public boolean isRunning() {
        return running;
    }

    @Override
    public ChartFileManager getChartFileManager() {
        return chartFileManager;
    }

    @Override
    public AlarmManager getAlarmManager() {
        return alarmManager;
    }

    @Override
    public AISTargetManager getAisTargetManager() {
        return aisTargetManager;
    }

    @Override
    public RouteManager getRouteManager() {
        return routeManager;
    }

    @Override
    public WorkspaceManager getWorkspaceManager() {
        return workspaceManager;
    }

    @Override
    public void loadWorkspace(String workspacePath) throws FacadeException {
        Objects.requireNonNull(workspacePath, "workspacePath cannot be null");
        
        if (!initialized) {
            throw new FacadeException("loadWorkspace", "Application not initialized");
        }
        
        try {
            java.io.File file = new java.io.File(workspacePath);
            workspaceManager.loadWorkspace(file);
        } catch (Exception e) {
            throw new FacadeException("loadWorkspace", 
                    "Failed to load workspace: " + workspacePath, e);
        }
    }

    @Override
    public void saveWorkspace() throws FacadeException {
        if (!initialized) {
            throw new FacadeException("saveWorkspace", "Application not initialized");
        }
        
        try {
            workspaceManager.saveCurrentWorkspace();
        } catch (Exception e) {
            throw new FacadeException("saveWorkspace", "Failed to save workspace", e);
        }
    }

    @Override
    public void loadChart(String filePath) throws FacadeException {
        Objects.requireNonNull(filePath, "filePath cannot be null");
        
        if (!initialized) {
            throw new FacadeException("loadChart", "Application not initialized");
        }
        
        try {
            chartFileManager.loadChart(filePath);
        } catch (Exception e) {
            throw new FacadeException("loadChart", 
                    "Failed to load chart: " + filePath, e);
        }
    }

    @Override
    public void unloadChart(String chartId) throws FacadeException {
        Objects.requireNonNull(chartId, "chartId cannot be null");
        
        if (!initialized) {
            throw new FacadeException("unloadChart", "Application not initialized");
        }
        
        try {
            chartFileManager.unloadChart(chartId);
        } catch (Exception e) {
            throw new FacadeException("unloadChart", 
                    "Failed to unload chart: " + chartId, e);
        }
    }
}
