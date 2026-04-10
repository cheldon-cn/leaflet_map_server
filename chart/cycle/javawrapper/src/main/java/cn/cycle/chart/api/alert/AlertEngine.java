package cn.cycle.chart.api.alert;

import cn.cycle.chart.api.geometry.Coordinate;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.CopyOnWriteArrayList;

public class AlertEngine {

    private final List<Alert> alerts;
    private final List<AlertListener> listeners;
    private boolean enabled;

    public AlertEngine() {
        this.alerts = new CopyOnWriteArrayList<>();
        this.listeners = new ArrayList<>();
        this.enabled = true;
    }

    public void addAlert(Alert alert) {
        if (alert != null) {
            alerts.add(alert);
            fireAlertAdded(alert);
        }
    }

    public void removeAlert(Alert alert) {
        if (alert != null && alerts.remove(alert)) {
            fireAlertRemoved(alert);
        }
    }

    public void clearAlerts() {
        for (Alert alert : alerts) {
            fireAlertRemoved(alert);
        }
        alerts.clear();
    }

    public List<Alert> getAlerts() {
        return new ArrayList<>(alerts);
    }

    public List<Alert> getAlertsByType(Alert.Type type) {
        List<Alert> result = new ArrayList<>();
        for (Alert alert : alerts) {
            if (alert.getType() == type) {
                result.add(alert);
            }
        }
        return result;
    }

    public List<Alert> getActiveAlerts() {
        List<Alert> result = new ArrayList<>();
        for (Alert alert : alerts) {
            if (!alert.isAcknowledged()) {
                result.add(alert);
            }
        }
        return result;
    }

    public void acknowledgeAlert(Alert alert) {
        if (alert != null) {
            alert.setAcknowledged(true);
            fireAlertUpdated(alert);
        }
    }

    public void acknowledgeAll() {
        for (Alert alert : alerts) {
            alert.setAcknowledged(true);
            fireAlertUpdated(alert);
        }
    }

    public int getAlertCount() {
        return alerts.size();
    }

    public int getActiveAlertCount() {
        int count = 0;
        for (Alert alert : alerts) {
            if (!alert.isAcknowledged()) {
                count++;
            }
        }
        return count;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public void addListener(AlertListener listener) {
        if (listener != null && !listeners.contains(listener)) {
            listeners.add(listener);
        }
    }

    public void removeListener(AlertListener listener) {
        listeners.remove(listener);
    }

    private void fireAlertAdded(Alert alert) {
        for (AlertListener listener : listeners) {
            listener.onAlertAdded(alert);
        }
    }

    private void fireAlertRemoved(Alert alert) {
        for (AlertListener listener : listeners) {
            listener.onAlertRemoved(alert);
        }
    }

    private void fireAlertUpdated(Alert alert) {
        for (AlertListener listener : listeners) {
            listener.onAlertUpdated(alert);
        }
    }

    public interface AlertListener {
        void onAlertAdded(Alert alert);
        void onAlertRemoved(Alert alert);
        void onAlertUpdated(Alert alert);
    }
}
