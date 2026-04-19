package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxRightTabPanel;
import cn.cycle.echart.ui.dialog.ErrorDialog;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.stage.FileChooser;

import java.io.File;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

/**
 * 日志面板。
 * 
 * <p>显示应用程序日志信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class LogPanel extends BorderPane implements FxRightTabPanel {

    private static final String TAB_ID = "log-panel";
    private static final DateTimeFormatter TIME_FORMATTER = DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss");
    
    private final TextArea logArea;
    private final TextField filterField;
    private final Button exportButton;
    private final Button clearButton;
    private final Tab tab;

    public LogPanel() {
        this.logArea = new TextArea();
        this.filterField = new TextField();
        this.exportButton = new Button("导出");
        this.clearButton = new Button("清除");
        this.tab = new Tab("日志");
        this.tab.setContent(this);
        this.tab.setClosable(false);
        
        initializeLayout();
    }

    private void initializeLayout() {
        logArea.setEditable(false);
        logArea.setWrapText(true);
        logArea.setStyle("-fx-font-family: monospace;");
        
        filterField.setPromptText("过滤日志...");
        filterField.textProperty().addListener((obs, oldVal, newVal) -> filterLogs(newVal));
        
        exportButton.setOnAction(e -> exportLogs());
        clearButton.setOnAction(e -> logArea.clear());
        
        setTop(createToolBar());
        setCenter(logArea);
        
        getStyleClass().add("log-panel");
        
        log("日志面板已初始化");
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();
        toolBar.getItems().addAll(filterField, exportButton, clearButton);
        return toolBar;
    }

    public void log(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] %s%n", timestamp, message));
    }

    public void logError(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] ERROR: %s%n", timestamp, message));
    }

    public void logWarning(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] WARN: %s%n", timestamp, message));
    }

    public void logInfo(String message) {
        log(message);
    }

    public void logDebug(String message) {
        String timestamp = LocalDateTime.now().format(TIME_FORMATTER);
        logArea.appendText(String.format("[%s] DEBUG: %s%n", timestamp, message));
    }

    private void filterLogs(String keyword) {
    }

    private void exportLogs() {
        FileChooser fileChooser = new FileChooser();
        fileChooser.setTitle("导出日志");
        fileChooser.getExtensionFilters().add(
                new FileChooser.ExtensionFilter("文本文件", "*.txt"));
        
        File file = fileChooser.showSaveDialog(getScene().getWindow());
        if (file != null) {
            try {
                java.nio.file.Files.write(file.toPath(), logArea.getText().getBytes());
                log("日志已导出到: " + file.getAbsolutePath());
            } catch (Exception e) {
                ErrorDialog.showError("导出失败", "无法导出日志文件", e.getMessage());
            }
        }
    }

    @Override
    public String getTabId() {
        return TAB_ID;
    }

    @Override
    public String getIcon() {
        return "/icons/log.png";
    }

    @Override
    public Tab getTab() {
        return tab;
    }

    @Override
    public javafx.scene.Node getContent() {
        return this;
    }

    @Override
    public void onSelected() {
    }

    @Override
    public void onDeselected() {
    }

    @Override
    public void refresh() {
    }
}
