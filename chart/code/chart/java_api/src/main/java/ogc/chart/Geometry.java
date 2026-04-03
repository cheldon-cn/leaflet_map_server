package ogc.chart;

/**
 * 几何对象基类
 * 
 * <p>表示地理几何对象，如点、线、面等。</p>
 * 
 * @version 1.0.0
 * @since 1.0.0
 */
public class Geometry {
    
    /**
     * 几何类型枚举
     */
    public enum Type {
        /**
         * 点
         */
        POINT,
        /**
         * 线
         */
        LINESTRING,
        /**
         * 多边形
         */
        POLYGON,
        /**
         * 多点
         */
        MULTIPOINT,
        /**
         * 多线
         */
        MULTILINESTRING,
        /**
         * 多面
         */
        MULTIPOLYGON,
        /**
         * 几何集合
         */
        GEOMETRYCOLLECTION
    }
    
    private Type type;
    private double[] coordinates;
    
    /**
     * 默认构造函数
     */
    public Geometry() {
    }
    
    /**
     * 构造函数
     * 
     * @param type 几何类型
     * @param coordinates 坐标数组 [x1, y1, x2, y2, ...]
     */
    public Geometry(Type type, double[] coordinates) {
        this.type = type;
        this.coordinates = coordinates;
    }
    
    /**
     * 获取几何类型
     * 
     * @return 几何类型
     */
    public Type getType() {
        return type;
    }
    
    /**
     * 设置几何类型
     * 
     * @param type 几何类型
     */
    public void setType(Type type) {
        this.type = type;
    }
    
    /**
     * 获取坐标数组
     * 
     * @return 坐标数组 [x1, y1, x2, y2, ...]
     */
    public double[] getCoordinates() {
        return coordinates;
    }
    
    /**
     * 设置坐标数组
     * 
     * @param coordinates 坐标数组 [x1, y1, x2, y2, ...]
     */
    public void setCoordinates(double[] coordinates) {
        this.coordinates = coordinates;
    }
    
    /**
     * 获取坐标点数量
     * 
     * @return 坐标点数量
     */
    public int getPointCount() {
        return coordinates != null ? coordinates.length / 2 : 0;
    }
    
    /**
     * 获取指定索引的X坐标
     * 
     * @param index 点索引
     * @return X坐标
     */
    public double getX(int index) {
        if (coordinates == null || index < 0 || index * 2 >= coordinates.length) {
            throw new IndexOutOfBoundsException("Index: " + index);
        }
        return coordinates[index * 2];
    }
    
    /**
     * 获取指定索引的Y坐标
     * 
     * @param index 点索引
     * @return Y坐标
     */
    public double getY(int index) {
        if (coordinates == null || index < 0 || index * 2 + 1 >= coordinates.length) {
            throw new IndexOutOfBoundsException("Index: " + index);
        }
        return coordinates[index * 2 + 1];
    }
    
    /**
     * 获取边界范围
     * 
     * @return 边界范围 [minX, minY, maxX, maxY]
     */
    public double[] getBounds() {
        if (coordinates == null || coordinates.length == 0) {
            return new double[]{0, 0, 0, 0};
        }
        
        double minX = Double.MAX_VALUE;
        double minY = Double.MAX_VALUE;
        double maxX = Double.MIN_VALUE;
        double maxY = Double.MIN_VALUE;
        
        for (int i = 0; i < coordinates.length; i += 2) {
            minX = Math.min(minX, coordinates[i]);
            maxX = Math.max(maxX, coordinates[i]);
            minY = Math.min(minY, coordinates[i + 1]);
            maxY = Math.max(maxY, coordinates[i + 1]);
        }
        
        return new double[]{minX, minY, maxX, maxY};
    }
    
    /**
     * 转换为WKT格式
     * 
     * @return WKT字符串
     */
    public String toWKT() {
        if (type == null || coordinates == null) {
            return "";
        }
        
        StringBuilder sb = new StringBuilder();
        sb.append(type.name());
        sb.append(" (");
        
        for (int i = 0; i < coordinates.length; i += 2) {
            if (i > 0) {
                sb.append(", ");
            }
            sb.append(coordinates[i]);
            sb.append(" ");
            sb.append(coordinates[i + 1]);
        }
        
        sb.append(")");
        return sb.toString();
    }
    
    @Override
    public String toString() {
        return toWKT();
    }
}
