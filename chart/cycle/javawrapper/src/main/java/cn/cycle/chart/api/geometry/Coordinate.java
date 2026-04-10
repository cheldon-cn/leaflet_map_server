package cn.cycle.chart.api.geometry;

import java.util.Objects;

public final class Coordinate {

    private final double x;
    private final double y;
    private final double z;
    private final boolean hasZ;

    public Coordinate(double x, double y) {
        this.x = x;
        this.y = y;
        this.z = 0.0;
        this.hasZ = false;
    }

    public Coordinate(double x, double y, double z) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.hasZ = true;
    }

    public double getX() {
        return x;
    }

    public double getY() {
        return y;
    }

    public double getZ() {
        return z;
    }

    public boolean hasZ() {
        return hasZ;
    }

    public double distance(Coordinate other) {
        if (other == null) {
            throw new IllegalArgumentException("other coordinate must not be null");
        }
        double dx = this.x - other.x;
        double dy = this.y - other.y;
        if (this.hasZ && other.hasZ) {
            double dz = this.z - other.z;
            return Math.sqrt(dx * dx + dy * dy + dz * dz);
        }
        return Math.sqrt(dx * dx + dy * dy);
    }

    public double distance2D(Coordinate other) {
        if (other == null) {
            throw new IllegalArgumentException("other coordinate must not be null");
        }
        double dx = this.x - other.x;
        double dy = this.y - other.y;
        return Math.sqrt(dx * dx + dy * dy);
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        Coordinate other = (Coordinate) obj;
        if (Double.compare(this.x, other.x) != 0) return false;
        if (Double.compare(this.y, other.y) != 0) return false;
        if (this.hasZ != other.hasZ) return false;
        if (this.hasZ && Double.compare(this.z, other.z) != 0) return false;
        return true;
    }

    @Override
    public int hashCode() {
        return Objects.hash(x, y, hasZ ? z : null);
    }

    @Override
    public String toString() {
        if (hasZ) {
            return String.format("Coordinate(%.6f, %.6f, %.6f)", x, y, z);
        }
        return String.format("Coordinate(%.6f, %.6f)", x, y);
    }

    public static Coordinate fromWKT(String wkt) {
        if (wkt == null || wkt.isEmpty()) {
            return null;
        }
        String content = wkt.trim();
        if (content.startsWith("(") && content.endsWith(")")) {
            content = content.substring(1, content.length() - 1);
        }
        String[] parts = content.split("\\s+");
        if (parts.length >= 2) {
            double x = Double.parseDouble(parts[0]);
            double y = Double.parseDouble(parts[1]);
            if (parts.length >= 3) {
                double z = Double.parseDouble(parts[2]);
                return new Coordinate(x, y, z);
            }
            return new Coordinate(x, y);
        }
        throw new IllegalArgumentException("Invalid coordinate format: " + wkt);
    }

    public String toWKT() {
        if (hasZ) {
            return String.format("%.6f %.6f %.6f", x, y, z);
        }
        return String.format("%.6f %.6f", x, y);
    }
}
