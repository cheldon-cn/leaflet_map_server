package cn.cycle.echart.alarm;

import java.time.Duration;
import java.time.Instant;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 预警抑制管理器。
 * 
 * <p>管理预警的抑制规则，避免重复预警或短时间内大量预警。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmSuppressionManager {

    private final Map<String, SuppressionRecord> suppressionRecords;
    private final List<SuppressionRule> rules;
    private final List<SuppressionListener> listeners;
    
    private boolean suppressionEnabled = true;
    private Duration defaultSuppressionDuration = Duration.ofMinutes(5);

    public AlarmSuppressionManager() {
        this.suppressionRecords = new ConcurrentHashMap<>();
        this.rules = new ArrayList<>();
        this.listeners = new ArrayList<>();
        
        addDefaultRules();
    }

    private void addDefaultRules() {
        rules.add(new SameAlarmSuppressionRule());
        rules.add(new RateLimitingRule(10, Duration.ofMinutes(1)));
    }

    public boolean shouldSuppress(Alarm alarm) {
        if (!suppressionEnabled) {
            return false;
        }

        for (SuppressionRule rule : rules) {
            if (rule.shouldSuppress(alarm, this)) {
                notifySuppression(alarm, rule);
                return true;
            }
        }

        return false;
    }

    public void recordAlarm(Alarm alarm) {
        String key = generateKey(alarm);
        suppressionRecords.put(key, new SuppressionRecord(alarm, Instant.now()));
    }

    public void addRule(SuppressionRule rule) {
        rules.add(rule);
    }

    public void removeRule(SuppressionRule rule) {
        rules.remove(rule);
    }

    public void clearRules() {
        rules.clear();
    }

    public void setSuppressionEnabled(boolean enabled) {
        this.suppressionEnabled = enabled;
    }

    public boolean isSuppressionEnabled() {
        return suppressionEnabled;
    }

    public void setDefaultSuppressionDuration(Duration duration) {
        this.defaultSuppressionDuration = duration;
    }

    public Duration getDefaultSuppressionDuration() {
        return defaultSuppressionDuration;
    }

    public SuppressionRecord getSuppressionRecord(Alarm alarm) {
        return suppressionRecords.get(generateKey(alarm));
    }

    public void clearExpiredRecords() {
        Instant now = Instant.now();
        suppressionRecords.entrySet().removeIf(entry -> 
                entry.getValue().isExpired(now, defaultSuppressionDuration));
    }

    public int getSuppressionCount() {
        return suppressionRecords.size();
    }

    private String generateKey(Alarm alarm) {
        return alarm.getType().name() + ":" + alarm.getTitle();
    }

    private void notifySuppression(Alarm alarm, SuppressionRule rule) {
        for (SuppressionListener listener : listeners) {
            listener.onAlarmSuppressed(alarm, rule);
        }
    }

    public void addListener(SuppressionListener listener) {
        listeners.add(listener);
    }

    public void removeListener(SuppressionListener listener) {
        listeners.remove(listener);
    }

    public interface SuppressionRule {
        boolean shouldSuppress(Alarm alarm, AlarmSuppressionManager manager);
        String getName();
    }

    public interface SuppressionListener {
        void onAlarmSuppressed(Alarm alarm, SuppressionRule rule);
    }

    public static class SuppressionRecord {
        private final Alarm alarm;
        private final Instant suppressedAt;
        private int suppressionCount;

        public SuppressionRecord(Alarm alarm, Instant suppressedAt) {
            this.alarm = alarm;
            this.suppressedAt = suppressedAt;
            this.suppressionCount = 1;
        }

        public Alarm getAlarm() {
            return alarm;
        }

        public Instant getSuppressedAt() {
            return suppressedAt;
        }

        public int getSuppressionCount() {
            return suppressionCount;
        }

        public void incrementCount() {
            suppressionCount++;
        }

        public boolean isExpired(Instant now, Duration maxDuration) {
            return now.isAfter(suppressedAt.plus(maxDuration));
        }
    }

    private static class SameAlarmSuppressionRule implements SuppressionRule {
        @Override
        public boolean shouldSuppress(Alarm alarm, AlarmSuppressionManager manager) {
            SuppressionRecord record = manager.getSuppressionRecord(alarm);
            if (record != null) {
                Duration timeSinceLast = Duration.between(record.getSuppressedAt(), Instant.now());
                if (timeSinceLast.compareTo(manager.getDefaultSuppressionDuration()) < 0) {
                    record.incrementCount();
                    return true;
                }
            }
            return false;
        }

        @Override
        public String getName() {
            return "SameAlarmSuppression";
        }
    }

    private static class RateLimitingRule implements SuppressionRule {
        private final int maxAlarms;
        private final Duration window;
        private final Map<String, List<Instant>> alarmTimestamps = new ConcurrentHashMap<>();

        public RateLimitingRule(int maxAlarms, Duration window) {
            this.maxAlarms = maxAlarms;
            this.window = window;
        }

        @Override
        public boolean shouldSuppress(Alarm alarm, AlarmSuppressionManager manager) {
            String typeKey = alarm.getType().name();
            List<Instant> timestamps = alarmTimestamps.computeIfAbsent(typeKey, k -> new ArrayList<>());
            
            Instant now = Instant.now();
            Instant windowStart = now.minus(window);
            
            timestamps.removeIf(t -> t.isBefore(windowStart));
            
            if (timestamps.size() >= maxAlarms) {
                return true;
            }
            
            timestamps.add(now);
            return false;
        }

        @Override
        public String getName() {
            return "RateLimiting";
        }
    }
}
