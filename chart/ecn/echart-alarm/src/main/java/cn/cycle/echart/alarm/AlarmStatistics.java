package cn.cycle.echart.alarm;

import java.time.LocalDate;
import java.util.EnumMap;
import java.util.Map;

/**
 * 预警统计。
 * 
 * <p>统计预警数据的各项指标。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmStatistics {

    private final Map<AlarmType, TypeStatistics> typeStats;
    private final Map<AlarmLevel, LevelStatistics> levelStats;
    private final Map<LocalDate, DailyStatistics> dailyStats;
    
    private long totalAlarms;
    private long acknowledgedAlarms;
    private long clearedAlarms;
    private long escalatedAlarms;

    public AlarmStatistics() {
        this.typeStats = new EnumMap<>(AlarmType.class);
        this.levelStats = new EnumMap<>(AlarmLevel.class);
        this.dailyStats = new java.util.TreeMap<>();
        
        for (AlarmType type : AlarmType.values()) {
            typeStats.put(type, new TypeStatistics());
        }
        for (AlarmLevel level : AlarmLevel.values()) {
            levelStats.put(level, new LevelStatistics());
        }
    }

    public void recordAlarmTriggered(Alarm alarm) {
        totalAlarms++;
        
        TypeStatistics typeStat = typeStats.get(alarm.getType());
        if (typeStat != null) {
            typeStat.incrementTriggered();
        }
        
        LevelStatistics levelStat = levelStats.get(alarm.getLevel());
        if (levelStat != null) {
            levelStat.incrementTriggered();
        }
        
        LocalDate today = LocalDate.now();
        DailyStatistics dailyStat = dailyStats.computeIfAbsent(today, k -> new DailyStatistics());
        dailyStat.incrementTriggered();
    }

    public void recordAlarmAcknowledged(Alarm alarm) {
        acknowledgedAlarms++;
        
        TypeStatistics typeStat = typeStats.get(alarm.getType());
        if (typeStat != null) {
            typeStat.incrementAcknowledged();
        }
        
        LevelStatistics levelStat = levelStats.get(alarm.getLevel());
        if (levelStat != null) {
            levelStat.incrementAcknowledged();
        }
    }

    public void recordAlarmCleared(Alarm alarm) {
        clearedAlarms++;
        
        TypeStatistics typeStat = typeStats.get(alarm.getType());
        if (typeStat != null) {
            typeStat.incrementCleared();
        }
        
        LevelStatistics levelStat = levelStats.get(alarm.getLevel());
        if (levelStat != null) {
            levelStat.incrementCleared();
        }
    }

    public void recordAlarmEscalated(Alarm alarm) {
        escalatedAlarms++;
        
        TypeStatistics typeStat = typeStats.get(alarm.getType());
        if (typeStat != null) {
            typeStat.incrementEscalated();
        }
        
        LevelStatistics levelStat = levelStats.get(alarm.getLevel());
        if (levelStat != null) {
            levelStat.incrementEscalated();
        }
    }

    public long getTotalAlarms() {
        return totalAlarms;
    }

    public long getAcknowledgedAlarms() {
        return acknowledgedAlarms;
    }

    public long getClearedAlarms() {
        return clearedAlarms;
    }

    public long getEscalatedAlarms() {
        return escalatedAlarms;
    }

    public long getActiveAlarms() {
        return totalAlarms - clearedAlarms;
    }

    public double getAcknowledgmentRate() {
        if (totalAlarms == 0) return 0;
        return (double) acknowledgedAlarms / totalAlarms * 100;
    }

    public TypeStatistics getTypeStatistics(AlarmType type) {
        return typeStats.get(type);
    }

    public LevelStatistics getLevelStatistics(AlarmLevel level) {
        return levelStats.get(level);
    }

    public DailyStatistics getDailyStatistics(LocalDate date) {
        return dailyStats.get(date);
    }

    public Map<LocalDate, DailyStatistics> getDailyStats() {
        return dailyStats;
    }

    public static class TypeStatistics {
        private long triggered;
        private long acknowledged;
        private long cleared;
        private long escalated;

        public void incrementTriggered() {
            triggered++;
        }

        public void incrementAcknowledged() {
            acknowledged++;
        }

        public void incrementCleared() {
            cleared++;
        }

        public void incrementEscalated() {
            escalated++;
        }

        public long getTriggered() {
            return triggered;
        }

        public long getAcknowledged() {
            return acknowledged;
        }

        public long getCleared() {
            return cleared;
        }

        public long getEscalated() {
            return escalated;
        }

        public long getActive() {
            return triggered - cleared;
        }
    }

    public static class LevelStatistics {
        private long triggered;
        private long acknowledged;
        private long cleared;
        private long escalated;

        public void incrementTriggered() {
            triggered++;
        }

        public void incrementAcknowledged() {
            acknowledged++;
        }

        public void incrementCleared() {
            cleared++;
        }

        public void incrementEscalated() {
            escalated++;
        }

        public long getTriggered() {
            return triggered;
        }

        public long getAcknowledged() {
            return acknowledged;
        }

        public long getCleared() {
            return cleared;
        }

        public long getEscalated() {
            return escalated;
        }

        public long getActive() {
            return triggered - cleared;
        }
    }

    public static class DailyStatistics {
        private long triggered;
        private long acknowledged;
        private long cleared;

        public void incrementTriggered() {
            triggered++;
        }

        public void incrementAcknowledged() {
            acknowledged++;
        }

        public void incrementCleared() {
            cleared++;
        }

        public long getTriggered() {
            return triggered;
        }

        public long getAcknowledged() {
            return acknowledged;
        }

        public long getCleared() {
            return cleared;
        }
    }
}
