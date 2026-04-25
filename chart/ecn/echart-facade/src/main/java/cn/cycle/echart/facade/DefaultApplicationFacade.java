package cn.cycle.echart.facade;

import cn.cycle.echart.alarm.AlarmManager;
import cn.cycle.echart.ais.AISAlarmAssociation;
import cn.cycle.echart.ais.AISTargetManager;
import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.core.ServiceLocator;
import cn.cycle.echart.data.ChartFile;
import cn.cycle.echart.data.ChartFileManager;
import cn.cycle.echart.route.RouteManager;
import cn.cycle.echart.workspace.PanelManager;
import cn.cycle.echart.workspace.WorkspaceManager;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
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
    
    private AlarmFacade alarmFacade;
    private RouteFacade routeFacade;
    private WorkspaceFacade workspaceFacade;
    private AISFacade aisFacade;
    
    private InitializationOrchestrator orchestrator;
    
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
            
            orchestrator = new InitializationOrchestrator();
            orchestrator.setTimeoutSeconds(60);
            
            orchestrator.registerTask("event-bus", "初始化事件总线", 
                    () -> {}, 1);
            orchestrator.registerTask("chart-file-manager", "初始化海图文件管理器", 
                    () -> { chartFileManager = new ChartFileManager(eventBus); }, 2,
                    listOf("event-bus"));
            orchestrator.registerTask("alarm-manager", "初始化预警管理器", 
                    () -> { alarmManager = new AlarmManager(eventBus); }, 2,
                    listOf("event-bus"));
            orchestrator.registerTask("ais-target-manager", "初始化AIS目标管理器", 
                    () -> { aisTargetManager = new AISTargetManager(eventBus); }, 2,
                    listOf("event-bus"));
            orchestrator.registerTask("route-manager", "初始化航线管理器", 
                    () -> { routeManager = new RouteManager(eventBus); }, 2,
                    listOf("event-bus"));
            orchestrator.registerTask("workspace-manager", "初始化工作区管理器", 
                    () -> { workspaceManager = new WorkspaceManager(eventBus); }, 3,
                    listOf("event-bus"));
            orchestrator.registerTask("facades", "初始化业务门面层", 
                    () -> { initializeFacades(); }, 4,
                    listOf("chart-file-manager", "alarm-manager", "ais-target-manager", 
                           "route-manager", "workspace-manager"));
            orchestrator.registerTask("services", "注册服务", 
                    () -> { registerServices(); }, 5,
                    listOf("facades"));
            
            orchestrator.initialize();
            
            initialized = true;
        } catch (InitializationOrchestrator.InitializationException e) {
            throw new FacadeException("initialize", "Failed to initialize application", e);
        } catch (Exception e) {
            throw new FacadeException("initialize", "Failed to initialize application", e);
        }
    }
    
    private List<String> listOf(String... items) {
        List<String> result = new ArrayList<>();
        for (String item : items) {
            result.add(item);
        }
        return result;
    }

    protected void initializeManagers() {
        chartFileManager = new ChartFileManager(eventBus);
        alarmManager = new AlarmManager(eventBus);
        aisTargetManager = new AISTargetManager(eventBus);
        routeManager = new RouteManager(eventBus);
        workspaceManager = new WorkspaceManager(eventBus);
        
        initializeFacades();
    }

    protected void initializeFacades() {
        alarmFacade = new AlarmFacade(alarmManager, eventBus);
        routeFacade = new RouteFacade(routeManager, eventBus);
        workspaceFacade = new WorkspaceFacade(workspaceManager, new PanelManager(eventBus), eventBus);
        aisFacade = new AISFacade(aisTargetManager, new AISAlarmAssociation(alarmManager, aisTargetManager), eventBus);
    }

    protected void registerServices() {
        ServiceLocator.register(ChartFileManager.class, chartFileManager);
        ServiceLocator.register(AlarmManager.class, alarmManager);
        ServiceLocator.register(AISTargetManager.class, aisTargetManager);
        ServiceLocator.register(RouteManager.class, routeManager);
        ServiceLocator.register(WorkspaceManager.class, workspaceManager);
        
        ServiceLocator.register(AlarmFacade.class, alarmFacade);
        ServiceLocator.register(RouteFacade.class, routeFacade);
        ServiceLocator.register(WorkspaceFacade.class, workspaceFacade);
        ServiceLocator.register(AISFacade.class, aisFacade);
        ServiceLocator.register(ApplicationFacade.class, this);
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
        
        if (orchestrator != null) {
            orchestrator.shutdown();
            orchestrator = null;
        }
        
        alarmFacade = null;
        routeFacade = null;
        workspaceFacade = null;
        aisFacade = null;
        
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
    public AlarmFacade getAlarmFacade() {
        return alarmFacade;
    }

    @Override
    public RouteFacade getRouteFacade() {
        return routeFacade;
    }

    @Override
    public WorkspaceFacade getWorkspaceFacade() {
        return workspaceFacade;
    }

    @Override
    public AISFacade getAisFacade() {
        return aisFacade;
    }

    @Override
    public List<ChartFile> getLoadedCharts() {
        if (chartFileManager == null) {
            return Collections.emptyList();
        }
        return chartFileManager.getLoadedCharts();
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
