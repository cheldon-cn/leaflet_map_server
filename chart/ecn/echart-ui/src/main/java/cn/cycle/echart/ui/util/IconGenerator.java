package cn.cycle.echart.ui.util;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.embed.swing.SwingFXUtils;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.layout.StackPane;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class IconGenerator extends Application {

    private static final int ICON_SIZE = 32;
    private static final Color STROKE_COLOR = Color.web("#555555");
    private static final Color FILL_COLOR = Color.web("#4A90D9");
    private static final String OUTPUT_DIR = "src/main/resources/icons/";

    private static final Map<String, IconDrawer> ICON_DRAWERS = new HashMap<>();

    static {
        ICON_DRAWERS.put("new_32x32", IconGenerator::drawNewIcon);
        ICON_DRAWERS.put("open_32x32", IconGenerator::drawOpenIcon);
        ICON_DRAWERS.put("save_32x32", IconGenerator::drawSaveIcon);
        ICON_DRAWERS.put("export_32x32", IconGenerator::drawExportIcon);
        ICON_DRAWERS.put("print_32x32", IconGenerator::drawPrintIcon);
        ICON_DRAWERS.put("import_32x32", IconGenerator::drawImportIcon);
        ICON_DRAWERS.put("zoom-in_32x32", IconGenerator::drawZoomInIcon);
        ICON_DRAWERS.put("zoom-out_32x32", IconGenerator::drawZoomOutIcon);
        ICON_DRAWERS.put("fit_32x32", IconGenerator::drawFitIcon);
        ICON_DRAWERS.put("load-chart_32x32", IconGenerator::drawLoadChartIcon);
        ICON_DRAWERS.put("unload-chart_32x32", IconGenerator::drawUnloadChartIcon);
        ICON_DRAWERS.put("layers_32x32", IconGenerator::drawLayersIcon);
        ICON_DRAWERS.put("property_32x32", IconGenerator::drawPropertyIcon);
        ICON_DRAWERS.put("search_32x32", IconGenerator::drawSearchIcon);
        ICON_DRAWERS.put("new-route_32x32", IconGenerator::drawNewRouteIcon);
        ICON_DRAWERS.put("edit-route_32x32", IconGenerator::drawEditRouteIcon);
        ICON_DRAWERS.put("delete_32x32", IconGenerator::drawDeleteIcon);
        ICON_DRAWERS.put("check_32x32", IconGenerator::drawCheckIcon);
        ICON_DRAWERS.put("settings_32x32", IconGenerator::drawSettingsIcon);
        ICON_DRAWERS.put("rules_32x32", IconGenerator::drawRulesIcon);
        ICON_DRAWERS.put("history_32x32", IconGenerator::drawHistoryIcon);
        ICON_DRAWERS.put("statistics_32x32", IconGenerator::drawStatisticsIcon);
        ICON_DRAWERS.put("test_32x32", IconGenerator::drawTestIcon);
        ICON_DRAWERS.put("distance_32x32", IconGenerator::drawDistanceIcon);
        ICON_DRAWERS.put("area_32x32", IconGenerator::drawAreaIcon);
        ICON_DRAWERS.put("bearing_32x32", IconGenerator::drawBearingIcon);
        ICON_DRAWERS.put("options_32x32", IconGenerator::drawOptionsIcon);
        ICON_DRAWERS.put("theme_32x32", IconGenerator::drawThemeIcon);
        ICON_DRAWERS.put("sidebar_32x32", IconGenerator::drawSidebarIcon);
        ICON_DRAWERS.put("right-tab_32x32", IconGenerator::drawRightTabIcon);
        ICON_DRAWERS.put("status-bar_32x32", IconGenerator::drawStatusBarIcon);
    }

    @Override
    public void start(Stage primaryStage) {
        generateAllIcons();
        Platform.exit();
    }

    private void generateAllIcons() {
        File outputDir = new File(OUTPUT_DIR);
        if (!outputDir.exists()) {
            outputDir.mkdirs();
        }

        Canvas canvas = new Canvas(ICON_SIZE, ICON_SIZE);
        GraphicsContext gc = canvas.getGraphicsContext2D();

        for (Map.Entry<String, IconDrawer> entry : ICON_DRAWERS.entrySet()) {
            String iconName = entry.getKey();
            IconDrawer drawer = entry.getValue();

            gc.clearRect(0, 0, ICON_SIZE, ICON_SIZE);
            gc.setStroke(STROKE_COLOR);
            gc.setFill(FILL_COLOR);
            gc.setLineWidth(2.0);

            drawer.draw(gc, ICON_SIZE);

            saveIcon(canvas, iconName + ".png");
            System.out.println("Generated: " + iconName + ".png");
        }

        System.out.println("All icons generated successfully!");
    }

    private void saveIcon(Canvas canvas, String fileName) {
        try {
            BufferedImage image = SwingFXUtils.fromFXImage(canvas.snapshot(null, null), null);
            BufferedImage transparentImage = new BufferedImage(ICON_SIZE, ICON_SIZE, BufferedImage.TYPE_INT_ARGB);
            transparentImage.getGraphics().drawImage(image, 0, 0, null);
            ImageIO.write(transparentImage, "PNG", new File(OUTPUT_DIR + fileName));
        } catch (IOException e) {
            System.err.println("Failed to save icon: " + fileName);
            e.printStackTrace();
        }
    }

    @FunctionalInterface
    private interface IconDrawer {
        void draw(GraphicsContext gc, int size);
    }

    private static int m(int size) {
        return size / 2;
    }

    private static void drawNewIcon(GraphicsContext gc, int size) {
        int s = size;
        int m = size / 2;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(m, p + 4, m, s - p - 4);
        gc.strokeLine(p + 4, m, s - p - 4, m);
    }

    private static void drawOpenIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, s/4, s - 2*p, s * 3/4 - p);
        gc.beginPath();
        gc.moveTo(p, s/4);
        gc.lineTo(s/4, s/8);
        gc.lineTo(s * 3/4, s/8);
        gc.lineTo(s * 3/4, s/4);
        gc.stroke();
    }

    private static void drawSaveIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.fillRect(p + 4, p, s/3, s/4);
        gc.strokeRect(p + 4, s/2, s - 2*p - 8, s/2 - p - 4);
    }

    private static void drawExportIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, s/2, s/2, s/2 - p);
        gc.beginPath();
        gc.moveTo(s/2 + p, s * 5/8);
        gc.lineTo(s - p, s * 5/8);
        gc.lineTo(s - p - 4, s/2 - 4);
        gc.moveTo(s - p, s * 5/8);
        gc.lineTo(s - p - 4, s * 3/4);
        gc.stroke();
        gc.fillPolygon(new double[]{s - p - 8, s - p, s - p - 8}, new double[]{s/2 - 4, s * 5/8, s * 3/4 + 4}, 3);
    }

    private static void drawPrintIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p + 4, s/2, s - 2*p - 8, s/2 - p);
        gc.strokeRect(p, s/4, s - 2*p, s/4);
        gc.beginPath();
        gc.moveTo(p + 4, s/4);
        gc.lineTo(p + 4, p + 4);
        gc.lineTo(s - p - 4, p + 4);
        gc.lineTo(s - p - 4, s/4);
        gc.stroke();
    }

    private static void drawImportIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(s/2, s/2, s/2 - p, s/2 - p);
        gc.beginPath();
        gc.moveTo(s/2 - 4, s * 5/8);
        gc.lineTo(p, s * 5/8);
        gc.lineTo(p + 4, s/2 - 4);
        gc.moveTo(p, s * 5/8);
        gc.lineTo(p + 4, s * 3/4);
        gc.stroke();
        gc.fillPolygon(new double[]{p + 8, p, p + 8}, new double[]{s/2 - 4, s * 5/8, s * 3/4 + 4}, 3);
    }

    private static void drawZoomInIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int r = size / 3;
        gc.strokeOval(p, p, r * 2, r * 2);
        gc.strokeLine(p + r + r/2, p + r + r/2, s - p, s - p);
        int cx = p + r;
        int cy = p + r;
        gc.strokeLine(cx - r/2, cy, cx + r/2, cy);
        gc.strokeLine(cx, cy - r/2, cx, cy + r/2);
    }

    private static void drawZoomOutIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int r = size / 3;
        gc.strokeOval(p, p, r * 2, r * 2);
        gc.strokeLine(p + r + r/2, p + r + r/2, s - p, s - p);
        int cx = p + r;
        int cy = p + r;
        gc.strokeLine(cx - r/2, cy, cx + r/2, cy);
    }

    private static void drawFitIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p, p, p + s/4, p + s/4);
        gc.strokeLine(s - p, p, s - p - s/4, p + s/4);
        gc.strokeLine(p, s - p, p + s/4, s - p - s/4);
        gc.strokeLine(s - p, s - p, s - p - s/4, s - p - s/4);
    }

    private static void drawLoadChartIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        gc.strokeLine(m, p, m, s * 5/8);
        gc.strokeLine(m - s/6, s/2 - s/8, m, s * 5/8);
        gc.strokeLine(m + s/6, s/2 - s/8, m, s * 5/8);
        gc.strokeRect(p, s * 3/4, s - 2*p, s/8);
    }

    private static void drawUnloadChartIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        gc.strokeLine(m, s - p, m, s/4);
        gc.strokeLine(m - s/6, s/2 + s/8, m, s/4);
        gc.strokeLine(m + s/6, s/2 + s/8, m, s/4);
        gc.strokeRect(p, p, s - 2*p, s/8);
    }

    private static void drawLayersIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        gc.beginPath();
        gc.moveTo(m, p);
        gc.lineTo(p, m - p);
        gc.lineTo(m, m);
        gc.lineTo(s - p, m - p);
        gc.closePath();
        gc.stroke();
        gc.beginPath();
        gc.moveTo(m, m);
        gc.lineTo(p, m + p);
        gc.lineTo(m, s - p);
        gc.lineTo(s - p, m + p);
        gc.closePath();
        gc.stroke();
    }

    private static void drawPropertyIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p * 2, p * 3, s - p * 2, p * 3);
        gc.strokeLine(p * 2, m(s), s - p * 2, m(s));
        gc.strokeLine(p * 2, s - p * 3, s/2, s - p * 3);
    }

    private static void drawSearchIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int r = size / 3;
        gc.strokeOval(p, p, r * 2, r * 2);
        gc.strokeLine(p + r + r/2, p + r + r/2, s - p, s - p);
    }

    private static void drawNewRouteIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.beginPath();
        gc.moveTo(p, s * 3/4);
        gc.lineTo(s/4, p * 2);
        gc.lineTo(s/2, s/2);
        gc.lineTo(s * 3/4, p * 2);
        gc.lineTo(s - p, s * 3/4);
        gc.stroke();
        gc.fillOval(p - 2, s * 3/4 - 2, 4, 4);
        gc.fillOval(s - p - 2, s * 3/4 - 2, 4, 4);
    }

    private static void drawEditRouteIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeLine(s - p, p, s - p * 2, p * 2);
        gc.strokeLine(p * 2, s - p * 2, p, s - p);
        gc.strokeLine(p * 2, s - p * 2, s - p * 2, p * 2);
        gc.setFill(STROKE_COLOR);
        gc.fillPolygon(new double[]{s - p, s - p * 2, s - p * 3}, new double[]{p, p * 2, p}, 3);
    }

    private static void drawDeleteIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        gc.strokeOval(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p + s/4, p + s/4, s - p - s/4, s - p - s/4);
        gc.strokeLine(s - p - s/4, p + s/4, p + s/4, s - p - s/4);
    }

    private static void drawCheckIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeLine(p, m(s), s/3, s - p);
        gc.strokeLine(s/3, s - p, s - p, p);
    }

    private static void drawSettingsIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        int r = size / 4;
        gc.strokeOval(m - r, m - r, r * 2, r * 2);
        gc.strokeLine(m, p, m, p + s/8);
        gc.strokeLine(m, s - p - s/8, m, s - p);
        gc.strokeLine(p, m, p + s/8, m);
        gc.strokeLine(s - p - s/8, m, s - p, m);
        gc.strokeLine(p + s/8, p + s/8, p + s/4, p + s/4);
        gc.strokeLine(s - p - s/4, s - p - s/4, s - p - s/8, s - p - s/8);
        gc.strokeLine(s - p - s/8, p + s/8, s - p - s/4, p + s/4);
        gc.strokeLine(p + s/4, s - p - s/4, p + s/8, s - p - s/8);
    }

    private static void drawRulesIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p * 2, p * 3, s - p * 2, p * 3);
        gc.strokeLine(p * 2, m(s), s - p * 2, m(s));
        gc.strokeLine(p * 2, s - p * 3, s - p * 2, s - p * 3);
    }

    private static void drawHistoryIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        int r = s/2 - p;
        gc.strokeOval(p, p, r * 2, r * 2);
        gc.strokeLine(m, m - r/2, m, m);
        gc.strokeLine(m, m, m + r/3, m + r/4);
        gc.beginPath();
        gc.moveTo(p, m);
        gc.lineTo(p + s/8, m - s/8);
        gc.lineTo(p + s/8, m + s/8);
        gc.closePath();
        gc.fill();
    }

    private static void drawStatisticsIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeLine(p, s - p, s - p, s - p);
        gc.fillRect(p + s/8, s/2, s/4, s/2 - p);
        gc.fillRect(m(s) - s/8, p + s/4, s/4, s/2 - p + s/8);
        gc.fillRect(s - p - s/3, s/3, s/4, s * 2/3 - p);
    }

    private static void drawTestIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        int r = s/2 - p;
        gc.strokeOval(p, p, r * 2, r * 2);
        gc.strokeLine(p + s/4, m, m - s/8, m + s/4);
        gc.strokeLine(m - s/8, m + s/4, s - p - s/4, p + s/4);
    }

    private static void drawDistanceIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        gc.strokeLine(p, m, s - p, m);
        gc.strokeLine(p, m - s/8, p, m + s/8);
        gc.strokeLine(s - p, m - s/8, s - p, m + s/8);
        gc.strokeLine(m - s/8, m - s/6, m + s/8, m - s/6);
        gc.strokeLine(m - s/8, m + s/6, m + s/8, m + s/6);
    }

    private static void drawAreaIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p, p, s - p, s - p);
    }

    private static void drawBearingIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        int r = s/2 - p;
        gc.strokeOval(p, p, r * 2, r * 2);
        gc.strokeLine(m, m, m, p + s/8);
        gc.strokeLine(m, m, s - p - s/8, m);
        gc.fillPolygon(new double[]{m, m - s/8, m + s/8}, new double[]{p, p + s/4, p + s/4}, 3);
    }

    private static void drawOptionsIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeLine(p, p * 3, s * 2/3, p * 3);
        gc.strokeLine(p, m(s), s * 2/3, m(s));
        gc.strokeLine(p, s - p * 3, s * 2/3, s - p * 3);
        gc.fillOval(s/2, p * 2, s/4, s/8);
        gc.fillOval(s/3, m(s) - s/16, s/4, s/8);
        gc.fillOval(s/2, s - p * 4, s/4, s/8);
    }

    private static void drawThemeIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        int m = size / 2;
        int r = s/2 - p;
        gc.strokeOval(p, p, r * 2, r * 2);
        gc.fillArc(p, p, r * 2, r * 2, 90, 180, javafx.scene.shape.ArcType.CHORD);
    }

    private static void drawSidebarIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p + s/4, p, p + s/4, s - p);
    }

    private static void drawRightTabIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p, p + s/4, s - p, p + s/4);
    }
    
    private static void drawStatusBarIcon(GraphicsContext gc, int size) {
        int s = size;
        int p = size / 8;
        gc.strokeRect(p, p, s - 2*p, s - 2*p);
        gc.strokeLine(p, s - p - s/4, s - p, s - p - s/4);
    }
}
