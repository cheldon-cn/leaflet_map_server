package cn.cycle.echart.workspace;

import cn.cycle.echart.ais.AISTarget;
import cn.cycle.echart.route.Route;
import cn.cycle.echart.route.Waypoint;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 * 工作区持久化器。
 * 
 * <p>负责工作区的保存和加载。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class WorkspacePersister {

    private static final DateTimeFormatter DATE_FORMATTER = DateTimeFormatter.ISO_LOCAL_DATE_TIME;

    public WorkspacePersister() {
    }

    /**
     * 保存工作区。
     * 
     * @param workspace 工作区
     * @param file 文件
     * @throws IOException IO异常
     */
    public void save(Workspace workspace, File file) throws IOException {
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(file))) {
            writer.write("[Workspace]");
            writer.newLine();
            
            writer.write("id=" + workspace.getId());
            writer.newLine();
            writer.write("name=" + workspace.getName());
            writer.newLine();
            writer.write("description=" + workspace.getDescription());
            writer.newLine();
            writer.write("created=" + workspace.getCreatedTime().format(DATE_FORMATTER));
            writer.newLine();
            writer.write("modified=" + workspace.getModifiedTime().format(DATE_FORMATTER));
            writer.newLine();
            writer.newLine();
            
            saveConfig(workspace.getConfig(), writer);
            writer.newLine();
            
            saveCharts(workspace.getLoadedCharts(), writer);
            writer.newLine();
            
            saveRoutes(workspace.getRoutes(), writer);
            writer.newLine();
            
            saveProperties(workspace.getProperties(), writer);
        }
    }

    private void saveConfig(WorkspaceConfig config, BufferedWriter writer) throws IOException {
        writer.write("[Config]");
        writer.newLine();
        
        writer.write("centerLat=" + config.getCenterLatitude());
        writer.newLine();
        writer.write("centerLon=" + config.getCenterLongitude());
        writer.newLine();
        writer.write("zoom=" + config.getZoomLevel());
        writer.newLine();
        writer.write("rotation=" + config.getRotation());
        writer.newLine();
        writer.write("showAis=" + config.isShowAisLayer());
        writer.newLine();
        writer.write("showRoute=" + config.isShowRouteLayer());
        writer.newLine();
        writer.write("showAlarm=" + config.isShowAlarmLayer());
        writer.newLine();
        writer.write("showChart=" + config.isShowChartLayer());
        writer.newLine();
        writer.write("displayMode=" + config.getDisplayMode());
        writer.newLine();
        writer.write("colorScheme=" + config.getColorScheme());
        writer.newLine();
    }

    private void saveCharts(List<String> charts, BufferedWriter writer) throws IOException {
        writer.write("[Charts]");
        writer.newLine();
        writer.write("count=" + charts.size());
        writer.newLine();
        
        for (int i = 0; i < charts.size(); i++) {
            writer.write("chart" + i + "=" + charts.get(i));
            writer.newLine();
        }
    }

    private void saveRoutes(List<Route> routes, BufferedWriter writer) throws IOException {
        writer.write("[Routes]");
        writer.newLine();
        writer.write("count=" + routes.size());
        writer.newLine();
        
        for (int i = 0; i < routes.size(); i++) {
            Route route = routes.get(i);
            writer.write("[Route" + i + "]");
            writer.newLine();
            writer.write("id=" + route.getId());
            writer.newLine();
            writer.write("name=" + route.getName());
            writer.newLine();
            writer.write("status=" + route.getStatus());
            writer.newLine();
            writer.write("waypointCount=" + route.getWaypointCount());
            writer.newLine();
            
            List<Waypoint> waypoints = route.getWaypoints();
            for (int j = 0; j < waypoints.size(); j++) {
                Waypoint wp = waypoints.get(j);
                writer.write("wp" + j + "=" + wp.getLatitude() + "," + wp.getLongitude() + "," + wp.getName());
                writer.newLine();
            }
        }
    }

    private void saveProperties(Map<String, Object> properties, BufferedWriter writer) throws IOException {
        writer.write("[Properties]");
        writer.newLine();
        writer.write("count=" + properties.size());
        writer.newLine();
        
        int i = 0;
        for (Map.Entry<String, Object> entry : properties.entrySet()) {
            writer.write("prop" + i + "=" + entry.getKey() + "=" + String.valueOf(entry.getValue()));
            writer.newLine();
            i++;
        }
    }

    /**
     * 加载工作区。
     * 
     * @param file 文件
     * @return 工作区
     * @throws IOException IO异常
     */
    public Workspace load(File file) throws IOException {
        Workspace workspace = new Workspace();
        Map<String, String> section = new HashMap<>();
        String currentSection = "";
        
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                
                if (line.isEmpty()) {
                    continue;
                }
                
                if (line.startsWith("[") && line.endsWith("]")) {
                    if (!currentSection.isEmpty()) {
                        processSection(currentSection, section, workspace);
                    }
                    currentSection = line.substring(1, line.length() - 1);
                    section.clear();
                } else if (line.contains("=")) {
                    int idx = line.indexOf('=');
                    String key = line.substring(0, idx);
                    String value = line.substring(idx + 1);
                    section.put(key, value);
                }
            }
            
            if (!currentSection.isEmpty()) {
                processSection(currentSection, section, workspace);
            }
        }
        
        return workspace;
    }

    private void processSection(String sectionName, Map<String, String> section, Workspace workspace) {
        switch (sectionName) {
            case "Workspace":
                processWorkspaceSection(section, workspace);
                break;
            case "Config":
                processConfigSection(section, workspace.getConfig());
                break;
            case "Charts":
                processChartsSection(section, workspace);
                break;
            case "Properties":
                processPropertiesSection(section, workspace);
                break;
            default:
                if (sectionName.startsWith("Route")) {
                    processRouteSection(sectionName, section, workspace);
                }
                break;
        }
    }

    private void processWorkspaceSection(Map<String, String> section, Workspace workspace) {
        if (section.containsKey("name")) {
            workspace.setName(section.get("name"));
        }
        if (section.containsKey("description")) {
            workspace.setDescription(section.get("description"));
        }
        if (section.containsKey("created")) {
            try {
                workspace.setCreatedTime(LocalDateTime.parse(section.get("created"), DATE_FORMATTER));
            } catch (Exception e) {
            }
        }
        if (section.containsKey("modified")) {
            try {
                workspace.setModifiedTime(LocalDateTime.parse(section.get("modified"), DATE_FORMATTER));
            } catch (Exception e) {
            }
        }
    }

    private void processConfigSection(Map<String, String> section, WorkspaceConfig config) {
        if (section.containsKey("centerLat")) {
            config.setCenterLatitude(Double.parseDouble(section.get("centerLat")));
        }
        if (section.containsKey("centerLon")) {
            config.setCenterLongitude(Double.parseDouble(section.get("centerLon")));
        }
        if (section.containsKey("zoom")) {
            config.setZoomLevel(Double.parseDouble(section.get("zoom")));
        }
        if (section.containsKey("rotation")) {
            config.setRotation(Double.parseDouble(section.get("rotation")));
        }
        if (section.containsKey("showAis")) {
            config.setShowAisLayer(Boolean.parseBoolean(section.get("showAis")));
        }
        if (section.containsKey("showRoute")) {
            config.setShowRouteLayer(Boolean.parseBoolean(section.get("showRoute")));
        }
        if (section.containsKey("showAlarm")) {
            config.setShowAlarmLayer(Boolean.parseBoolean(section.get("showAlarm")));
        }
        if (section.containsKey("showChart")) {
            config.setShowChartLayer(Boolean.parseBoolean(section.get("showChart")));
        }
        if (section.containsKey("displayMode")) {
            config.setDisplayMode(section.get("displayMode"));
        }
        if (section.containsKey("colorScheme")) {
            config.setColorScheme(section.get("colorScheme"));
        }
    }

    private void processChartsSection(Map<String, String> section, Workspace workspace) {
        int count = section.containsKey("count") ? Integer.parseInt(section.get("count")) : 0;
        
        for (int i = 0; i < count; i++) {
            String key = "chart" + i;
            if (section.containsKey(key)) {
                workspace.addChart(section.get(key));
            }
        }
    }

    private void processRouteSection(String sectionName, Map<String, String> section, Workspace workspace) {
        Route route = new Route();
        
        if (section.containsKey("name")) {
            route.setName(section.get("name"));
        }
        if (section.containsKey("status")) {
            route.setStatus(Integer.parseInt(section.get("status")));
        }
        
        int waypointCount = section.containsKey("waypointCount") ? 
                Integer.parseInt(section.get("waypointCount")) : 0;
        
        for (int i = 0; i < waypointCount; i++) {
            String key = "wp" + i;
            if (section.containsKey(key)) {
                String[] parts = section.get(key).split(",");
                if (parts.length >= 2) {
                    double lat = Double.parseDouble(parts[0]);
                    double lon = Double.parseDouble(parts[1]);
                    Waypoint wp = new Waypoint(lat, lon);
                    if (parts.length >= 3) {
                        wp.setName(parts[2]);
                    }
                    route.addWaypoint(wp);
                }
            }
        }
        
        workspace.addRoute(route);
    }

    private void processPropertiesSection(Map<String, String> section, Workspace workspace) {
        int count = section.containsKey("count") ? Integer.parseInt(section.get("count")) : 0;
        
        for (int i = 0; i < count; i++) {
            String key = "prop" + i;
            if (section.containsKey(key)) {
                String[] parts = section.get(key).split("=", 2);
                if (parts.length == 2) {
                    workspace.setProperty(parts[0], parts[1]);
                }
            }
        }
    }
}
