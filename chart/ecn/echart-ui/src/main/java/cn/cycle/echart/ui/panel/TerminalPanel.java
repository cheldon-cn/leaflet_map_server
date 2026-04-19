package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxRightTabPanel;
import javafx.scene.control.*;
import javafx.scene.input.KeyCode;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;

import java.util.ArrayList;
import java.util.List;

/**
 * 终端面板。
 * 
 * <p>提供命令行交互界面。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class TerminalPanel extends BorderPane implements FxRightTabPanel {

    private static final String TAB_ID = "terminal-panel";
    
    private final TextArea outputArea;
    private final TextField inputField;
    private final Button sendButton;
    private final Button clearButton;
    private final Tab tab;
    
    private final List<String> commandHistory;
    private int historyIndex;

    public TerminalPanel() {
        this.outputArea = new TextArea();
        this.inputField = new TextField();
        this.sendButton = new Button("发送");
        this.clearButton = new Button("清除");
        this.tab = new Tab("终端");
        this.tab.setContent(this);
        this.tab.setClosable(false);
        this.commandHistory = new ArrayList<>();
        this.historyIndex = -1;
        
        initializeLayout();
    }

    private void initializeLayout() {
        outputArea.setEditable(false);
        outputArea.setWrapText(true);
        outputArea.setStyle("-fx-font-family: monospace; -fx-background-color: #1e1e1e; -fx-text-fill: #d4d4d4;");
        
        inputField.setPromptText("输入命令...");
        inputField.setStyle("-fx-font-family: monospace;");
        
        inputField.setOnAction(e -> executeCommand());
        inputField.setOnKeyPressed(e -> {
            if (e.getCode() == KeyCode.UP) {
                navigateHistory(-1);
                e.consume();
            } else if (e.getCode() == KeyCode.DOWN) {
                navigateHistory(1);
                e.consume();
            }
        });
        
        sendButton.setOnAction(e -> executeCommand());
        clearButton.setOnAction(e -> outputArea.clear());
        
        VBox inputBox = new VBox(5);
        ToolBar toolBar = new ToolBar();
        toolBar.getItems().addAll(sendButton, clearButton);
        inputBox.getChildren().addAll(inputField, toolBar);
        
        setCenter(outputArea);
        setBottom(inputBox);
        
        getStyleClass().add("terminal-panel");
        
        appendOutput("终端已就绪。输入 'help' 查看可用命令。");
    }

    private void executeCommand() {
        String command = inputField.getText().trim();
        if (command.isEmpty()) {
            return;
        }
        
        commandHistory.add(command);
        historyIndex = commandHistory.size();
        
        appendOutput("> " + command);
        
        processCommand(command);
        
        inputField.clear();
    }

    private void processCommand(String command) {
        String[] parts = command.split("\\s+");
        String cmd = parts[0].toLowerCase();
        
        switch (cmd) {
            case "help":
                showHelp();
                break;
            case "clear":
                outputArea.clear();
                break;
            case "echo":
                if (parts.length > 1) {
                    StringBuilder sb = new StringBuilder();
                    for (int i = 1; i < parts.length; i++) {
                        sb.append(parts[i]).append(" ");
                    }
                    appendOutput(sb.toString().trim());
                }
                break;
            case "version":
                appendOutput("E-Chart Terminal v1.0.0");
                break;
            case "date":
                appendOutput(java.time.LocalDateTime.now().toString());
                break;
            case "exit":
                appendOutput("终端会话结束");
                break;
            default:
                appendOutput("未知命令: " + cmd + "。输入 'help' 查看可用命令。");
        }
    }

    private void showHelp() {
        appendOutput("可用命令:");
        appendOutput("  help    - 显示帮助信息");
        appendOutput("  clear   - 清除输出");
        appendOutput("  echo    - 回显文本");
        appendOutput("  version - 显示版本信息");
        appendOutput("  date    - 显示当前日期时间");
        appendOutput("  exit    - 退出终端");
    }

    private void navigateHistory(int direction) {
        if (commandHistory.isEmpty()) {
            return;
        }
        
        historyIndex += direction;
        
        if (historyIndex < 0) {
            historyIndex = 0;
        } else if (historyIndex >= commandHistory.size()) {
            historyIndex = commandHistory.size();
            inputField.clear();
            return;
        }
        
        inputField.setText(commandHistory.get(historyIndex));
        inputField.positionCaret(inputField.getText().length());
    }

    public void appendOutput(String text) {
        outputArea.appendText(text + "\n");
    }

    @Override
    public String getTabId() {
        return TAB_ID;
    }

    @Override
    public String getIcon() {
        return "/icons/terminal.png";
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
        inputField.requestFocus();
    }

    @Override
    public void onDeselected() {
    }

    @Override
    public void refresh() {
    }
}
