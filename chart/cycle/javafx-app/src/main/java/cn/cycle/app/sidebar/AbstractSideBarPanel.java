package cn.cycle.app.sidebar;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;
import cn.cycle.app.state.StatePersistable;
import javafx.geometry.Insets;
import javafx.scene.Node;
import javafx.scene.control.Label;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.Rectangle;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;

import java.util.HashMap;
import java.util.Map;

public abstract class AbstractSideBarPanel extends AbstractLifecycleComponent implements SideBarPanel {
    
    private final String id;
    private final String title;
    private final String iconText;
    private final Color iconColor;
    private BorderPane content;
    
    public AbstractSideBarPanel(String id, String title, String iconText, Color iconColor) {
        this.id = id;
        this.title = title;
        this.iconText = iconText;
        this.iconColor = iconColor;
    }
    
    @Override
    public String getId() {
        return id;
    }
    
    @Override
    public String getTitle() {
        return title;
    }
    
    @Override
    public Node getIcon() {
        StackPane iconPane = new StackPane();
        iconPane.setPrefSize(24, 24);
        iconPane.setMinSize(24, 24);
        iconPane.setMaxSize(24, 24);
        
        Rectangle background = new Rectangle(24, 24);
        background.setFill(iconColor);
        background.setArcWidth(4);
        background.setArcHeight(4);
        
        Label textLabel = new Label(iconText);
        textLabel.setTextFill(Color.WHITE);
        textLabel.setFont(Font.font("System", FontWeight.BOLD, 10));
        
        iconPane.getChildren().addAll(background, textLabel);
        return iconPane;
    }
    
    @Override
    public Node createContent() {
        if (content == null) {
            content = new BorderPane();
            content.setTop(createHeader());
            content.setCenter(createCenterContent());
            content.setPrefWidth(250);
            content.setStyle("-fx-background-color: white;");
        }
        return content;
    }
    
    protected Node createHeader() {
        HBox header = new HBox();
        header.setPadding(new Insets(10));
        header.setStyle("-fx-background-color: #f0f0f0; -fx-border-color: #d0d0d0; -fx-border-width: 0 0 1 0;");
        
        Label titleLabel = new Label(getTitle());
        titleLabel.setFont(Font.font("System", FontWeight.BOLD, 14));
        header.getChildren().add(titleLabel);
        
        return header;
    }
    
    protected abstract Node createCenterContent();
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
        content = null;
    }
    
    @Override
    public Map<String, Object> saveState() {
        return new HashMap<>();
    }
    
    @Override
    public void restoreState(Map<String, Object> state) {
    }
    
    @Override
    public String getStateKey() {
        return "sidebar.panel." + id;
    }
}
