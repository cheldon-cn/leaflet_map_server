package cn.cycle.echart.facade;

import cn.cycle.echart.data.ChartFile;

import java.util.List;

/**
 * 应用门面接口。
 * 
 * <p>提供统一的业务服务接口层，封装底层模块的复杂性。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface ApplicationFacade {

    /**
     * 初始化应用。
     * 
     * @throws FacadeException 初始化失败时抛出
     */
    void initialize() throws FacadeException;

    /**
     * 启动应用。
     * 
     * @throws FacadeException 启动失败时抛出
     */
    void start() throws FacadeException;

    /**
     * 停止应用。
     */
    void stop();

    /**
     * 销毁应用。
     */
    void destroy();

    /**
     * 检查应用是否已初始化。
     * 
     * @return 是否已初始化
     */
    boolean isInitialized();

    /**
     * 检查应用是否正在运行。
     * 
     * @return 是否正在运行
     */
    boolean isRunning();

    AlarmFacade getAlarmFacade();

    RouteFacade getRouteFacade();

    WorkspaceFacade getWorkspaceFacade();

    AISFacade getAisFacade();

    List<ChartFile> getLoadedCharts();

    /**
     * 加载工作区。
     * 
     * @param workspacePath 工作区路径
     * @throws FacadeException 加载失败时抛出
     */
    void loadWorkspace(String workspacePath) throws FacadeException;

    /**
     * 保存工作区。
     * 
     * @throws FacadeException 保存失败时抛出
     */
    void saveWorkspace() throws FacadeException;

    /**
     * 加载海图文件。
     * 
     * @param filePath 海图文件路径
     * @throws FacadeException 加载失败时抛出
     */
    void loadChart(String filePath) throws FacadeException;

    /**
     * 卸载海图文件。
     * 
     * @param chartId 海图ID
     * @throws FacadeException 卸载失败时抛出
     */
    void unloadChart(String chartId) throws FacadeException;
}
