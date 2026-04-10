package cn.cycle.chart.api.cache;

import java.util.Objects;

public class TileKey {

    private final int x;
    private final int y;
    private final int z;
    private final String layerId;

    public TileKey(int x, int y, int z) {
        this(x, y, z, null);
    }

    public TileKey(int x, int y, int z, String layerId) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.layerId = layerId != null ? layerId : "default";
    }

    public int getX() {
        return x;
    }

    public int getY() {
        return y;
    }

    public int getZ() {
        return z;
    }

    public String getLayerId() {
        return layerId;
    }

    public String getKey() {
        return String.format("%s:%d:%d:%d", layerId, z, x, y);
    }

    public static TileKey fromKey(String key) {
        if (key == null || key.isEmpty()) {
            return null;
        }
        String[] parts = key.split(":");
        if (parts.length >= 4) {
            String layerId = parts[0];
            int z = Integer.parseInt(parts[1]);
            int x = Integer.parseInt(parts[2]);
            int y = Integer.parseInt(parts[3]);
            return new TileKey(x, y, z, layerId);
        } else if (parts.length == 3) {
            int z = Integer.parseInt(parts[0]);
            int x = Integer.parseInt(parts[1]);
            int y = Integer.parseInt(parts[2]);
            return new TileKey(x, y, z);
        }
        return null;
    }

    public TileKey getParent() {
        if (z <= 0) {
            return null;
        }
        return new TileKey(x / 2, y / 2, z - 1, layerId);
    }

    public TileKey getChild(int quadrant) {
        int childX = x * 2 + (quadrant % 2);
        int childY = y * 2 + (quadrant / 2);
        return new TileKey(childX, childY, z + 1, layerId);
    }

    public long getTileCount() {
        return 1L << (2 * z);
    }

    public boolean contains(double lon, double lat) {
        double tileSize = 360.0 / (1 << z);
        double minLon = x * tileSize - 180.0;
        double maxLon = (x + 1) * tileSize - 180.0;
        double minLat = y * tileSize - 90.0;
        double maxLat = (y + 1) * tileSize - 90.0;
        return lon >= minLon && lon < maxLon && lat >= minLat && lat < maxLat;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        TileKey other = (TileKey) obj;
        return x == other.x && y == other.y && z == other.z 
            && Objects.equals(layerId, other.layerId);
    }

    @Override
    public int hashCode() {
        return Objects.hash(x, y, z, layerId);
    }

    @Override
    public String toString() {
        return String.format("TileKey[%s:%d/%d/%d]", layerId, z, x, y);
    }
}
