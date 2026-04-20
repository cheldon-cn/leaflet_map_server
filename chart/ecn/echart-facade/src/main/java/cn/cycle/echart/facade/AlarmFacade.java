package cn.cycle.echart.facade;

import cn.cycle.chart.api.alert.Alert;
import cn.cycle.chart.api.alert.Alert.Type;
import cn.cycle.chart.api.alert.Alert.Severity;
import cn.cycle.echart.alarm.AlarmManager;
import cn.cycle.echart.core.EventBus;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 预警业务门面。
 * 
 * <p>提供预警系统的统一访问接口。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmFacade {

    private final AlarmManager alarmManager;
    private final EventBus eventBus;

    public AlarmFacade(AlarmManager alarmManager, EventBus eventBus) {
        this.alarmManager = Objects.requireNonNull(alarmManager, "alarmManager cannot be null");
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
    }

    public Alert triggerAlert(Type type, Severity severity, String title, String message) {
        return alarmManager.triggerAlert(type, severity, title, message);
    }

    public Alert triggerSafetyAlert(Severity severity, String title, String message) {
        return alarmManager.triggerSafetyAlert(severity, title, message);
    }

    public Alert triggerNavigationAlert(Severity severity, String title, String message) {
        return alarmManager.triggerNavigationAlert(severity, title, message);
    }

    public Alert triggerSystemAlert(Severity severity, String title, String message) {
        return alarmManager.triggerSystemAlert(severity, title, message);
    }

    public Alert triggerWeatherAlert(Severity severity, String title, String message) {
        return alarmManager.triggerWeatherAlert(severity, title, message);
    }

    public Alert triggerAisAlert(Severity severity, String title, String message) {
        return alarmManager.triggerAisAlert(severity, title, message);
    }

    public void acknowledgeAlert(String alertId) {
        alarmManager.acknowledgeAlert(alertId);
    }

    public void clearAlert(String alertId) {
        alarmManager.clearAlert(alertId);
    }

    public Alert getAlert(String alertId) {
        return alarmManager.getAlert(alertId);
    }

    public List<Alert> getActiveAlerts() {
        return alarmManager.getActiveAlerts();
    }

    public List<Alert> getUnacknowledgedAlerts() {
        return alarmManager.getUnacknowledgedAlerts();
    }

    public List<Alert> getAlertsByType(Type type) {
        List<Alert> result = new ArrayList<>();
        for (Alert alert : alarmManager.getActiveAlerts()) {
            if (alert.getType() == type) {
                result.add(alert);
            }
        }
        return result;
    }

    public List<Alert> getAlertsBySeverity(Severity severity) {
        List<Alert> result = new ArrayList<>();
        for (Alert alert : alarmManager.getActiveAlerts()) {
            if (alert.getSeverity() == severity) {
                result.add(alert);
            }
        }
        return result;
    }

    public void clearAllAlerts() {
        alarmManager.clearAllAlerts();
    }

    public int getAlertCount() {
        return alarmManager.getAlertCount();
    }

    public boolean hasCriticalAlerts() {
        for (Alert alert : alarmManager.getActiveAlerts()) {
            if (alert.getSeverity() == Severity.CRITICAL) {
                return true;
            }
        }
        return false;
    }

    public void addAlarmListener(AlarmManager.AlarmListener listener) {
        alarmManager.addAlarmListener(listener);
    }

    public void removeAlarmListener(AlarmManager.AlarmListener listener) {
        alarmManager.removeAlarmListener(listener);
    }
}
