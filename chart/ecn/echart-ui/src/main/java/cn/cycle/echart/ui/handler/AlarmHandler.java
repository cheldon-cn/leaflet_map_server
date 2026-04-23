package cn.cycle.echart.ui.handler;

public class AlarmHandler {

    private final MessageCallback messageCallback;

    public AlarmHandler(MessageCallback messageCallback) {
        this.messageCallback = messageCallback;
    }

    public void onAlarmSettings() {
        messageCallback.showInfo("预警设置", "预警设置功能开发中...");
    }

    public void onAlarmRules() {
        messageCallback.showInfo("预警规则", "预警规则功能开发中...");
    }

    public void onAlarmHistory() {
        messageCallback.showInfo("预警历史", "预警历史功能开发中...");
    }

    public void onAlarmStatistics() {
        messageCallback.showInfo("预警统计", "预警统计功能开发中...");
    }

    public void onAlarmTest() {
        messageCallback.showInfo("预警测试", "预警测试功能开发中...");
    }
}
