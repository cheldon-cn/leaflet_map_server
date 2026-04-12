package cn.cycle.app.splash;

import javafx.application.Platform;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.scene.Scene;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressBar;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.StackPane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.stage.Screen;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

public class SplashScreen {
    
    private Stage stage;
    private Label statusLabel;
    private ProgressBar progressBar;
    private Label progressLabel;
    private static final double WIDTH_RATIO = 0.35;
    
    public SplashScreen() {
        createSplashStage();
    }
    
    private void createSplashStage() {
        stage = new Stage(StageStyle.UNDECORATED);
        stage.setAlwaysOnTop(true);
        
        StackPane root = new StackPane();
        root.setStyle("-fx-background-color: transparent;");
        
        VBox container = new VBox(10);
        container.setAlignment(Pos.CENTER);
        container.setPadding(new Insets(20));
        container.setStyle(
            "-fx-background-color: linear-gradient(to bottom, #2c3e50, #3498db);" +
            "-fx-background-radius: 10;" +
            "-fx-border-radius: 10;" +
            "-fx-effect: dropshadow(three-pass-box, rgba(0,0,0,0.5), 10, 0, 0, 5);"
        );
        
        ImageView logoView = new ImageView();
        
        double screenWidth = Screen.getPrimary().getVisualBounds().getWidth();
        double imageWidth = screenWidth * WIDTH_RATIO;
        double imageHeight = 300;
        
        try {
            Image logoImage = new Image(getClass().getResourceAsStream("/control/splashlogo.png"));
            logoView.setImage(logoImage);
            
            double originalWidth = logoImage.getWidth();
            double originalHeight = logoImage.getHeight();
            double aspectRatio = originalWidth / originalHeight;
            
            imageHeight = imageWidth / aspectRatio;
            
            logoView.setFitWidth(imageWidth);
            logoView.setPreserveRatio(true);
            
            System.out.println("[DEBUG] Splash image original size: " + originalWidth + "x" + originalHeight);
            System.out.println("[DEBUG] Splash image display size: " + imageWidth + "x" + imageHeight);
            
        } catch (Exception e) {
            System.out.println("[WARNING] Failed to load splash image: " + e.getMessage());
            Label placeholderLogo = new Label("Cycle Chart");
            placeholderLogo.setStyle("-fx-font-size: 32px; -fx-text-fill: white; -fx-font-weight: bold;");
            container.getChildren().add(placeholderLogo);
        }
        
        if (logoView.getImage() != null) {
            container.getChildren().add(logoView);
        }
        
        statusLabel = new Label("正在初始化...");
        statusLabel.setStyle("-fx-font-size: 14px; -fx-text-fill: white;");
        statusLabel.setAlignment(Pos.CENTER);
        
        progressBar = new ProgressBar(0);
        progressBar.setPrefWidth(imageWidth - 50);
        progressBar.setStyle(
            "-fx-accent: #27ae60;" +
            "-fx-background-color: rgba(255,255,255,0.3);"
        );
        
        progressLabel = new Label("0%");
        progressLabel.setStyle("-fx-font-size: 12px; -fx-text-fill: white;");
        progressLabel.setAlignment(Pos.CENTER);
        
        container.getChildren().addAll(statusLabel, progressBar, progressLabel);
        root.getChildren().add(container);
        
        double sceneWidth = imageWidth + 40;
        double sceneHeight = imageHeight + 120;
        
        Scene scene = new Scene(root, sceneWidth, sceneHeight);
        scene.setFill(Color.TRANSPARENT);
        stage.setScene(scene);
        
        stage.centerOnScreen();
    }
    
    public void show() {
        Platform.runLater(() -> {
            stage.show();
        });
    }
    
    public void hide() {
        Platform.runLater(() -> {
            stage.hide();
        });
    }
    
    public void close() {
        Platform.runLater(() -> {
            stage.close();
        });
    }
    
    public void updateProgress(double progress, String status) {
        Platform.runLater(() -> {
            progressBar.setProgress(progress);
            progressLabel.setText(String.format("%.0f%%", progress * 100));
            if (status != null && !status.isEmpty()) {
                statusLabel.setText(status);
            }
        });
    }
    
    public void updateStatus(String status) {
        Platform.runLater(() -> {
            statusLabel.setText(status);
        });
    }
}
