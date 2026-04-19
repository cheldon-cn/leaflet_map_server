package cn.cycle.echart.render;

import java.awt.geom.Rectangle2D;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * 脏区域管理器。
 * 
 * <p>管理需要重绘的区域，优化渲染性能。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class DirtyRegionManager {

    private final List<Rectangle2D> regions;
    private Rectangle2D mergedRegion;

    public DirtyRegionManager() {
        this.regions = new ArrayList<>();
        this.mergedRegion = null;
    }

    /**
     * 添加脏区域。
     * 
     * @param x X坐标
     * @param y Y坐标
     * @param width 宽度
     * @param height 高度
     */
    public void addRegion(int x, int y, int width, int height) {
        Rectangle2D region = new Rectangle2D.Double(x, y, width, height);
        addRegion(region);
    }

    /**
     * 添加脏区域。
     * 
     * @param region 区域
     */
    public void addRegion(Rectangle2D region) {
        regions.add(region);
        
        if (mergedRegion == null) {
            mergedRegion = new Rectangle2D.Double();
            mergedRegion.setRect(region);
        } else {
            Rectangle2D.union(mergedRegion, region, mergedRegion);
        }
    }

    /**
     * 获取所有脏区域。
     * 
     * @return 脏区域列表
     */
    public List<Rectangle2D> getRegions() {
        return Collections.unmodifiableList(regions);
    }

    /**
     * 获取合并后的脏区域。
     * 
     * @return 合并后的脏区域，如果没有脏区域返回null
     */
    public Rectangle2D getMergedRegion() {
        return mergedRegion;
    }

    /**
     * 检查是否有脏区域。
     * 
     * @return 是否有脏区域
     */
    public boolean hasDirtyRegions() {
        return !regions.isEmpty();
    }

    /**
     * 检查指定区域是否与脏区域相交。
     * 
     * @param region 区域
     * @return 是否相交
     */
    public boolean intersects(Rectangle2D region) {
        if (mergedRegion == null) {
            return false;
        }
        return mergedRegion.intersects(region);
    }

    /**
     * 获取脏区域数量。
     * 
     * @return 脏区域数量
     */
    public int getRegionCount() {
        return regions.size();
    }

    /**
     * 清除所有脏区域。
     */
    public void clear() {
        regions.clear();
        mergedRegion = null;
    }
}
