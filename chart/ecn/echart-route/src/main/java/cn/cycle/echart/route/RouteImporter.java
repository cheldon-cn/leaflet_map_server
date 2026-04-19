package cn.cycle.echart.route;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

/**
 * 航线导入器。
 * 
 * <p>从文件导入航线数据。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class RouteImporter {

    public RouteImporter() {
    }

    /**
     * 从文件导入航线。
     * 
     * @param file 文件
     * @return 航线
     * @throws IOException IO异常
     */
    public Route importFromFile(File file) throws IOException {
        String name = file.getName();
        int dotIndex = name.lastIndexOf('.');
        String extension = dotIndex > 0 ? name.substring(dotIndex + 1).toLowerCase() : "";
        
        switch (extension) {
            case "csv":
                return importFromCsv(file);
            case "txt":
                return importFromTxt(file);
            default:
                throw new IOException("不支持的文件格式: " + extension);
        }
    }

    /**
     * 从CSV文件导入航线。
     * 
     * @param file CSV文件
     * @return 航线
     * @throws IOException IO异常
     */
    public Route importFromCsv(File file) throws IOException {
        Route route = new Route(file.getName().replaceAll("\\.csv$", ""));
        
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            boolean firstLine = true;
            
            while ((line = reader.readLine()) != null) {
                if (firstLine) {
                    firstLine = false;
                    if (line.toLowerCase().contains("latitude") || line.toLowerCase().contains("lat")) {
                        continue;
                    }
                }
                
                String[] parts = line.split(",");
                if (parts.length >= 2) {
                    try {
                        double lat = Double.parseDouble(parts[0].trim());
                        double lon = Double.parseDouble(parts[1].trim());
                        
                        Waypoint wp = new Waypoint(lat, lon);
                        
                        if (parts.length >= 3) {
                            wp.setName(parts[2].trim());
                        }
                        if (parts.length >= 4) {
                            wp.setArrivalRadius(Double.parseDouble(parts[3].trim()));
                        }
                        
                        route.addWaypoint(wp);
                    } catch (NumberFormatException e) {
                        continue;
                    }
                }
            }
        }
        
        return route;
    }

    /**
     * 从TXT文件导入航线。
     * 
     * @param file TXT文件
     * @return 航线
     * @throws IOException IO异常
     */
    public Route importFromTxt(File file) throws IOException {
        Route route = new Route(file.getName().replaceAll("\\.txt$", ""));
        
        try (BufferedReader reader = new BufferedReader(new FileReader(file))) {
            String line;
            
            while ((line = reader.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#")) {
                    continue;
                }
                
                String[] parts = line.split("\\s+");
                if (parts.length >= 2) {
                    try {
                        double lat = Double.parseDouble(parts[0]);
                        double lon = Double.parseDouble(parts[1]);
                        
                        Waypoint wp = new Waypoint(lat, lon);
                        
                        if (parts.length >= 3) {
                            wp.setName(parts[2]);
                        }
                        
                        route.addWaypoint(wp);
                    } catch (NumberFormatException e) {
                        continue;
                    }
                }
            }
        }
        
        return route;
    }

    /**
     * 批量导入航线。
     * 
     * @param directory 目录
     * @return 航线列表
     * @throws IOException IO异常
     */
    public List<Route> importFromDirectory(File directory) throws IOException {
        List<Route> routes = new ArrayList<>();
        
        if (!directory.isDirectory()) {
            throw new IOException("不是有效的目录: " + directory.getPath());
        }
        
        File[] files = directory.listFiles((dir, name) -> 
                name.toLowerCase().endsWith(".csv") || name.toLowerCase().endsWith(".txt"));
        
        if (files != null) {
            for (File file : files) {
                try {
                    routes.add(importFromFile(file));
                } catch (IOException e) {
                    continue;
                }
            }
        }
        
        return routes;
    }
}
