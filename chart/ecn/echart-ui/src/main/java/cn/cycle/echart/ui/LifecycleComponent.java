package cn.cycle.echart.ui;

/**
 * 生命周期组件接口。
 * 
 * <p>定义UI组件的生命周期方法，包括初始化、激活、停用和销毁。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public interface LifecycleComponent {

    /**
     * 初始化组件。
     * 
     * <p>在组件首次创建时调用，用于执行一次性初始化操作。</p>
     */
    void initialize();

    /**
     * 激活组件。
     * 
     * <p>在组件变为可见或获得焦点时调用。</p>
     */
    void activate();

    /**
     * 停用组件。
     * 
     * <p>在组件变为不可见或失去焦点时调用。</p>
     */
    void deactivate();

    /**
     * 销毁组件。
     * 
     * <p>在组件被永久移除时调用，用于释放资源。</p>
     */
    void dispose();

    /**
     * 检查组件是否已初始化。
     * 
     * @return 如果已初始化返回true，否则返回false
     */
    boolean isInitialized();

    /**
     * 检查组件是否处于激活状态。
     * 
     * @return 如果已激活返回true，否则返回false
     */
    boolean isActive();

    /**
     * 检查组件是否已销毁。
     * 
     * @return 如果已销毁返回true，否则返回false
     */
    boolean isDisposed();
}
