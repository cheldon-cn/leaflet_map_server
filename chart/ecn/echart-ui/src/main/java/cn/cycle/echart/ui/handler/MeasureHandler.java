package cn.cycle.echart.ui.handler;

public class MeasureHandler {

    private final MessageCallback messageCallback;

    public MeasureHandler(MessageCallback messageCallback) {
        this.messageCallback = messageCallback;
    }

    public void onMeasureDistance() {
        messageCallback.showInfo("距离测量", "距离测量功能开发中...");
    }

    public void onMeasureArea() {
        messageCallback.showInfo("面积测量", "面积测量功能开发中...");
    }

    public void onMeasureBearing() {
        messageCallback.showInfo("方位测量", "方位测量功能开发中...");
    }
}
