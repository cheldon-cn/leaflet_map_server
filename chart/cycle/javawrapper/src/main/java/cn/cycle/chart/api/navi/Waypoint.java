package cn.cycle.chart.api.navi;

import cn.cycle.chart.api.geometry.Coordinate;

import java.time.LocalDateTime;

public class Waypoint {

    public enum Type {
        WAYPOINT,
        DEPARTURE,
        DESTINATION,
        TURNING_POINT,
        ANCHORAGE,
        PILOT_STATION
    }

    private String name;
    private Coordinate position;
    private Type type;
    private double speed;
    private double xteLimit;
    private String notes;
    private LocalDateTime eta;
    private int sequence;

    public Waypoint() {
        this.type = Type.WAYPOINT;
        this.speed = 0;
        this.xteLimit = 0.25;
    }

    public Waypoint(String name, Coordinate position) {
        this();
        this.name = name;
        this.position = position;
    }

    public Waypoint(String name, double x, double y) {
        this(name, new Coordinate(x, y));
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Coordinate getPosition() {
        return position;
    }

    public void setPosition(Coordinate position) {
        this.position = position;
    }

    public void setPosition(double x, double y) {
        this.position = new Coordinate(x, y);
    }

    public Type getType() {
        return type;
    }

    public void setType(Type type) {
        this.type = type;
    }

    public double getSpeed() {
        return speed;
    }

    public void setSpeed(double speed) {
        this.speed = speed;
    }

    public double getXteLimit() {
        return xteLimit;
    }

    public void setXteLimit(double xteLimit) {
        this.xteLimit = xteLimit;
    }

    public String getNotes() {
        return notes;
    }

    public void setNotes(String notes) {
        this.notes = notes;
    }

    public LocalDateTime getEta() {
        return eta;
    }

    public void setEta(LocalDateTime eta) {
        this.eta = eta;
    }

    public int getSequence() {
        return sequence;
    }

    public void setSequence(int sequence) {
        this.sequence = sequence;
    }

    public double distanceTo(Waypoint other) {
        if (position == null || other == null || other.position == null) {
            return Double.NaN;
        }
        return position.distance(other.position);
    }

    public double bearingTo(Waypoint other) {
        if (position == null || other == null || other.position == null) {
            return Double.NaN;
        }
        double dx = other.position.getX() - position.getX();
        double dy = other.position.getY() - position.getY();
        return Math.toDegrees(Math.atan2(dx, dy));
    }

    @Override
    public String toString() {
        return String.format("Waypoint[%s: %s]", name, position);
    }
}
