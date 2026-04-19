package cn.cycle.echart.route;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.List;

/**
 * 航线导出器。
 * 
 * <p>将航线数据导出到文件。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RouteExporter {

    private final RouteCalculator calculator;

    public RouteExporter() {
        this.calculator = new RouteCalculator();
    }

    /**
     * 导出航线到文件。
     * 
     * @param route 航线
     * @param file 文件
     * @throws IOException IO异常
     */
    public void exportToFile(Route route, File file) throws IOException {
        String name = file.getName();
        int dotIndex = name.lastIndexOf('.');
        String extension = dotIndex > 0 ? name.substring(dotIndex + 1).toLowerCase() : "";
        
        switch (extension) {
            case "csv":
                exportToCsv(route, file);
                break;
            case "txt":
                exportToTxt(route, file);
                break;
            default:
                throw new IOException("不支持的文件格式: " + extension);
        }
    }

    /**
     * 导出航线到CSV文件。
     * 
     * @param route 航线
     * @param file CSV文件
     * @throws IOException IO异常
     */
    public void exportToCsv(Route route, File file) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(file))) {
            writer.write("Latitude,Longitude,Name,ArrivalRadius,TurnRadius,Description");
            writer.newLine();
            
            List<Waypoint> waypoints = route.getWaypoints();
            for (int i = 0; i < waypoints.size(); i++) {
                Waypoint wp = waypoints.get(i);
                writer.write(String.format("%.6f,%.6f,%s,%.2f,%.2f,%s",
                        wp.getLatitude(),
                        wp.getLongitude(),
                        escapeCsv(wp.getName()),
                        wp.getArrivalRadius(),
                        wp.getTurnRadius(),
                        escapeCsv(wp.getDescription())));
                writer.newLine();
            }
        }
    }

    /**
     * 导出航线到TXT文件。
     * 
     * @param route 航线
     * @param file TXT文件
     * @throws IOException IO异常
     */
    public void exportToTxt(Route route, File file) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(file))) {
            writer.write("# Route: " + route.getName());
            writer.newLine();
            writer.write("# Total Distance: " + String.format("%.2f nm", route.getTotalDistance()));
            writer.newLine();
            writer.write("# Waypoints: " + route.getWaypointCount());
            writer.newLine();
            writer.write("# Exported: " + new SimpleDateFormat("yyyy-MM-dd HH:mm:ss").format(new Date()));
            writer.newLine();
            writer.newLine();
            
            List<Waypoint> waypoints = route.getWaypoints();
            for (int i = 0; i < waypoints.size(); i++) {
                Waypoint wp = waypoints.get(i);
                writer.write(String.format("%.6f %.6f %s",
                        wp.getLatitude(),
                        wp.getLongitude(),
                        wp.getName()));
                writer.newLine();
            }
        }
    }

    /**
     * 导出航线报告。
     * 
     * @param route 航线
     * @param file 报告文件
     * @throws IOException IO异常
     */
    public void exportReport(Route route, File file) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(file))) {
            writer.write("=====================================");
            writer.newLine();
            writer.write("航线报告: " + route.getName());
            writer.newLine();
            writer.write("=====================================");
            writer.newLine();
            writer.newLine();
            
            writer.write("航线ID: " + route.getId());
            writer.newLine();
            writer.write("航点数量: " + route.getWaypointCount());
            writer.newLine();
            writer.write("总距离: " + String.format("%.2f nm", route.getTotalDistance()));
            writer.newLine();
            writer.write("创建时间: " + route.getCreatedTime());
            writer.newLine();
            writer.write("修改时间: " + route.getModifiedTime());
            writer.newLine();
            writer.newLine();
            
            writer.write("航点列表:");
            writer.newLine();
            writer.write("-------------------------------------");
            writer.newLine();
            
            List<Waypoint> waypoints = route.getWaypoints();
            for (int i = 0; i < waypoints.size(); i++) {
                Waypoint wp = waypoints.get(i);
                writer.write(String.format("%d. %s", i + 1, wp.getName().isEmpty() ? "航点" + (i + 1) : wp.getName()));
                writer.newLine();
                writer.write(String.format("   位置: %.4f°N, %.4f°E", wp.getLatitude(), wp.getLongitude()));
                writer.newLine();
                writer.write(String.format("   到达半径: %.2f nm", wp.getArrivalRadius()));
                writer.newLine();
                writer.write(String.format("   转弯半径: %.2f nm", wp.getTurnRadius()));
                writer.newLine();
                
                if (i < waypoints.size() - 1) {
                    Waypoint next = waypoints.get(i + 1);
                    double distance = calculator.calculateLegDistance(wp, next);
                    double bearing = calculator.calculateLegBearing(wp, next);
                    writer.write(String.format("   下一航段: %.2f nm, 方位 %.1f°", distance, bearing));
                    writer.newLine();
                }
                
                writer.newLine();
            }
        }
    }

    private String escapeCsv(String value) {
        if (value == null) {
            return "";
        }
        if (value.contains(",") || value.contains("\"") || value.contains("\n")) {
            return "\"" + value.replace("\"", "\"\"") + "\"";
        }
        return value;
    }
}
