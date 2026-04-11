package cn.cycle.app.sidebar;

import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;

public class QueryPanel extends AbstractSideBarPanel {
    
    public QueryPanel() {
        super("query", "数据查询", "查", Color.GREEN);
    }
    
    @Override
    protected Node createCenterContent() {
        VBox container = new VBox(10);
        container.setPadding(new javafx.geometry.Insets(10));
        
        Label titleLabel = new Label("数据查询");
        titleLabel.setStyle("-fx-font-weight: bold;");
        
        TextField searchField = new TextField();
        searchField.setPromptText("输入查询条件...");
        searchField.setPrefWidth(200);
        
        ComboBox<String> fieldCombo = new ComboBox<>();
        fieldCombo.getItems().addAll("名称", "类型", "属性", "坐标");
        fieldCombo.setPromptText("选择查询字段");
        fieldCombo.setPrefWidth(200);
        
        Button queryButton = new Button("查询");
        queryButton.setMaxWidth(Double.MAX_VALUE);
        
        ListView<String> resultListView = new ListView<>();
        resultListView.setPrefHeight(200);
        VBox.setVgrow(resultListView, javafx.scene.layout.Priority.ALWAYS);
        
        container.getChildren().addAll(titleLabel, searchField, fieldCombo, queryButton, resultListView);
        return container;
    }
}
