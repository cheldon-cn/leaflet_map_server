package cn.cycle.echart.ui.handler;

public class RouteHandler {

    private final MessageCallback messageCallback;

    public RouteHandler(MessageCallback messageCallback) {
        this.messageCallback = messageCallback;
    }

    public void onCreateRoute() {
        messageCallback.showInfo("创建航线", "创建航线功能开发中...");
    }

    public void onEditRoute() {
        messageCallback.showInfo("编辑航线", "编辑航线功能开发中...");
    }

    public void onDeleteRoute() {
        messageCallback.showInfo("删除航线", "删除航线功能开发中...");
    }

    public void onImportRoute() {
        messageCallback.showInfo("导入航线", "导入航线功能开发中...");
    }

    public void onExportRoute() {
        messageCallback.showInfo("导出航线", "导出航线功能开发中...");
    }

    public void onCheckRoute() {
        messageCallback.showInfo("航线检查", "航线检查功能开发中...");
    }
}
