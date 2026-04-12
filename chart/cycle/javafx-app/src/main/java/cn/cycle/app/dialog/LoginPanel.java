package cn.cycle.app.dialog;

import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.control.Button;
import javafx.scene.control.CheckBox;
import javafx.scene.control.Hyperlink;
import javafx.scene.control.Label;
import javafx.scene.control.PasswordField;
import javafx.scene.control.Separator;
import javafx.scene.control.TextField;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Priority;
import javafx.scene.layout.Region;
import javafx.scene.layout.VBox;
import javafx.stage.Popup;
import javafx.stage.Window;

public class LoginPanel {
    
    private Popup loginPopup;
    private VBox mainContainer;
    private Label userLabel;
    private TextField usernameField;
    private PasswordField passwordField;
    
    public LoginPanel() {
        createPanel();
    }
    
    private void createPanel() {
        loginPopup = new Popup();
        loginPopup.setAutoHide(true);
        loginPopup.setAutoFix(true);
        
        mainContainer = new VBox();
        mainContainer.setStyle("-fx-background-color: white; -fx-border-color: #cccccc; -fx-border-width: 1; -fx-effect: dropshadow(three-pass-box, rgba(0,0,0,0.2), 10, 0, 0, 5);");
        mainContainer.setPrefWidth(280);
        
        createHeader();
        createContent();
        createFooter();
        
        loginPopup.getContent().add(mainContainer);
    }
    
    private void createHeader() {
        HBox headerBox = new HBox(10);
        headerBox.setPadding(new Insets(10, 15, 10, 15));
        headerBox.setAlignment(Pos.CENTER_LEFT);
        headerBox.setStyle("-fx-background-color: #f5f5f5;");
        
        ImageView avatarView = new ImageView();
        try {
            Image avatar = new Image(getClass().getResourceAsStream("/icons/user_avatar_48.png"));
            avatarView.setImage(avatar);
        } catch (Exception e) {
            avatarView.setStyle("-fx-background-color: #e0e0e0; -fx-background-radius: 20;");
        }
        avatarView.setFitWidth(40);
        avatarView.setFitHeight(40);
        avatarView.setPreserveRatio(true);
        
        userLabel = new Label("未登录");
        userLabel.setStyle("-fx-font-size: 14px; -fx-font-weight: bold; -fx-text-fill: #333333;");
        
        Region spacer = new Region();
        HBox.setHgrow(spacer, Priority.ALWAYS);
        
        Button closeButton = new Button("×");
        closeButton.setStyle("-fx-background-color: transparent; -fx-text-fill: #666666; -fx-font-size: 16px; -fx-padding: 0 5 0 5; -fx-cursor: hand; -fx-border-width: 0;");
        closeButton.setOnAction(e -> loginPopup.hide());
        closeButton.setOnMouseEntered(e -> closeButton.setStyle("-fx-background-color: #e0e0e0; -fx-text-fill: #333333; -fx-font-size: 16px; -fx-padding: 0 5 0 5; -fx-cursor: hand; -fx-border-width: 0; -fx-background-radius: 3;"));
        closeButton.setOnMouseExited(e -> closeButton.setStyle("-fx-background-color: transparent; -fx-text-fill: #666666; -fx-font-size: 16px; -fx-padding: 0 5 0 5; -fx-cursor: hand; -fx-border-width: 0;"));
        
        headerBox.getChildren().addAll(avatarView, userLabel, spacer, closeButton);
        mainContainer.getChildren().add(headerBox);
    }
    
    private void createContent() {
        VBox contentBox = new VBox(8);
        contentBox.setPadding(new Insets(15));
        
        usernameField = new TextField();
        usernameField.setPromptText("用户名/邮箱");
        usernameField.setPrefHeight(32);
        usernameField.setStyle("-fx-background-color: white; -fx-border-color: #cccccc; -fx-border-radius: 3;");
        
        passwordField = new PasswordField();
        passwordField.setPromptText("密码");
        passwordField.setPrefHeight(32);
        passwordField.setStyle("-fx-background-color: white; -fx-border-color: #cccccc; -fx-border-radius: 3;");
        
        HBox optionsRow = new HBox(15);
        CheckBox rememberBox = new CheckBox("记住密码");
        CheckBox autoLoginBox = new CheckBox("自动登录");
        optionsRow.getChildren().addAll(rememberBox, autoLoginBox);
        
        Button loginButton = new Button("登录");
        loginButton.setPrefWidth(Double.MAX_VALUE);
        loginButton.setStyle("-fx-background-color: #2196f3; -fx-text-fill: white; -fx-font-weight: bold; -fx-background-radius: 3; -fx-cursor: hand;");
        loginButton.setOnAction(e -> doLogin());
        loginButton.setOnMouseEntered(e -> loginButton.setStyle("-fx-background-color: #1976d2; -fx-text-fill: white; -fx-font-weight: bold; -fx-background-radius: 3; -fx-cursor: hand;"));
        loginButton.setOnMouseExited(e -> loginButton.setStyle("-fx-background-color: #2196f3; -fx-text-fill: white; -fx-font-weight: bold; -fx-background-radius: 3; -fx-cursor: hand;"));
        
        contentBox.getChildren().addAll(usernameField, passwordField, optionsRow, loginButton);
        mainContainer.getChildren().add(contentBox);
    }
    
    private void createFooter() {
        Separator sep = new Separator();
        sep.setStyle("-fx-background-color: #e0e0e0;");
        
        VBox footerBox = new VBox(5);
        footerBox.setPadding(new Insets(10, 15, 10, 15));
        footerBox.setStyle("-fx-background-color: #fafafa;");
        
        Hyperlink registerLink = new Hyperlink("注册新账号");
        registerLink.setStyle("-fx-text-fill: #2196f3;");
        Hyperlink forgotLink = new Hyperlink("忘记密码?");
        forgotLink.setStyle("-fx-text-fill: #2196f3;");
        Hyperlink thirdPartyLink = new Hyperlink("第三方账号登录");
        thirdPartyLink.setStyle("-fx-text-fill: #2196f3;");
        
        HBox linksRow = new HBox(15);
        linksRow.getChildren().addAll(registerLink, forgotLink);
        
        footerBox.getChildren().addAll(linksRow, thirdPartyLink);
        mainContainer.getChildren().addAll(sep, footerBox);
    }
    
    private void doLogin() {
        String username = usernameField.getText();
        String password = passwordField.getText();
        
        if (username.isEmpty()) {
            System.out.println("请输入用户名");
            return;
        }
        
        System.out.println("登录: " + username);
        userLabel.setText(username);
        loginPopup.hide();
    }
    
    public void show(Window owner, double x, double y) {
        loginPopup.show(owner, x, y);
    }
    
    public void hide() {
        loginPopup.hide();
    }
    
    public boolean isShowing() {
        return loginPopup.isShowing();
    }
}
