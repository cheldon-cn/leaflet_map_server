package cn.cycle.echart.data;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.core.EventHandler;

import java.awt.geom.Rectangle2D;
import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;

/**
 * 海图文件管理器。
 * 
 * <p>管理海图文件的加载、卸载和查询。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartFileManager {

    private final Map<String, ChartFile> charts;
    private final EventBus eventBus;
    private final List<ChartFileListener> listeners;

    public ChartFileManager(EventBus eventBus) {
        this.charts = new HashMap<>();
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.listeners = new ArrayList<>();
    }

    /**
     * 加载海图文件。
     * 
     * @param filePath 文件路径
     * @return 海图文件，如果加载失败返回null
     */
    public ChartFile loadChart(String filePath) {
        Objects.requireNonNull(filePath, "filePath cannot be null");
        
        File file = new File(filePath);
        if (!file.exists() || !file.isFile()) {
            return null;
        }
        
        String id = generateId();
        String name = file.getName();
        String format = detectFormat(file);
        long fileSize = file.length();
        long lastModified = file.lastModified();
        
        ChartFile chartFile = new ChartFile(id, name, filePath, null, 
                format, fileSize, lastModified);
        
        charts.put(id, chartFile);
        chartFile.setLoaded(true);
        
        notifyChartLoaded(chartFile);
        
        return chartFile;
    }

    /**
     * 卸载海图文件。
     * 
     * @param chartId 海图ID
     * @return 是否成功卸载
     */
    public boolean unloadChart(String chartId) {
        Objects.requireNonNull(chartId, "chartId cannot be null");
        
        ChartFile chartFile = charts.remove(chartId);
        if (chartFile != null) {
            chartFile.setLoaded(false);
            notifyChartUnloaded(chartFile);
            return true;
        }
        return false;
    }

    /**
     * 获取海图文件。
     * 
     * @param chartId 海图ID
     * @return 海图文件，如果未找到返回null
     */
    public ChartFile getChart(String chartId) {
        return charts.get(chartId);
    }

    /**
     * 获取所有已加载的海图文件。
     * 
     * @return 海图文件列表
     */
    public List<ChartFile> getLoadedCharts() {
        return new ArrayList<>(charts.values());
    }

    /**
     * 获取已加载海图数量。
     * 
     * @return 海图数量
     */
    public int getChartCount() {
        return charts.size();
    }

    /**
     * 检查海图是否已加载。
     * 
     * @param chartId 海图ID
     * @return 是否已加载
     */
    public boolean isChartLoaded(String chartId) {
        ChartFile chartFile = charts.get(chartId);
        return chartFile != null && chartFile.isLoaded();
    }

    /**
     * 根据名称查找海图。
     * 
     * @param name 海图名称
     * @return 海图文件列表
     */
    public List<ChartFile> findChartsByName(String name) {
        List<ChartFile> result = new ArrayList<>();
        for (ChartFile chart : charts.values()) {
            if (chart.getName().contains(name)) {
                result.add(chart);
            }
        }
        return result;
    }

    /**
     * 根据范围查找海图。
     * 
     * @param bounds 地理范围
     * @return 海图文件列表
     */
    public List<ChartFile> findChartsByBounds(Rectangle2D bounds) {
        List<ChartFile> result = new ArrayList<>();
        for (ChartFile chart : charts.values()) {
            if (chart.getBounds() != null && chart.getBounds().intersects(bounds)) {
                result.add(chart);
            }
        }
        return result;
    }

    /**
     * 卸载所有海图。
     */
    public void unloadAll() {
        List<String> ids = new ArrayList<>(charts.keySet());
        for (String id : ids) {
            unloadChart(id);
        }
    }

    /**
     * 添加海图监听器。
     * 
     * @param listener 监听器
     */
    public void addChartFileListener(ChartFileListener listener) {
        listeners.add(listener);
    }

    /**
     * 移除海图监听器。
     * 
     * @param listener 监听器
     */
    public void removeChartFileListener(ChartFileListener listener) {
        listeners.remove(listener);
    }

    private String generateId() {
        return UUID.randomUUID().toString();
    }

    private String detectFormat(File file) {
        String name = file.getName().toLowerCase();
        if (name.endsWith(".s57") || name.endsWith(".000")) {
            return "S57";
        } else if (name.endsWith(".s63")) {
            return "S63";
        } else if (name.endsWith(".geojson")) {
            return "GeoJSON";
        } else if (name.endsWith(".shp")) {
            return "Shapefile";
        }
        return "UNKNOWN";
    }

    private void notifyChartLoaded(ChartFile chartFile) {
        eventBus.publish(new AppEvent(this, AppEventType.CHART_LOADED, chartFile));
        
        for (ChartFileListener listener : listeners) {
            listener.onChartLoaded(chartFile);
        }
    }

    private void notifyChartUnloaded(ChartFile chartFile) {
        eventBus.publish(new AppEvent(this, AppEventType.CHART_UNLOADED, chartFile));
        
        for (ChartFileListener listener : listeners) {
            listener.onChartUnloaded(chartFile);
        }
    }

    /**
     * 海图文件监听器接口。
     */
    public interface ChartFileListener {
        void onChartLoaded(ChartFile chartFile);
        void onChartUnloaded(ChartFile chartFile);
    }
}
