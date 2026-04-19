package cn.cycle.echart.ais;

import cn.cycle.chart.api.alert.Alert;
import cn.cycle.chart.api.alert.Alert.Severity;
import cn.cycle.echart.alarm.AlarmManager;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * AIS预警关联管理器。
 * 
 * <p>管理AIS目标与预警的关联关系。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AISAlarmAssociation {

    private final AlarmManager alarmManager;
    private final AISTargetManager targetManager;
    private final Map<Integer, List<Alert>> targetAlerts;
    private double cpaThreshold;
    private double tcpaThreshold;

    public AISAlarmAssociation(AlarmManager alarmManager, AISTargetManager targetManager) {
        this.alarmManager = Objects.requireNonNull(alarmManager, "alarmManager cannot be null");
        this.targetManager = Objects.requireNonNull(targetManager, "targetManager cannot be null");
        this.targetAlerts = new ConcurrentHashMap<>();
        this.cpaThreshold = 2.0;
        this.tcpaThreshold = 30.0;
    }

    /**
     * 设置CPA阈值。
     * 
     * @param threshold 阈值（海里）
     */
    public void setCpaThreshold(double threshold) {
        this.cpaThreshold = threshold;
    }

    /**
     * 获取CPA阈值。
     * 
     * @return 阈值（海里）
     */
    public double getCpaThreshold() {
        return cpaThreshold;
    }

    /**
     * 设置TCPA阈值。
     * 
     * @param threshold 阈值（分钟）
     */
    public void setTcpaThreshold(double threshold) {
        this.tcpaThreshold = threshold;
    }

    /**
     * 获取TCPA阈值。
     * 
     * @return 阈值（分钟）
     */
    public double getTcpaThreshold() {
        return tcpaThreshold;
    }

    /**
     * 检查AIS目标是否触发预警。
     * 
     * @param target AIS目标
     * @param cpa CPA值
     * @param tcpa TCPA值
     * @return 是否触发预警
     */
    public boolean checkAlarm(AISTarget target, double cpa, double tcpa) {
        if (cpa < cpaThreshold && tcpa > 0 && tcpa < tcpaThreshold) {
            Severity severity = determineSeverity(cpa, tcpa);
            String title = "碰撞预警";
            String message = String.format(
                    "目标MMSI: %d, CPA: %.2f nm, TCPA: %.1f min",
                    target.getMmsi(), cpa, tcpa);
            
            Alert alert = alarmManager.triggerAisAlert(severity, title, message);
            
            int mmsi = target.getMmsi();
            targetAlerts.computeIfAbsent(mmsi, k -> new ArrayList<>()).add(alert);
            
            return true;
        }
        return false;
    }

    /**
     * 获取目标关联的预警列表。
     * 
     * @param mmsi MMSI号
     * @return 预警列表
     */
    public List<Alert> getTargetAlerts(int mmsi) {
        return targetAlerts.getOrDefault(mmsi, new ArrayList<>());
    }

    /**
     * 清除目标关联的所有预警。
     * 
     * @param mmsi MMSI号
     */
    public void clearTargetAlerts(int mmsi) {
        List<Alert> alerts = targetAlerts.remove(mmsi);
        if (alerts != null) {
            for (Alert alert : alerts) {
                alarmManager.clearAlert(alert.getId());
            }
        }
    }

    /**
     * 清除所有预警关联。
     */
    public void clearAll() {
        for (Integer mmsi : targetAlerts.keySet()) {
            clearTargetAlerts(mmsi);
        }
    }

    private Severity determineSeverity(double cpa, double tcpa) {
        if (cpa < 0.5 || tcpa < 10) {
            return Severity.EMERGENCY;
        } else if (cpa < 1.0 || tcpa < 20) {
            return Severity.CRITICAL;
        } else {
            return Severity.WARNING;
        }
    }
}
