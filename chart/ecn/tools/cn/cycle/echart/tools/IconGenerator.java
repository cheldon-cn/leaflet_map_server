package cn.cycle.echart.tools;

import javax.imageio.ImageIO;
import java.awt.*;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

/**
 * 图标生成工具。
 * 
 * <p>生成应用程序所需的PNG图标资源。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class IconGenerator {

    private static final Map<String, Color> ICON_COLORS = new HashMap<>();
    
    static {
        ICON_COLORS.put("alarm", new Color(220, 53, 69));
        ICON_COLORS.put("alarm_small", new Color(220, 53, 69));
        ICON_COLORS.put("layers", new Color(0, 123, 255));
        ICON_COLORS.put("layers_small", new Color(0, 123, 255));
        ICON_COLORS.put("search", new Color(108, 117, 125));
        ICON_COLORS.put("route", new Color(40, 167, 69));
        ICON_COLORS.put("ais", new Color(23, 162, 184));
        ICON_COLORS.put("measure", new Color(111, 66, 193));
        ICON_COLORS.put("settings", new Color(108, 117, 125));
        ICON_COLORS.put("terminal", new Color(33, 37, 41));
        ICON_COLORS.put("property", new Color(0, 123, 255));
        ICON_COLORS.put("log", new Color(40, 167, 69));
        ICON_COLORS.put("light", new Color(255, 193, 7));
        ICON_COLORS.put("dark", new Color(52, 58, 64));
        ICON_COLORS.put("compass", new Color(255, 128, 0));
        ICON_COLORS.put("location", new Color(220, 53, 69));
        ICON_COLORS.put("back", new Color(108, 117, 125));
        ICON_COLORS.put("forward", new Color(108, 117, 125));
    }

    public static void main(String[] args) {
        String baseDir = args.length > 0 ? args[0] : "src/main/resources";
        
        generateUIIcons(baseDir + "/icons");
        generateThemeIcons(baseDir + "/themes/icons");
        
        System.out.println("图标生成完成！");
    }

    private static void generateUIIcons(String outputDir) {
        File dir = new File(outputDir);
        if (!dir.exists()) {
            dir.mkdirs();
        }
        
        String[] smallIcons = {"alarm_small", "layers_small"};
        for (String name : smallIcons) {
            generateIcon(outputDir, name, 12);
        }
        
        String[] standardIcons = {
            "layers", "search", "route", "alarm", "ais", 
            "measure", "settings", "terminal", "property", "log",
            "compass", "location", "back", "forward"
        };
        for (String name : standardIcons) {
            generateIcon(outputDir, name, 24);
        }
    }

    private static void generateThemeIcons(String outputDir) {
        File dir = new File(outputDir);
        if (!dir.exists()) {
            dir.mkdirs();
        }
        
        generateIcon(outputDir, "light", 24);
        generateIcon(outputDir, "dark", 24);
    }

    private static void generateIcon(String outputDir, String name, int size) {
        try {
            BufferedImage image = new BufferedImage(size, size, BufferedImage.TYPE_INT_ARGB);
            Graphics2D g2d = image.createGraphics();
            
            g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
            g2d.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
            
            Color color = ICON_COLORS.getOrDefault(name, Color.GRAY);
            
            drawIcon(g2d, name, color, size);
            
            g2d.dispose();
            
            File outputFile = new File(outputDir, name + ".png");
            ImageIO.write(image, "PNG", outputFile);
            System.out.println("生成图标: " + outputFile.getAbsolutePath());
            
        } catch (IOException e) {
            System.err.println("生成图标失败: " + name + " - " + e.getMessage());
        }
    }

    private static void drawIcon(Graphics2D g2d, String name, Color color, int size) {
        g2d.setColor(color);
        
        int padding = size / 6;
        int iconSize = size - padding * 2;
        
        if (name.contains("alarm")) {
            drawAlarmIcon(g2d, padding, padding, iconSize);
        } else if (name.contains("layers")) {
            drawLayersIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("search")) {
            drawSearchIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("route")) {
            drawRouteIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("ais")) {
            drawAisIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("measure")) {
            drawMeasureIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("settings")) {
            drawSettingsIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("terminal")) {
            drawTerminalIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("property")) {
            drawPropertyIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("log")) {
            drawLogIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("light")) {
            drawLightIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("dark")) {
            drawDarkIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("compass")) {
            drawCompassIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("location")) {
            drawLocationIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("back")) {
            drawBackIcon(g2d, padding, padding, iconSize);
        } else if (name.equals("forward")) {
            drawForwardIcon(g2d, padding, padding, iconSize);
        } else {
            g2d.fillOval(padding, padding, iconSize, iconSize);
        }
    }

    private static void drawAlarmIcon(Graphics2D g2d, int x, int y, int size) {
        int bellWidth = size * 3 / 4;
        int bellHeight = size * 2 / 3;
        int bellX = x + (size - bellWidth) / 2;
        int bellY = y + size / 6;
        
        g2d.fillArc(bellX, bellY, bellWidth, bellHeight * 2, 0, 180);
        
        int clapperSize = size / 5;
        int clapperX = x + (size - clapperSize) / 2;
        int clapperY = y + size - clapperSize - size / 8;
        g2d.fillOval(clapperX, clapperY, clapperSize, clapperSize);
    }

    private static void drawLayersIcon(Graphics2D g2d, int x, int y, int size) {
        int layerHeight = size / 4;
        int offset = size / 8;
        
        for (int i = 2; i >= 0; i--) {
            int layerY = y + i * offset;
            g2d.fillRect(x, layerY, size, layerHeight);
        }
    }

    private static void drawSearchIcon(Graphics2D g2d, int x, int y, int size) {
        int circleSize = size * 2 / 3;
        g2d.setStroke(new BasicStroke(size / 8));
        g2d.drawOval(x, y, circleSize, circleSize);
        
        int handleX = x + circleSize - size / 8;
        int handleY = y + circleSize - size / 8;
        int handleLen = size / 3;
        g2d.drawLine(handleX, handleY, handleX + handleLen, handleY + handleLen);
    }

    private static void drawRouteIcon(Graphics2D g2d, int x, int y, int size) {
        g2d.setStroke(new BasicStroke(size / 6, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        
        int dotSize = size / 4;
        g2d.fillOval(x, y, dotSize, dotSize);
        g2d.fillOval(x + size - dotSize, y + size - dotSize, dotSize, dotSize);
        
        g2d.drawLine(x + dotSize / 2, y + dotSize / 2, 
                    x + size - dotSize / 2, y + size - dotSize / 2);
    }

    private static void drawAisIcon(Graphics2D g2d, int x, int y, int size) {
        int[] xPoints = {
            x + size / 2,
            x + size,
            x + size / 2,
            x
        };
        int[] yPoints = {
            y,
            y + size * 3 / 4,
            y + size,
            y + size * 3 / 4
        };
        g2d.fillPolygon(xPoints, yPoints, 4);
    }

    private static void drawMeasureIcon(Graphics2D g2d, int x, int y, int size) {
        g2d.setStroke(new BasicStroke(size / 6));
        
        g2d.drawLine(x, y + size / 2, x + size, y + size / 2);
        
        int tickHeight = size / 4;
        int tickSpacing = size / 4;
        for (int i = 0; i <= 4; i++) {
            int tickX = x + i * tickSpacing;
            int tickH = (i % 2 == 0) ? tickHeight : tickHeight / 2;
            g2d.drawLine(tickX, y + size / 2 - tickH, tickX, y + size / 2 + tickH);
        }
    }

    private static void drawSettingsIcon(Graphics2D g2d, int x, int y, int size) {
        int centerX = x + size / 2;
        int centerY = y + size / 2;
        int outerRadius = size / 2 - 2;
        int innerRadius = size / 4;
        int teeth = 8;
        
        int[] xPoints = new int[teeth * 2];
        int[] yPoints = new int[teeth * 2];
        
        for (int i = 0; i < teeth; i++) {
            double angle1 = 2 * Math.PI * i / teeth - Math.PI / 2;
            double angle2 = 2 * Math.PI * (i + 0.5) / teeth - Math.PI / 2;
            
            xPoints[i * 2] = centerX + (int)(outerRadius * Math.cos(angle1));
            yPoints[i * 2] = centerY + (int)(outerRadius * Math.sin(angle1));
            xPoints[i * 2 + 1] = centerX + (int)(innerRadius * Math.cos(angle2));
            yPoints[i * 2 + 1] = centerY + (int)(innerRadius * Math.sin(angle2));
        }
        
        g2d.fillPolygon(xPoints, yPoints, teeth * 2);
    }

    private static void drawTerminalIcon(Graphics2D g2d, int x, int y, int size) {
        g2d.setStroke(new BasicStroke(size / 8));
        
        g2d.drawLine(x + size / 8, y + size / 4, x + size / 3, y + size / 2);
        g2d.drawLine(x + size / 8, y + size * 3 / 4, x + size / 3, y + size / 2);
        
        g2d.drawLine(x + size / 2, y + size * 3 / 4, x + size - size / 8, y + size * 3 / 4);
    }

    private static void drawPropertyIcon(Graphics2D g2d, int x, int y, int size) {
        int lineSpacing = size / 4;
        int lineHeight = size / 8;
        
        for (int i = 0; i < 3; i++) {
            int lineY = y + i * lineSpacing + size / 8;
            int lineWidth = (i == 0) ? size : size * 2 / 3;
            g2d.fillRect(x, lineY, lineWidth, lineHeight);
        }
    }

    private static void drawLogIcon(Graphics2D g2d, int x, int y, int size) {
        g2d.fillRect(x, y, size, size);
        
        g2d.setColor(Color.WHITE);
        int lineSpacing = size / 4;
        int lineHeight = size / 10;
        
        for (int i = 0; i < 3; i++) {
            int lineY = y + size / 6 + i * lineSpacing;
            g2d.fillRect(x + size / 8, lineY, size * 3 / 4, lineHeight);
        }
    }

    private static void drawLightIcon(Graphics2D g2d, int x, int y, int size) {
        int centerX = x + size / 2;
        int centerY = y + size / 2;
        int radius = size / 3;
        
        g2d.fillOval(centerX - radius, centerY - radius, radius * 2, radius * 2);
        
        g2d.setStroke(new BasicStroke(size / 10));
        int rayLen = size / 6;
        int rayStart = radius + size / 10;
        
        for (int i = 0; i < 8; i++) {
            double angle = 2 * Math.PI * i / 8;
            int startX = centerX + (int)(rayStart * Math.cos(angle));
            int startY = centerY + (int)(rayStart * Math.sin(angle));
            int endX = centerX + (int)((rayStart + rayLen) * Math.cos(angle));
            int endY = centerY + (int)((rayStart + rayLen) * Math.sin(angle));
            g2d.drawLine(startX, startY, endX, endY);
        }
    }

    private static void drawDarkIcon(Graphics2D g2d, int x, int y, int size) {
        int centerX = x + size / 2;
        int centerY = y + size / 2;
        int radius = size / 3;
        
        g2d.fillArc(centerX - radius, centerY - radius, radius * 2, radius * 2, 90, 270);
        
        int starRadius = size / 6;
        int starX = centerX + radius / 2;
        int starY = centerY - radius / 2;
        g2d.fillOval(starX, starY, starRadius, starRadius);
    }

    private static void drawCompassIcon(Graphics2D g2d, int x, int y, int size) {
        int centerX = x + size / 2;
        int centerY = y + size / 2;
        int radius = size / 2 - 2;
        
        g2d.setStroke(new BasicStroke(size / 12));
        g2d.drawOval(x + 2, y + 2, size - 4, size - 4);
        
        int needleLen = size / 3;
        int[] xPoints = {centerX, centerX - size / 8, centerX + size / 8};
        int[] yPoints = {y + 2, centerY, centerY};
        g2d.fillPolygon(xPoints, yPoints, 3);
        
        int[] xPoints2 = {centerX, centerX - size / 8, centerX + size / 8};
        int[] yPoints2 = {y + size - 2, centerY, centerY};
        g2d.fillPolygon(xPoints2, yPoints2, 3);
        
        g2d.fillOval(centerX - size / 10, centerY - size / 10, size / 5, size / 5);
    }

    private static void drawLocationIcon(Graphics2D g2d, int x, int y, int size) {
        int[] xPoints = {
            x + size / 2,
            x + size * 3 / 4,
            x + size / 2,
            x + size / 4
        };
        int[] yPoints = {
            y,
            y + size * 2 / 3,
            y + size,
            y + size * 2 / 3
        };
        g2d.fillPolygon(xPoints, yPoints, 4);
        
        int dotRadius = size / 6;
        g2d.setColor(Color.WHITE);
        g2d.fillOval(x + size / 2 - dotRadius, y + size / 3 - dotRadius / 2, 
                    dotRadius * 2, dotRadius * 2);
    }

    private static void drawBackIcon(Graphics2D g2d, int x, int y, int size) {
        g2d.setStroke(new BasicStroke(size / 6, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        
        int arrowX = x + size * 2 / 3;
        int arrowY = y + size / 2;
        int arrowLen = size * 2 / 3;
        
        g2d.drawLine(arrowX, arrowY, arrowX - arrowLen, arrowY);
        g2d.drawLine(arrowX - arrowLen, arrowY, x + size / 4, y + size / 4);
        g2d.drawLine(arrowX - arrowLen, arrowY, x + size / 4, y + size * 3 / 4);
    }

    private static void drawForwardIcon(Graphics2D g2d, int x, int y, int size) {
        g2d.setStroke(new BasicStroke(size / 6, BasicStroke.CAP_ROUND, BasicStroke.JOIN_ROUND));
        
        int arrowX = x + size / 3;
        int arrowY = y + size / 2;
        int arrowLen = size * 2 / 3;
        
        g2d.drawLine(arrowX, arrowY, arrowX + arrowLen, arrowY);
        g2d.drawLine(arrowX + arrowLen, arrowY, x + size * 3 / 4, y + size / 4);
        g2d.drawLine(arrowX + arrowLen, arrowY, x + size * 3 / 4, y + size * 3 / 4);
    }
}
