# ogc_proj 模块 - 头文件索引

## 模块描述

ogc_proj 是 OGC 图表系统的**坐标转换库**，提供坐标转换和投影变换能力。依赖 PROJ 库实现专业的 GIS 坐标系统转换，支持 WGS84、WebMercator、CGCS2000、北京54、西安80 等多种坐标系统。作为核心层的坐标转换服务，为导航、渲染、预警等模块提供统一的坐标转换接口。

## 核心特性

- **多坐标系支持**：WGS84、WebMercator、CGCS2000、北京54、西安80、GCJ02、BD09
- **PROJ库集成**：基于 PROJ 库实现高精度坐标转换
- **仿射变换**：支持矩阵变换的坐标转换
- **几何对象转换**：支持点、线、面等几何对象的批量转换
- **坐标系预设**：内置常用坐标系统配置
- **中国坐标系**：支持 GCJ02、BD09 等中国特有坐标系

---

## 头文件清单

| File | Description | Core Classes |
|------|-------------|--------------|
| [export.h](ogc/proj/export.h) | DLL导出宏定义 | `OGC_PROJ_API` |
| [transform_matrix.h](ogc/proj/transform_matrix.h) | 变换矩阵 | `TransformMatrix`, `Point`, `Rect` |
| [coordinate_transform.h](ogc/proj/coordinate_transform.h) | 坐标转换核心 | `CoordinateTransform` |
| [coordinate_transformer.h](ogc/proj/coordinate_transformer.h) | 转换器接口 | `CoordinateTransformer` |
| [proj_transformer.h](ogc/proj/proj_transformer.h) | PROJ转换器 | `ProjTransformer` |
| [coord_system_preset.h](ogc/proj/coord_system_preset.h) | 坐标系预设 | `CoordSystemPreset`, `CoordSystemInfo` |

---

## 类继承关系图

```
CoordinateTransformer (抽象接口)
    └── ProjTransformer (PROJ实现)

CoordinateTransform
    └── 仿射变换实现

CoordSystemPreset
    └── (单例模式)
```

---

## 依赖关系图

```
ogc_proj
    │
    ├── ogc_geometry (几何类型)
    │       ├── Geometry
    │       ├── Coordinate
    │       └── Envelope
    │
    └── PROJ (外部库)
            └── proj.h
```

---

## 文件分类

| Category | Files |
|----------|-------|
| **Transform** | coordinate_transform.h, transform_matrix.h |
| **Interface** | coordinate_transformer.h |
| **Implementation** | proj_transformer.h |
| **Preset** | coord_system_preset.h |
| **Build** | export.h |

---

## 关键类

### CoordinateTransformer (坐标转换器接口)

**File**: [coordinate_transformer.h](ogc/proj/coordinate_transformer.h)  
**Description**: 坐标转换器抽象接口

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

---

### ProjTransformer (PROJ转换器)

**File**: [proj_transformer.h](ogc/proj/proj_transformer.h)  
**Description**: 基于 PROJ 库的坐标转换器实现

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

---

### CoordinateTransform (坐标变换)

**File**: [coordinate_transform.h](ogc/proj/coordinate_transform.h)  
**Description**: 基于矩阵的仿射坐标变换

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

---

### CoordSystemPreset (坐标系预设)

**File**: [coord_system_preset.h](ogc/proj/coord_system_preset.h)  
**Description**: 常用坐标系统预设配置

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

---

### TransformMatrix (变换矩阵)

**File**: [transform_matrix.h](ogc/proj/transform_matrix.h)  
**Description**: 3x3 仿射变换矩阵

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

## 类型定义

### 常用坐标系 EPSG 代码

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

## 使用示例

### 坐标系转换

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
    
    // 批量转换
    double x[] = {121.5, 122.0, 122.5};
    double y[] = {31.2, 31.5, 31.8};
    transformer->TransformArray(x, y, 3);
}
```

### 世界坐标到屏幕坐标

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
```

### 中国坐标系转换

```cpp
#include <ogc/proj/coord_system_preset.h>

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
```

---

## 修改历史

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| v1.0 | 2026-04-06 | Team | 从 ogc_graph 拆分创建 |

---

**Generated**: 2026-04-07  
**Module Version**: v1.0  
**C++ Standard**: C++11
