package cn.cycle.chart.api.geometry;

import java.util.Objects;

public final class Envelope {

    private final double minX;
    private final double minY;
    private final double maxX;
    private final double maxY;

    public Envelope(double minX, double minY, double maxX, double maxY) {
        if (minX > maxX) {
            double temp = minX;
            minX = maxX;
            maxX = temp;
        }
        if (minY > maxY) {
            double temp = minY;
            minY = maxY;
            maxY = temp;
        }
        this.minX = minX;
        this.minY = minY;
        this.maxX = maxX;
        this.maxY = maxY;
    }

    public Envelope(Coordinate... coordinates) {
        if (coordinates == null || coordinates.length == 0) {
            throw new IllegalArgumentException("coordinates must not be null or empty");
        }
        double tempMinX = Double.MAX_VALUE;
        double tempMinY = Double.MAX_VALUE;
        double tempMaxX = -Double.MAX_VALUE;
        double tempMaxY = -Double.MAX_VALUE;
        for (Coordinate coord : coordinates) {
            if (coord != null) {
                tempMinX = Math.min(tempMinX, coord.getX());
                tempMinY = Math.min(tempMinY, coord.getY());
                tempMaxX = Math.max(tempMaxX, coord.getX());
                tempMaxY = Math.max(tempMaxY, coord.getY());
            }
        }
        this.minX = tempMinX;
        this.minY = tempMinY;
        this.maxX = tempMaxX;
        this.maxY = tempMaxY;
    }

    public double getMinX() {
        return minX;
    }

    public double getMinY() {
        return minY;
    }

    public double getMaxX() {
        return maxX;
    }

    public double getMaxY() {
        return maxY;
    }

    public double getWidth() {
        return maxX - minX;
    }

    public double getHeight() {
        return maxY - minY;
    }

    public double getArea() {
        return getWidth() * getHeight();
    }

    public Coordinate getCenter() {
        double cx = (minX + maxX) / 2.0;
        double cy = (minY + maxY) / 2.0;
        return new Coordinate(cx, cy);
    }

    public boolean contains(double x, double y) {
        return x >= minX && x <= maxX && y >= minY && y <= maxY;
    }

    public boolean contains(Coordinate coord) {
        if (coord == null) return false;
        return contains(coord.getX(), coord.getY());
    }

    public boolean contains(Envelope other) {
        if (other == null) return false;
        return other.minX >= minX && other.maxX <= maxX
            && other.minY >= minY && other.maxY <= maxY;
    }

    public boolean intersects(Envelope other) {
        if (other == null) return false;
        return !(other.maxX < minX || other.minX > maxX
              || other.maxY < minY || other.minY > maxY);
    }

    public Envelope intersection(Envelope other) {
        if (!intersects(other)) {
            return null;
        }
        return new Envelope(
            Math.max(minX, other.minX),
            Math.max(minY, other.minY),
            Math.min(maxX, other.maxX),
            Math.min(maxY, other.maxY)
        );
    }

    public Envelope expand(Envelope other) {
        if (other == null) return this;
        return new Envelope(
            Math.min(minX, other.minX),
            Math.min(minY, other.minY),
            Math.max(maxX, other.maxX),
            Math.max(maxY, other.maxY)
        );
    }

    public Envelope expand(double deltaX, double deltaY) {
        return new Envelope(
            minX - deltaX,
            minY - deltaY,
            maxX + deltaX,
            maxY + deltaY
        );
    }

    public Envelope expand(double distance) {
        return expand(distance, distance);
    }

    public boolean isEmpty() {
        return minX > maxX || minY > maxY;
    }

    public boolean isNull() {
        return minX == 0 && minY == 0 && maxX == 0 && maxY == 0;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Envelope other = (Envelope) obj;
        return Double.compare(this.minX, other.minX) == 0
            && Double.compare(this.minY, other.minY) == 0
            && Double.compare(this.maxX, other.maxX) == 0
            && Double.compare(this.maxY, other.maxY) == 0;
    }

    @Override
    public int hashCode() {
        return Objects.hash(minX, minY, maxX, maxY);
    }

    @Override
    public String toString() {
        return String.format("Envelope[%.6f, %.6f, %.6f, %.6f]", minX, minY, maxX, maxY);
    }

    public String toWKT() {
        return String.format("POLYGON((%.6f %.6f, %.6f %.6f, %.6f %.6f, %.6f %.6f, %.6f %.6f))",
            minX, minY, maxX, minY, maxX, maxY, minX, maxY, minX, minY);
    }

    public static Envelope fromWKT(String wkt) {
        if (wkt == null || wkt.isEmpty()) {
            return null;
        }
        throw new UnsupportedOperationException("fromWKT not implemented yet");
    }

    public static Envelope createEmpty() {
        return new Envelope(0, 0, 0, 0);
    }
}
