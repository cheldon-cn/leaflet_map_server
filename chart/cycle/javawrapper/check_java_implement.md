# Java接口逻辑正确性检查报告

> **基准**: `install/include/ogc/capi/sdk_c_api.h` + `cycle/jni/src/native/*.cpp`
> **检查范围**: `cycle/javawrapper/src/main/java/cn/cycle/chart/api/`
> **版本**: v1.0
> **日期**: 2026-04-19

---

## 一、检查概要

| 统计项 | 数量 |
|--------|------|
| 已检查Java类 | 25 |
| 严重问题（功能缺失/逻辑错误） | 8 |
| 中等问题（API不一致/设计问题） | 7 |
| 轻微问题（命名/风格） | 5 |
| 正确实现 | 5 |

---

## 二、严重问题（必须修复）

### S-01: MultiPoint/MultiLineString/MultiPolygon/GeometryCollection 缺少公共方法

**涉及类**: MultiPoint, MultiLineString, MultiPolygon, GeometryCollection

**问题描述**: 这4个Multi*类声明了native方法（nativeGetNumGeometries, nativeGetGeometryN, nativeAddGeometry），但没有暴露任何公共Java方法。C API中定义了完整的公共接口，但Java端完全不可用。

**C API定义**:
```c
// MultiPoint
SDK_C_API size_t ogc_multipoint_get_num_geometries(const ogc_geometry_t* mp);
SDK_C_API ogc_geometry_t* ogc_multipoint_get_geometry_n(const ogc_geometry_t* mp, size_t index);
SDK_C_API void ogc_multipoint_add_geometry(ogc_geometry_t* mp, ogc_geometry_t* point);
```

**Java当前实现** (MultiPoint为例):
```java
private native long nativeGetNumGeometries(long ptr);    // 无公共方法
private native long nativeGetGeometryN(long ptr, long index);  // 无公共方法
private native void nativeAddGeometry(long ptr, long pointPtr); // 无公共方法
```

**应添加的公共方法**:
```java
public long getNumGeometries() {
    checkNotDisposed();
    return nativeGetNumGeometries(getNativePtr());
}

public Geometry getGeometryN(long index) {
    checkNotDisposed();
    long ptr = nativeGetGeometryN(getNativePtr(), index);
    return GeometryFactory.fromNativePtr(ptr);
}

public void addGeometry(Geometry geom) {
    checkNotDisposed();
    if (geom == null) throw new IllegalArgumentException("geom must not be null");
    nativeAddGeometry(getNativePtr(), geom.getNativePtr());
}
```

**影响**: 用户无法通过Java API操作Multi*几何类型的子几何，功能完全不可用。

---

### S-02: Feature类缺少公共方法和nativeDispose实现

**涉及类**: Feature

**问题描述**: Feature类声明了多个native方法但无公共方法暴露，且nativeDispose为空实现。C API中Feature有完整的CRUD操作。

**C API定义**:
```c
SDK_C_API ogc_feature_t* ogc_feature_create(ogc_feature_defn_t* defn);
SDK_C_API void ogc_feature_destroy(ogc_feature_t* feature);
SDK_C_API long long ogc_feature_get_fid(const ogc_feature_t* feature);
SDK_C_API void ogc_feature_set_fid(ogc_feature_t* feature, long long fid);
SDK_C_API ogc_feature_defn_t* ogc_feature_get_defn(const ogc_feature_t* feature);
SDK_C_API size_t ogc_feature_get_field_count(const ogc_feature_t* feature);
SDK_C_API int ogc_feature_is_field_set(const ogc_feature_t* feature, size_t index);
SDK_C_API int ogc_feature_is_field_null(const ogc_feature_t* feature, size_t index);
SDK_C_API int ogc_feature_get_field_as_integer(const ogc_feature_t* feature, size_t index);
SDK_C_API double ogc_feature_get_field_as_real(const ogc_feature_t* feature, size_t index);
SDK_C_API const char* ogc_feature_get_field_as_string(const ogc_feature_t* feature, size_t index);
SDK_C_API void ogc_feature_set_field_integer(ogc_feature_t* feature, size_t index, int value);
SDK_C_API void ogc_feature_set_field_real(ogc_feature_t* feature, size_t index, double value);
SDK_C_API void ogc_feature_set_field_string(ogc_feature_t* feature, size_t index, const char* value);
SDK_C_API void ogc_feature_set_field_null(ogc_feature_t* feature, size_t index);
SDK_C_API ogc_geometry_t* ogc_feature_get_geometry(const ogc_feature_t* feature);
SDK_C_API void ogc_feature_set_geometry(ogc_feature_t* feature, ogc_geometry_t* geom);
SDK_C_API ogc_geometry_t* ogc_feature_steal_geometry(ogc_feature_t* feature);
SDK_C_API ogc_envelope_t* ogc_feature_get_envelope(const ogc_feature_t* feature);
SDK_C_API ogc_feature_t* ogc_feature_clone(const ogc_feature_t* feature);
// 按名称访问
SDK_C_API const char* ogc_feature_get_field_as_string_by_name(...);
SDK_C_API int ogc_feature_set_field_string_by_name(...);
SDK_C_API int ogc_feature_get_field_as_integer_by_name(...);
SDK_C_API double ogc_feature_get_field_as_real_by_name(...);
```

**Java当前实现**: 所有native方法均为private，无公共方法；nativeDispose为空。

**应添加的公共方法**:
- getFid() / setFid()
- getDefn()
- getFieldCount()
- isFieldSet() / isFieldNull()
- getFieldAsInteger() / getFieldAsReal() / getFieldAsString()
- setFieldInteger() / setFieldReal() / setFieldString() / setFieldNull()
- getGeometry() / setGeometry() / stealGeometry()
- getEnvelope() / clone()
- 按名称访问方法

**注意**: FeatureInfo类已实现了Feature的大部分功能，但Feature和FeatureInfo职责重叠需明确。FeatureInfo更完整，Feature几乎不可用。

---

### S-03: DataSource类缺少公共方法和nativeDispose实现

**涉及类**: DataSource

**问题描述**: DataSource类声明了native方法但无公共方法，nativeDispose为空。C API中DataSource有完整的打开/关闭/层管理操作。

**C API定义**:
```c
SDK_C_API ogc_datasource_t* ogc_datasource_open(const char* path);
SDK_C_API void ogc_datasource_close(ogc_datasource_t* ds);
SDK_C_API int ogc_datasource_is_open(const ogc_datasource_t* ds);
SDK_C_API const char* ogc_datasource_get_path(const ogc_datasource_t* ds);
SDK_C_API size_t ogc_datasource_get_layer_count(const ogc_datasource_t* ds);
SDK_C_API ogc_layer_t* ogc_datasource_get_layer(const ogc_datasource_t* ds, size_t index);
SDK_C_API ogc_layer_t* ogc_datasource_get_layer_by_name(const ogc_datasource_t* ds, const char* name);
SDK_C_API ogc_layer_t* ogc_datasource_create_layer(ogc_datasource_t* ds, const char* name, int geom_type);
SDK_C_API int ogc_datasource_delete_layer(ogc_datasource_t* ds, const char* name);
```

**应添加的公共方法**:
```java
public static DataSource open(String path) { ... }
public void close() { ... }
public boolean isOpen() { ... }
public String getPath() { ... }
public long getLayerCount() { ... }
public Layer getLayer(long index) { ... }
public Layer getLayerByName(String name) { ... }
public Layer createLayer(String name, int geomType) { ... }
public int deleteLayer(String name) { ... }
```

---

### S-04: Color类缺少公共方法，设计不合理

**涉及类**: Color

**问题描述**: C API中Color是值类型（ogc_color_t结构体），但Java端将其设计为NativeObject（堆对象），且所有native方法均为private，无公共方法可用。

**C API定义**:
```c
typedef struct ogc_color_t {
    unsigned char r, g, b, a;
} ogc_color_t;
SDK_C_API ogc_color_t ogc_color_from_rgba(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
SDK_C_API ogc_color_t ogc_color_from_rgb(unsigned char r, unsigned char g, unsigned char b);
SDK_C_API ogc_color_t ogc_color_from_hex(unsigned int hex);
SDK_C_API unsigned int ogc_color_to_hex(const ogc_color_t* color);
SDK_C_API void ogc_color_to_rgba(const ogc_color_t* color, unsigned char* r, unsigned char* g, unsigned char* b, unsigned char* a);
```

**建议**: Color应为纯Java值类型，不继承NativeObject：
```java
public final class Color {
    private final int r, g, b, a;
    public static Color fromRGBA(int r, int g, int b, int a) { ... }
    public static Color fromRGB(int r, int g, int b) { ... }
    public static Color fromHex(int hex) { ... }
    public int toHex() { ... }
    public int getR() { ... }
    public int getG() { ... }
    public int getB() { ... }
    public int getA() { ... }
}
```

---

### S-05: FeatureDefn类缺少公共方法

**涉及类**: FeatureDefn

**问题描述**: FeatureDefn声明了native方法但无公共方法暴露。C API中FeatureDefn有完整的字段管理操作。

**C API定义**:
```c
SDK_C_API const char* ogc_feature_defn_get_name(const ogc_feature_defn_t* defn);
SDK_C_API size_t ogc_feature_defn_get_field_count(const ogc_feature_defn_t* defn);
SDK_C_API ogc_field_defn_t* ogc_feature_defn_get_field_defn(const ogc_feature_defn_t* defn, size_t index);
SDK_C_API int ogc_feature_defn_get_field_index(const ogc_feature_defn_t* defn, const char* name);
SDK_C_API void ogc_feature_defn_add_field_defn(ogc_feature_defn_t* defn, ogc_field_defn_t* field);
SDK_C_API size_t ogc_feature_defn_get_geom_field_count(const ogc_feature_defn_t* defn);
```

**应添加的公共方法**:
```java
public String getName() { ... }
public long getFieldCount() { ... }
public FieldDefn getFieldDefn(long index) { ... }
public int getFieldIndex(String name) { ... }
public void addFieldDefn(FieldDefn field) { ... }
public long getGeomFieldCount() { ... }
```

---

### S-06: DrawStyle/Pen/Brush/Font类缺少公共方法

**涉及类**: DrawStyle, Pen, Brush, Font

**问题描述**: 这些绘制类声明了native方法但无公共方法暴露。C API中有完整的样式操作。

**C API对应**:
- DrawStyle: getPen/getBrush/getFont/setPen/setBrush/setFont
- Pen: getColor/getWidth/setColor/setWidth/getStyle/setStyle
- Brush: getColor/setColor/getStyle/setStyle
- Font: getFamily/getSize/isBold/isItalic/setBold/setItalic

**应添加公共方法**: 每个类都需要暴露对应的getter/setter公共方法。

---

### S-07: Image类缺少公共方法

**涉及类**: Image

**问题描述**: Image类声明了native方法但无公共方法暴露。C API中有完整的图像操作。

**C API定义**:
```c
SDK_C_API int ogc_image_get_width(const ogc_image_t* image);
SDK_C_API int ogc_image_get_height(const ogc_image_t* image);
SDK_C_API int ogc_image_get_channels(const ogc_image_t* image);
SDK_C_API unsigned char* ogc_image_get_data(ogc_image_t* image);
SDK_C_API ogc_image_t* ogc_image_load_from_file(const char* path);
SDK_C_API int ogc_image_save_to_file(const ogc_image_t* image, const char* path);
```

**应添加公共方法**: getWidth/getHeight/getChannels/getData/loadFromFile/saveToFile

---

### S-08: RenderContext类缺少公共方法

**涉及类**: RenderContext

**问题描述**: RenderContext声明了native方法但无公共方法暴露。C API中DrawContext有完整的绘制操作。

**C API定义**:
```c
SDK_C_API void ogc_draw_context_begin_draw(ogc_draw_context_t* ctx);
SDK_C_API void ogc_draw_context_end_draw(ogc_draw_context_t* ctx);
SDK_C_API void ogc_draw_context_clear(ogc_draw_context_t* ctx, ogc_color_t color);
SDK_C_API void ogc_draw_context_draw_geometry(ogc_draw_context_t* ctx, const ogc_geometry_t* geom, const ogc_draw_style_t* style);
SDK_C_API void ogc_draw_context_draw_point/draw_line/draw_rect/fill_rect/draw_text(...);
SDK_C_API void ogc_draw_context_set_transform(ogc_draw_context_t* ctx, double* matrix);
SDK_C_API void ogc_draw_context_reset_transform(ogc_draw_context_t* ctx);
SDK_C_API void ogc_draw_context_clip(ogc_draw_context_t* ctx, const ogc_geometry_t* geom);
SDK_C_API void ogc_draw_context_reset_clip(ogc_draw_context_t* ctx);
```

**应添加公共方法**: beginDraw/endDraw/clear/drawGeometry/drawPoint/drawLine/drawRect/fillRect/drawText/setTransform/resetTransform/clip/resetClip

---

## 三、中等问题（建议修复）

### M-01: Geometry.buffer()缺少segments参数

**C API**: `ogc_geometry_buffer(geom, distance, segments)` - 3个参数
**Java**: `buffer(double distance)` - 1个参数

**问题**: C API支持指定缓冲区段数（segments），Java端丢失了此参数。应提供重载方法：
```java
public Geometry buffer(double distance, int segments) { ... }
public Geometry buffer(double distance) { return buffer(distance, 8); }
```

---

### M-02: Geometry.asText()/asGeoJSON()缺少precision参数

**C API**:
```c
SDK_C_API char* ogc_geometry_as_text(const ogc_geometry_t* geom, int precision);
SDK_C_API char* ogc_geometry_as_geojson(const ogc_geometry_t* geom, int precision);
```

**Java**: `asText()` / `asGeoJSON()` - 无precision参数

**问题**: 缺少精度控制。应提供重载：
```java
public String asText(int precision) { ... }
public String asText() { return asText(6); }
```

---

### M-03: Geometry.equals()缺少tolerance参数

**C API**: `ogc_geometry_equals(a, b, tolerance)` - 3个参数
**Java**: `equals(Geometry other)` - 1个参数（不含tolerance）

**问题**: 缺少容差参数。应提供重载：
```java
public boolean equals(Geometry other, double tolerance) { ... }
public boolean equals(Geometry other) { return equals(other, 0.0); }
```

---

### M-04: FieldDefn类型常量不完整

**C API定义**:
```c
OGC_FIELD_TYPE_INTEGER = 0
OGC_FIELD_TYPE_INTEGER64 = 1
OGC_FIELD_TYPE_REAL = 2
OGC_FIELD_TYPE_STRING = 3
OGC_FIELD_TYPE_BINARY = 4
OGC_FIELD_TYPE_DATE = 5
OGC_FIELD_TYPE_TIME = 6
OGC_FIELD_TYPE_DATETIME = 7
OGC_FIELD_TYPE_INTEGER_LIST = 8
OGC_FIELD_TYPE_REAL_LIST = 9
OGC_FIELD_TYPE_STRING_LIST = 10
```

**Java当前**:
```java
public static final int TYPE_INTEGER = 0;
public static final int TYPE_REAL = 1;    // 错误！应为2
public static final int TYPE_STRING = 2;  // 错误！应为3
```

**问题**: 
1. 类型常量值与C API不匹配（TYPE_REAL=1应为2, TYPE_STRING=2应为3）
2. 缺少INTEGER64/BINARY/DATE/TIME/DATETIME/INTEGER_LIST/REAL_LIST/STRING_LIST类型

---

### M-05: FieldValue.getAsDouble()命名与C API不一致

**C API**: `ogc_field_value_get_as_real()` → 返回double
**Java**: `getAsDouble()` 

**问题**: C API使用"real"命名，Java使用"double"。虽然功能正确，但与C API命名不一致。建议保持一致或使用Java惯用命名但添加注释说明。

---

### M-06: LinearRing.getType()返回LINESTRING而非独立类型

**Java当前**:
```java
@Override
public Type getType() {
    return Type.LINESTRING;  // LinearRing是LineString的子类
}
```

**C API**: LinearRing有独立的创建函数（ogc_linearring_create），但类型枚举中没有LINEARRING值。这是OGC SFAS的设计——LinearRing是LineString的特化。当前实现合理，但应在文档中说明。

---

### M-07: LayerManager.getLayerByName()未使用native方法

**Java当前**: 使用Java循环遍历实现getLayerByName：
```java
public Layer getLayerByName(String name) {
    long count = getLayerCount();
    for (long i = 0; i < count; i++) {
        Layer layer = getLayer(i);
        if (layer != null && name.equals(layer.getName())) {
            return layer;
        }
    }
    return null;
}
```

**C API**: `ogc_layer_manager_get_layer_by_name(mgr, name)` - 有原生实现

**问题**: Java端声明了`nativeGetLayerByName`但未使用，而是用Java循环实现。应使用native方法以提高性能。

---

## 四、轻微问题（建议改进）

### L-01: Envelope使用nativePtr但字段存储在Java端

**问题**: Envelope同时维护Java字段（minX/minY/maxX/maxY）和native指针。构造函数中两者同步，但通过nativePtr构造时（如Geometry.getEnvelope()返回），Java字段初始化为0，与native数据不一致。

**当前代码**:
```java
Envelope(long nativePtr) {
    this.minX = 0;  // 错误！应从native获取
    this.minY = 0;
    this.maxX = 0;
    this.maxY = 0;
    setNativePtr(nativePtr);
}
```

**建议**: 通过native方法获取实际值，或使用getWidth/getHeight等native方法替代Java字段。

---

### L-02: DrawEngine.initialize()返回boolean而非int

**C API**: `ogc_draw_engine_initialize(engine, device)` 返回int（0成功，非0失败）
**Java**: `nativeInitialize(long ptr, long devicePtr)` 返回boolean

**问题**: 返回值类型不匹配。C API用int表示错误码，Java端丢失了错误码信息。建议返回int。

---

### L-03: TileCache API与C API参数不匹配

**C API**: `ogc_tile_cache_has_tile(cache, key)` 使用TileKey结构体指针
**Java**: `hasTile(int x, int y, int z)` 使用三个int参数

**问题**: Java端未使用TileKey对象作为参数，虽然功能等价但API风格不一致。TileKey类已存在但未被TileCache使用。

---

### L-04: Feature与FeatureInfo职责重叠

**问题**: Feature和FeatureInfo两个类都封装了ogc_feature_t，但FeatureInfo功能完整而Feature几乎不可用。建议：
- 方案A: 删除Feature，统一使用FeatureInfo
- 方案B: Feature作为轻量级只读接口，FeatureInfo作为完整功能接口

---

### L-05: Coordinate未使用native指针

**C API**: Coordinate是值类型结构体（ogc_coordinate_t），有create/destroy/distance方法
**Java**: Coordinate是纯Java值对象，无native方法

**问题**: C API的`ogc_coordinate_distance()`未被Java端暴露。当前Java端Coordinate仅作为数据传输对象使用，这是合理的设计选择，但distance方法缺失。

---

## 五、正确实现的类

| 类名 | 评价 |
|------|------|
| Point | 完整实现，公共方法与C API一一对应 |
| LineString | 完整实现，包含所有C API方法 |
| Polygon | 完整实现，包含所有C API方法 |
| ChartViewer | 完整实现，包含异步操作和便捷方法 |
| Viewport | 完整实现，包含所有C API方法 |

---

## 六、问题汇总表

| ID | 严重度 | 类名 | 问题 | 状态 |
|----|--------|------|------|------|
| S-01 | 严重 | MultiPoint/MultiLineString/MultiPolygon/GeometryCollection | 缺少公共方法 | 待修复 |
| S-02 | 严重 | Feature | 缺少公共方法和nativeDispose | 待修复 |
| S-03 | 严重 | DataSource | 缺少公共方法和nativeDispose | 待修复 |
| S-04 | 严重 | Color | 设计不合理，缺少公共方法 | 待修复 |
| S-05 | 严重 | FeatureDefn | 缺少公共方法 | 待修复 |
| S-06 | 严重 | DrawStyle/Pen/Brush/Font | 缺少公共方法 | 待修复 |
| S-07 | 严重 | Image | 缺少公共方法 | 待修复 |
| S-08 | 严重 | RenderContext | 缺少公共方法 | 待修复 |
| M-01 | 中等 | Geometry | buffer()缺少segments参数 | 待修复 |
| M-02 | 中等 | Geometry | asText()/asGeoJSON()缺少precision参数 | 待修复 |
| M-03 | 中等 | Geometry | equals()缺少tolerance参数 | 待修复 |
| M-04 | 中等 | FieldDefn | 类型常量值错误且不完整 | 待修复 |
| M-05 | 中等 | FieldValue | getAsDouble()与C API命名不一致 | 待评估 |
| M-06 | 中等 | LinearRing | getType()返回LINESTRING | 已确认合理 |
| M-07 | 中等 | LayerManager | getLayerByName未使用native方法 | 待修复 |
| L-01 | 轻微 | Envelope | nativePtr构造时Java字段为0 | 待修复 |
| L-02 | 轻微 | DrawEngine | initialize()返回类型不匹配 | 待评估 |
| L-03 | 轻微 | TileCache | 未使用TileKey对象作为参数 | 待评估 |
| L-04 | 轻微 | Feature/FeatureInfo | 职责重叠 | 待决策 |
| L-05 | 轻微 | Coordinate | distance()方法缺失 | 待评估 |

---

## 七、修复优先级建议

1. **P0（立即修复）**: S-01 ~ S-08 — 所有"缺少公共方法"的问题，这些类当前对外不可用
2. **P1（尽快修复）**: M-04（FieldDefn类型常量错误）、M-01~M-03（参数缺失）
3. **P2（计划修复）**: M-07、L-01
4. **P3（评估后决定）**: M-05、M-06、L-02~L-05

---

**版本**: v1.0
