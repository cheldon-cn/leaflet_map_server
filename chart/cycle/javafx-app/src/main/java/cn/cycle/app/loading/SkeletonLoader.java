package cn.cycle.app.loading;

import javafx.scene.layout.Region;
import javafx.scene.layout.VBox;
import javafx.scene.layout.HBox;
import javafx.scene.shape.Rectangle;
import javafx.animation.Timeline;
import javafx.animation.KeyFrame;
import javafx.util.Duration;

public class SkeletonLoader extends Region {
    
    private static final String DEFAULT_COLOR = "#e0e0e0";
    private static final String SHIMMER_COLOR = "#f0f0f0";
    
    private final Timeline animation;
    
    public SkeletonLoader() {
        this(200, 20);
    }
    
    public SkeletonLoader(double width, double height) {
        setPrefSize(width, height);
        setMinSize(Region.USE_PREF_SIZE, Region.USE_PREF_SIZE);
        setMaxSize(Region.USE_PREF_SIZE, Region.USE_PREF_SIZE);
        
        setStyle("-fx-background-color: " + DEFAULT_COLOR + "; -fx-background-radius: 4;");
        
        animation = createShimmerAnimation();
    }
    
    private Timeline createShimmerAnimation() {
        Timeline timeline = new Timeline(
            new KeyFrame(Duration.ZERO, e -> {
                setStyle("-fx-background-color: " + DEFAULT_COLOR + "; -fx-background-radius: 4;");
            }),
            new KeyFrame(Duration.millis(500), e -> {
                setStyle("-fx-background-color: " + SHIMMER_COLOR + "; -fx-background-radius: 4;");
            }),
            new KeyFrame(Duration.millis(1000), e -> {
                setStyle("-fx-background-color: " + DEFAULT_COLOR + "; -fx-background-radius: 4;");
            })
        );
        timeline.setCycleCount(Timeline.INDEFINITE);
        return timeline;
    }
    
    public void startAnimation() {
        if (animation != null) {
            animation.play();
        }
    }
    
    public void stopAnimation() {
        if (animation != null) {
            animation.stop();
        }
    }
    
    public static VBox createCardSkeleton() {
        VBox skeleton = new VBox(8);
        skeleton.setPadding(new javafx.geometry.Insets(12));
        
        SkeletonLoader title = new SkeletonLoader(180, 16);
        SkeletonLoader line1 = new SkeletonLoader(200, 12);
        SkeletonLoader line2 = new SkeletonLoader(160, 12);
        SkeletonLoader line3 = new SkeletonLoader(140, 12);
        
        skeleton.getChildren().addAll(title, line1, line2, line3);
        
        startAllAnimations(skeleton);
        return skeleton;
    }
    
    public static HBox createListItemSkeleton() {
        HBox skeleton = new HBox(12);
        skeleton.setPadding(new javafx.geometry.Insets(8));
        
        SkeletonLoader avatar = new SkeletonLoader(40, 40);
        VBox content = new VBox(6);
        
        SkeletonLoader title = new SkeletonLoader(120, 14);
        SkeletonLoader subtitle = new SkeletonLoader(80, 10);
        
        content.getChildren().addAll(title, subtitle);
        skeleton.getChildren().addAll(avatar, content);
        
        startAllAnimations(skeleton);
        return skeleton;
    }
    
    public static VBox createTreeItemSkeleton() {
        VBox skeleton = new VBox(4);
        
        for (int i = 0; i < 5; i++) {
            HBox row = new HBox(8);
            SkeletonLoader icon = new SkeletonLoader(16, 16);
            SkeletonLoader text = new SkeletonLoader(100 + Math.random() * 50, 14);
            row.getChildren().addAll(icon, text);
            skeleton.getChildren().add(row);
        }
        
        startAllAnimations(skeleton);
        return skeleton;
    }
    
    private static void startAllAnimations(Region parent) {
        for (javafx.scene.Node node : parent.getChildrenUnmodifiable()) {
            if (node instanceof SkeletonLoader) {
                ((SkeletonLoader) node).startAnimation();
            } else if (node instanceof Region) {
                startAllAnimations((Region) node);
            }
        }
    }
    
    public static void stopAllAnimations(Region parent) {
        for (javafx.scene.Node node : parent.getChildrenUnmodifiable()) {
            if (node instanceof SkeletonLoader) {
                ((SkeletonLoader) node).stopAnimation();
            } else if (node instanceof Region) {
                stopAllAnimations((Region) node);
            }
        }
    }
}
