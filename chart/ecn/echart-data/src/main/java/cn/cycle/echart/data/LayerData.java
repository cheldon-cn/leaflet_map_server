package cn.cycle.echart.data;

import java.awt.geom.Rectangle2D;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;

/**
 * 图层数据模型。
 * 
 * <p>表示一个图层的几何数据和属性数据。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class LayerData {

    private final String id;
    private final String name;
    private final String type;
    private final List<FeatureData> features;
    private final Map<String, Object> properties;
    private Rectangle2D bounds;
    private boolean visible;

    public LayerData(String id, String name, String type) {
        this.id = Objects.requireNonNull(id, "id cannot be null");
        this.name = Objects.requireNonNull(name, "name cannot be null");
        this.type = type != null ? type : "UNKNOWN";
        this.features = new ArrayList<>();
        this.properties = new HashMap<>();
        this.bounds = null;
        this.visible = true;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getType() {
        return type;
    }

    public List<FeatureData> getFeatures() {
        return Collections.unmodifiableList(features);
    }

    public int getFeatureCount() {
        return features.size();
    }

    public void addFeature(FeatureData feature) {
        Objects.requireNonNull(feature, "feature cannot be null");
        features.add(feature);
        updateBounds(feature);
    }

    public void removeFeature(FeatureData feature) {
        features.remove(feature);
        recalculateBounds();
    }

    public void clearFeatures() {
        features.clear();
        bounds = null;
    }

    public Rectangle2D getBounds() {
        return bounds;
    }

    public Map<String, Object> getProperties() {
        return Collections.unmodifiableMap(properties);
    }

    public void setProperty(String key, Object value) {
        properties.put(key, value);
    }

    public Object getProperty(String key) {
        return properties.get(key);
    }

    public boolean isVisible() {
        return visible;
    }

    public void setVisible(boolean visible) {
        this.visible = visible;
    }

    private void updateBounds(FeatureData feature) {
        if (feature.getBounds() == null) {
            return;
        }
        
        if (bounds == null) {
            bounds = new java.awt.geom.Rectangle2D.Double();
            bounds.setRect(feature.getBounds());
        } else {
            Rectangle2D.union(bounds, feature.getBounds(), bounds);
        }
    }

    private void recalculateBounds() {
        bounds = null;
        for (FeatureData feature : features) {
            updateBounds(feature);
        }
    }

    @Override
    public String toString() {
        return "LayerData{" +
               "id='" + id + '\'' +
               ", name='" + name + '\'' +
               ", type='" + type + '\'' +
               ", featureCount=" + features.size() +
               '}';
    }
}
