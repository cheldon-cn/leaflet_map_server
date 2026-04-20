package cn.cycle.echart.alarm;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * 预警规则引擎。
 * 
 * <p>管理和执行预警规则，支持自定义规则扩展。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmRuleEngine {

    private final Map<String, AlarmRule> rules;
    private final List<RuleEvaluationListener> listeners;
    private boolean enabled = true;

    public AlarmRuleEngine() {
        this.rules = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        registerDefaultRules();
    }

    private void registerDefaultRules() {
        registerRule(new CollisionAlarmRule());
        registerRule(new DeviationAlarmRule());
        registerRule(new ShallowWaterAlarmRule());
        registerRule(new RestrictedAreaAlarmRule());
        registerRule(new WatchAlarmRule());
    }

    public void registerRule(AlarmRule rule) {
        rules.put(rule.getId(), rule);
    }

    public void unregisterRule(String ruleId) {
        rules.remove(ruleId);
    }

    public AlarmRule getRule(String ruleId) {
        return rules.get(ruleId);
    }

    public List<AlarmRule> getAllRules() {
        return new ArrayList<>(rules.values());
    }

    public List<AlarmRule> getEnabledRules() {
        List<AlarmRule> enabledRules = new ArrayList<>();
        for (AlarmRule rule : rules.values()) {
            if (rule.isEnabled()) {
                enabledRules.add(rule);
            }
        }
        return enabledRules;
    }

    public List<AlarmRule> getRulesByType(AlarmType type) {
        List<AlarmRule> result = new ArrayList<>();
        for (AlarmRule rule : rules.values()) {
            if (rule.getAlarmType() == type) {
                result.add(rule);
            }
        }
        return result;
    }

    public EvaluationResult evaluate(AlarmContext context) {
        if (!enabled) {
            return EvaluationResult.disabled();
        }

        List<Alarm> triggeredAlarms = new ArrayList<>();
        List<AlarmRule> triggeredRules = new ArrayList<>();

        for (AlarmRule rule : getEnabledRules()) {
            try {
                if (rule.evaluate(context)) {
                    Alarm alarm = rule.createAlarm(context);
                    if (alarm != null) {
                        triggeredAlarms.add(alarm);
                        triggeredRules.add(rule);
                        notifyRuleTriggered(rule, alarm);
                    }
                }
            } catch (Exception e) {
                notifyRuleError(rule, e);
            }
        }

        return new EvaluationResult(triggeredAlarms, triggeredRules);
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void enableRule(String ruleId) {
        AlarmRule rule = rules.get(ruleId);
        if (rule != null) {
            rule.setEnabled(true);
        }
    }

    public void disableRule(String ruleId) {
        AlarmRule rule = rules.get(ruleId);
        if (rule != null) {
            rule.setEnabled(false);
        }
    }

    private void notifyRuleTriggered(AlarmRule rule, Alarm alarm) {
        for (RuleEvaluationListener listener : listeners) {
            listener.onRuleTriggered(rule, alarm);
        }
    }

    private void notifyRuleError(AlarmRule rule, Exception e) {
        for (RuleEvaluationListener listener : listeners) {
            listener.onRuleError(rule, e);
        }
    }

    public void addListener(RuleEvaluationListener listener) {
        listeners.add(listener);
    }

    public void removeListener(RuleEvaluationListener listener) {
        listeners.remove(listener);
    }

    public interface RuleEvaluationListener {
        void onRuleTriggered(AlarmRule rule, Alarm alarm);
        void onRuleError(AlarmRule rule, Exception e);
    }

    public static class EvaluationResult {
        private final List<Alarm> triggeredAlarms;
        private final List<AlarmRule> triggeredRules;
        private final boolean enabled;

        public EvaluationResult(List<Alarm> triggeredAlarms, List<AlarmRule> triggeredRules) {
            this(triggeredAlarms, triggeredRules, true);
        }

        private EvaluationResult(List<Alarm> triggeredAlarms, List<AlarmRule> triggeredRules, boolean enabled) {
            this.triggeredAlarms = triggeredAlarms;
            this.triggeredRules = triggeredRules;
            this.enabled = enabled;
        }

        public static EvaluationResult disabled() {
            return new EvaluationResult(new ArrayList<>(), new ArrayList<>(), false);
        }

        public List<Alarm> getTriggeredAlarms() {
            return triggeredAlarms;
        }

        public List<AlarmRule> getTriggeredRules() {
            return triggeredRules;
        }

        public boolean hasTriggeredAlarms() {
            return !triggeredAlarms.isEmpty();
        }

        public boolean isEnabled() {
            return enabled;
        }
    }

    public static class AlarmContext {
        private final Map<String, Object> data;

        public AlarmContext() {
            this.data = new ConcurrentHashMap<>();
        }

        public AlarmContext set(String key, Object value) {
            data.put(key, value);
            return this;
        }

        @SuppressWarnings("unchecked")
        public <T> T get(String key) {
            return (T) data.get(key);
        }

        @SuppressWarnings("unchecked")
        public <T> T get(String key, Class<T> type) {
            Object value = data.get(key);
            if (value != null && type.isInstance(value)) {
                return (T) value;
            }
            return null;
        }

        public double getDouble(String key, double defaultValue) {
            Object value = data.get(key);
            if (value instanceof Number) {
                return ((Number) value).doubleValue();
            }
            return defaultValue;
        }

        public int getInt(String key, int defaultValue) {
            Object value = data.get(key);
            if (value instanceof Number) {
                return ((Number) value).intValue();
            }
            return defaultValue;
        }

        public long getLong(String key, long defaultValue) {
            Object value = data.get(key);
            if (value instanceof Number) {
                return ((Number) value).longValue();
            }
            return defaultValue;
        }

        public String getString(String key, String defaultValue) {
            Object value = data.get(key);
            if (value instanceof String) {
                return (String) value;
            }
            return defaultValue;
        }

        public boolean getBoolean(String key, boolean defaultValue) {
            Object value = data.get(key);
            if (value instanceof Boolean) {
                return (Boolean) value;
            }
            return defaultValue;
        }
    }

    public abstract static class AlarmRule {
        private final String id;
        private final String name;
        private final AlarmType alarmType;
        private boolean enabled = true;

        protected AlarmRule(String id, String name, AlarmType alarmType) {
            this.id = id;
            this.name = name;
            this.alarmType = alarmType;
        }

        public String getId() {
            return id;
        }

        public String getName() {
            return name;
        }

        public AlarmType getAlarmType() {
            return alarmType;
        }

        public boolean isEnabled() {
            return enabled;
        }

        public void setEnabled(boolean enabled) {
            this.enabled = enabled;
        }

        public abstract boolean evaluate(AlarmContext context);
        public abstract Alarm createAlarm(AlarmContext context);
    }

    private static class CollisionAlarmRule extends AlarmRule {
        private double cpaThreshold = 2.0;
        private double tcpaThreshold = 12.0;

        public CollisionAlarmRule() {
            super("collision-rule", "碰撞预警规则", AlarmType.COLLISION);
        }

        @Override
        public boolean evaluate(AlarmContext context) {
            double cpa = context.getDouble("cpa", Double.MAX_VALUE);
            double tcpa = context.getDouble("tcpa", Double.MAX_VALUE);
            
            return cpa < cpaThreshold && tcpa < tcpaThreshold && tcpa > 0;
        }

        @Override
        public Alarm createAlarm(AlarmContext context) {
            return Alarm.builder()
                    .type(AlarmType.COLLISION)
                    .level(determineLevel(context.getDouble("tcpa", 0)))
                    .title("碰撞预警")
                    .message(buildMessage(context))
                    .build();
        }

        private AlarmLevel determineLevel(double tcpa) {
            if (tcpa < 6) return AlarmLevel.CRITICAL;
            if (tcpa < 12) return AlarmLevel.WARNING;
            return AlarmLevel.INFO;
        }

        private String buildMessage(AlarmContext context) {
            return String.format("目标 %s CPA %.2f 海里，TCPA %.1f 分钟",
                    context.getString("targetName", "未知"),
                    context.getDouble("cpa", 0),
                    context.getDouble("tcpa", 0));
        }
    }

    private static class DeviationAlarmRule extends AlarmRule {
        private double maxDeviationNm = 1.0;

        public DeviationAlarmRule() {
            super("deviation-rule", "偏航预警规则", AlarmType.DEVIATION);
        }

        @Override
        public boolean evaluate(AlarmContext context) {
            double deviation = context.getDouble("deviationDistance", 0);
            return deviation > maxDeviationNm;
        }

        @Override
        public Alarm createAlarm(AlarmContext context) {
            return Alarm.builder()
                    .type(AlarmType.DEVIATION)
                    .level(AlarmLevel.WARNING)
                    .title("偏航预警")
                    .message(String.format("偏航 %.2f 海里", 
                            context.getDouble("deviationDistance", 0)))
                    .build();
        }
    }

    private static class ShallowWaterAlarmRule extends AlarmRule {
        private double minUkcMeters = 2.0;

        public ShallowWaterAlarmRule() {
            super("shallow-water-rule", "浅水预警规则", AlarmType.SHALLOW_WATER);
        }

        @Override
        public boolean evaluate(AlarmContext context) {
            double ukc = context.getDouble("ukc", Double.MAX_VALUE);
            return ukc < minUkcMeters;
        }

        @Override
        public Alarm createAlarm(AlarmContext context) {
            return Alarm.builder()
                    .type(AlarmType.SHALLOW_WATER)
                    .level(AlarmLevel.WARNING)
                    .title("浅水预警")
                    .message(String.format("富余水深 %.1f 米", 
                            context.getDouble("ukc", 0)))
                    .build();
        }
    }

    private static class RestrictedAreaAlarmRule extends AlarmRule {
        public RestrictedAreaAlarmRule() {
            super("restricted-area-rule", "禁航区预警规则", AlarmType.RESTRICTED_AREA);
        }

        @Override
        public boolean evaluate(AlarmContext context) {
            return context.getBoolean("insideRestrictedArea", false);
        }

        @Override
        public Alarm createAlarm(AlarmContext context) {
            return Alarm.builder()
                    .type(AlarmType.RESTRICTED_AREA)
                    .level(AlarmLevel.CRITICAL)
                    .title("禁航区预警")
                    .message("已进入禁航区域")
                    .build();
        }
    }

    private static class WatchAlarmRule extends AlarmRule {
        private long watchTimeoutMs = 12 * 60 * 1000;

        public WatchAlarmRule() {
            super("watch-rule", "值班报警规则", AlarmType.WATCH);
        }

        @Override
        public boolean evaluate(AlarmContext context) {
            long lastActivity = context.getLong("lastActivityTime", 0);
            long now = System.currentTimeMillis();
            return (now - lastActivity) > watchTimeoutMs;
        }

        @Override
        public Alarm createAlarm(AlarmContext context) {
            return Alarm.builder()
                    .type(AlarmType.WATCH)
                    .level(AlarmLevel.WARNING)
                    .title("值班报警")
                    .message("操作超时，请确认")
                    .build();
        }
    }
}
