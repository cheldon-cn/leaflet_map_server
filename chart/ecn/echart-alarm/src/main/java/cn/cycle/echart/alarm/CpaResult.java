package cn.cycle.echart.alarm;

/**
 * CPA/TCPA计算结果。
 * 
 * <p>包含最近会遇点距离和最近会遇时间。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class CpaResult {

    private final double cpa;
    private final double tcpa;
    private final double distance;
    private final double bearing;
    private final boolean dangerous;

    public CpaResult(double cpa, double tcpa, double distance, double bearing, boolean dangerous) {
        this.cpa = cpa;
        this.tcpa = tcpa;
        this.distance = distance;
        this.bearing = bearing;
        this.dangerous = dangerous;
    }

    /**
     * 获取CPA（最近会遇距离）。
     * 
     * @return CPA（海里）
     */
    public double getCpa() {
        return cpa;
    }

    /**
     * 获取TCPA（最近会遇时间）。
     * 
     * @return TCPA（分钟）
     */
    public double getTcpa() {
        return tcpa;
    }

    /**
     * 获取当前距离。
     * 
     * @return 距离（海里）
     */
    public double getDistance() {
        return distance;
    }

    /**
     * 获取方位角。
     * 
     * @return 方位角（度）
     */
    public double getBearing() {
        return bearing;
    }

    /**
     * 检查是否危险。
     * 
     * @return 是否危险
     */
    public boolean isDangerous() {
        return dangerous;
    }

    @Override
    public String toString() {
        return String.format("CpaResult{cpa=%.2f nm, tcpa=%.2f min, dangerous=%b}", 
                cpa, tcpa, dangerous);
    }
}
