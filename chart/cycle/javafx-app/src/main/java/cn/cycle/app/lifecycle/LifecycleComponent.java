package cn.cycle.app.lifecycle;

public interface LifecycleComponent {
    
    void initialize();
    
    void activate();
    
    void deactivate();
    
    void dispose();
    
    boolean isInitialized();
    
    boolean isActive();
}
