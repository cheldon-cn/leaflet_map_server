-- 示例数据插入脚本
-- 为测试目的插入一些示例要素

-- 为 roads 图层插入示例道路
INSERT OR IGNORE INTO features (feature_id, layer_id, geometry, properties) VALUES
    ('road_1', 'roads', 
     -- 简单的线几何（WKB格式简化表示，实际应用中需要使用空间函数）
     NULL,
     '{"name": "主干道1", "type": "highway", "length": 5000, "lanes": 4}'),
    
    ('road_2', 'roads', 
     NULL,
     '{"name": "城市道路A", "type": "urban", "length": 2000, "lanes": 2}'),
    
    ('road_3', 'roads', 
     NULL,
     '{"name": "乡村道路", "type": "rural", "length": 3000, "lanes": 1}');

-- 为 buildings 图层插入示例建筑物
INSERT OR IGNORE INTO features (feature_id, layer_id, geometry, properties) VALUES
    ('building_1', 'buildings', 
     NULL,
     '{"name": "行政大楼", "type": "office", "floors": 10, "area": 5000}'),
    
    ('building_2', 'buildings', 
     NULL,
     '{"name": "住宅楼A", "type": "residential", "floors": 6, "area": 3000}'),
    
    ('building_3', 'buildings', 
     NULL,
     '{"name": "购物中心", "type": "commercial", "floors": 3, "area": 10000}');

-- 为 water 图层插入示例水域
INSERT OR IGNORE INTO features (feature_id, layer_id, geometry, properties) VALUES
    ('water_1', 'water', 
     NULL,
     '{"name": "西湖", "type": "lake", "area": 6500000}'),
    
    ('water_2', 'water', 
     NULL,
     '{"name": "长江支流", "type": "river", "length": 15000}');

-- 为 landuse 图层插入示例土地利用
INSERT OR IGNORE INTO features (feature_id, layer_id, geometry, properties) VALUES
    ('landuse_1', 'landuse', 
     NULL,
     '{"type": "residential", "area": 5000000, "population": 50000}'),
    
    ('landuse_2', 'landuse', 
     NULL,
     '{"type": "commercial", "area": 1000000, "businesses": 200}'),
    
    ('landuse_3', 'landuse', 
     NULL,
     '{"type": "agricultural", "area": 10000000, "crop_type": "wheat"}');

-- 更新元数据
UPDATE metadata SET 
    value = CURRENT_TIMESTAMP, 
    updated_at = CURRENT_TIMESTAMP 
WHERE key = 'last_updated';

-- 输出统计信息
SELECT '数据库初始化完成' AS message;
SELECT '图层数量:' AS label, COUNT(*) AS count FROM layers;
SELECT '要素总数:' AS label, COUNT(*) AS count FROM features;
SELECT '空间参考系统:' AS label, COUNT(*) AS count FROM spatial_ref_sys;