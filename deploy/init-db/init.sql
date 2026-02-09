-- Leaf 项目数据库初始化脚本
-- 创建空间数据库表结构

-- 启用 SQLite 空间扩展（如果可用）
-- SELECT load_extension('mod_spatialite');

-- 创建空间参考系统表
CREATE TABLE IF NOT EXISTS spatial_ref_sys (
    srid INTEGER PRIMARY KEY,
    auth_name TEXT,
    auth_srid INTEGER,
    srtext TEXT,
    proj4text TEXT
);

-- 创建图层表
CREATE TABLE IF NOT EXISTS layers (
    layer_id TEXT PRIMARY KEY,
    name TEXT NOT NULL,
    description TEXT,
    geometry_type TEXT CHECK(geometry_type IN ('POINT', 'LINESTRING', 'POLYGON', 'MULTIPOINT', 'MULTILINESTRING', 'MULTIPOLYGON')),
    srid INTEGER DEFAULT 4326,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (srid) REFERENCES spatial_ref_sys(srid)
);

-- 创建要素表（通用结构）
CREATE TABLE IF NOT EXISTS features (
    feature_id TEXT PRIMARY KEY,
    layer_id TEXT NOT NULL,
    geometry BLOB, -- 存储空间几何数据
    properties TEXT, -- JSON 格式的属性数据
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (layer_id) REFERENCES layers(layer_id) ON DELETE CASCADE
);

-- 创建空间索引（如果支持 R-Tree）
CREATE VIRTUAL TABLE IF NOT EXISTS features_rtree USING rtree(
    id,              -- 主键
    minx, maxx,      -- 最小/最大 X 坐标
    miny, maxy       -- 最小/最大 Y 坐标
);

-- 创建元数据表
CREATE TABLE IF NOT EXISTS metadata (
    key TEXT PRIMARY KEY,
    value TEXT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 插入默认图层数据
INSERT OR IGNORE INTO layers (layer_id, name, description, geometry_type, srid) VALUES
    ('roads', '道路网络', '主要道路和高速公路', 'LINESTRING', 4326),
    ('buildings', '建筑物', '建筑物多边形', 'POLYGON', 4326),
    ('water', '水域', '河流、湖泊等水域', 'POLYGON', 4326),
    ('landuse', '土地利用', '土地利用分类', 'POLYGON', 4326);

-- 插入默认空间参考系统
INSERT OR IGNORE INTO spatial_ref_sys (srid, auth_name, auth_srid, srtext, proj4text) VALUES
    (4326, 'EPSG', 4326, 'GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]]', '+proj=longlat +datum=WGS84 +no_defs'),
    (3857, 'EPSG', 3857, 'PROJCS["WGS 84 / Pseudo-Mercator",GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0,AUTHORITY["EPSG","8901"]],UNIT["degree",0.0174532925199433,AUTHORITY["EPSG","9122"]],AUTHORITY["EPSG","4326"]],PROJECTION["Mercator_1SP"],PARAMETER["central_meridian",0],PARAMETER["scale_factor",1],PARAMETER["false_easting",0],PARAMETER["false_northing",0],UNIT["metre",1,AUTHORITY["EPSG","9001"]],AXIS["X",EAST],AXIS["Y",NORTH],AUTHORITY["EPSG","3857"]]', '+proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0 +k=1.0 +units=m +nadgrids=@null +wktext +no_defs');

-- 插入默认元数据
INSERT OR IGNORE INTO metadata (key, value) VALUES
    ('version', '1.0.0'),
    ('last_updated', CURRENT_TIMESTAMP),
    ('description', 'Leaf 项目空间数据库');

-- 创建触发器：自动更新更新时间戳
CREATE TRIGGER IF NOT EXISTS update_layers_timestamp 
AFTER UPDATE ON layers 
FOR EACH ROW 
BEGIN
    UPDATE layers SET updated_at = CURRENT_TIMESTAMP 
    WHERE layer_id = OLD.layer_id;
END;

CREATE TRIGGER IF NOT EXISTS update_features_timestamp 
AFTER UPDATE ON features 
FOR EACH ROW 
BEGIN
    UPDATE features SET updated_at = CURRENT_TIMESTAMP 
    WHERE feature_id = OLD.feature_id;
END;