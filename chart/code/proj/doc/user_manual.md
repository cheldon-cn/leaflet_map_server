# ogc_proj 模块用户手册

> **版本**: v1.0  
> **更新日期**: 2026-04-07  
> **适用范围**: OGC Chart 系统坐标转换模块

---

## 一、模块描述

ogc_proj 是 OGC 图表系统的**坐标转换库**，位于系统架构的核心层（Layer 2）。该模块提供坐标转换和投影变换能力，依赖 PROJ 库实现专业的 GIS 坐标系统转换，支持 WGS84、WebMercator、CGCS2000、北京54、西安80、GCJ02、BD09 等多种坐标系统。作为核心层的坐标转换服务，为导航、渲染、预警等模块提供统一的坐标转换接口。

### 1.1 模块定位

在 OGC Chart 系统架构中，ogc_proj 位于核心层：

```
┌─────────────────────────────────────────────────────────────┐
│                    渲染层 (graph, symbology)                │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    数据层 (cache, layer, feature)           │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    核心层 (ogc_proj, geom, draw) ← 当前模块 │
└─────────────────────────────────────────────────────────────┘
                              ↓
┌─────────────────────────────────────────────────────────────┐
│                    基础层 (base)                            │
└─────────────────────────────────────────────────────────────┘
```

---

## 二、核心特性

| 特性 | 说明 |
|------|------|
| **多坐标系支持** | WGS84、WebMercator、CGCS2000、北京54、西安80、GCJ02、BD09 |
| **PROJ库集成** | 基于 PROJ 库实现高精度坐标转换 |
| **仿射变换** | 支持矩阵变换的坐标转换 |
| **几何对象转换** | 支持点、线、面等几何对象的批量转换 |
| **坐标系预设** | 内置常用坐标系统配置 |
| **中国坐标系** | 支持 GCJ02、BD09 等中国特有坐标系 |

---

## 三、依赖关系

### 3.1 依赖库

| 依赖类型 | 库名称 | 说明 |
|----------|--------|------|
| 内部依赖 | ogc_geometry | 几何类型（Geometry, Coordinate, Envelope） |
| 外部依赖 | PROJ | 专业坐标转换库 |
| 外部依赖 | C++11 STL | 标准库 |

### 3.2 被依赖关系

ogc_proj 被以下模块依赖：
- ogc_cache（缓存）
- ogc_symbology（符号化）
- ogc_graph（地图渲染）
- ogc_navi（导航）
- ogc_alert（警报）

---

## 四、目录结构

```
proj/
├── include/ogc/proj/
│   ├── export.h
│   ├── coordinate_transform.h      # 坐标变换核心
│   ├── coordinate_transformer.h    # 转换器接口
│   ├── proj_transformer.h          # PROJ转换器实现
│   ├── coord_system_preset.h       # 坐标系预设
│   └── transform_matrix.h          # 变换矩阵
├── src/
│   ├── coordinate_transform.cpp
│   ├── coordinate_transformer.cpp
│   ├── proj_transformer.cpp
│   ├── coord_system_preset.cpp
│   └── transform_matrix.cpp
├── tests/
│   ├── CMakeLists.txt
│   ├── test_coordinate_transform.cpp
│   ├── test_coordinate_transformer.cpp
│   ├── test_proj_transformer.cpp
│   └── test_coord_system_preset.cpp
├── doc/
│   └── user_manual.md
└── CMakeLists.txt
```

---

## 五、关键类和主要方法

### 5.1 CoordinateTransformer（坐标转换器接口）

**头文件**: `<ogc/proj/coordinate_transformer.h>`

```cpp
namespace ogc {
namespace proj {

class CoordinateTransformer;
typedef std::shared_ptr<CoordinateTransformer> CoordinateTransformerPtr;

class OGC_PROJ_API CoordinateTransformer {
public:
    virtual ~CoordinateTransformer() = default;
    
    virtual bool IsValid() const = 0;
    
    // 坐标转换
    virtual ogc::Coordinate Transform(const ogc::Coordinate& coord) const = 0;
    virtual ogc::Coordinate TransformInverse(const ogc::Coordinate& coord) const = 0;
    virtual void Transform(double& x, double& y) const = 0;
    virtual void TransformInverse(double& x, double& y) const = 0;
    
    // 批量转换
    virtual void TransformArray(double* x, double* y, size_t count) const = 0;
    virtual void TransformArrayInverse(double* x, double* y, size_t count) const = 0;
    
    // 几何对象转换
    virtual ogc::Envelope Transform(const ogc::Envelope& env) const = 0;
    virtual ogc::GeometryPtr Transform(const ogc::Geometry* geometry) const = 0;
    
    // 元信息
    virtual std::string GetSourceCRS() const = 0;
    virtual std::string GetTargetCRS() const = 0;
    
    // 工厂方法
    static CoordinateTransformerPtr Create(const std::string& sourceCRS, const std::string& targetCRS);
    static CoordinateTransformerPtr CreateWGS84ToWebMercator();
    static CoordinateTransformerPtr CreateWebMercatorToWGS84();
};

} // namespace proj
} // namespace ogc
```

### 5.2 ProjTransformer（PROJ转换器）

**头文件**: `<ogc/proj/proj_transformer.h>`

```cpp
namespace ogc {
namespace proj {

class OGC_PROJ_API ProjTransformer : public CoordinateTransformer {
public:
    ProjTransformer();
    ProjTransformer(const std::string& sourceCRS, const std::string& targetCRS);
    virtual ~ProjTransformer();
    
    // CoordinateTransformer 接口实现
    virtual bool IsValid() const override;
    virtual ogc::Coordinate Transform(const ogc::Coordinate& coord) const override;
    virtual void Transform(double& x, double& y) const override;
    // ... 其他方法
    
    // 初始化
    bool Initialize(const std::string& sourceCRS, const std::string& targetCRS);
    
    // 工厂方法
    static ProjTransformerPtr Create(const std::string& sourceCRS, const std::string& targetCRS);
    static ProjTransformerPtr CreateFromEpsg(int sourceEpsg, int targetEpsg);
    static ProjTransformerPtr CreateFromWkt(const std::string& sourceWkt, const std::string& targetWkt);
    
    // PROJ 工具
    static bool IsProjAvailable();
    static std::string GetProjVersion();
};

} // namespace proj
} // namespace ogc
```

### 5.3 CoordinateTransform（坐标变换）

**头文件**: `<ogc/proj/coordinate_transform.h>`

```cpp
namespace ogc {
namespace proj {

class OGC_PROJ_API CoordinateTransform {
public:
    CoordinateTransform();
    CoordinateTransform(const TransformMatrix& matrix);
    CoordinateTransform(double offsetX, double offsetY, double scaleX, double scaleY);
    
    void SetMatrix(const TransformMatrix& matrix);
    TransformMatrix GetMatrix() const;
    
    void SetOffset(double offsetX, double offsetY);
    void SetScale(double scaleX, double scaleY);
    
    // 世界坐标到屏幕坐标
    void SetWorldToScreen(double worldMinX, double worldMinY, double worldMaxX, double worldMaxY,
                          double screenMinX, double screenMinY, double screenMaxX, double screenMaxY);
    
    // 坐标转换
    void Transform(double& x, double& y) const;
    void TransformInverse(double& x, double& y) const;
    
    ogc::Coordinate Transform(const ogc::Coordinate& coord) const;
    ogc::Envelope Transform(const ogc::Envelope& env) const;
    ogc::GeometryPtr Transform(const ogc::Geometry* geometry) const;
    
    // 批量转换
    void TransformArray(double* x, double* y, size_t count) const;
    
    // 工厂方法
    static CoordinateTransformPtr Create();
    static CoordinateTransformPtr CreateWorldToScreen(double worldMinX, double worldMinY, 
                                                       double worldMaxX, double worldMaxY,
                                                       double screenMinX, double screenMinY,
                                                       double screenMaxX, double screenMaxY);
};

} // namespace proj
} // namespace ogc
```

### 5.4 CoordSystemPreset（坐标系预设）

**头文件**: `<ogc/proj/coord_system_preset.h>`

```cpp
namespace ogc {
namespace proj {

struct CoordSystemInfo {
    std::string name;
    std::string epsg;
    std::string wkt;
    std::string projString;
    std::string description;
    std::string area;
    double accuracy;
};

class OGC_PROJ_API CoordSystemPreset {
public:
    static CoordSystemPreset& Instance();
    
    CoordSystemInfo GetCoordSystem(const std::string& name) const;
    std::vector<std::string> GetCoordSystemNames() const;
    std::vector<CoordSystemInfo> GetAllCoordSystems() const;
    
    void RegisterCoordSystem(const CoordSystemInfo& info);
    bool RemoveCoordSystem(const std::string& name);
    
    // 常用坐标系快捷方法
    static std::string WGS84();              // EPSG:4326
    static std::string WebMercator();        // EPSG:3857
    static std::string CGCS2000();           // EPSG:4490
    static std::string Beijing54();          // EPSG:4214
    static std::string Xian80();             // EPSG:4610
    static std::string GCJ02();              // GCJ02
    static std::string BD09();               // BD09
    
    // UTM 投影
    static std::string WGS84_UTM(int zone, bool north = true);
    static std::string CGCS2000_UTM(int zone, bool north = true);
    
    // 坐标系判断
    static bool IsChineseCoordSystem(const std::string& crs);
    static bool IsGeographicCRS(const std::string& crs);
    static bool IsProjectedCRS(const std::string& crs);
};

} // namespace proj
} // namespace ogc
```

### 5.5 TransformMatrix（变换矩阵）

**头文件**: `<ogc/proj/transform_matrix.h>`

```cpp
namespace ogc {
namespace proj {

struct Point {
    double x, y;
};

struct Rect {
    double x, y, w, h;
};

class OGC_PROJ_API TransformMatrix {
public:
    double m[3][3];
    
    TransformMatrix();
    TransformMatrix(double m00, double m01, double m02,
                    double m10, double m11, double m12);
    
    void LoadIdentity();
    void Reset();
    
    // 静态工厂方法
    static TransformMatrix Identity();
    static TransformMatrix Translate(double dx, double dy);
    static TransformMatrix Rotate(double angleRadians);
    static TransformMatrix Scale(double sx, double sy);
    
    // 矩阵运算
    TransformMatrix Multiply(const TransformMatrix& other) const;
    TransformMatrix Inverse() const;
    
    // 点变换
    Point Transform(const Point& p) const;
    Rect Transform(const Rect& r) const;
};

} // namespace proj
} // namespace ogc
```

---

## 六、使用示例

### 6.1 坐标系转换

```cpp
#include <ogc/proj/proj_transformer.h>
#include <ogc/proj/coord_system_preset.h>

using namespace ogc::proj;

// WGS84 转 WebMercator
auto transformer = ProjTransformer::Create(
    CoordSystemPreset::WGS84(),
    CoordSystemPreset::WebMercator()
);

if (transformer->IsValid()) {
    // 单点转换
    ogc::Coordinate wgs84(121.5, 31.2);
    ogc::Coordinate mercator = transformer->Transform(wgs84);
    
    std::cout << "WGS84: (" << wgs84.x << ", " << wgs84.y << ")" << std::endl;
    std::cout << "Mercator: (" << mercator.x << ", " << mercator.y << ")" << std::endl;
    
    // 批量转换
    double x[] = {121.5, 122.0, 122.5};
    double y[] = {31.2, 31.5, 31.8};
    transformer->TransformArray(x, y, 3);
}
```

### 6.2 世界坐标到屏幕坐标

```cpp
#include <ogc/proj/coordinate_transform.h>

using namespace ogc::proj;

// 创建世界坐标到屏幕坐标的变换
auto transform = CoordinateTransform::CreateWorldToScreen(
    120.0, 30.0,  // 世界坐标最小值
    122.0, 32.0,  // 世界坐标最大值
    0, 0,         // 屏幕坐标最小值
    800, 600      // 屏幕坐标最大值
);

// 转换坐标
double screenX = 121.0, screenY = 31.0;
transform->Transform(screenX, screenY);
// screenX, screenY 现在是屏幕坐标

// 反向转换（屏幕坐标到世界坐标）
transform->TransformInverse(screenX, screenY);
```

### 6.3 中国坐标系转换

```cpp
#include <ogc/proj/coord_system_preset.h>
#include <ogc/proj/proj_transformer.h>

using namespace ogc::proj;

// 判断坐标系类型
std::string crs = "GCJ02";
if (CoordSystemPreset::IsChineseCoordSystem(crs)) {
    std::cout << "这是中国特有坐标系" << std::endl;
}

// 获取坐标系信息
auto info = CoordSystemPreset::Instance().GetCoordSystem("WGS84");
std::cout << "Name: " << info.name << std::endl;
std::cout << "EPSG: " << info.epsg << std::endl;
std::cout << "Description: " << info.description << std::endl;

// GCJ02 转 WGS84
auto gcj02ToWgs84 = ProjTransformer::Create(
    CoordSystemPreset::GCJ02(),
    CoordSystemPreset::WGS84()
);

ogc::Coordinate gcj02(121.5, 31.2);
ogc::Coordinate wgs84 = gcj02ToWgs84->Transform(gcj02);
```

### 6.4 几何对象转换

```cpp
#include <ogc/proj/proj_transformer.h>

using namespace ogc::proj;

// 创建转换器
auto transformer = ProjTransformer::Create(
    CoordSystemPreset::WGS84(),
    CoordSystemPreset::WebMercator()
);

// 转换线几何对象
ogc::LineString* line = ogc::LineString::Create();
line->AddPoint(121.0, 31.0);
line->AddPoint(121.5, 31.5);
line->AddPoint(122.0, 32.0);

ogc::GeometryPtr transformed = transformer->Transform(line);

// 转换包络矩形
ogc::Envelope env(120.0, 30.0, 122.0, 32.0);
ogc::Envelope transformedEnv = transformer->Transform(env);
```

---

## 七、常用坐标系 EPSG 代码

| 坐标系 | EPSG代码 | 说明 |
|--------|----------|------|
| WGS84 | EPSG:4326 | 全球通用地理坐标系 |
| WebMercator | EPSG:3857 | Web地图投影 |
| CGCS2000 | EPSG:4490 | 中国大地坐标系2000 |
| Beijing54 | EPSG:4214 | 北京54坐标系 |
| Xian80 | EPSG:4610 | 西安80坐标系 |
| GCJ02 | GCJ02 | 火星坐标系（中国加密） |
| BD09 | BD09 | 百度坐标系 |

---

## 八、编译和集成

### 8.1 CMake 配置

```cmake
# 添加 proj 模块
add_subdirectory(proj)

# 链接 proj 库
target_link_libraries(your_target PRIVATE ogc_proj)

# 链接 PROJ 库
find_package(PROJ REQUIRED)
target_link_libraries(your_target PRIVATE ${PROJ_LIBRARIES})
```

### 8.2 头文件包含

```cpp
#include <ogc/proj/coordinate_transformer.h>
#include <ogc/proj/proj_transformer.h>
#include <ogc/proj/coordinate_transform.h>
#include <ogc/proj/coord_system_preset.h>
#include <ogc/proj/transform_matrix.h>
```

---

## 九、注意事项

1. **PROJ 库依赖**：使用 ProjTransformer 前需确保 PROJ 库已正确安装
2. **坐标系字符串**：支持 EPSG 代码（如 "EPSG:4326"）和 WKT 字符串
3. **中国坐标系**：GCJ02 和 BD09 为非标准坐标系，PROJ 库可能不支持，需特殊处理
4. **精度问题**：坐标转换存在精度损失，重要应用需进行精度验证
5. **性能优化**：批量转换使用 TransformArray 方法，避免循环调用单点转换

---

## 十、版本历史

| 版本 | 日期 | 变更说明 |
|------|------|----------|
| v1.0 | 2026-04-06 | 从 ogc_graph 拆分创建，包含坐标转换和投影变换功能 |

---

**文档维护**: OGC Chart 开发团队  
**技术支持**: 参见项目 README.md
