package cn.cycle.echart.data;

import java.awt.geom.Rectangle2D;
import java.util.Objects;

/**
 * 海图文件数据模型。
 * 
 * <p>表示一个海图文件的基本信息。</p>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class ChartFile {

    private final String id;
    private final String name;
    private final String path;
    private final Rectangle2D bounds;
    private final String format;
    private final long fileSize;
    private final long lastModified;
    private boolean loaded;

    public ChartFile(String id, String name, String path, Rectangle2D bounds, 
            String format, long fileSize, long lastModified) {
        this.id = Objects.requireNonNull(id, "id cannot be null");
        this.name = Objects.requireNonNull(name, "name cannot be null");
        this.path = Objects.requireNonNull(path, "path cannot be null");
        this.bounds = bounds;
        this.format = format != null ? format : "UNKNOWN";
        this.fileSize = fileSize;
        this.lastModified = lastModified;
        this.loaded = false;
    }

    public String getId() {
        return id;
    }

    public String getName() {
        return name;
    }

    public String getPath() {
        return path;
    }

    public Rectangle2D getBounds() {
        return bounds;
    }

    public String getFormat() {
        return format;
    }

    public long getFileSize() {
        return fileSize;
    }

    public long getLastModified() {
        return lastModified;
    }

    public boolean isLoaded() {
        return loaded;
    }

    public void setLoaded(boolean loaded) {
        this.loaded = loaded;
    }

    @Override
    public boolean equals(Object obj) {
        if (this == obj) return true;
        if (obj == null || getClass() != obj.getClass()) return false;
        ChartFile other = (ChartFile) obj;
        return id.equals(other.id);
    }

    @Override
    public int hashCode() {
        return id.hashCode();
    }

    @Override
    public String toString() {
        return "ChartFile{" +
               "id='" + id + '\'' +
               ", name='" + name + '\'' +
               ", format='" + format + '\'' +
               ", loaded=" + loaded +
               '}';
    }
}
