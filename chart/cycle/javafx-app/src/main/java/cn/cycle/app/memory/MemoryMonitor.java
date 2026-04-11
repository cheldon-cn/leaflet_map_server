package cn.cycle.app.memory;

import cn.cycle.app.event.AppEvent;
import cn.cycle.app.event.AppEventBus;
import cn.cycle.app.event.AppEventType;
import cn.cycle.app.lifecycle.AbstractLifecycleComponent;

import java.lang.management.ManagementFactory;
import java.lang.management.MemoryMXBean;
import java.lang.management.MemoryUsage;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

public class MemoryMonitor extends AbstractLifecycleComponent {
    
    private static final MemoryMonitor INSTANCE = new MemoryMonitor();
    private static final long DEFAULT_CHECK_INTERVAL_MS = 10000;
    private static final double WARNING_THRESHOLD = 0.75;
    private static final double CRITICAL_THRESHOLD = 0.90;
    
    private final MemoryMXBean memoryBean;
    private final ScheduledExecutorService scheduler;
    private final long checkIntervalMs;
    
    private long lastUsedMemory = 0;
    private long lastMaxMemory = 0;
    private double lastUsageRatio = 0;
    
    private MemoryMonitor() {
        this(DEFAULT_CHECK_INTERVAL_MS);
    }
    
    private MemoryMonitor(long checkIntervalMs) {
        this.memoryBean = ManagementFactory.getMemoryMXBean();
        this.scheduler = Executors.newSingleThreadScheduledExecutor();
        this.checkIntervalMs = checkIntervalMs;
    }
    
    public static MemoryMonitor getInstance() {
        return INSTANCE;
    }
    
    public long getUsedMemory() {
        MemoryUsage heapUsage = memoryBean.getHeapMemoryUsage();
        return heapUsage.getUsed();
    }
    
    public long getMaxMemory() {
        MemoryUsage heapUsage = memoryBean.getHeapMemoryUsage();
        return heapUsage.getMax();
    }
    
    public long getCommittedMemory() {
        MemoryUsage heapUsage = memoryBean.getHeapMemoryUsage();
        return heapUsage.getCommitted();
    }
    
    public double getUsageRatio() {
        MemoryUsage heapUsage = memoryBean.getHeapMemoryUsage();
        long used = heapUsage.getUsed();
        long max = heapUsage.getMax();
        return max > 0 ? (double) used / max : 0;
    }
    
    public boolean isMemoryWarning() {
        return getUsageRatio() >= WARNING_THRESHOLD;
    }
    
    public boolean isMemoryCritical() {
        return getUsageRatio() >= CRITICAL_THRESHOLD;
    }
    
    public MemoryStatus getStatus() {
        double ratio = getUsageRatio();
        if (ratio >= CRITICAL_THRESHOLD) {
            return MemoryStatus.CRITICAL;
        } else if (ratio >= WARNING_THRESHOLD) {
            return MemoryStatus.WARNING;
        } else {
            return MemoryStatus.NORMAL;
        }
    }
    
    public String getStatusText() {
        switch (getStatus()) {
            case CRITICAL:
                return "内存严重不足";
            case WARNING:
                return "内存使用较高";
            default:
                return "内存正常";
        }
    }
    
    public void gc() {
        System.gc();
    }
    
    private void checkMemory() {
        lastUsedMemory = getUsedMemory();
        lastMaxMemory = getMaxMemory();
        lastUsageRatio = getUsageRatio();
        
        if (isMemoryCritical()) {
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.MEMORY_CRITICAL, this)
                    .withData("usedMemory", lastUsedMemory)
                    .withData("maxMemory", lastMaxMemory)
                    .withData("usageRatio", lastUsageRatio)
            );
        } else if (isMemoryWarning()) {
            AppEventBus.getInstance().publish(
                new AppEvent(AppEventType.MEMORY_WARNING, this)
                    .withData("usedMemory", lastUsedMemory)
                    .withData("maxMemory", lastMaxMemory)
                    .withData("usageRatio", lastUsageRatio)
            );
        }
    }
    
    @Override
    protected void doInitialize() {
        scheduler.scheduleAtFixedRate(
            this::checkMemory,
            checkIntervalMs,
            checkIntervalMs,
            TimeUnit.MILLISECONDS
        );
    }
    
    @Override
    protected void doDispose() {
        scheduler.shutdown();
    }
    
    public enum MemoryStatus {
        NORMAL,
        WARNING,
        CRITICAL
    }
}
