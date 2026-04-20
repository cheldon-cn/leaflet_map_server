package cn.cycle.echart.alarm;

import java.time.Instant;
import java.time.LocalDate;
import java.time.LocalDateTime;
import java.time.ZoneId;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;

/**
 * 预警历史记录。
 * 
 * <p>记录和管理预警的历史数据。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmHistory {

    private final Map<String, AlarmRecord> history;
    private final int maxRecords;
    private final List<AlarmHistoryListener> listeners;

    public AlarmHistory() {
        this(10000);
    }

    public AlarmHistory(int maxRecords) {
        this.maxRecords = maxRecords;
        this.history = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
    }

    public void recordAlarm(Alarm alarm) {
        AlarmRecord record = new AlarmRecord(alarm);
        history.put(alarm.getId(), record);
        trimHistory();
        notifyListeners(record);
    }

    public void updateAlarmRecord(String alarmId, AlarmStatus newStatus) {
        AlarmRecord record = history.get(alarmId);
        if (record != null) {
            record.setStatus(newStatus);
            record.setUpdateTime(Instant.now());
        }
    }

    public AlarmRecord getRecord(String alarmId) {
        return history.get(alarmId);
    }

    public List<AlarmRecord> getAllRecords() {
        return new ArrayList<>(history.values());
    }

    public List<AlarmRecord> getRecordsByType(AlarmType type) {
        return history.values().stream()
                .filter(r -> r.getAlarmType() == type)
                .collect(Collectors.toList());
    }

    public List<AlarmRecord> getRecordsByLevel(AlarmLevel level) {
        return history.values().stream()
                .filter(r -> r.getAlarmLevel() == level)
                .collect(Collectors.toList());
    }

    public List<AlarmRecord> getRecordsByDate(LocalDate date) {
        return history.values().stream()
                .filter(r -> {
                    LocalDateTime triggerTime = LocalDateTime.ofInstant(
                            r.getTriggeredTime(), ZoneId.systemDefault());
                    return triggerTime.toLocalDate().equals(date);
                })
                .collect(Collectors.toList());
    }

    public List<AlarmRecord> getRecordsByDateRange(LocalDate start, LocalDate end) {
        return history.values().stream()
                .filter(r -> {
                    LocalDateTime triggerTime = LocalDateTime.ofInstant(
                            r.getTriggeredTime(), ZoneId.systemDefault());
                    LocalDate date = triggerTime.toLocalDate();
                    return !date.isBefore(start) && !date.isAfter(end);
                })
                .collect(Collectors.toList());
    }

    public List<AlarmRecord> getRecordsByStatus(AlarmStatus status) {
        return history.values().stream()
                .filter(r -> r.getStatus() == status)
                .collect(Collectors.toList());
    }

    public void clearHistory() {
        history.clear();
    }

    public void clearHistoryBefore(Instant time) {
        history.entrySet().removeIf(entry -> 
                entry.getValue().getTriggeredTime().isBefore(time));
    }

    public int getRecordCount() {
        return history.size();
    }

    private void trimHistory() {
        if (history.size() > maxRecords) {
            List<AlarmRecord> records = new ArrayList<>(history.values());
            records.sort((a, b) -> b.getTriggeredTime().compareTo(a.getTriggeredTime()));
            
            history.clear();
            for (int i = 0; i < maxRecords && i < records.size(); i++) {
                AlarmRecord record = records.get(i);
                history.put(record.getAlarmId(), record);
            }
        }
    }

    public void addListener(AlarmHistoryListener listener) {
        listeners.add(listener);
    }

    public void removeListener(AlarmHistoryListener listener) {
        listeners.remove(listener);
    }

    private void notifyListeners(AlarmRecord record) {
        for (AlarmHistoryListener listener : listeners) {
            listener.onAlarmRecorded(record);
        }
    }

    public interface AlarmHistoryListener {
        void onAlarmRecorded(AlarmRecord record);
    }

    public static class AlarmRecord {
        private final String alarmId;
        private final AlarmType alarmType;
        private final AlarmLevel alarmLevel;
        private final String title;
        private final String message;
        private final Instant triggeredTime;
        private final Map<String, Object> alarmData;
        
        private AlarmStatus status;
        private Instant acknowledgedTime;
        private Instant clearedTime;
        private Instant updateTime;
        private String acknowledgedBy;
        private String clearedBy;

        public AlarmRecord(Alarm alarm) {
            this.alarmId = alarm.getId();
            this.alarmType = alarm.getType();
            this.alarmLevel = alarm.getLevel();
            this.title = alarm.getTitle();
            this.message = alarm.getMessage();
            this.triggeredTime = alarm.getTriggeredTime();
            this.status = alarm.getStatus();
            this.alarmData = new ConcurrentHashMap<>(alarm.getData());
            this.updateTime = Instant.now();
        }

        public String getAlarmId() {
            return alarmId;
        }

        public AlarmType getAlarmType() {
            return alarmType;
        }

        public AlarmLevel getAlarmLevel() {
            return alarmLevel;
        }

        public String getTitle() {
            return title;
        }

        public String getMessage() {
            return message;
        }

        public Instant getTriggeredTime() {
            return triggeredTime;
        }

        public AlarmStatus getStatus() {
            return status;
        }

        public void setStatus(AlarmStatus status) {
            this.status = status;
        }

        public Instant getAcknowledgedTime() {
            return acknowledgedTime;
        }

        public void setAcknowledgedTime(Instant acknowledgedTime) {
            this.acknowledgedTime = acknowledgedTime;
        }

        public Instant getClearedTime() {
            return clearedTime;
        }

        public void setClearedTime(Instant clearedTime) {
            this.clearedTime = clearedTime;
        }

        public Instant getUpdateTime() {
            return updateTime;
        }

        public void setUpdateTime(Instant updateTime) {
            this.updateTime = updateTime;
        }

        public String getAcknowledgedBy() {
            return acknowledgedBy;
        }

        public void setAcknowledgedBy(String acknowledgedBy) {
            this.acknowledgedBy = acknowledgedBy;
        }

        public String getClearedBy() {
            return clearedBy;
        }

        public void setClearedBy(String clearedBy) {
            this.clearedBy = clearedBy;
        }

        public Map<String, Object> getAlarmData() {
            return alarmData;
        }
    }
}
