package cn.cycle.echart.core;

/**
 * 生命周期组件接口。
 * 
 * <p>所有需要生命周期管理的组件都应实现此接口。</p>
 * 
 * <h2>生命周期状态</h2>
 * <ul>
 *   <li>CREATED - 组件已创建</li>
 *   <li>INITIALIZED - 组件已初始化</li>
 *   <li>STARTED - 组件已启动</li>
 *   <li>STOPPED - 组件已停止</li>
 *   <li>DISPOSED - 组件已销毁</li>
 * </ul>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface LifecycleComponent {

    /**
     * 生命周期状态枚举。
     */
    enum State {
        CREATED,
        INITIALIZED,
        STARTED,
        STOPPED,
        DISPOSED
    }

    /**
     * 获取组件名称。
     * 
     * @return 组件名称，不为null
     */
    String getName();

    /**
     * 获取当前状态。
     * 
     * @return 当前状态，不为null
     */
    State getState();

    /**
     * 初始化组件。
     * 
     * <p>此方法应在组件创建后调用一次。</p>
     * 
     * @throws IllegalStateException 如果组件状态不允许初始化
     */
    void initialize();

    /**
     * 启动组件。
     * 
     * <p>此方法应在初始化后调用。</p>
     * 
     * @throws IllegalStateException 如果组件状态不允许启动
     */
    void start();

    /**
     * 停止组件。
     * 
     * <p>此方法应在启动后调用。</p>
     * 
     * @throws IllegalStateException 如果组件状态不允许停止
     */
    void stop();

    /**
     * 销毁组件。
     * 
     * <p>此方法释放所有资源，调用后组件不可再使用。</p>
     */
    void dispose();

    /**
     * 检查组件是否已初始化。
     * 
     * @return 如果已初始化返回true
     */
    boolean isInitialized();

    /**
     * 检查组件是否正在运行。
     * 
     * @return 如果正在运行返回true
     */
    boolean isRunning();

    /**
     * 检查组件是否已销毁。
     * 
     * @return 如果已销毁返回true
     */
    boolean isDisposed();
}
