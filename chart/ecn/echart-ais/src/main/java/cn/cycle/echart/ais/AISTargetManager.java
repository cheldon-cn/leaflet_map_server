package cn.cycle.echart.ais;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * AIS目标管理器。
 * 
 * <p>管理AIS目标的添加、移除和查询。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AISTargetManager {

    private final EventBus eventBus;
    private final Map<Integer, AISTarget> targets;
    private final List<AISTargetListener> listeners;

    public AISTargetManager(EventBus eventBus) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.targets = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
    }

    /**
     * 获取AIS目标。
     * 
     * @param mmsi MMSI号
     * @return AIS目标
     */
    public AISTarget getTarget(int mmsi) {
        return targets.get(mmsi);
    }

    /**
     * 添加或更新AIS目标。
     * 
     * @param target AIS目标
     */
    public void addTarget(AISTarget target) {
        Objects.requireNonNull(target, "target cannot be null");
        boolean isNew = !targets.containsKey(target.getMmsi());
        targets.put(target.getMmsi(), target);
        
        if (isNew) {
            notifyTargetAdded(target);
        } else {
            notifyTargetUpdated(target);
        }
    }

    /**
     * 获取所有AIS目标的MMSI列表。
     * 
     * @return MMSI列表
     */
    public List<Integer> getAllTargetMmsi() {
        return new ArrayList<>(targets.keySet());
    }

    /**
     * 获取所有AIS目标。
     * 
     * @return AIS目标列表
     */
    public List<AISTarget> getAllTargets() {
        return new ArrayList<>(targets.values());
    }

    /**
     * 获取AIS目标数量。
     * 
     * @return 目标数量
     */
    public int getTargetCount() {
        return targets.size();
    }

    /**
     * 移除AIS目标。
     * 
     * @param mmsi MMSI号
     * @return 是否成功移除
     */
    public boolean removeTarget(int mmsi) {
        AISTarget target = targets.remove(mmsi);
        if (target != null) {
            notifyTargetLost(target);
            return true;
        }
        return false;
    }

    /**
     * 清除所有目标。
     */
    public void clear() {
        List<AISTarget> toRemove = new ArrayList<>(targets.values());
        targets.clear();
        for (AISTarget target : toRemove) {
            notifyTargetLost(target);
        }
    }

    /**
     * 添加AIS目标监听器。
     * 
     * @param listener 监听器
     */
    public void addAISTargetListener(AISTargetListener listener) {
        listeners.add(listener);
    }

    /**
     * 移除AIS目标监听器。
     * 
     * @param listener 监听器
     */
    public void removeAISTargetListener(AISTargetListener listener) {
        listeners.remove(listener);
    }

    private void notifyTargetAdded(AISTarget target) {
        for (AISTargetListener listener : listeners) {
            listener.onTargetAdded(target);
        }
    }

    private void notifyTargetUpdated(AISTarget target) {
        eventBus.publish(new AppEvent(this, AppEventType.AIS_TARGET_UPDATED, target));
        
        for (AISTargetListener listener : listeners) {
            listener.onTargetUpdated(target);
        }
    }

    private void notifyTargetLost(AISTarget target) {
        eventBus.publish(new AppEvent(this, AppEventType.AIS_TARGET_LOST, target));
        
        for (AISTargetListener listener : listeners) {
            listener.onTargetLost(target);
        }
    }

    /**
     * AIS目标监听器接口。
     */
    public interface AISTargetListener {
        void onTargetAdded(AISTarget target);
        void onTargetUpdated(AISTarget target);
        void onTargetLost(AISTarget target);
    }
}
