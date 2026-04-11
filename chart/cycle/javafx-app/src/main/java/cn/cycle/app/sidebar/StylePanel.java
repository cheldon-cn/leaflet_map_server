package cn.cycle.app.sidebar;

import javafx.scene.Node;
import javafx.scene.control.*;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;

public class StylePanel extends AbstractSideBarPanel {
    
    public StylePanel() {
        super("style", "符号样式", "样", Color.PURPLE);
    }
    
    @Override
    protected Node createCenterContent() {
        VBox container = new VBox(10);
        container.setPadding(new javafx.geometry.Insets(10));
        
        Label titleLabel = new Label("符号样式设置");
        titleLabel.setStyle("-fx-font-weight: bold;");
        
        GridPane form = new GridPane();
        form.setHgap(10);
        form.setVgap(10);
        
        Label colorLabel = new Label("颜色:");
        ColorPicker colorPicker = new ColorPicker(Color.BLUE);
        
        Label sizeLabel = new Label("大小:");
        Slider sizeSlider = new Slider(1, 20, 5);
        sizeSlider.setShowTickLabels(true);
        sizeSlider.setShowTickMarks(true);
        
        Label opacityLabel = new Label("透明度:");
        Slider opacitySlider = new Slider(0, 1, 1);
        opacitySlider.setShowTickLabels(true);
        
        Label styleLabel = new Label("样式:");
        ComboBox<String> styleCombo = new ComboBox<>();
        styleCombo.getItems().addAll("实线", "虚线", "点线");
        styleCombo.setValue("实线");
        
        form.add(colorLabel, 0, 0);
        form.add(colorPicker, 1, 0);
        form.add(sizeLabel, 0, 1);
        form.add(sizeSlider, 1, 1);
        form.add(opacityLabel, 0, 2);
        form.add(opacitySlider, 1, 2);
        form.add(styleLabel, 0, 3);
        form.add(styleCombo, 1, 3);
        
        Button applyButton = new Button("应用样式");
        applyButton.setMaxWidth(Double.MAX_VALUE);
        
        container.getChildren().addAll(titleLabel, form, applyButton);
        return container;
    }
}
