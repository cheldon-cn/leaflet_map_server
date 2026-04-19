package cn.cycle.echart.alarm;

/**
 * UKC（富余水深）计算结果。
 * 
 * <p>包含富余水深和相关计算结果。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class UkcResult {

    private final double ukc;
    private final double squat;
    private final double chartedDepth;
    private final double tideHeight;
    private final double draft;
    private final double safetyMargin;
    private final boolean safe;

    public UkcResult(double ukc, double squat, double chartedDepth, 
            double tideHeight, double draft, double safetyMargin, boolean safe) {
        this.ukc = ukc;
        this.squat = squat;
        this.chartedDepth = chartedDepth;
        this.tideHeight = tideHeight;
        this.draft = draft;
        this.safetyMargin = safetyMargin;
        this.safe = safe;
    }

    /**
     * 获取UKC（富余水深）。
     * 
     * @return UKC（米）
     */
    public double getUkc() {
        return ukc;
    }

    /**
     * 获取下沉量。
     * 
     * @return 下沉量（米）
     */
    public double getSquat() {
        return squat;
    }

    /**
     * 获取海图水深。
     * 
     * @return 海图水深（米）
     */
    public double getChartedDepth() {
        return chartedDepth;
    }

    /**
     * 获取潮高。
     * 
     * @return 潮高（米）
     */
    public double getTideHeight() {
        return tideHeight;
    }

    /**
     * 获取吃水。
     * 
     * @return 吃水（米）
     */
    public double getDraft() {
        return draft;
    }

    /**
     * 获取安全余量。
     * 
     * @return 安全余量（米）
     */
    public double getSafetyMargin() {
        return safetyMargin;
    }

    /**
     * 检查是否安全。
     * 
     * @return 是否安全
     */
    public boolean isSafe() {
        return safe;
    }

    @Override
    public String toString() {
        return String.format("UkcResult{ukc=%.2f m, squat=%.2f m, safe=%b}", 
                ukc, squat, safe);
    }
}
