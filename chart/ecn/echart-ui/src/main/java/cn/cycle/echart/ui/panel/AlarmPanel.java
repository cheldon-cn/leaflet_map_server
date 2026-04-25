package cn.cycle.echart.ui.panel;

import cn.cycle.echart.ui.FxRightTabPanel;
import cn.cycle.echart.facade.AlarmFacade;
import cn.cycle.chart.api.alert.Alert;
import cn.cycle.chart.api.alert.Alert.Severity;
import cn.cycle.chart.api.alert.Alert.Type;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.VBox;

import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.Objects;

/**
 * 预警面板。
 * 
 * <p>显示和管理预警信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AlarmPanel extends BorderPane implements FxRightTabPanel {

    private static final String TAB_ID = "alarm-panel";
    private static final DateTimeFormatter TIME_FORMATTER = DateTimeFormatter.ofPattern("HH:mm:ss");
    
    private final ListView<Alert> activeListView;
    private final ListView<String> historyListView;
    private final TabPane tabPane;
    private final Tab activeTab;
    private final Tab historyTab;
    
    private AlarmFacade alarmFacade;
    private final Tab tab;

    public AlarmPanel() {
        this(null);
    }

    public AlarmPanel(AlarmFacade alarmFacade) {
        this.alarmFacade = alarmFacade;
        this.activeListView = new ListView<>();
        this.historyListView = new ListView<>();
        this.tabPane = new TabPane();
        this.activeTab = new Tab("活动预警");
        this.historyTab = new Tab("历史记录");
        this.tab = new Tab("预警");
        this.tab.setContent(this);
        this.tab.setClosable(false);
        
        initializeLayout();
        if (alarmFacade != null) {
            loadData();
        }
    }
    
    public void setAlarmFacade(AlarmFacade alarmFacade) {
        this.alarmFacade = alarmFacade;
        if (alarmFacade != null) {
            loadData();
        }
    }

    private void initializeLayout() {
        activeTab.setContent(activeListView);
        activeTab.setClosable(false);
        
        historyTab.setContent(historyListView);
        historyTab.setClosable(false);
        
        tabPane.getTabs().addAll(activeTab, historyTab);
        
        activeListView.setCellFactory(param -> new ListCell<Alert>() {
            @Override
            protected void updateItem(Alert alert, boolean empty) {
                super.updateItem(alert, empty);
                if (empty || alert == null) {
                    setText(null);
                    setGraphic(null);
                } else {
                    setText(String.format("[%s] %s: %s", 
                            alert.getSeverity(), alert.getTitle(), alert.getMessage()));
                    setStyle(getSeverityStyle(alert.getSeverity()));
                }
            }
        });
        
        setTop(createToolBar());
        setCenter(tabPane);
        
        getStyleClass().add("alarm-panel");
    }

    private String getSeverityStyle(Severity severity) {
        if (severity == Severity.EMERGENCY) {
            return "-fx-text-fill: red; -fx-font-weight: bold;";
        } else if (severity == Severity.CRITICAL) {
            return "-fx-text-fill: orange; -fx-font-weight: bold;";
        } else if (severity == Severity.WARNING) {
            return "-fx-text-fill: yellow;";
        } else {
            return "-fx-text-fill: green;";
        }
    }

    private ToolBar createToolBar() {
        ToolBar toolBar = new ToolBar();
        
        Button acknowledgeButton = new Button("确认");
        acknowledgeButton.setOnAction(e -> acknowledgeAlarm());
        
        Button clearButton = new Button("清除");
        clearButton.setOnAction(e -> clearAlarm());
        
        Button clearAllButton = new Button("全部清除");
        clearAllButton.setOnAction(e -> clearAllAlarms());
        
        Button refreshButton = new Button("刷新");
        refreshButton.setOnAction(e -> loadData());
        
        toolBar.getItems().addAll(acknowledgeButton, clearButton, clearAllButton, 
                new Separator(), refreshButton);
        return toolBar;
    }

    private void loadData() {
        activeListView.getItems().clear();
        
        if (alarmFacade == null) {
            activeListView.getItems().add(new Alert(Type.SYSTEM, Severity.INFO, "预警门面未初始化", "等待AlarmFacade注入"));
            loadHistory();
            return;
        }
        
        List<Alert> alerts = alarmFacade.getActiveAlerts();
        activeListView.getItems().addAll(alerts);
        
        loadHistory();
    }

    private void loadHistory() {
        historyListView.getItems().clear();
        historyListView.getItems().add("预警历史功能暂未实现");
    }

    private void acknowledgeAlarm() {
        if (alarmFacade == null) {
            showWarning("预警模块尚未初始化，暂时无法使用预警相关功能");
            return;
        }
        Alert selected = activeListView.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showWarning("请先选择一个预警");
            return;
        }
        
        alarmFacade.acknowledgeAlert(selected.getId());
        activeListView.refresh();
    }

    private void clearAlarm() {
        if (alarmFacade == null) {
            showWarning("预警模块尚未初始化，暂时无法使用预警相关功能");
            return;
        }
        Alert selected = activeListView.getSelectionModel().getSelectedItem();
        if (selected == null) {
            showWarning("请先选择一个预警");
            return;
        }
        
        alarmFacade.clearAlert(selected.getId());
        activeListView.getItems().remove(selected);
    }

    private void clearAllAlarms() {
        javafx.scene.control.Alert alert = new javafx.scene.control.Alert(
                javafx.scene.control.Alert.AlertType.CONFIRMATION);
        alert.setTitle("确认清除");
        alert.setHeaderText("确认清除所有预警？");
        alert.setContentText("此操作不可撤销");
        
        alert.showAndWait().ifPresent(response -> {
            if (response == ButtonType.OK) {
                alarmFacade.clearAllAlerts();
                activeListView.getItems().clear();
            }
        });
    }

    private void showWarning(String message) {
        javafx.scene.control.Alert alert = new javafx.scene.control.Alert(
                javafx.scene.control.Alert.AlertType.WARNING);
        alert.setTitle("警告");
        alert.setContentText(message);
        alert.showAndWait();
    }

    @Override
    public String getTabId() {
        return TAB_ID;
    }

    @Override
    public String getIcon() {
        return "/icons/alarm.png";
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
        loadData();
    }

    @Override
    public void onDeselected() {
    }

    @Override
    public void refresh() {
        loadData();
    }
}
