package cn.cycle.echart.facade;

import cn.cycle.echart.ais.AISAlarmAssociation;
import cn.cycle.echart.ais.AISTarget;
import cn.cycle.echart.ais.AISTargetManager;
import cn.cycle.echart.ais.CPATCPACalculator;
import cn.cycle.echart.alarm.CpaResult;
import cn.cycle.echart.core.EventBus;

import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

/**
 * AIS业务门面。
 * 
 * <p>提供AIS系统的统一访问接口。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class AISFacade {

    private final AISTargetManager targetManager;
    private final AISAlarmAssociation alarmAssociation;
    private final CPATCPACalculator cpaTcpaCalculator;
    private final EventBus eventBus;

    public AISFacade(AISTargetManager targetManager, AISAlarmAssociation alarmAssociation, 
                     EventBus eventBus) {
        this.targetManager = Objects.requireNonNull(targetManager, "targetManager cannot be null");
        this.alarmAssociation = Objects.requireNonNull(alarmAssociation, "alarmAssociation cannot be null");
        this.eventBus = Objects.requireNonNull(eventBus, "eventBus cannot be null");
        this.cpaTcpaCalculator = new CPATCPACalculator();
    }

    public void updateTarget(int mmsi, double latitude, double longitude, 
                             double sog, double cog, double heading) {
        AISTarget target = targetManager.getTarget(mmsi);
        
        if (target == null) {
            target = new AISTarget(mmsi);
            target.setLatitude(latitude);
            target.setLongitude(longitude);
            targetManager.addTarget(target);
        }
        
        target.setSpeed(sog);
        target.setCourse(cog);
        target.setHeading(heading);
        target.setLatitude(latitude);
        target.setLongitude(longitude);
    }

    public void updateTarget(int mmsi, double latitude, double longitude, 
                             double sog, double cog, double heading,
                             String name, String callsign, int shipType) {
        AISTarget target = targetManager.getTarget(mmsi);
        
        if (target == null) {
            target = new AISTarget(mmsi);
            target.setLatitude(latitude);
            target.setLongitude(longitude);
            targetManager.addTarget(target);
        }
        
        target.setSpeed(sog);
        target.setCourse(cog);
        target.setHeading(heading);
        target.setLatitude(latitude);
        target.setLongitude(longitude);
        target.setName(name);
    }

    public AISTarget getTarget(int mmsi) {
        return targetManager.getTarget(mmsi);
    }

    public List<AISTarget> getAllTargets() {
        return targetManager.getAllTargets();
    }

    public List<AISTarget> getActiveTargets() {
        return targetManager.getAllTargets();
    }

    public List<AISTarget> getDangerousTargets(double cpaThreshold, double tcpaThreshold) {
        List<AISTarget> dangerous = new ArrayList<>();
        for (AISTarget target : targetManager.getAllTargets()) {
            CpaResult result = cpaTcpaCalculator.calculate(
                    0, 0, 0, 0,
                    target.getLatitude(), target.getLongitude(),
                    target.getSpeed(), target.getCourse());
            if (result != null && result.getCpa() < cpaThreshold && result.getTcpa() < tcpaThreshold) {
                dangerous.add(target);
            }
        }
        return dangerous;
    }

    public void removeTarget(int mmsi) {
        targetManager.removeTarget(mmsi);
        alarmAssociation.clearTargetAlerts(mmsi);
    }

    public void clearAllTargets() {
        targetManager.clear();
        alarmAssociation.clearAll();
    }

    public int getTargetCount() {
        return targetManager.getTargetCount();
    }

    public CpaResult calculateCPA(double ownLat, double ownLon, double ownSog, double ownCog,
                                   AISTarget target) {
        return cpaTcpaCalculator.calculate(
                ownLat, ownLon, ownSog, ownCog,
                target.getLatitude(), target.getLongitude(),
                target.getSpeed(), target.getCourse());
    }

    public boolean checkCollisionRisk(AISTarget target, double ownLat, double ownLon,
                                      double ownSog, double ownCog,
                                      double cpaThreshold, double tcpaThreshold) {
        CpaResult result = calculateCPA(ownLat, ownLon, ownSog, ownCog, target);
        
        if (result != null) {
            return alarmAssociation.checkAlarm(target, result.getCpa(), result.getTcpa());
        }
        
        return false;
    }

    public void setCPAThreshold(double thresholdNm) {
        alarmAssociation.setCpaThreshold(thresholdNm);
    }

    public double getCPAThreshold() {
        return alarmAssociation.getCpaThreshold();
    }

    public void setTCPAThreshold(double thresholdMinutes) {
        alarmAssociation.setTcpaThreshold(thresholdMinutes);
    }

    public double getTCPAThreshold() {
        return alarmAssociation.getTcpaThreshold();
    }

    public List<Object> getTargetAlerts(int mmsi) {
        return new ArrayList<>(alarmAssociation.getTargetAlerts(mmsi));
    }

    public void clearTargetAlerts(int mmsi) {
        alarmAssociation.clearTargetAlerts(mmsi);
    }

    public Object getTargetSummary(int mmsi) {
        AISTarget target = getTarget(mmsi);
        if (target == null) {
            return null;
        }

        java.util.Map<String, Object> summary = new java.util.HashMap<>();
        summary.put("mmsi", target.getMmsi());
        summary.put("name", target.getName());
        summary.put("latitude", target.getLatitude());
        summary.put("longitude", target.getLongitude());
        summary.put("sog", target.getSpeed());
        summary.put("cog", target.getCourse());
        summary.put("heading", target.getHeading());
        summary.put("navStatus", target.getNavStatus());
        summary.put("lastUpdate", target.getLastUpdate());

        return summary;
    }

    public List<Object> getAllTargetSummaries() {
        List<Object> summaries = new ArrayList<>();
        
        for (AISTarget target : getAllTargets()) {
            java.util.Map<String, Object> summary = new java.util.HashMap<>();
            summary.put("mmsi", target.getMmsi());
            summary.put("name", target.getName());
            summary.put("latitude", target.getLatitude());
            summary.put("longitude", target.getLongitude());
            summary.put("sog", target.getSpeed());
            summary.put("cog", target.getCourse());
            summaries.add(summary);
        }
        
        return summaries;
    }
}
