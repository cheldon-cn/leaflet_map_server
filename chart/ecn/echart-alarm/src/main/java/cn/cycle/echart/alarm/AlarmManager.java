package cn.cycle.echart.alarm;

import cn.cycle.chart.api.alert.Alert;
import cn.cycle.chart.api.alert.Alert.Type;
import cn.cycle.chart.api.alert.Alert.Severity;
import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;

/**
 * 预警管理器。
 * 
 * <p>管理预警的创建、触发、确认和清除。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmManager {

    private final EventBus eventBus;
    private final Map<String, Alert> alerts;
    private final List<AlarmListener> listeners;
    private final AtomicLong alertCounter;

    public AlarmManager(EventBus eventBus) {
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.alerts = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        this.alertCounter = new AtomicLong(0);
    }

    /**
     * 触发预警。
     * 
     * @param type 预警类型
     * @param severity 严重程度
     * @param title 标题
     * @param message 消息
     * @return 预警对象
     */
    public Alert triggerAlert(Type type, Severity severity, String title, String message) {
        Alert alert = new Alert(type, severity, title, message);
        alerts.put(alert.getId(), alert);
        notifyAlarmTriggered(alert);
        return alert;
    }

    /**
     * 触发安全预警。
     * 
     * @param severity 严重程度
     * @param title 标题
     * @param message 消息
     * @return 预警对象
     */
    public Alert triggerSafetyAlert(Severity severity, String title, String message) {
        return triggerAlert(Type.SAFETY, severity, title, message);
    }

    /**
     * 触发导航预警。
     * 
     * @param severity 严重程度
     * @param title 标题
     * @param message 消息
     * @return 预警对象
     */
    public Alert triggerNavigationAlert(Severity severity, String title, String message) {
        return triggerAlert(Type.NAVIGATION, severity, title, message);
    }

    /**
     * 触发系统预警。
     * 
     * @param severity 严重程度
     * @param title 标题
     * @param message 消息
     * @return 预警对象
     */
    public Alert triggerSystemAlert(Severity severity, String title, String message) {
        return triggerAlert(Type.SYSTEM, severity, title, message);
    }

    /**
     * 触发天气预警。
     * 
     * @param severity 严重程度
     * @param title 标题
     * @param message 消息
     * @return 预警对象
     */
    public Alert triggerWeatherAlert(Severity severity, String title, String message) {
        return triggerAlert(Type.WEATHER, severity, title, message);
    }

    /**
     * 触发AIS预警。
     * 
     * @param severity 严重程度
     * @param title 标题
     * @param message 消息
     * @return 预警对象
     */
    public Alert triggerAisAlert(Severity severity, String title, String message) {
        return triggerAlert(Type.AIS, severity, title, message);
    }

    /**
     * 确认预警。
     * 
     * @param alertId 预警ID
     * @return 是否成功
     */
    public boolean acknowledgeAlert(String alertId) {
        Alert alert = alerts.get(alertId);
        if (alert != null && !alert.isAcknowledged()) {
            alert.setAcknowledged(true);
            notifyAlarmAcknowledged(alert);
            return true;
        }
        return false;
    }

    /**
     * 清除预警。
     * 
     * @param alertId 预警ID
     * @return 是否成功
     */
    public boolean clearAlert(String alertId) {
        Alert alert = alerts.remove(alertId);
        if (alert != null) {
            notifyAlarmCleared(alert);
            return true;
        }
        return false;
    }

    /**
     * 获取预警。
     * 
     * @param alertId 预警ID
     * @return 预警对象
     */
    public Alert getAlert(String alertId) {
        return alerts.get(alertId);
    }

    /**
     * 获取所有活动预警。
     * 
     * @return 预警列表
     */
    public List<Alert> getActiveAlerts() {
        return new ArrayList<>(alerts.values());
    }

    /**
     * 获取未确认预警。
     * 
     * @return 预警列表
     */
    public List<Alert> getUnacknowledgedAlerts() {
        List<Alert> result = new ArrayList<>();
        for (Alert alert : alerts.values()) {
            if (!alert.isAcknowledged()) {
                result.add(alert);
            }
        }
        return result;
    }

    /**
     * 获取预警数量。
     * 
     * @return 预警数量
     */
    public int getAlertCount() {
        return alerts.size();
    }

    /**
     * 清除所有预警。
     */
    public void clearAllAlerts() {
        List<Alert> toRemove = new ArrayList<>(alerts.values());
        alerts.clear();
        for (Alert alert : toRemove) {
            notifyAlarmCleared(alert);
        }
    }

    /**
     * 添加预警监听器。
     * 
     * @param listener 监听器
     */
    public void addAlarmListener(AlarmListener listener) {
        listeners.add(listener);
    }

    /**
     * 移除预警监听器。
     * 
     * @param listener 监听器
     */
    public void removeAlarmListener(AlarmListener listener) {
        listeners.remove(listener);
    }

    private void notifyAlarmTriggered(Alert alert) {
        eventBus.publish(new AppEvent(this, AppEventType.ALARM_TRIGGERED, alert));
        
        for (AlarmListener listener : listeners) {
            listener.onAlarmTriggered(alert);
        }
    }

    private void notifyAlarmAcknowledged(Alert alert) {
        eventBus.publish(new AppEvent(this, AppEventType.ALARM_ACKNOWLEDGED, alert));
        
        for (AlarmListener listener : listeners) {
            listener.onAlarmAcknowledged(alert);
        }
    }

    private void notifyAlarmCleared(Alert alert) {
        eventBus.publish(new AppEvent(this, AppEventType.ALARM_CLEARED, alert));
        
        for (AlarmListener listener : listeners) {
            listener.onAlarmCleared(alert);
        }
    }

    /**
     * 预警监听器接口。
     */
    public interface AlarmListener {
        void onAlarmTriggered(Alert alert);
        void onAlarmAcknowledged(Alert alert);
        void onAlarmCleared(Alert alert);
    }
}
