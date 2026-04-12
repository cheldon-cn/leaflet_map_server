package cn.cycle.app.model;

import java.io.File;
import java.text.SimpleDateFormat;
import java.util.Date;

public class DataItem {
    
    public enum DataType {
        CHART("海图", "海图数据"),
        IMAGE("图片", "图片文件"),
        VECTOR("矢量", "矢量数据"),
        RASTER("栅格", "栅格数据"),
        UNKNOWN("其他", "未知类型");
        
        private final String displayName;
        private final String description;
        
        DataType(String displayName, String description) {
            this.displayName = displayName;
            this.description = description;
        }
        
        public String getDisplayName() {
            return displayName;
        }
        
        public String getDescription() {
            return description;
        }
    }
    
    public enum DataFormat {
        S57("S-57", "国际海图数据标准"),
        ENC("ENC", "电子海图"),
        PNG("PNG", "便携式网络图形"),
        JPG("JPEG", "联合图像专家组"),
        BMP("BMP", "位图"),
        GIF("GIF", "图形交换格式"),
        SHP("Shapefile", "ESRI矢量格式"),
        GEOTIFF("GeoTIFF", "地理标记TIFF"),
        UNKNOWN("未知", "未知格式");
        
        private final String displayName;
        private final String description;
        
        DataFormat(String displayName, String description) {
            this.displayName = displayName;
            this.description = description;
        }
        
        public String getDisplayName() {
            return displayName;
        }
        
        public String getDescription() {
            return description;
        }
    }
    
    private String id;
    private String name;
    private File file;
    private DataType dataType;
    private DataFormat dataFormat;
    private long fileSize;
    private long createTime;
    private long modifyTime;
    private int width;
    private int height;
    
    public DataItem(File file) {
        this.id = generateId();
        this.file = file;
        this.name = file.getName();
        this.fileSize = file.length();
        this.createTime = System.currentTimeMillis();
        this.modifyTime = file.lastModified();
        this.dataType = detectDataType(file);
        this.dataFormat = detectDataFormat(file);
        this.width = 0;
        this.height = 0;
    }
    
    public DataItem(String name, DataType type) {
        this.id = generateId();
        this.name = name;
        this.dataType = type;
        this.dataFormat = DataFormat.UNKNOWN;
    }
    
    private String generateId() {
        return "DATA_" + System.currentTimeMillis() + "_" + (int)(Math.random() * 10000);
    }
    
    private DataType detectDataType(File file) {
        String name = file.getName().toLowerCase();
        if (name.endsWith(".000") || name.endsWith(".enc")) {
            return DataType.CHART;
        } else if (name.endsWith(".png") || name.endsWith(".jpg") || 
                   name.endsWith(".jpeg") || name.endsWith(".bmp") || 
                   name.endsWith(".gif")) {
            return DataType.IMAGE;
        } else if (name.endsWith(".shp")) {
            return DataType.VECTOR;
        } else if (name.endsWith(".tif") || name.endsWith(".tiff")) {
            return DataType.RASTER;
        }
        return DataType.UNKNOWN;
    }
    
    private DataFormat detectDataFormat(File file) {
        String name = file.getName().toLowerCase();
        if (name.endsWith(".000")) return DataFormat.S57;
        if (name.endsWith(".enc")) return DataFormat.ENC;
        if (name.endsWith(".png")) return DataFormat.PNG;
        if (name.endsWith(".jpg") || name.endsWith(".jpeg")) return DataFormat.JPG;
        if (name.endsWith(".bmp")) return DataFormat.BMP;
        if (name.endsWith(".gif")) return DataFormat.GIF;
        if (name.endsWith(".shp")) return DataFormat.SHP;
        if (name.endsWith(".tif") || name.endsWith(".tiff")) return DataFormat.GEOTIFF;
        return DataFormat.UNKNOWN;
    }
    
    public String getId() {
        return id;
    }
    
    public String getName() {
        return name;
    }
    
    public File getFile() {
        return file;
    }
    
    public DataType getDataType() {
        return dataType;
    }
    
    public DataFormat getDataFormat() {
        return dataFormat;
    }
    
    public long getFileSize() {
        return fileSize;
    }
    
    public String getFileSizeFormatted() {
        if (fileSize < 1024) {
            return fileSize + " B";
        } else if (fileSize < 1024 * 1024) {
            return String.format("%.2f KB", fileSize / 1024.0);
        } else if (fileSize < 1024 * 1024 * 1024) {
            return String.format("%.2f MB", fileSize / (1024.0 * 1024));
        } else {
            return String.format("%.2f GB", fileSize / (1024.0 * 1024 * 1024));
        }
    }
    
    public long getCreateTime() {
        return createTime;
    }
    
    public String getCreateTimeFormatted() {
        return formatDate(createTime);
    }
    
    public long getModifyTime() {
        return modifyTime;
    }
    
    public String getModifyTimeFormatted() {
        return formatDate(modifyTime);
    }
    
    private String formatDate(long time) {
        SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss");
        return sdf.format(new Date(time));
    }
    
    public String getFilePath() {
        return file != null ? file.getAbsolutePath() : "";
    }
    
    public int getWidth() {
        return width;
    }
    
    public void setWidth(int width) {
        this.width = width;
    }
    
    public int getHeight() {
        return height;
    }
    
    public void setHeight(int height) {
        this.height = height;
    }
    
    public String getDimensions() {
        if (width > 0 && height > 0) {
            return width + " × " + height;
        }
        return "-";
    }
    
    public boolean isPreviewable() {
        return dataType == DataType.IMAGE || dataType == DataType.CHART;
    }
    
    @Override
    public String toString() {
        return name;
    }
}
