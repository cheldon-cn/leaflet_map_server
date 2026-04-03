package ogc.chart;

/**
 * 海图图层类
 * 
 * <p>表示海图中的一个图层，可以控制图层的可见性和样式。</p>
 * 
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartLayer {
    
    private String layerId;
    private String layerName;
    private boolean visible;
    private float opacity;
    private int zIndex;
    
    /**
     * 默认构造函数
     */
    public ChartLayer() {
        this.visible = true;
        this.opacity = 1.0f;
        this.zIndex = 0;
    }
    
    /**
     * 构造函数
     * 
     * @param layerId 图层ID
     * @param layerName 图层名称
     */
    public ChartLayer(String layerId, String layerName) {
        this();
        this.layerId = layerId;
        this.layerName = layerName;
    }
    
    /**
     * 获取图层ID
     * 
     * @return 图层ID
     */
    public String getLayerId() {
        return layerId;
    }
    
    /**
     * 设置图层ID
     * 
     * @param layerId 图层ID
     */
    public void setLayerId(String layerId) {
        this.layerId = layerId;
    }
    
    /**
     * 获取图层名称
     * 
     * @return 图层名称
     */
    public String getLayerName() {
        return layerName;
    }
    
    /**
     * 设置图层名称
     * 
     * @param layerName 图层名称
     */
    public void setLayerName(String layerName) {
        this.layerName = layerName;
    }
    
    /**
     * 获取图层是否可见
     * 
     * @return 是否可见
     */
    public boolean isVisible() {
        return visible;
    }
    
    /**
     * 设置图层可见性
     * 
     * @param visible 是否可见
     */
    public void setVisible(boolean visible) {
        this.visible = visible;
    }
    
    /**
     * 获取图层透明度
     * 
     * @return 透明度 (0.0-1.0)
     */
    public float getOpacity() {
        return opacity;
    }
    
    /**
     * 设置图层透明度
     * 
     * @param opacity 透明度 (0.0-1.0)
     */
    public void setOpacity(float opacity) {
        this.opacity = Math.max(0.0f, Math.min(1.0f, opacity));
    }
    
    /**
     * 获取图层Z序
     * 
     * @return Z序
     */
    public int getZIndex() {
        return zIndex;
    }
    
    /**
     * 设置图层Z序
     * 
     * @param zIndex Z序
     */
    public void setZIndex(int zIndex) {
        this.zIndex = zIndex;
    }
    
    @Override
    public String toString() {
        return "ChartLayer{" +
                "layerId='" + layerId + '\'' +
                ", layerName='" + layerName + '\'' +
                ", visible=" + visible +
                ", opacity=" + opacity +
                ", zIndex=" + zIndex +
                '}';
    }
}
