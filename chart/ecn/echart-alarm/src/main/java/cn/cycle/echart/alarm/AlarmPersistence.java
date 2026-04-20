package cn.cycle.echart.alarm;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.time.Instant;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 预警持久化。
 * 
 * <p>将预警数据持久化到文件系统。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmPersistence {

    private final Path persistencePath;
    private final List<PersistenceListener> listeners;

    public AlarmPersistence() {
        this(Paths.get(System.getProperty("user.home"), ".echart", "alarms"));
    }

    public AlarmPersistence(Path persistencePath) {
        this.persistencePath = persistencePath;
        this.listeners = new ArrayList<>();
        
        File dir = persistencePath.toFile();
        if (!dir.exists()) {
            dir.mkdirs();
        }
    }

    public void saveAlarm(Alarm alarm) throws IOException {
        Path filePath = getAlarmFilePath(alarm.getId());
        AlarmData data = new AlarmData(alarm);
        
        try (ObjectOutputStream oos = new ObjectOutputStream(
                new FileOutputStream(filePath.toFile()))) {
            oos.writeObject(data);
        }
        
        notifySaved(alarm);
    }

    public void saveAlarms(List<Alarm> alarms) throws IOException {
        for (Alarm alarm : alarms) {
            saveAlarm(alarm);
        }
    }

    public Alarm loadAlarm(String alarmId) throws IOException, ClassNotFoundException {
        Path filePath = getAlarmFilePath(alarmId);
        
        if (!filePath.toFile().exists()) {
            return null;
        }
        
        try (ObjectInputStream ois = new ObjectInputStream(
                new FileInputStream(filePath.toFile()))) {
            AlarmData data = (AlarmData) ois.readObject();
            return data.toAlarm();
        }
    }

    public List<Alarm> loadAllAlarms() throws IOException, ClassNotFoundException {
        List<Alarm> alarms = new ArrayList<>();
        File dir = persistencePath.toFile();
        
        File[] files = dir.listFiles((d, name) -> name.endsWith(".alarm"));
        if (files != null) {
            for (File file : files) {
                try (ObjectInputStream ois = new ObjectInputStream(
                        new FileInputStream(file))) {
                    AlarmData data = (AlarmData) ois.readObject();
                    alarms.add(data.toAlarm());
                }
            }
        }
        
        return alarms;
    }

    public void deleteAlarm(String alarmId) {
        Path filePath = getAlarmFilePath(alarmId);
        File file = filePath.toFile();
        
        if (file.exists()) {
            file.delete();
            notifyDeleted(alarmId);
        }
    }

    public void clearAllAlarms() {
        File dir = persistencePath.toFile();
        File[] files = dir.listFiles((d, name) -> name.endsWith(".alarm"));
        
        if (files != null) {
            for (File file : files) {
                file.delete();
            }
        }
    }

    public void saveHistory(AlarmHistory history) throws IOException {
        Path historyPath = persistencePath.resolve("history.dat");
        
        List<AlarmHistory.AlarmRecord> records = history.getAllRecords();
        
        try (ObjectOutputStream oos = new ObjectOutputStream(
                new FileOutputStream(historyPath.toFile()))) {
            oos.writeObject(records);
        }
    }

    @SuppressWarnings("unchecked")
    public List<AlarmHistory.AlarmRecord> loadHistory() throws IOException, ClassNotFoundException {
        Path historyPath = persistencePath.resolve("history.dat");
        
        if (!historyPath.toFile().exists()) {
            return new ArrayList<>();
        }
        
        try (ObjectInputStream ois = new ObjectInputStream(
                new FileInputStream(historyPath.toFile()))) {
            return (List<AlarmHistory.AlarmRecord>) ois.readObject();
        }
    }

    private Path getAlarmFilePath(String alarmId) {
        return persistencePath.resolve(alarmId + ".alarm");
    }

    private void notifySaved(Alarm alarm) {
        for (PersistenceListener listener : listeners) {
            listener.onAlarmSaved(alarm);
        }
    }

    private void notifyDeleted(String alarmId) {
        for (PersistenceListener listener : listeners) {
            listener.onAlarmDeleted(alarmId);
        }
    }

    public void addListener(PersistenceListener listener) {
        listeners.add(listener);
    }

    public void removeListener(PersistenceListener listener) {
        listeners.remove(listener);
    }

    public interface PersistenceListener {
        void onAlarmSaved(Alarm alarm);
        void onAlarmDeleted(String alarmId);
    }

    public static class AlarmData implements Serializable {
        private static final long serialVersionUID = 1L;

        private final String id;
        private final String typeName;
        private final String levelName;
        private final String statusName;
        private final String title;
        private final String message;
        private final long triggeredTimeEpoch;
        private final long acknowledgedTimeEpoch;
        private final long clearedTimeEpoch;
        private final String acknowledgedBy;
        private final String clearedBy;
        private final Map<String, Object> data;

        public AlarmData(Alarm alarm) {
            this.id = alarm.getId();
            this.typeName = alarm.getType().name();
            this.levelName = alarm.getLevel().name();
            this.statusName = alarm.getStatus().name();
            this.title = alarm.getTitle();
            this.message = alarm.getMessage();
            this.triggeredTimeEpoch = alarm.getTriggeredTime().toEpochMilli();
            this.acknowledgedTimeEpoch = alarm.getAcknowledgedTime() != null 
                    ? alarm.getAcknowledgedTime().toEpochMilli() : 0;
            this.clearedTimeEpoch = alarm.getClearedTime() != null 
                    ? alarm.getClearedTime().toEpochMilli() : 0;
            this.acknowledgedBy = alarm.getAcknowledgedBy();
            this.clearedBy = alarm.getClearedBy();
            this.data = new HashMap<>(alarm.getData());
        }

        public Alarm toAlarm() {
            Alarm alarm = Alarm.builder()
                    .type(AlarmType.valueOf(typeName))
                    .level(AlarmLevel.valueOf(levelName))
                    .title(title)
                    .message(message)
                    .build();

            for (Map.Entry<String, Object> entry : data.entrySet()) {
                alarm.withData(entry.getKey(), entry.getValue());
            }

            if (acknowledgedTimeEpoch > 0) {
                alarm.acknowledge(acknowledgedBy);
            }
            if (clearedTimeEpoch > 0) {
                alarm.clear(clearedBy);
            }

            return alarm;
        }
    }
}
