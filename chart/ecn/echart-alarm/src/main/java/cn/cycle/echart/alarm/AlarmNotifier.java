package cn.cycle.echart.alarm;

import cn.cycle.echart.core.PlatformAdapter;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * 预警通知器。
 * 
 * <p>负责预警的通知发送，使用PlatformAdapter实现平台无关的通知机制。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmNotifier {

    private final PlatformAdapter platformAdapter;
    private final List<AlarmNotificationListener> listeners;
    private boolean notificationEnabled = true;

    public AlarmNotifier(PlatformAdapter platformAdapter) {
        this.platformAdapter = Objects.requireNonNull(platformAdapter, "platformAdapter cannot be null");
        this.listeners = new ArrayList<>();
    }

    public void notifyAlarm(Alarm alarm) {
        if (!notificationEnabled) {
            return;
        }

        if (platformAdapter != null) {
            platformAdapter.runOnUiThread(() -> {
                for (AlarmNotificationListener listener : listeners) {
                    listener.onAlarmNotification(alarm);
                }
            });
        }

        if (alarm.getLevel() == AlarmLevel.CRITICAL) {
            AlarmSoundManager.getInstance().playAlarmSound(alarm.getLevel());
        }
    }

    public void notifyAlarmAcknowledged(Alarm alarm) {
        if (!notificationEnabled) {
            return;
        }

        if (platformAdapter != null) {
            platformAdapter.runOnUiThread(() -> {
                for (AlarmNotificationListener listener : listeners) {
                    listener.onAlarmAcknowledgedNotification(alarm);
                }
            });
        }
    }

    public void notifyAlarmCleared(Alarm alarm) {
        if (!notificationEnabled) {
            return;
        }

        if (alarm.getLevel() == AlarmLevel.CRITICAL) {
            AlarmSoundManager.getInstance().stopCriticalSound();
        }

        if (platformAdapter != null) {
            platformAdapter.runOnUiThread(() -> {
                for (AlarmNotificationListener listener : listeners) {
                    listener.onAlarmClearedNotification(alarm);
                }
            });
        }
    }

    public void addNotificationListener(AlarmNotificationListener listener) {
        listeners.add(listener);
    }

    public void removeNotificationListener(AlarmNotificationListener listener) {
        listeners.remove(listener);
    }

    public void setNotificationEnabled(boolean enabled) {
        this.notificationEnabled = enabled;
    }

    public boolean isNotificationEnabled() {
        return notificationEnabled;
    }

    public interface AlarmNotificationListener {
        void onAlarmNotification(Alarm alarm);
        void onAlarmAcknowledgedNotification(Alarm alarm);
        void onAlarmClearedNotification(Alarm alarm);
    }
}
