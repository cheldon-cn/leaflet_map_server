package cn.cycle.echart.core;

import java.util.concurrent.atomic.AtomicReference;

/**
 * 生命周期组件抽象基类。
 * 
 * <p>提供生命周期状态管理和线程安全的状态转换。</p>
 * 
 * <h2>使用示例</h2>
 * <pre>{@code
 * public class MyComponent extends AbstractLifecycleComponent {
 *     protected void doInitialize() {
 *         // 初始化逻辑
 *     }
 *     protected void doStart() {
 *         // 启动逻辑
 *     }
 *     protected void doStop() {
 *         // 停止逻辑
 *     }
 *     protected void doDispose() {
 *         // 销毁逻辑
 *     }
 * }
 * }</pre>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public abstract class AbstractLifecycleComponent implements LifecycleComponent {

    private final String name;
    private final AtomicReference<State> state;

    /**
     * 创建组件实例。
     * 
     * @param name 组件名称，不能为null
     * @throws IllegalArgumentException 如果name为null
     */
    protected AbstractLifecycleComponent(String name) {
        if (name == null) {
            throw new IllegalArgumentException("name cannot be null");
        }
        this.name = name;
        this.state = new AtomicReference<>(State.CREATED);
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public State getState() {
        return state.get();
    }

    @Override
    public void initialize() {
        State currentState = state.get();
        if (currentState != State.CREATED) {
            throw new IllegalStateException(
                "Cannot initialize component in state: " + currentState);
        }
        doInitialize();
        state.set(State.INITIALIZED);
    }

    @Override
    public void start() {
        State currentState = state.get();
        if (currentState != State.INITIALIZED && currentState != State.STOPPED) {
            throw new IllegalStateException(
                "Cannot start component in state: " + currentState);
        }
        doStart();
        state.set(State.STARTED);
    }

    @Override
    public void stop() {
        State currentState = state.get();
        if (currentState != State.STARTED) {
            throw new IllegalStateException(
                "Cannot stop component in state: " + currentState);
        }
        doStop();
        state.set(State.STOPPED);
    }

    @Override
    public void dispose() {
        State currentState = state.get();
        if (currentState == State.DISPOSED) {
            return;
        }
        if (currentState == State.STARTED) {
            doStop();
        }
        doDispose();
        state.set(State.DISPOSED);
    }

    @Override
    public boolean isInitialized() {
        State currentState = state.get();
        return currentState != State.CREATED && currentState != State.DISPOSED;
    }

    @Override
    public boolean isRunning() {
        return state.get() == State.STARTED;
    }

    @Override
    public boolean isDisposed() {
        return state.get() == State.DISPOSED;
    }

    /**
     * 执行初始化逻辑。
     * 
     * <p>子类应重写此方法实现具体的初始化逻辑。</p>
     */
    protected abstract void doInitialize();

    /**
     * 执行启动逻辑。
     * 
     * <p>子类应重写此方法实现具体的启动逻辑。</p>
     */
    protected abstract void doStart();

    /**
     * 执行停止逻辑。
     * 
     * <p>子类应重写此方法实现具体的停止逻辑。</p>
     */
    protected abstract void doStop();

    /**
     * 执行销毁逻辑。
     * 
     * <p>子类应重写此方法实现具体的销毁逻辑。</p>
     */
    protected abstract void doDispose();

    @Override
    public String toString() {
        return "AbstractLifecycleComponent{" +
               "name='" + name + '\'' +
               ", state=" + state.get() +
               '}';
    }
}
