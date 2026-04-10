package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

import java.util.*;
import java.util.concurrent.ConcurrentHashMap;

public class AisManager {

    private final Map<Integer, AisTarget> targets;
    private final List<AisListener> listeners;
    private double rangeLimit;
    private boolean enabled;

    public AisManager() {
        this.targets = new ConcurrentHashMap<>();
        this.listeners = new ArrayList<>();
        this.rangeLimit = 50.0;
        this.enabled = true;
    }

    public void updateTarget(int mmsi, Coordinate position, double speed, double heading, double course) {
        AisTarget target = targets.get(mmsi);
        if (target == null) {
            target = new AisTarget(mmsi);
            targets.put(mmsi, target);
        }
        target.setPosition(position);
        target.setSpeed(speed);
        target.setHeading(heading);
        target.setCourse(course);
        target.setLastUpdate(System.currentTimeMillis());
        fireTargetUpdated(target);
    }

    public void removeTarget(int mmsi) {
        AisTarget target = targets.remove(mmsi);
        if (target != null) {
            fireTargetRemoved(target);
        }
    }

    public AisTarget getTarget(int mmsi) {
        return targets.get(mmsi);
    }

    public Collection<AisTarget> getAllTargets() {
        return Collections.unmodifiableCollection(targets.values());
    }

    public List<AisTarget> getTargetsInRange(Coordinate position, double range) {
        List<AisTarget> inRange = new ArrayList<>();
        if (position == null) {
            return inRange;
        }
        for (AisTarget target : targets.values()) {
            Coordinate targetPos = target.getPosition();
            if (targetPos != null) {
                double distance = position.distance(targetPos);
                if (distance <= range) {
                    inRange.add(target);
                }
            }
        }
        return inRange;
    }

    public void clearOldTargets(long maxAge) {
        long now = System.currentTimeMillis();
        Iterator<Map.Entry<Integer, AisTarget>> it = targets.entrySet().iterator();
        while (it.hasNext()) {
            Map.Entry<Integer, AisTarget> entry = it.next();
            if (now - entry.getValue().getLastUpdate() > maxAge) {
                it.remove();
                fireTargetRemoved(entry.getValue());
            }
        }
    }

    public void clearAllTargets() {
        targets.clear();
    }

    public int getTargetCount() {
        return targets.size();
    }

    public boolean isEnabled() {
        return enabled;
    }

    public void setEnabled(boolean enabled) {
        this.enabled = enabled;
    }

    public double getRangeLimit() {
        return rangeLimit;
    }

    public void setRangeLimit(double rangeLimit) {
        this.rangeLimit = rangeLimit;
    }

    public void addListener(AisListener listener) {
        if (listener != null && !listeners.contains(listener)) {
            listeners.add(listener);
        }
    }

    public void removeListener(AisListener listener) {
        listeners.remove(listener);
    }

    private void fireTargetUpdated(AisTarget target) {
        for (AisListener listener : listeners) {
            listener.onTargetUpdated(target);
        }
    }

    private void fireTargetRemoved(AisTarget target) {
        for (AisListener listener : listeners) {
            listener.onTargetRemoved(target);
        }
    }

    public interface AisListener {
        void onTargetUpdated(AisTarget target);
        void onTargetRemoved(AisTarget target);
    }

    public static class AisTarget {
        private final int mmsi;
        private String name;
        private String callSign;
        private int shipType;
        private Coordinate position;
        private double speed;
        private double heading;
        private double course;
        private double length;
        private double width;
        private long lastUpdate;

        public AisTarget(int mmsi) {
            this.mmsi = mmsi;
        }

        public int getMmsi() {
            return mmsi;
        }

        public String getName() {
            return name;
        }

        public void setName(String name) {
            this.name = name;
        }

        public String getCallSign() {
            return callSign;
        }

        public void setCallSign(String callSign) {
            this.callSign = callSign;
        }

        public int getShipType() {
            return shipType;
        }

        public void setShipType(int shipType) {
            this.shipType = shipType;
        }

        public Coordinate getPosition() {
            return position;
        }

        public void setPosition(Coordinate position) {
            this.position = position;
        }

        public double getSpeed() {
            return speed;
        }

        public void setSpeed(double speed) {
            this.speed = speed;
        }

        public double getHeading() {
            return heading;
        }

        public void setHeading(double heading) {
            this.heading = heading;
        }

        public double getCourse() {
            return course;
        }

        public void setCourse(double course) {
            this.course = course;
        }

        public double getLength() {
            return length;
        }

        public void setLength(double length) {
            this.length = length;
        }

        public double getWidth() {
            return width;
        }

        public void setWidth(double width) {
            this.width = width;
        }

        public long getLastUpdate() {
            return lastUpdate;
        }

        public void setLastUpdate(long lastUpdate) {
            this.lastUpdate = lastUpdate;
        }

        @Override
        public String toString() {
            return String.format("AisTarget[MMSI=%d, name=%s, pos=%s]", mmsi, name, position);
        }
    }
}
