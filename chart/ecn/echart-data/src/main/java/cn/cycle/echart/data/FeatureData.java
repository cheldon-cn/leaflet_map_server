package cn.cycle.echart.data;

import java.awt.geom.Rectangle2D;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/**
 * 要素数据模型。
 * 
 * <p>表示一个地理要素的几何和属性数据。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class FeatureData {

    private final String id;
    private final String geometryType;
    private final byte[] geometryData;
    private final Map<String, Object> attributes;
    private Rectangle2D bounds;

    public FeatureData(String id, String geometryType, byte[] geometryData) {
        this.id = Objects.requireNonNull(id, "id cannot be null");
        this.geometryType = geometryType != null ? geometryType : "UNKNOWN";
        this.geometryData = geometryData != null ? geometryData.clone() : new byte[0];
        this.attributes = new HashMap<>();
        this.bounds = null;
    }

    public String getId() {
        return id;
    }

    public String getGeometryType() {
        return geometryType;
    }

    public byte[] getGeometryData() {
        return geometryData != null ? geometryData.clone() : new byte[0];
    }

    public Map<String, Object> getAttributes() {
        return Collections.unmodifiableMap(attributes);
    }

    public void setAttribute(String key, Object value) {
        attributes.put(key, value);
    }

    public Object getAttribute(String key) {
        return attributes.get(key);
    }

    public String getAttributeAsString(String key) {
        Object value = attributes.get(key);
        return value != null ? value.toString() : null;
    }

    public Integer getAttributeAsInt(String key) {
        Object value = attributes.get(key);
        if (value instanceof Number) {
            return ((Number) value).intValue();
        }
        return null;
    }

    public Double getAttributeAsDouble(String key) {
        Object value = attributes.get(key);
        if (value instanceof Number) {
            return ((Number) value).doubleValue();
        }
        return null;
    }

    public Rectangle2D getBounds() {
        return bounds;
    }

    public void setBounds(Rectangle2D bounds) {
        this.bounds = bounds;
    }

    @Override
    public String toString() {
        return "FeatureData{" +
               "id='" + id + '\'' +
               ", geometryType='" + geometryType + '\'' +
               ", attributeCount=" + attributes.size() +
               '}';
    }
}
