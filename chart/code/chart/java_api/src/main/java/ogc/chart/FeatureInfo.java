package ogc.chart;

import java.util.Map;
import java.util.HashMap;

/**
 * 海图特征信息类
 * 
 * <p>表示海图上的一个特征对象，包含其属性和几何信息。</p>
 * 
 * @version 1.0.0
 * @since 1.0.0
 */
public class FeatureInfo {
    
    private String featureId;
    private String featureName;
    private String featureType;
    private Map<String, Object> attributes;
    private Geometry geometry;
    
    /**
     * 默认构造函数
     */
    public FeatureInfo() {
        this.attributes = new HashMap<>();
    }
    
    /**
     * 获取特征ID
     * 
     * @return 特征ID
     */
    public String getFeatureId() {
        return featureId;
    }
    
    /**
     * 设置特征ID
     * 
     * @param featureId 特征ID
     */
    public void setFeatureId(String featureId) {
        this.featureId = featureId;
    }
    
    /**
     * 获取特征名称
     * 
     * @return 特征名称
     */
    public String getFeatureName() {
        return featureName;
    }
    
    /**
     * 设置特征名称
     * 
     * @param featureName 特征名称
     */
    public void setFeatureName(String featureName) {
        this.featureName = featureName;
    }
    
    /**
     * 获取特征类型
     * 
     * @return 特征类型
     */
    public String getFeatureType() {
        return featureType;
    }
    
    /**
     * 设置特征类型
     * 
     * @param featureType 特征类型
     */
    public void setFeatureType(String featureType) {
        this.featureType = featureType;
    }
    
    /**
     * 获取所有属性
     * 
     * @return 属性Map
     */
    public Map<String, Object> getAttributes() {
        return attributes;
    }
    
    /**
     * 设置属性
     * 
     * @param attributes 属性Map
     */
    public void setAttributes(Map<String, Object> attributes) {
        this.attributes = attributes != null ? attributes : new HashMap<String, Object>();
    }
    
    /**
     * 获取指定属性值
     * 
     * @param key 属性键
     * @return 属性值
     */
    public Object getAttribute(String key) {
        return attributes.get(key);
    }
    
    /**
     * 设置属性值
     * 
     * @param key 属性键
     * @param value 属性值
     */
    public void setAttribute(String key, Object value) {
        attributes.put(key, value);
    }
    
    /**
     * 获取几何对象
     * 
     * @return 几何对象
     */
    public Geometry getGeometry() {
        return geometry;
    }
    
    /**
     * 设置几何对象
     * 
     * @param geometry 几何对象
     */
    public void setGeometry(Geometry geometry) {
        this.geometry = geometry;
    }
    
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("FeatureInfo{");
        sb.append("featureId='").append(featureId).append('\'');
        sb.append(", featureName='").append(featureName).append('\'');
        sb.append(", featureType='").append(featureType).append('\'');
        sb.append(", attributes=").append(attributes);
        sb.append(", geometry=").append(geometry);
        sb.append('}');
        return sb.toString();
    }
}
