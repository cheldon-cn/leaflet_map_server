package cn.cycle.app.view;

import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.scene.control.*;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

public class AlertPanel extends BorderPane {

    private final ListView<AlertItem> alertListView;
    private final ObservableList<AlertItem> alerts;

    public AlertPanel() {
        alerts = FXCollections.observableArrayList();
        alertListView = new ListView<>(alerts);
        alertListView.setCellFactory(list -> new AlertListCell());

        setCenter(alertListView);
        setTop(createHeader());
        setBottom(createToolBar());
        setPrefWidth(300);
        setPrefHeight(200);
    }

    private HBox createHeader() {
        HBox header = new HBox(10);
        header.setPadding(new Insets(10));
        header.setStyle("-fx-background-color: #ff9800;");

        Label titleLabel = new Label("警报面板");
        titleLabel.setStyle("-fx-text-fill: white; -fx-font-weight: bold;");

        header.getChildren().add(titleLabel);
        return header;
    }

    private HBox createToolBar() {
        HBox toolBar = new HBox(10);
        toolBar.setPadding(new Insets(5));

        Button clearButton = new Button("清除全部");
        clearButton.setOnAction(e -> clearAlerts());

        Button acknowledgeButton = new Button("确认选中");
        acknowledgeButton.setOnAction(e -> acknowledgeSelected());

        toolBar.getChildren().addAll(clearButton, acknowledgeButton);
        return toolBar;
    }

    public void addAlert(AlertLevel level, String message) {
        AlertItem alert = new AlertItem(level, message, LocalDateTime.now());
        alerts.add(0, alert);
    }

    public void addInfo(String message) {
        addAlert(AlertLevel.INFO, message);
    }

    public void addWarning(String message) {
        addAlert(AlertLevel.WARNING, message);
    }

    public void addError(String message) {
        addAlert(AlertLevel.ERROR, message);
    }

    public void clearAlerts() {
        alerts.clear();
    }

    public void acknowledgeSelected() {
        AlertItem selected = alertListView.getSelectionModel().getSelectedItem();
        if (selected != null) {
            selected.setAcknowledged(true);
            alertListView.refresh();
        }
    }

    public int getAlertCount() {
        return alerts.size();
    }

    public int getUnacknowledgedCount() {
        return (int) alerts.stream().filter(a -> !a.isAcknowledged()).count();
    }

    public enum AlertLevel {
        INFO("#2196F3"),
        WARNING("#FF9800"),
        ERROR("#F44336");

        private final String color;

        AlertLevel(String color) {
            this.color = color;
        }

        public String getColor() {
            return color;
        }
    }

    public static class AlertItem {
        private final AlertLevel level;
        private final String message;
        private final LocalDateTime timestamp;
        private boolean acknowledged;

        public AlertItem(AlertLevel level, String message, LocalDateTime timestamp) {
            this.level = level;
            this.message = message;
            this.timestamp = timestamp;
            this.acknowledged = false;
        }

        public AlertLevel getLevel() {
            return level;
        }

        public String getMessage() {
            return message;
        }

        public LocalDateTime getTimestamp() {
            return timestamp;
        }

        public boolean isAcknowledged() {
            return acknowledged;
        }

        public void setAcknowledged(boolean acknowledged) {
            this.acknowledged = acknowledged;
        }

        @Override
        public String toString() {
            DateTimeFormatter formatter = DateTimeFormatter.ofPattern("HH:mm:ss");
            return String.format("[%s] %s: %s", timestamp.format(formatter), level, message);
        }
    }

    private static class AlertListCell extends ListCell<AlertItem> {
        private static final DateTimeFormatter TIME_FORMAT = DateTimeFormatter.ofPattern("HH:mm:ss");

        @Override
        protected void updateItem(AlertItem item, boolean empty) {
            super.updateItem(item, empty);
            if (empty || item == null) {
                setGraphic(null);
                setText(null);
                setStyle("");
            } else {
                VBox content = new VBox(2);
                
                Label timeLabel = new Label(item.getTimestamp().format(TIME_FORMAT));
                timeLabel.setStyle("-fx-font-size: 9px; -fx-text-fill: gray;");
                
                Label msgLabel = new Label(item.getMessage());
                msgLabel.setWrapText(true);
                msgLabel.setMaxWidth(250);
                
                content.getChildren().addAll(timeLabel, msgLabel);
                
                String bgColor = item.isAcknowledged() ? "#f5f5f5" : item.getLevel().getColor() + "20";
                String borderColor = item.getLevel().getColor();
                setStyle(String.format("-fx-background-color: %s; -fx-border-color: %s; -fx-border-width: 0 0 0 3;", 
                    bgColor, borderColor));
                
                setGraphic(content);
            }
        }
    }
}
