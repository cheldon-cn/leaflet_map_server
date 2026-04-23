package cn.cycle.echart.ui.handler;

import cn.cycle.echart.ui.SideBarManager;

public class ChartHandler {

    private final SideBarManager sideBarManager;
    private final MessageCallback messageCallback;

    public ChartHandler(SideBarManager sideBarManager, MessageCallback messageCallback) {
        this.sideBarManager = sideBarManager;
        this.messageCallback = messageCallback;
    }

    public void onLoadChart() {
        messageCallback.showInfo("加载海图", "加载海图功能开发中...");
    }

    public void onUnloadChart() {
        messageCallback.showInfo("卸载海图", "卸载海图功能开发中...");
    }

    public void onLayerManager() {
        sideBarManager.showPanel("layers");
    }

    public void onPropertyQuery() {
        messageCallback.showInfo("属性查询", "属性查询功能开发中...");
    }

    public void onFeatureSearch() {
        sideBarManager.showPanel("search");
    }
}
