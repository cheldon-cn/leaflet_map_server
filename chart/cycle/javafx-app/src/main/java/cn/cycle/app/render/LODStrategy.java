package cn.cycle.app.render;

import cn.cycle.app.lifecycle.AbstractLifecycleComponent;

import java.util.Arrays;
import java.util.List;

public class LODStrategy extends AbstractLifecycleComponent {
    
    private static final double[] SCALE_THRESHOLDS = {
        1.0 / 50000,
        1.0 / 200000,
        1.0 / 500000
    };
    
    public static final int HIGH_DETAIL = 3;
    public static final int MEDIUM_DETAIL = 2;
    public static final int LOW_DETAIL = 1;
    
    public LODStrategy() {
    }
    
    public int getDetailLevel(double scale) {
        for (int i = 0; i < SCALE_THRESHOLDS.length; i++) {
            if (scale > SCALE_THRESHOLDS[i]) {
                return SCALE_THRESHOLDS.length - i;
            }
        }
        return LOW_DETAIL;
    }
    
    public int getDetailLevelFromZoom(double zoom) {
        if (zoom >= 8) {
            return HIGH_DETAIL;
        } else if (zoom >= 4) {
            return MEDIUM_DETAIL;
        } else {
            return LOW_DETAIL;
        }
    }
    
    public List<String> getVisibleLayers(int detailLevel) {
        switch (detailLevel) {
            case HIGH_DETAIL:
                return Arrays.asList("base", "navigation", "depth", "obstacles", "text");
            case MEDIUM_DETAIL:
                return Arrays.asList("base", "navigation", "depth", "obstacles");
            case LOW_DETAIL:
            default:
                return Arrays.asList("base", "navigation");
        }
    }
    
    public double getSymbolSizeFactor(int detailLevel) {
        switch (detailLevel) {
            case HIGH_DETAIL:
                return 1.0;
            case MEDIUM_DETAIL:
                return 0.8;
            case LOW_DETAIL:
                return 0.6;
            default:
                return 0.5;
        }
    }
    
    public double getMinFeatureSize(int detailLevel) {
        switch (detailLevel) {
            case HIGH_DETAIL:
                return 1.0;
            case MEDIUM_DETAIL:
                return 5.0;
            case LOW_DETAIL:
                return 10.0;
            default:
                return 20.0;
        }
    }
    
    public boolean shouldRenderFeature(int detailLevel, double featureSize) {
        return featureSize >= getMinFeatureSize(detailLevel);
    }
    
    public int getTileLevel(int detailLevel) {
        switch (detailLevel) {
            case HIGH_DETAIL:
                return 12;
            case MEDIUM_DETAIL:
                return 8;
            case LOW_DETAIL:
            default:
                return 4;
        }
    }
    
    public double[] getScaleThresholds() {
        return SCALE_THRESHOLDS.clone();
    }
    
    @Override
    protected void doInitialize() {
    }
    
    @Override
    protected void doDispose() {
    }
}
