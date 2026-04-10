package cn.cycle.chart.api.alert;

import cn.cycle.chart.api.geometry.Coordinate;

import java.time.LocalDateTime;

public class Alert {

    public enum Type {
        SAFETY,
        NAVIGATION,
        SYSTEM,
        WEATHER,
        AIS
    }

    public enum Severity {
        INFO,
        WARNING,
        CRITICAL,
        EMERGENCY
    }

    private final String id;
    private final Type type;
    private Severity severity;
    private String title;
    private String message;
    private Coordinate position;
    private LocalDateTime timestamp;
    private boolean acknowledged;
    private Object source;

    private static int idCounter = 0;

    public Alert(Type type, Severity severity, String title, String message) {
        this.id = "ALT-" + (++idCounter);
        this.type = type;
        this.severity = severity;
        this.title = title;
        this.message = message;
        this.timestamp = LocalDateTime.now();
        this.acknowledged = false;
    }

    public String getId() {
        return id;
    }

    public Type getType() {
        return type;
    }

    public Severity getSeverity() {
        return severity;
    }

    public void setSeverity(Severity severity) {
        this.severity = severity;
    }

    public String getTitle() {
        return title;
    }

    public void setTitle(String title) {
        this.title = title;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }

    public Coordinate getPosition() {
        return position;
    }

    public void setPosition(Coordinate position) {
        this.position = position;
    }

    public LocalDateTime getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(LocalDateTime timestamp) {
        this.timestamp = timestamp;
    }

    public boolean isAcknowledged() {
        return acknowledged;
    }

    public void setAcknowledged(boolean acknowledged) {
        this.acknowledged = acknowledged;
    }

    public Object getSource() {
        return source;
    }

    public void setSource(Object source) {
        this.source = source;
    }

    public static Alert createSafetyAlert(Severity severity, String title, String message) {
        return new Alert(Type.SAFETY, severity, title, message);
    }

    public static Alert createNavigationAlert(Severity severity, String title, String message) {
        return new Alert(Type.NAVIGATION, severity, title, message);
    }

    public static Alert createSystemAlert(Severity severity, String title, String message) {
        return new Alert(Type.SYSTEM, severity, title, message);
    }

    public static Alert createWeatherAlert(Severity severity, String title, String message) {
        return new Alert(Type.WEATHER, severity, title, message);
    }

    public static Alert createAisAlert(Severity severity, String title, String message) {
        return new Alert(Type.AIS, severity, title, message);
    }

    @Override
    public String toString() {
        return String.format("Alert[%s: %s - %s]", id, severity, title);
    }
}
