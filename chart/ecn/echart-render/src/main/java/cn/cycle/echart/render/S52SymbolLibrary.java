package cn.cycle.echart.render;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.ConcurrentHashMap;

/**
 * S-52符号库。
 * 
 * <p>提供S-52标准海图符号的管理和查询功能。</p>
 * 
 * <h2>S-52符号分类</h2>
 * <ul>
 *   <li>点符号 (Point Symbols)</li>
 *   <li>线符号 (Line Symbols)</li>
 *   <li>面符号 (Area Patterns)</li>
 *   <li>文本符号 (Text Styles)</li>
 * </ul>
 * 
 * @author Cycle Team
 * @version 1.0.0
 * @since 1.0.0
 */
public class S52SymbolLibrary {

    private static volatile S52SymbolLibrary instance;

    private final Map<String, Symbol> symbols;
    private final Map<String, LookupTable> lookupTables;
    private volatile boolean loaded;

    private S52SymbolLibrary() {
        this.symbols = new ConcurrentHashMap<>();
        this.lookupTables = new ConcurrentHashMap<>();
        this.loaded = false;
    }

    /**
     * 获取单例实例。
     * 
     * @return S52SymbolLibrary实例
     */
    public static synchronized S52SymbolLibrary getInstance() {
        if (instance == null) {
            instance = new S52SymbolLibrary();
        }
        return instance;
    }

    /**
     * 加载符号库。
     * 
     * @param libraryPath 符号库路径
     * @return 是否加载成功
     */
    public boolean load(String libraryPath) {
        Objects.requireNonNull(libraryPath, "libraryPath cannot be null");
        
        try {
            loadSymbols(libraryPath);
            loadLookupTables(libraryPath);
            loaded = true;
            return true;
        } catch (Exception e) {
            System.err.println("Failed to load symbol library: " + e.getMessage());
            loaded = false;
            return false;
        }
    }

    private void loadSymbols(String basePath) {
        symbols.put("SOUNDG", new Symbol("SOUNDG", SymbolType.POINT, "Soundings"));
        symbols.put("LIGHTS", new Symbol("LIGHTS", SymbolType.POINT, "Lights"));
        symbols.put("BCNLAT", new Symbol("BCNLAT", SymbolType.POINT, "Beacon Lateral"));
        symbols.put("BCNCAR", new Symbol("BCNCAR", SymbolType.POINT, "Beacon Cardinal"));
        symbols.put("BOYCAR", new Symbol("BOYCAR", SymbolType.POINT, "Buoy Cardinal"));
        symbols.put("BOYLAT", new Symbol("BOYLAT", SymbolType.POINT, "Buoy Lateral"));
        symbols.put("WRECKS", new Symbol("WRECKS", SymbolType.POINT, "Wrecks"));
        symbols.put("OBSTRN", new Symbol("OBSTRN", SymbolType.POINT, "Obstruction"));
        symbols.put("DEPARE", new Symbol("DEPARE", SymbolType.AREA, "Depth Area"));
        symbols.put("DRGARE", new Symbol("DRGARE", SymbolType.AREA, "Dredged Area"));
        symbols.put("LNDARE", new Symbol("LNDARE", SymbolType.AREA, "Land Area"));
        symbols.put("COALNE", new Symbol("COALNE", SymbolType.LINE, "Coastline"));
        symbols.put("DEPCNT", new Symbol("DEPCNT", SymbolType.LINE, "Depth Contour"));
        symbols.put("NAVLNE", new Symbol("NAVLNE", SymbolType.LINE, "Navigation Line"));
        symbols.put("TSSLPT", new Symbol("TSSLPT", SymbolType.LINE, "Traffic Separation Lane"));
        symbols.put("RECTRC", new Symbol("RECTRC", SymbolType.LINE, "Recommended Track"));
    }

    private void loadLookupTables(String basePath) {
        lookupTables.put("CHART", new LookupTable("CHART", "Chart symbols"));
        lookupTables.put("PLAIN", new LookupTable("PLAIN", "Plain symbols"));
        lookupTables.put("OTHER", new LookupTable("OTHER", "Other symbols"));
    }

    /**
     * 获取符号。
     * 
     * @param symbolCode 符号代码
     * @return 符号对象，如果未找到返回null
     */
    public Symbol getSymbol(String symbolCode) {
        Objects.requireNonNull(symbolCode, "symbolCode cannot be null");
        return symbols.get(symbolCode);
    }

    /**
     * 检查符号是否存在。
     * 
     * @param symbolCode 符号代码
     * @return 如果存在返回true
     */
    public boolean hasSymbol(String symbolCode) {
        Objects.requireNonNull(symbolCode, "symbolCode cannot be null");
        return symbols.containsKey(symbolCode);
    }

    /**
     * 获取所有符号。
     * 
     * @return 符号映射（只读）
     */
    public Map<String, Symbol> getAllSymbols() {
        return Collections.unmodifiableMap(symbols);
    }

    /**
     * 获取查找表。
     * 
     * @param tableName 表名
     * @return 查找表，如果未找到返回null
     */
    public LookupTable getLookupTable(String tableName) {
        Objects.requireNonNull(tableName, "tableName cannot be null");
        return lookupTables.get(tableName);
    }

    /**
     * 检查符号库是否已加载。
     * 
     * @return 如果已加载返回true
     */
    public boolean isLoaded() {
        return loaded;
    }

    /**
     * 获取符号数量。
     * 
     * @return 符号数量
     */
    public int getSymbolCount() {
        return symbols.size();
    }

    /**
     * 清除符号库。
     */
    public void clear() {
        symbols.clear();
        lookupTables.clear();
        loaded = false;
    }

    /**
     * 符号类型枚举。
     */
    public enum SymbolType {
        POINT,
        LINE,
        AREA,
        TEXT
    }

    /**
     * 符号定义。
     */
    public static class Symbol {
        private final String code;
        private final SymbolType type;
        private final String description;
        private final Map<String, Object> properties;

        public Symbol(String code, SymbolType type, String description) {
            this.code = code;
            this.type = type;
            this.description = description;
            this.properties = new HashMap<>();
        }

        public String getCode() {
            return code;
        }

        public SymbolType getType() {
            return type;
        }

        public String getDescription() {
            return description;
        }

        public void setProperty(String key, Object value) {
            properties.put(key, value);
        }

        @SuppressWarnings("unchecked")
        public <T> T getProperty(String key) {
            return (T) properties.get(key);
        }

        public Map<String, Object> getProperties() {
            return Collections.unmodifiableMap(properties);
        }
    }

    /**
     * 查找表定义。
     */
    public static class LookupTable {
        private final String name;
        private final String description;
        private final Map<String, String> entries;

        public LookupTable(String name, String description) {
            this.name = name;
            this.description = description;
            this.entries = new HashMap<>();
        }

        public String getName() {
            return name;
        }

        public String getDescription() {
            return description;
        }

        public void addEntry(String key, String symbolCode) {
            entries.put(key, symbolCode);
        }

        public String getSymbolCode(String key) {
            return entries.get(key);
        }

        public Map<String, String> getEntries() {
            return Collections.unmodifiableMap(entries);
        }
    }
}
