package cn.cycle.echart.render;

import java.awt.geom.Rectangle2D;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.Objects;

/**
 * 图层管理器。
 * 
 * <p>管理渲染图层的添加、移除、排序和可见性。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class LayerManager {

    private final List<Layer> layers;
    private final List<LayerChangeListener> listeners;

    public LayerManager() {
        this.layers = new ArrayList<>();
        this.listeners = new ArrayList<>();
    }

    /**
     * 添加图层。
     * 
     * @param layer 图层
     * @throws IllegalArgumentException 如果layer为null
     */
    public void addLayer(Layer layer) {
        addLayer(layer, layers.size());
    }

    /**
     * 在指定位置添加图层。
     * 
     * @param layer 图层
     * @param index 位置索引
     * @throws IllegalArgumentException 如果layer为null
     */
    public void addLayer(Layer layer, int index) {
        Objects.requireNonNull(layer, "layer cannot be null");
        
        if (index < 0 || index > layers.size()) {
            index = layers.size();
        }
        
        layers.add(index, layer);
        notifyLayerAdded(layer, index);
    }

    /**
     * 移除图层。
     * 
     * @param layer 图层
     * @return 是否成功移除
     */
    public boolean removeLayer(Layer layer) {
        int index = layers.indexOf(layer);
        if (index >= 0) {
            layers.remove(index);
            notifyLayerRemoved(layer, index);
            return true;
        }
        return false;
    }

    /**
     * 获取图层列表。
     * 
     * @return 不可修改的图层列表
     */
    public List<Layer> getLayers() {
        return Collections.unmodifiableList(layers);
    }

    /**
     * 获取可见图层列表。
     * 
     * @return 可见图层列表（按渲染顺序）
     */
    public List<Layer> getVisibleLayers() {
        List<Layer> visible = new ArrayList<>();
        for (Layer layer : layers) {
            if (layer.isVisible()) {
                visible.add(layer);
            }
        }
        return visible;
    }

    /**
     * 获取图层数量。
     * 
     * @return 图层数量
     */
    public int getLayerCount() {
        return layers.size();
    }

    /**
     * 获取指定索引的图层。
     * 
     * @param index 索引
     * @return 图层
     */
    public Layer getLayer(int index) {
        if (index >= 0 && index < layers.size()) {
            return layers.get(index);
        }
        return null;
    }

    /**
     * 根据名称获取图层。
     * 
     * @param name 图层名称
     * @return 图层，如果未找到返回null
     */
    public Layer getLayerByName(String name) {
        for (Layer layer : layers) {
            if (layer.getName().equals(name)) {
                return layer;
            }
        }
        return null;
    }

    /**
     * 移动图层顺序。
     * 
     * @param layer 图层
     * @param newIndex 新索引
     */
    public void moveLayer(Layer layer, int newIndex) {
        int oldIndex = layers.indexOf(layer);
        if (oldIndex >= 0 && newIndex >= 0 && newIndex < layers.size()) {
            layers.remove(oldIndex);
            layers.add(newIndex, layer);
            notifyLayerMoved(layer, oldIndex, newIndex);
        }
    }

    /**
     * 清除所有图层。
     */
    public void clear() {
        layers.clear();
        notifyLayersCleared();
    }

    /**
     * 添加图层变更监听器。
     * 
     * @param listener 监听器
     */
    public void addLayerChangeListener(LayerChangeListener listener) {
        listeners.add(listener);
    }

    /**
     * 移除图层变更监听器。
     * 
     * @param listener 监听器
     */
    public void removeLayerChangeListener(LayerChangeListener listener) {
        listeners.remove(listener);
    }

    private void notifyLayerAdded(Layer layer, int index) {
        for (LayerChangeListener listener : listeners) {
            listener.onLayerAdded(layer, index);
        }
    }

    private void notifyLayerRemoved(Layer layer, int index) {
        for (LayerChangeListener listener : listeners) {
            listener.onLayerRemoved(layer, index);
        }
    }

    private void notifyLayerMoved(Layer layer, int oldIndex, int newIndex) {
        for (LayerChangeListener listener : listeners) {
            listener.onLayerMoved(layer, oldIndex, newIndex);
        }
    }

    private void notifyLayersCleared() {
        for (LayerChangeListener listener : listeners) {
            listener.onLayersCleared();
        }
    }

    /**
     * 图层变更监听器接口。
     */
    public interface LayerChangeListener {
        void onLayerAdded(Layer layer, int index);
        void onLayerRemoved(Layer layer, int index);
        void onLayerMoved(Layer layer, int oldIndex, int newIndex);
        void onLayersCleared();
    }
}
