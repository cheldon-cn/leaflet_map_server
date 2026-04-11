package cn.cycle.app.sidebar;

import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;

public class DataConvertPanel extends AbstractSideBarPanel {
    
    public DataConvertPanel() {
        super("data-convert", "数据转换", "转", Color.ORANGE);
    }
    
    @Override
    protected Node createCenterContent() {
        VBox container = new VBox(10);
        container.setPadding(new javafx.geometry.Insets(10));
        
        Label titleLabel = new Label("数据转换工具");
        titleLabel.setStyle("-fx-font-weight: bold;");
        
        GridPane form = new GridPane();
        form.setHgap(10);
        form.setVgap(10);
        
        Label sourceLabel = new Label("源格式:");
        ComboBox<String> sourceCombo = new ComboBox<>();
        sourceCombo.getItems().addAll("S-57", "Shapefile", "GeoJSON", "KML");
        sourceCombo.setPrefWidth(150);
        
        Label targetLabel = new Label("目标格式:");
        ComboBox<String> targetCombo = new ComboBox<>();
        targetCombo.getItems().addAll("S-57", "Shapefile", "GeoJSON", "KML");
        targetCombo.setPrefWidth(150);
        
        Button convertButton = new Button("开始转换");
        convertButton.setMaxWidth(Double.MAX_VALUE);
        
        form.add(sourceLabel, 0, 0);
        form.add(sourceCombo, 1, 0);
        form.add(targetLabel, 0, 1);
        form.add(targetCombo, 1, 1);
        
        container.getChildren().addAll(titleLabel, form, convertButton);
        return container;
    }
}
