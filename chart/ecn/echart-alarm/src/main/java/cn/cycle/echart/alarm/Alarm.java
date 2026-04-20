package cn.cycle.echart.alarm;

import java.time.Instant;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;

/**
 * 预警数据模型。
 * 
 * <p>独立的预警数据模型，不依赖外部API。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class Alarm {

    private final String id;
    private final AlarmType type;
    private final AlarmLevel level;
    private final String title;
    private final String message;
    private final Instant triggeredTime;
    private final Map<String, Object> data;
    
    private AlarmStatus status;
    private Instant acknowledgedTime;
    private String acknowledgedBy;
    private Instant clearedTime;
    private String clearedBy;
    private int acknowledgeCount;

    public Alarm(AlarmType type, AlarmLevel level, String title, String message) {
        this.id = UUID.randomUUID().toString();
        this.type = Objects.requireNonNull(type, "type cannot be null");
        this.level = Objects.requireNonNull(level, "level cannot be null");
        this.title = Objects.requireNonNull(title, "title cannot be null");
        this.message = Objects.requireNonNull(message, "message cannot be null");
        this.triggeredTime = Instant.now();
        this.status = AlarmStatus.ACTIVE;
        this.data = new HashMap<>();
        this.acknowledgeCount = 0;
    }

    public String getId() {
        return id;
    }

    public AlarmType getType() {
        return type;
    }

    public AlarmLevel getLevel() {
        return level;
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

    public Instant getAcknowledgedTime() {
        return acknowledgedTime;
    }

    public String getAcknowledgedBy() {
        return acknowledgedBy;
    }

    public Instant getClearedTime() {
        return clearedTime;
    }

    public String getClearedBy() {
        return clearedBy;
    }

    public int getAcknowledgeCount() {
        return acknowledgeCount;
    }

    public Map<String, Object> getData() {
        return data;
    }

    public Alarm withData(String key, Object value) {
        this.data.put(key, value);
        return this;
    }

    @SuppressWarnings("unchecked")
    public <T> T getData(String key) {
        return (T) data.get(key);
    }

    @SuppressWarnings("unchecked")
    public <T> T getData(String key, Class<T> type) {
        Object value = data.get(key);
        if (value != null && type.isInstance(value)) {
            return (T) value;
        }
        return null;
    }

    public void acknowledge(String acknowledgedBy) {
        if (status == AlarmStatus.ACTIVE || status == AlarmStatus.ESCALATED) {
            this.status = AlarmStatus.ACKNOWLEDGED;
            this.acknowledgedTime = Instant.now();
            this.acknowledgedBy = acknowledgedBy;
            this.acknowledgeCount++;
        }
    }

    public void clear(String clearedBy) {
        this.status = AlarmStatus.CLEARED;
        this.clearedTime = Instant.now();
        this.clearedBy = clearedBy;
    }

    public void escalate() {
        if (status == AlarmStatus.ACTIVE) {
            this.status = AlarmStatus.ESCALATED;
        }
    }

    public void suppress() {
        this.status = AlarmStatus.SUPPRESSED;
    }

    public boolean isActive() {
        return status.isActive();
    }

    public boolean isAcknowledged() {
        return status.isAcknowledged();
    }

    public boolean isCleared() {
        return status.isCleared();
    }

    public long getDurationMillis() {
        Instant endTime = clearedTime != null ? clearedTime : Instant.now();
        return endTime.toEpochMilli() - triggeredTime.toEpochMilli();
    }

    @Override
    public boolean equals(Object o) {
        if (this == o) return true;
        if (o == null || getClass() != o.getClass()) return false;
        Alarm alarm = (Alarm) o;
        return Objects.equals(id, alarm.id);
    }

    @Override
    public int hashCode() {
        return Objects.hash(id);
    }

    @Override
    public String toString() {
        return "Alarm{" +
                "id='" + id + '\'' +
                ", type=" + type +
                ", level=" + level +
                ", status=" + status +
                ", title='" + title + '\'' +
                '}';
    }

    public static Builder builder() {
        return new Builder();
    }

    public static class Builder {
        private AlarmType type;
        private AlarmLevel level;
        private String title;
        private String message;
        private Map<String, Object> data = new HashMap<>();

        public Builder type(AlarmType type) {
            this.type = type;
            return this;
        }

        public Builder level(AlarmLevel level) {
            this.level = level;
            return this;
        }

        public Builder title(String title) {
            this.title = title;
            return this;
        }

        public Builder message(String message) {
            this.message = message;
            return this;
        }

        public Builder data(String key, Object value) {
            this.data.put(key, value);
            return this;
        }

        public Alarm build() {
            Alarm alarm = new Alarm(type, level, title, message);
            for (Map.Entry<String, Object> entry : data.entrySet()) {
                alarm.withData(entry.getKey(), entry.getValue());
            }
            return alarm;
        }
    }
}
