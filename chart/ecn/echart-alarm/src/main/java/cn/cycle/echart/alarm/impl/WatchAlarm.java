package cn.cycle.echart.alarm.impl;

import cn.cycle.echart.alarm.Alarm;
import cn.cycle.echart.alarm.AlarmLevel;
import cn.cycle.echart.alarm.AlarmType;

/**
 * 值班报警。
 * 
 * <p>ECDIS系统的强制性安全功能，当操作员在设定时间内未进行任何操作时触发。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WatchAlarm extends Alarm {

    public enum WatchAlarmState {
        IDLE,
        PRE_ALARM,
        ALARM,
        ESCALATED,
        ACKNOWLEDGED
    }

    private final long watchTimeoutMs;
    private final long preAlarmDurationMs;
    private final long ackTimeoutMs;
    private final long lastActivityTimeMs;
    private WatchAlarmState watchState;

    public WatchAlarm(long watchTimeoutMs, long preAlarmDurationMs,
                      long ackTimeoutMs, long lastActivityTimeMs,
                      WatchAlarmState watchState) {
        super(AlarmType.WATCH, determineLevel(watchState),
              "值班报警", buildMessage(watchState, watchTimeoutMs));
        
        this.watchTimeoutMs = watchTimeoutMs;
        this.preAlarmDurationMs = preAlarmDurationMs;
        this.ackTimeoutMs = ackTimeoutMs;
        this.lastActivityTimeMs = lastActivityTimeMs;
        this.watchState = watchState;

        withData("watchTimeoutMs", watchTimeoutMs);
        withData("preAlarmDurationMs", preAlarmDurationMs);
        withData("ackTimeoutMs", ackTimeoutMs);
        withData("lastActivityTimeMs", lastActivityTimeMs);
        withData("watchState", watchState);
    }

    private static AlarmLevel determineLevel(WatchAlarmState state) {
        switch (state) {
            case ESCALATED:
                return AlarmLevel.CRITICAL;
            case ALARM:
            case PRE_ALARM:
                return AlarmLevel.WARNING;
            default:
                return AlarmLevel.INFO;
        }
    }

    private static String buildMessage(WatchAlarmState state, long timeoutMs) {
        long timeoutMinutes = timeoutMs / 60000;
        switch (state) {
            case PRE_ALARM:
                return String.format("值班预警：%.0f 分钟内无操作，请确认", (double) timeoutMinutes);
            case ALARM:
                return "值班报警：请立即确认";
            case ESCALATED:
                return "值班报警升级：请立即确认！";
            default:
                return String.format("值班报警（超时 %.0f 分钟）", (double) timeoutMinutes);
        }
    }

    public long getWatchTimeoutMs() {
        return watchTimeoutMs;
    }

    public long getPreAlarmDurationMs() {
        return preAlarmDurationMs;
    }

    public long getAckTimeoutMs() {
        return ackTimeoutMs;
    }

    public long getLastActivityTimeMs() {
        return lastActivityTimeMs;
    }

    public WatchAlarmState getWatchState() {
        return watchState;
    }

    public void setWatchState(WatchAlarmState state) {
        this.watchState = state;
    }
}
