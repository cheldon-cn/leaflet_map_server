package cn.cycle.echart.data;

import cn.cycle.echart.core.AppEvent;
import cn.cycle.echart.core.AppEventType;
import cn.cycle.echart.core.EventBus;
import cn.cycle.echart.core.EventHandler;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 * 数据层图层管理器。
 * 
 * <p>管理图层数据的增删改查、可见性控制、顺序调整等功能。</p>
 * 
 * <h2>功能说明</h2>
 * <ul>
 *   <li>图层添加/移除/更新</li>
 *   <li>图层可见性控制</li>
 *   <li>图层顺序管理</li>
 *   <li>图层查询和过滤</li>
 *   <li>事件通知</li>
 * </ul>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DataLayerManager {

    private static volatile DataLayerManager instance;

    private final Map<String, LayerData> layerMap;
    private final List<LayerData> layerOrder;
    private final List<LayerChangeListener> listeners;
    private final EventBus eventBus;

    private DataLayerManager() {
        this.layerMap = new ConcurrentHashMap<>();
        this.layerOrder = new CopyOnWriteArrayList<>();
        this.listeners = new CopyOnWriteArrayList<>();
        this.eventBus = null;
    }

    private DataLayerManager(EventBus eventBus) {
        this.layerMap = new ConcurrentHashMap<>();
        this.layerOrder = new CopyOnWriteArrayList<>();
        this.listeners = new CopyOnWriteArrayList<>();
        this.eventBus = eventBus;
    }

    /**
     * 获取单例实例。
     * 
     * @return DataLayerManager实例
     */
    public static synchronized DataLayerManager getInstance() {
        if (instance == null) {
            instance = new DataLayerManager();
        }
        return instance;
    }

    /**
     * 创建带事件总线的实例。
     * 
     * @param eventBus 事件总线
     * @return DataLayerManager实例
     */
    public static DataLayerManager create(EventBus eventBus) {
        return new DataLayerManager(eventBus);
    }

    /**
     * 添加图层。
     * 
     * @param layer 图层数据
     * @throws IllegalArgumentException 如果图层ID已存在
     */
    public void addLayer(LayerData layer) {
        Objects.requireNonNull(layer, "layer cannot be null");
        
        if (layerMap.containsKey(layer.getId())) {
            throw new IllegalArgumentException("Layer already exists: " + layer.getId());
        }
        
        layerMap.put(layer.getId(), layer);
        layerOrder.add(layer);
        
        notifyLayerAdded(layer);
        publishEvent(AppEventType.LAYER_ADDED, layer);
    }

    /**
     * 添加图层到指定位置。
     * 
     * @param layer 图层数据
     * @param index 插入位置
     */
    public void addLayer(LayerData layer, int index) {
        Objects.requireNonNull(layer, "layer cannot be null");
        
        if (layerMap.containsKey(layer.getId())) {
            throw new IllegalArgumentException("Layer already exists: " + layer.getId());
        }
        
        layerMap.put(layer.getId(), layer);
        layerOrder.add(Math.min(index, layerOrder.size()), layer);
        
        notifyLayerAdded(layer);
        publishEvent(AppEventType.LAYER_ADDED, layer);
    }

    /**
     * 移除图层。
     * 
     * @param layerId 图层ID
     * @return 被移除的图层，如果不存在返回null
     */
    public LayerData removeLayer(String layerId) {
        Objects.requireNonNull(layerId, "layerId cannot be null");
        
        LayerData layer = layerMap.remove(layerId);
        if (layer != null) {
            layerOrder.remove(layer);
            notifyLayerRemoved(layer);
            publishEvent(AppEventType.LAYER_REMOVED, layer);
        }
        return layer;
    }

    /**
     * 获取图层。
     * 
     * @param layerId 图层ID
     * @return 图层数据，如果不存在返回null
     */
    public LayerData getLayer(String layerId) {
        Objects.requireNonNull(layerId, "layerId cannot be null");
        return layerMap.get(layerId);
    }

    /**
     * 检查图层是否存在。
     * 
     * @param layerId 图层ID
     * @return 如果存在返回true
     */
    public boolean hasLayer(String layerId) {
        Objects.requireNonNull(layerId, "layerId cannot be null");
        return layerMap.containsKey(layerId);
    }

    /**
     * 获取所有图层（按顺序）。
     * 
     * @return 图层列表（只读）
     */
    public List<LayerData> getAllLayers() {
        return Collections.unmodifiableList(new ArrayList<>(layerOrder));
    }

    /**
     * 获取可见图层。
     * 
     * @return 可见图层列表
     */
    public List<LayerData> getVisibleLayers() {
        List<LayerData> visible = new ArrayList<>();
        for (LayerData layer : layerOrder) {
            if (layer.isVisible()) {
                visible.add(layer);
            }
        }
        return visible;
    }

    /**
     * 按类型获取图层。
     * 
     * @param type 图层类型
     * @return 图层列表
     */
    public List<LayerData> getLayersByType(String type) {
        Objects.requireNonNull(type, "type cannot be null");
        
        List<LayerData> result = new ArrayList<>();
        for (LayerData layer : layerOrder) {
            if (type.equals(layer.getType())) {
                result.add(layer);
            }
        }
        return result;
    }

    /**
     * 设置图层可见性。
     * 
     * @param layerId 图层ID
     * @param visible 是否可见
     * @return 如果操作成功返回true
     */
    public boolean setLayerVisibility(String layerId, boolean visible) {
        Objects.requireNonNull(layerId, "layerId cannot be null");
        
        LayerData layer = layerMap.get(layerId);
        if (layer == null) {
            return false;
        }
        
        if (layer.isVisible() != visible) {
            layer.setVisible(visible);
            notifyLayerVisibilityChanged(layer, visible);
            publishEvent(AppEventType.LAYER_VISIBILITY_CHANGED, layer);
        }
        return true;
    }

    /**
     * 移动图层顺序。
     * 
     * @param layerId 图层ID
     * @param newIndex 新位置
     * @return 如果操作成功返回true
     */
    public boolean moveLayer(String layerId, int newIndex) {
        Objects.requireNonNull(layerId, "layerId cannot be null");
        
        LayerData layer = layerMap.get(layerId);
        if (layer == null) {
            return false;
        }
        
        int oldIndex = layerOrder.indexOf(layer);
        if (oldIndex == -1 || oldIndex == newIndex) {
            return false;
        }
        
        layerOrder.remove(oldIndex);
        layerOrder.add(Math.min(newIndex, layerOrder.size()), layer);
        
        notifyLayerOrderChanged(layer, oldIndex, newIndex);
        publishEvent(AppEventType.LAYER_ORDER_CHANGED, layer);
        return true;
    }

    /**
     * 上移图层。
     * 
     * @param layerId 图层ID
     * @return 如果操作成功返回true
     */
    public boolean moveLayerUp(String layerId) {
        LayerData layer = layerMap.get(layerId);
        if (layer == null) {
            return false;
        }
        
        int index = layerOrder.indexOf(layer);
        if (index <= 0) {
            return false;
        }
        
        return moveLayer(layerId, index - 1);
    }

    /**
     * 下移图层。
     * 
     * @param layerId 图层ID
     * @return 如果操作成功返回true
     */
    public boolean moveLayerDown(String layerId) {
        LayerData layer = layerMap.get(layerId);
        if (layer == null) {
            return false;
        }
        
        int index = layerOrder.indexOf(layer);
        if (index >= layerOrder.size() - 1) {
            return false;
        }
        
        return moveLayer(layerId, index + 1);
    }

    /**
     * 获取图层索引。
     * 
     * @param layerId 图层ID
     * @return 索引，如果不存在返回-1
     */
    public int getLayerIndex(String layerId) {
        Objects.requireNonNull(layerId, "layerId cannot be null");
        
        LayerData layer = layerMap.get(layerId);
        if (layer == null) {
            return -1;
        }
        return layerOrder.indexOf(layer);
    }

    /**
     * 获取图层数量。
     * 
     * @return 图层数量
     */
    public int getLayerCount() {
        return layerOrder.size();
    }

    /**
     * 清除所有图层。
     */
    public void clear() {
        layerMap.clear();
        layerOrder.clear();
        notifyAllLayersCleared();
    }

    /**
     * 按名称排序图层。
     */
    public void sortLayersByName() {
        layerOrder.sort(Comparator.comparing(LayerData::getName));
        notifyLayerOrderChanged(null, -1, -1);
    }

    /**
     * 添加图层变更监听器。
     * 
     * @param listener 监听器
     */
    public void addLayerChangeListener(LayerChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.add(listener);
    }

    /**
     * 移除图层变更监听器。
     * 
     * @param listener 监听器
     */
    public void removeLayerChangeListener(LayerChangeListener listener) {
        Objects.requireNonNull(listener, "listener cannot be null");
        listeners.remove(listener);
    }

    private void notifyLayerAdded(LayerData layer) {
        for (LayerChangeListener listener : listeners) {
            try {
                listener.onLayerAdded(layer);
            } catch (Exception e) {
                System.err.println("Error notifying layer added: " + e.getMessage());
            }
        }
    }

    private void notifyLayerRemoved(LayerData layer) {
        for (LayerChangeListener listener : listeners) {
            try {
                listener.onLayerRemoved(layer);
            } catch (Exception e) {
                System.err.println("Error notifying layer removed: " + e.getMessage());
            }
        }
    }

    private void notifyLayerVisibilityChanged(LayerData layer, boolean visible) {
        for (LayerChangeListener listener : listeners) {
            try {
                listener.onLayerVisibilityChanged(layer, visible);
            } catch (Exception e) {
                System.err.println("Error notifying visibility changed: " + e.getMessage());
            }
        }
    }

    private void notifyLayerOrderChanged(LayerData layer, int oldIndex, int newIndex) {
        for (LayerChangeListener listener : listeners) {
            try {
                listener.onLayerOrderChanged(layer, oldIndex, newIndex);
            } catch (Exception e) {
                System.err.println("Error notifying order changed: " + e.getMessage());
            }
        }
    }

    private void notifyAllLayersCleared() {
        for (LayerChangeListener listener : listeners) {
            try {
                listener.onAllLayersCleared();
            } catch (Exception e) {
                System.err.println("Error notifying layers cleared: " + e.getMessage());
            }
        }
    }

    private void publishEvent(AppEventType eventType, LayerData layer) {
        if (eventBus != null) {
            AppEvent event = new AppEvent(this, eventType)
                .withData("layerId", layer.getId())
                .withData("layerName", layer.getName())
                .withData("layerType", layer.getType());
            eventBus.publish(event);
        }
    }

    /**
     * 图层变更监听器接口。
     */
    public interface LayerChangeListener {
        
        /**
         * 图层添加时调用。
         * 
         * @param layer 添加的图层
         */
        void onLayerAdded(LayerData layer);
        
        /**
         * 图层移除时调用。
         * 
         * @param layer 移除的图层
         */
        void onLayerRemoved(LayerData layer);
        
        /**
         * 图层可见性变化时调用。
         * 
         * @param layer 变化的图层
         * @param visible 新的可见性
         */
        void onLayerVisibilityChanged(LayerData layer, boolean visible);
        
        /**
         * 图层顺序变化时调用。
         * 
         * @param layer 变化的图层（批量排序时可能为null）
         * @param oldIndex 旧索引
         * @param newIndex 新索引
         */
        void onLayerOrderChanged(LayerData layer, int oldIndex, int newIndex);
        
        /**
         * 所有图层清除时调用。
         */
        void onAllLayersCleared();
    }
}
