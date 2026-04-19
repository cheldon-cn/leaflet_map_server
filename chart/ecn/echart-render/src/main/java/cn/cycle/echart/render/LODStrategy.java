package cn.cycle.echart.render;

/**
 * LOD（细节层次）策略。
 * 
 * <p>根据缩放级别确定渲染细节层次。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class LODStrategy {

    private static final double[] ZOOM_THRESHOLDS = {
        0.001, 0.005, 0.01, 0.05, 0.1, 0.5, 1.0, 5.0, 10.0
    };
    
    private static final int[] LOD_LEVELS = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9
    };

    public LODStrategy() {
    }

    /**
     * 计算LOD级别。
     * 
     * @param zoom 缩放级别
     * @param layer 图层
     * @return LOD级别（0-9）
     */
    public int calculateLOD(double zoom, Layer layer) {
        for (int i = 0; i < ZOOM_THRESHOLDS.length; i++) {
            if (zoom < ZOOM_THRESHOLDS[i]) {
                return LOD_LEVELS[i];
            }
        }
        return LOD_LEVELS[LOD_LEVELS.length - 1];
    }

    /**
     * 根据LOD级别判断是否应该渲染要素。
     * 
     * @param lod LOD级别
     * @param featurePriority 要素优先级
     * @return 是否应该渲染
     */
    public boolean shouldRender(int lod, int featurePriority) {
        int threshold = lod / 2;
        return featurePriority >= threshold;
    }

    /**
     * 获取最大LOD级别。
     * 
     * @return 最大LOD级别
     */
    public int getMaxLOD() {
        return LOD_LEVELS[LOD_LEVELS.length - 1];
    }

    /**
     * 获取最小LOD级别。
     * 
     * @return 最小LOD级别
     */
    public int getMinLOD() {
        return LOD_LEVELS[0];
    }
}
