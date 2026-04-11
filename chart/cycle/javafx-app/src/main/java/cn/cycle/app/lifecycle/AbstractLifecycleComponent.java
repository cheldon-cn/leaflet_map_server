package cn.cycle.app.lifecycle;

public abstract class AbstractLifecycleComponent implements LifecycleComponent {
    
    private volatile boolean initialized = false;
    private volatile boolean active = false;
    private final Object lock = new Object();
    
    @Override
    public final void initialize() {
        synchronized (lock) {
            if (initialized) {
                return;
            }
            doInitialize();
            initialized = true;
        }
    }
    
    @Override
    public final void activate() {
        synchronized (lock) {
            if (!initialized) {
                initialize();
            }
            if (active) {
                return;
            }
            doActivate();
            active = true;
        }
    }
    
    @Override
    public final void deactivate() {
        synchronized (lock) {
            if (!active) {
                return;
            }
            doDeactivate();
            active = false;
        }
    }
    
    @Override
    public final void dispose() {
        synchronized (lock) {
            deactivate();
            if (!initialized) {
                return;
            }
            doDispose();
            initialized = false;
        }
    }
    
    @Override
    public boolean isInitialized() {
        return initialized;
    }
    
    @Override
    public boolean isActive() {
        return active;
    }
    
    protected abstract void doInitialize();
    
    protected void doActivate() {
    }
    
    protected void doDeactivate() {
    }
    
    protected void doDispose() {
    }
}
