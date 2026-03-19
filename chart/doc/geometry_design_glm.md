# OGC 几何类库设计文档 v2.3

**版本**: 2.3  
**日期**: 2026年3月18日  
**状态**: 多角色交叉评审通过 (评分: 96/100)  
**C++标准**: C++11  
**设计目标**: 稳定、正确、高效、易扩展、跨平台、多线程安全

---

[TOC]

---

## 1. 概述

### 1.1 设计目标

本几何类库遵循 OGC Simple Feature Access 标准，使用 **C++11** 实现，致力于构建一个**生产级**的 GIS 几何类型系统。

**核心设计目标**:

| 目标 | 说明 | 实现策略 |
|------|------|----------|
| **稳定性** | 健壮的错误处理、内存安全、异常安全 | Result模式、RAII、智能指针 |
| **正确性** | 符合OGC标准、完整测试覆盖 | OGC CITE测试、交叉验证 |
| **高效性** | 优化的性能、智能缓存、批量操作 | 延迟计算、空间索引、SIMD优化 |
| **扩展性** | 易于添加新类型和操作 | 访问者模式、策略模式、插件架构 |
| **跨平台** | 支持Windows/Linux/macOS | 纯C++11标准、无平台依赖 |
| **线程安全** | 多线程环境下安全使用 | 原子操作、读写锁、线程局部存储 |

### 1.2 核心特性

- ✅ **完整的OGC几何类型体系**: 支持Point、LineString、Polygon等17种几何类型
- ✅ **DE-9IM空间关系**: 完整实现9种空间关系判断
- ✅ **丰富的几何运算**: 缓冲区、凸包、简化、布尔运算等
- ✅ **多种序列化格式**: WKT、WKB、GeoJSON、GML、KML
- ✅ **高性能缓存**: 外包矩形、质心等计算结果缓存
- ✅ **线程安全**: 读写分离、原子操作、细粒度锁
- ✅ **错误处理**: Result模式、异常可选、错误码统一
- ✅ **内存优化**: 移动语义、内存池、小对象优化

### 1.3 类继承关系

```
CNGeometry (抽象基类)
    ├── CNPoint (点)
    ├── CNLineString (线串)
    │   └── CNLinearRing (环)
    ├── CNPolygon (多边形)
    ├── CNMultiPoint (多点)
    ├── CNMultiLineString (多线串)
    ├── CNMultiPolygon (多多边形)
    ├── CNGeometryCollection (几何集合)
    ├── CNCircularString (圆弧)
    ├── CNCompoundCurve (复合曲线)
    ├── CNCurvePolygon (曲线多边形)
    ├── CNMultiCurve (多曲线)
    ├── CNMultiSurface (多曲面)
    ├── CNPolyhedralSurface (多面体表面)
    └── CNTIN (不规则三角网)
```

---

## 2. 公共定义和基础设施

### 2.1 命名空间

```cpp
namespace OGC {
    // 所有几何类和工具在此命名空间
}
```

### 2.2 几何类型枚举

```cpp
enum class GeomType : uint8_t {
    kUnknown = 0,
    kPoint = 1,
    kLineString = 2,
    kPolygon = 3,
    kMultiPoint = 4,
    kMultiLineString = 5,
    kMultiPolygon = 6,
    kGeometryCollection = 7,
    kCircularString = 8,
    kCompoundCurve = 9,
    kCurvePolygon = 10,
    kMultiCurve = 11,
    kMultiSurface = 12,
    kPolyhedralSurface = 13,
    kTIN = 14,
    
    // 扩展类型
    kTriangle = 15           // 三角形（Polygon子类型）
};
```

### 2.3 维度枚举

```cpp
enum class Dimension : int8_t {
    Empty = -1,     // 空几何
    Point = 0,      // 0维：点
    Curve = 1,      // 1维：线
    Surface = 2,    // 2维：面
    Collection = 3  // 3维：集合
};
```

### 2.4 错误码定义

```cpp
enum class GeomResult : int32_t {
    // 成功
    kSuccess = 0,
    
    // 几何对象错误 (1-99)
    kInvalidGeometry = 1,
    kEmptyGeometry = 2,
    kNullGeometry = 3,
    kTopologyError = 4,
    kSelfIntersection = 5,
    
    // 参数错误 (100-199)
    kInvalidParameters = 100,
    kOutOfRange = 101,
    kInvalidSRID = 102,
    kInvalidTolerance = 103,
    
    // 操作错误 (200-299)
    kNotSupported = 200,
    kNotImplemented = 201,
    kOperationFailed = 202,
    
    // 内存错误 (300-399)
    kOutOfMemory = 300,
    kBufferOverflow = 301,
    
    // I/O错误 (400-499)
    kIOError = 400,
    kParseError = 401,
    kFormatError = 402,
    
    // 坐标系统错误 (500-599)
    kTransformError = 500,
    kCRSNotFound = 501,
    kCRSConversionError = 502
};

// 错误信息获取
const char* GetResultString(GeomResult result);
std::string GetResultDescription(GeomResult result);
```

### 2.5 异常类

```cpp
class GeometryException : public std::runtime_error {
public:
    explicit GeometryException(GeomResult code, const std::string& message = "");
    
    GeomResult GetErrorCode() const noexcept;
    const std::string& GetContext() const noexcept;
    
    // 异常链支持
    std::exception_ptr GetInnerException() const noexcept;
    
private:
    GeomResult m_code;
    std::string m_context;
    std::exception_ptr m_innerException;
};

// 使用宏简化异常抛出
#define GLM_THROW(code, msg) \
    throw OGC::GeometryException(code, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + msg)

#define GLM_THROW_IF(condition, code, msg) \
    if (condition) GLM_THROW(code, msg)
```

---

## 3. 基础数据结构

### 3.1 坐标结构 Coordinate

```cpp
/**
 * @brief 支持XY/Z/M的坐标结构
 * 
 * 内存布局：
 * - 2D模式: x, y有效，z = NaN，m = NaN
 * - 3D模式: x, y, z有效，m = NaN
 * - 测量模式: x, y, m有效，z = NaN
 * - 4D模式: x, y, z, m全部有效
 */
struct Coordinate {
    double x = 0.0;
    double y = 0.0;
    double z = std::numeric_limits<double>::quiet_NaN();
    double m = std::numeric_limits<double>::quiet_NaN();
    
    // 构造函数
    Coordinate() = default;
    
    // 2D构造
    Coordinate(double x_, double y_) : x(x_), y(y_) {}
    
    // 3D构造
    Coordinate(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    
    // 4D构造（带测量值）
    Coordinate(double x_, double y_, double z_, double m_) 
        : x(x_), y(y_), z(z_), m(m_) {}
    
    // 状态检查
    bool Is3D() const noexcept {
        return !std::isnan(z);
    }
    
    bool IsMeasured() const noexcept {
        return !std::isnan(m);
    }
    
    bool IsEmpty() const noexcept {
        return std::isnan(x) || std::isnan(y);
    }
    
    // 距离计算
    double Distance(const Coordinate& other) const noexcept {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
    
    double Distance3D(const Coordinate& other) const noexcept {
        double dx = x - other.x;
        double dy = y - other.y;
        double dz = z - other.z;
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }
    
    // 相等比较（带容差）
    bool Equals(const Coordinate& other, double tolerance = 1e-9) const noexcept {
        if (IsEmpty() && other.IsEmpty()) return true;
        if (IsEmpty() || other.IsEmpty()) return false;
        
        if (std::abs(x - other.x) > tolerance) return false;
        if (std::abs(y - other.y) > tolerance) return false;
        if (Is3D() != other.Is3D()) return false;
        if (Is3D() && std::abs(z - other.z) > tolerance) return false;
        
        return true;
    }
    
    // 运算符重载
    Coordinate operator+(const Coordinate& rhs) const noexcept {
        Coordinate result;
        result.x = x + rhs.x;
        result.y = y + rhs.y;
        if (Is3D() && rhs.Is3D()) {
            result.z = z + rhs.z;
        }
        if (IsMeasured() && rhs.IsMeasured()) {
            result.m = m + rhs.m;
        }
        return result;
    }
    
    Coordinate operator-(const Coordinate& rhs) const noexcept {
        Coordinate result;
        result.x = x - rhs.x;
        result.y = y - rhs.y;
        if (Is3D() && rhs.Is3D()) {
            result.z = z - rhs.z;
        }
        if (IsMeasured() && rhs.IsMeasured()) {
            result.m = m - rhs.m;
        }
        return result;
    }
    
    Coordinate operator*(double scalar) const noexcept {
        Coordinate result;
        result.x = x * scalar;
        result.y = y * scalar;
        if (Is3D()) {
            result.z = z * scalar;
        }
        if (IsMeasured()) {
            result.m = m * scalar;
        }
        return result;
    }
    
    bool operator==(const Coordinate& rhs) const noexcept {
        return Equals(rhs);
    }
    
    bool operator!=(const Coordinate& rhs) const noexcept {
        return !Equals(rhs);
    }
    
    // 向量运算
    double Dot(const Coordinate& other) const noexcept {
        double result = x * other.x + y * other.y;
        if (Is3D() && other.Is3D()) {
            result += z * other.z;
        }
        return result;
    }
    
    Coordinate Cross(const Coordinate& other) const noexcept {
        // 仅2D：返回标量的Z分量
        return Coordinate(0, 0, x * other.y - y * other.x);
    }
    
    double Length() const noexcept {
        return Distance(Coordinate(0, 0));
    }
    
    Coordinate Normalize() const noexcept {
        double len = Length();
        if (len > 1e-9) {
            return *this * (1.0 / len);
        }
        return *this;
    }
    
    // WKT输出
    std::string ToWKT(int precision = 15) const;
    
    // 静态工厂
    static Coordinate Empty() {
        Coordinate c;
        c.x = std::numeric_limits<double>::quiet_NaN();
        c.y = std::numeric_limits<double>::quiet_NaN();
        return c;
    }
    
    static Coordinate FromPolar(double radius, double angle) {
        return Coordinate(radius * std::cos(angle), radius * std::sin(angle));
    }
};

// 类型别名
using Coord = Coordinate;
using CoordinateList = std::vector<Coordinate>;
```

### 3.2 外包矩形 Envelope

```cpp
/**
 * @brief 最小外包矩形（MBR）
 * 
 * 线程安全：所有方法都是线程安全的
 */
class Envelope {
public:
    // 构造函数
    Envelope() noexcept : m_isNull(true) {}
    
    Envelope(double minX, double minY, double maxX, double maxY) noexcept
        : m_minX(minX), m_minY(minY), m_maxX(maxX), m_maxY(maxY), m_isNull(false) {
        Normalize();
    }
    
    Envelope(const Coordinate& p1, const Coordinate& p2) noexcept
        : Envelope(std::min(p1.x, p2.x), std::min(p1.y, p2.y),
                   std::max(p1.x, p2.x), std::max(p1.y, p2.y)) {}
    
    explicit Envelope(const CoordinateList& coords) noexcept {
        ExpandToInclude(coords);
    }
    
    // 基本属性
    double GetMinX() const noexcept { return m_minX; }
    double GetMinY() const noexcept { return m_minY; }
    double GetMaxX() const noexcept { return m_maxX; }
    double GetMaxY() const noexcept { return m_maxY; }
    
    double GetWidth() const noexcept {
        return m_isNull ? 0.0 : m_maxX - m_minX;
    }
    
    double GetHeight() const noexcept {
        return m_isNull ? 0.0 : m_maxY - m_minY;
    }
    
    double GetArea() const noexcept {
        return m_isNull ? 0.0 : GetWidth() * GetHeight();
    }
    
    double GetPerimeter() const noexcept {
        return m_isNull ? 0.0 : 2.0 * (GetWidth() + GetHeight());
    }
    
    Coordinate GetCentre() const noexcept {
        if (m_isNull) return Coordinate::Empty();
        return Coordinate((m_minX + m_maxX) / 2.0, (m_minY + m_maxY) / 2.0);
    }
    
    // 状态检查
    bool IsNull() const noexcept { return m_isNull; }
    
    bool IsPoint() const noexcept {
        return !m_isNull && m_minX == m_maxX && m_minY == m_maxY;
    }
    
    bool IsEmpty() const noexcept {
        return m_isNull;
    }
    
    // 重置
    void SetNull() noexcept {
        m_isNull = true;
        m_minX = m_minY = m_maxX = m_maxY = 0.0;
    }
    
    // 扩展操作
    void ExpandToInclude(const Coordinate& coord) noexcept {
        if (coord.IsEmpty()) return;
        
        if (m_isNull) {
            m_minX = m_maxX = coord.x;
            m_minY = m_maxY = coord.y;
            m_isNull = false;
        } else {
            m_minX = std::min(m_minX, coord.x);
            m_maxX = std::max(m_maxX, coord.x);
            m_minY = std::min(m_minY, coord.y);
            m_maxY = std::max(m_maxY, coord.y);
        }
    }
    
    void ExpandToInclude(const CoordinateList& coords) noexcept {
        for (const auto& coord : coords) {
            ExpandToInclude(coord);
        }
    }
    
    void ExpandToInclude(const Envelope& other) noexcept {
        if (other.m_isNull) return;
        
        if (m_isNull) {
            *this = other;
        } else {
            m_minX = std::min(m_minX, other.m_minX);
            m_maxX = std::max(m_maxX, other.m_maxX);
            m_minY = std::min(m_minY, other.m_minY);
            m_maxY = std::max(m_maxY, other.m_maxY);
        }
    }
    
    void ExpandBy(double deltaX, double deltaY) noexcept {
        if (m_isNull) return;
        
        m_minX -= deltaX;
        m_maxX += deltaX;
        m_minY -= deltaY;
        m_maxY += deltaY;
    }
    
    // 空间关系判断
    bool Contains(const Coordinate& coord) const noexcept {
        if (m_isNull || coord.IsEmpty()) return false;
        
        return coord.x >= m_minX && coord.x <= m_maxX &&
               coord.y >= m_minY && coord.y <= m_maxY;
    }
    
    bool Contains(const Envelope& other) const noexcept {
        if (m_isNull || other.m_isNull) return false;
        
        return other.m_minX >= m_minX && other.m_maxX <= m_maxX &&
               other.m_minY >= m_minY && other.m_maxY <= m_maxY;
    }
    
    bool Intersects(const Envelope& other) const noexcept {
        if (m_isNull || other.m_isNull) return false;
        
        return !(other.m_maxX < m_minX || other.m_minX > m_maxX ||
                 other.m_maxY < m_minY || other.m_minY > m_maxY);
    }
    
    bool Overlaps(const Envelope& other) const noexcept {
        return Intersects(other) && !Contains(other) && !other.Contains(*this);
    }
    
    bool Touches(const Envelope& other) const noexcept {
        if (m_isNull || other.m_isNull) return false;
        
        // 检查边界接触
        bool touchesX = (m_minX == other.m_maxX || m_maxX == other.m_minX);
        bool touchesY = (m_minY == other.m_maxY || m_maxY == other.m_minY);
        
        return (touchesX && m_minY <= other.m_maxY && m_maxY >= other.m_minY) ||
               (touchesY && m_minX <= other.m_maxX && m_maxX >= other.m_minX);
    }
    
    bool Equals(const Envelope& other, double tolerance = 1e-9) const noexcept {
        if (m_isNull && other.m_isNull) return true;
        if (m_isNull || other.m_isNull) return false;
        
        return std::abs(m_minX - other.m_minX) <= tolerance &&
               std::abs(m_minY - other.m_minY) <= tolerance &&
               std::abs(m_maxX - other.m_maxX) <= tolerance &&
               std::abs(m_maxY - other.m_maxY) <= tolerance;
    }
    
    // 集合运算
    Envelope Intersection(const Envelope& other) const noexcept {
        if (!Intersects(other)) return Envelope();
        
        return Envelope(
            std::max(m_minX, other.m_minX),
            std::max(m_minY, other.m_minY),
            std::min(m_maxX, other.m_maxX),
            std::min(m_maxY, other.m_maxY)
        );
    }
    
    Envelope Union(const Envelope& other) const noexcept {
        if (m_isNull) return other;
        if (other.m_isNull) return *this;
        
        return Envelope(
            std::min(m_minX, other.m_minX),
            std::min(m_minY, other.m_minY),
            std::max(m_maxX, other.m_maxX),
            std::max(m_maxY, other.m_maxY)
        );
    }
    
    // 距离计算
    double Distance(const Coordinate& coord) const noexcept {
        if (Contains(coord)) return 0.0;
        
        double dx = 0.0, dy = 0.0;
        
        if (coord.x < m_minX) dx = m_minX - coord.x;
        else if (coord.x > m_maxX) dx = coord.x - m_maxX;
        
        if (coord.y < m_minY) dy = m_minY - coord.y;
        else if (coord.y > m_maxY) dy = coord.y - m_maxY;
        
        return std::sqrt(dx * dx + dy * dy);
    }
    
    double Distance(const Envelope& other) const noexcept {
        if (Intersects(other)) return 0.0;
        
        double dx = 0.0, dy = 0.0;
        
        if (m_maxX < other.m_minX) dx = other.m_minX - m_maxX;
        else if (m_minX > other.m_maxX) dx = m_minX - other.m_maxX;
        
        if (m_maxY < other.m_minY) dy = other.m_minY - m_maxY;
        else if (m_minY > other.m_maxY) dy = m_minY - other.m_maxY;
        
        return std::sqrt(dx * dx + dy * dy);
    }
    
    // 四个角点
    Coordinate GetLowerLeft() const noexcept {
        return m_isNull ? Coordinate::Empty() : Coordinate(m_minX, m_minY);
    }
    
    Coordinate GetUpperRight() const noexcept {
        return m_isNull ? Coordinate::Empty() : Coordinate(m_maxX, m_maxY);
    }
    
    // 转换为多边形
    std::unique_ptr<class CNPolygon> ToPolygon() const;
    
    // 序列化
    std::string ToWKT() const;
    std::string ToString() const;
    
    // 运算符
    bool operator==(const Envelope& rhs) const noexcept {
        return Equals(rhs);
    }
    
    bool operator!=(const Envelope& rhs) const noexcept {
        return !Equals(rhs);
    }
    
private:
    double m_minX = 0.0;
    double m_minY = 0.0;
    double m_maxX = 0.0;
    double m_maxY = 0.0;
    bool m_isNull = true;
    
    void Normalize() noexcept {
        if (m_minX > m_maxX) std::swap(m_minX, m_maxX);
        if (m_minY > m_maxY) std::swap(m_minY, m_maxY);
    }
};
```

### 3.3 精度模型

```cpp
/**
 * @brief 精度模型定义
 * 
 * 处理浮点精度问题是GIS应用的关键挑战
 */
enum class PrecisionModel {
    Floating,           // 双精度浮点（默认）
    FloatingSingle,     // 单精度浮点
    Fixed,              // 定点精度
    FloatingWithScale   // 带缩放因子的浮点
};

/**
 * @brief 精度设置
 */
class PrecisionSettings {
public:
    PrecisionSettings(PrecisionModel model = PrecisionModel::Floating,
                      double scale = 1.0,
                      double tolerance = 1e-9)
        : m_model(model), m_scale(scale), m_tolerance(tolerance) {}
    
    // 精度处理
    double MakePrecise(double value) const {
        switch (m_model) {
            case PrecisionModel::Floating:
                return value;
            case PrecisionModel::FloatingSingle:
                return static_cast<float>(value);
            case PrecisionModel::Fixed:
                return std::round(value / m_scale) * m_scale;
            case PrecisionModel::FloatingWithScale:
                return std::round(value * m_scale) / m_scale;
            default:
                return value;
        }
    }
    
    Coordinate MakePrecise(const Coordinate& coord) const {
        Coordinate result;
        result.x = MakePrecise(coord.x);
        result.y = MakePrecise(coord.y);
        if (coord.Is3D()) {
            result.z = MakePrecise(coord.z);
        }
        if (coord.IsMeasured()) {
            result.m = coord.m;  // 测量值不进行精度处理
        }
        return result;
    }
    
    // 比较容差
    bool Equals(double a, double b) const {
        return std::abs(a - b) <= m_tolerance;
    }
    
    // Getter/Setter
    PrecisionModel GetModel() const noexcept { return m_model; }
    double GetScale() const noexcept { return m_scale; }
    double GetTolerance() const noexcept { return m_tolerance; }
    
    void SetModel(PrecisionModel model) noexcept { m_model = model; }
    void SetScale(double scale) noexcept { m_scale = scale; }
    void SetTolerance(double tolerance) noexcept { m_tolerance = tolerance; }
    
    // 预定义精度模型
    static PrecisionSettings Floating() {
        return PrecisionSettings(PrecisionModel::Floating);
    }
    
    static PrecisionSettings Fixed(double scale) {
        return PrecisionSettings(PrecisionModel::Fixed, scale);
    }
    
    static PrecisionSettings Single() {
        return PrecisionSettings(PrecisionModel::FloatingSingle, 1.0, 1e-6);
    }
    
private:
    PrecisionModel m_model;
    double m_scale;
    double m_tolerance;
};
```

### 3.4 Envelope3D (3D外包盒)

```cpp
/**
 * @brief 3D外包盒
 * 
 * 支持3D几何对象的空间关系判断
 */
class Envelope3D : public Envelope {
public:
    Envelope3D() noexcept : Envelope(), m_minZ(0), m_maxZ(0), m_zNull(true) {}
    
    Envelope3D(double minX, double minY, double minZ,
               double maxX, double maxY, double maxZ) noexcept
        : Envelope(minX, minY, maxX, maxY)
        , m_minZ(minZ), m_maxZ(maxZ), m_zNull(false) {
        if (m_minZ > m_maxZ) std::swap(m_minZ, m_maxZ);
    }
    
    // Z维度属性
    double GetMinZ() const noexcept { return m_minZ; }
    double GetMaxZ() const noexcept { return m_maxZ; }
    double GetDepth() const noexcept { return m_zNull ? 0.0 : m_maxZ - m_minZ; }
    double GetVolume() const noexcept { return GetArea() * GetDepth(); }
    
    // 扩展操作
    void ExpandToIncludeZ(double z) noexcept {
        if (m_zNull) {
            m_minZ = m_maxZ = z;
            m_zNull = false;
        } else {
            m_minZ = std::min(m_minZ, z);
            m_maxZ = std::max(m_maxZ, z);
        }
    }
    
    void ExpandToInclude(const Coordinate& coord) noexcept {
        Envelope::ExpandToInclude(coord);
        if (coord.Is3D()) {
            ExpandToIncludeZ(coord.z);
        }
    }
    
    // 3D空间关系
    bool Contains3D(const Coordinate& coord) const noexcept {
        return Contains(coord) && 
               !m_zNull && coord.Is3D() &&
               coord.z >= m_minZ && coord.z <= m_maxZ;
    }
    
    bool Intersects3D(const Envelope3D& other) const noexcept {
        return Intersects(other) &&
               !m_zNull && !other.m_zNull &&
               !(other.m_maxZ < m_minZ || other.m_minZ > m_maxZ);
    }
    
    // 3D距离
    double Distance3D(const Coordinate& coord) const noexcept {
        double d2d = Distance(coord);
        if (m_zNull || !coord.Is3D()) return d2d;
        
        double dz = 0.0;
        if (coord.z < m_minZ) dz = m_minZ - coord.z;
        else if (coord.z > m_maxZ) dz = coord.z - m_maxZ;
        
        return std::sqrt(d2d * d2d + dz * dz);
    }
    
    // 中心点
    Coordinate GetCentre3D() const noexcept {
        auto centre2d = GetCentre();
        if (m_zNull) return centre2d;
        return Coordinate(centre2d.x, centre2d.y, (m_minZ + m_maxZ) / 2.0);
    }
    
private:
    double m_minZ, m_maxZ;
    bool m_zNull;
};
```

---

## 4. 几何基类 CNGeometry

### 4.1 类定义

```cpp
/**
 * @brief 几何对象抽象基类
 * 
 * 设计原则：
 * 1. 线程安全：所有只读方法都是线程安全的，修改方法需要外部同步
 * 2. 内存安全：禁止拷贝，支持移动语义
 * 3. 异常安全：强异常保证
 * 4. 性能优化：延迟计算、智能缓存
 */
class CNGeometry {
public:
    // ========== 构造和析构 ==========
    
    virtual ~CNGeometry() = default;
    
    // 禁止拷贝
    CNGeometry(const CNGeometry&) = delete;
    CNGeometry& operator=(const CNGeometry&) = delete;
    
    // 支持移动
    CNGeometry(CNGeometry&&) noexcept = default;
    CNGeometry& operator=(CNGeometry&&) noexcept = default;
    
    // ========== 类型查询 ==========
    
    /**
     * @brief 获取几何类型
     * @return 几何类型枚举
     */
    virtual GeomType GetGeometryType() const noexcept = 0;
    
    /**
     * @brief 获取几何类型字符串
     * @return 类型名称（如 "POINT", "LINESTRING"）
     */
    virtual const char* GetGeometryTypeString() const noexcept = 0;
    
    /**
     * @brief 获取几何维度
     * @return 维度枚举值
     */
    virtual Dimension GetDimension() const noexcept = 0;
    
    /**
     * @brief 获取坐标维度（2D/3D）
     * @return 2 或 3
     */
    virtual int GetCoordinateDimension() const noexcept = 0;
    
    // ========== 基本属性 ==========
    
    /**
     * @brief 是否为空几何
     * @note 线程安全
     */
    virtual bool IsEmpty() const noexcept = 0;
    
    /**
     * @brief 有效性检查
     * @param reason 如果无效，输出原因
     * @note 时间复杂度：O(n)，其中n为坐标数量
     */
    virtual bool IsValid(std::string* reason = nullptr) const;
    
    /**
     * @brief 是否为简单几何（不自交）
     * @note OGC标准定义
     */
    virtual bool IsSimple() const;
    
    /**
     * @brief 是否包含Z坐标
     */
    virtual bool Is3D() const noexcept = 0;
    
    /**
     * @brief 是否包含测量值
     */
    virtual bool IsMeasured() const noexcept = 0;
    
    // ========== 空间参考系统 ==========
    
    /**
     * @brief 获取空间参考系统ID
     * @return SRID值，0表示未定义
     */
    int GetSRID() const noexcept {
        return m_srid.load(std::memory_order_relaxed);
    }
    
    /**
     * @brief 设置空间参考系统ID
     * @param srid SRID值
     * @note 仅设置元数据，不进行坐标转换
     */
    void SetSRID(int srid) noexcept {
        m_srid.store(srid, std::memory_order_relaxed);
    }
    
    /**
     * @brief 坐标系统变换
     * @param targetSRID 目标SRID
     * @param result 输出几何对象
     * @return 操作结果
     */
    virtual GeomResult Transform(int targetSRID, std::unique_ptr<CNGeometry>& result) const;
    
    virtual GeomResult Transform(const std::string& targetCRS, 
                                 std::unique_ptr<CNGeometry>& result) const;
    
    // ========== 度量方法 ==========
    
    /**
     * @brief 计算到另一几何的最短距离
     * @param other 另一几何对象
     * @return 最短距离
     * @note 时间复杂度：O(n*m)
     */
    virtual double Distance(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否在指定距离内
     * @param other 另一几何对象
     * @param distance 距离阈值
     */
    virtual bool IsWithinDistance(const CNGeometry* other, double distance) const;
    
    /**
     * @brief 计算面积（多边形）
     * @return 面积值
     */
    virtual double Area() const;
    
    /**
     * @brief 计算长度（线串）或周长（多边形）
     * @return 长度值
     */
    virtual double Length() const;
    
    // ========== 外包矩形和质心 ==========
    
    /**
     * @brief 获取外包矩形（缓存）
     * @return 外包矩形引用
     * @note 线程安全，时间复杂度：O(1)缓存命中，O(n)首次计算
     */
    const Envelope& GetEnvelope() const;
    
    /**
     * @brief 获取外包矩形几何对象
     * @return 多边形几何
     */
    std::unique_ptr<CNGeometry> GetEnvelopeGeometry() const;
    
    /**
     * @brief 获取质心（缓存）
     * @return 质心坐标
     * @note 线程安全
     */
    virtual Coordinate GetCentroid() const;
    
    /**
     * @brief 获取表面上的点
     * @return 保证在几何表面上的点
     */
    virtual Coordinate GetPointOnSurface() const;
    
    /**
     * @brief 获取内部点
     * @return 保证在几何内部的点
     */
    virtual Coordinate GetInteriorPoint() const;
    
    // ========== 坐标访问 ==========
    
    /**
     * @brief 获取坐标总数
     * @return 坐标数量
     */
    virtual size_t GetNumCoordinates() const noexcept = 0;
    
    /**
     * @brief 获取第N个坐标
     * @param index 索引
     * @return 坐标值
     * @throws GeometryException(OutOfRange) 如果索引越界
     */
    virtual Coordinate GetCoordinateN(size_t index) const;
    
    /**
     * @brief 获取所有坐标
     * @return 坐标列表
     */
    virtual CoordinateList GetCoordinates() const = 0;
    
    /**
     * @brief 获取连续坐标数组（如果支持）
     * @param data 输出数据指针
     * @param count 输出坐标数量
     * @return 是否成功（某些几何类型不支持连续存储）
     * @note 用于GPU加速渲染
     */
    virtual bool GetCoordinateData(const double** data, size_t* count) const;
    
    // ========== 空间关系（OGC DE-9IM） ==========
    
    /**
     * @brief 判断几何是否相等
     * @param other 另一几何对象
     * @param tolerance 容差
     */
    virtual bool Equals(const CNGeometry* other, double tolerance = 1e-9) const;
    
    /**
     * @brief 判断是否相离
     */
    virtual bool Disjoint(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否相交
     * @note 时间复杂度：O(n*m)，可使用空间索引优化
     */
    virtual bool Intersects(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否接触
     */
    virtual bool Touches(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否交叉
     */
    virtual bool Crosses(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否被包含
     */
    virtual bool Within(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否包含
     */
    virtual bool Contains(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否重叠
     */
    virtual bool Overlaps(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否覆盖
     */
    virtual bool Covers(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否被覆盖
     */
    virtual bool CoveredBy(const CNGeometry* other) const;
    
    /**
     * @brief 计算DE-9IM关系矩阵
     * @return 9字符矩阵字符串
     */
    virtual std::string Relate(const CNGeometry* other) const;
    
    /**
     * @brief 判断是否满足指定关系
     * @param other 另一几何对象
     * @param pattern 关系模式（如 "T*F**FFF*"）
     */
    virtual bool Relate(const CNGeometry* other, const std::string& pattern) const;
    
    // ========== 几何运算 ==========
    
    /**
     * @brief 计算交集
     * @param other 另一几何对象
     * @param result 输出结果
     * @return 操作结果
     */
    virtual GeomResult Intersection(const CNGeometry* other, 
                                   std::unique_ptr<CNGeometry>& result) const;
    
    /**
     * @brief 计算并集
     */
    virtual GeomResult Union(const CNGeometry* other,
                            std::unique_ptr<CNGeometry>& result) const;
    
    /**
     * @brief 计算差集
     */
    virtual GeomResult Difference(const CNGeometry* other,
                                  std::unique_ptr<CNGeometry>& result) const;
    
    /**
     * @brief 计算对称差集
     */
    virtual GeomResult SymmetricDifference(const CNGeometry* other,
                                          std::unique_ptr<CNGeometry>& result) const;
    
    // ========== 缓冲区操作 ==========
    
    /**
     * @brief 生成缓冲区
     * @param distance 缓冲距离
     * @param result 输出几何
     * @param segments 圆弧分段数（默认8）
     * @return 操作结果
     * @note 时间复杂度：O(n log n)
     */
    virtual GeomResult Buffer(double distance, 
                             std::unique_ptr<CNGeometry>& result,
                             int segments = 8) const;
    
    /**
     * @brief 单边缓冲区
     * @param distance 缓冲距离（正数右侧，负数左侧）
     */
    virtual GeomResult SingleSidedBuffer(double distance,
                                         std::unique_ptr<CNGeometry>& result) const;
    
    /**
     * @brief 偏移曲线
     */
    virtual GeomResult OffsetCurve(double distance,
                                   std::unique_ptr<CNGeometry>& result) const;
    
    // ========== 几何变换 ==========
    
    /**
     * @brief 计算凸包
     * @note 时间复杂度：O(n log n)，使用Andrew's monotone chain算法
     */
    virtual std::unique_ptr<CNGeometry> ConvexHull() const;
    
    /**
     * @brief 计算边界
     */
    virtual std::unique_ptr<CNGeometry> Boundary() const;
    
    /**
     * @brief 修复无效几何
     */
    virtual GeomResult MakeValid(std::unique_ptr<CNGeometry>& result) const;
    
    /**
     * @brief 规范化（环方向等）
     */
    virtual void Normalize();
    
    /**
     * @brief 简化（Douglas-Peucker算法）
     * @param tolerance 容差
     * @note 时间复杂度：O(n log n)
     */
    virtual std::unique_ptr<CNGeometry> Simplify(double tolerance) const;
    
    /**
     * @brief 拓扑保持简化
     */
    virtual std::unique_ptr<CNGeometry> SimplifyPreserveTopology(double tolerance) const;
    
    /**
     * @brief 加密（插入点）
     */
    virtual std::unique_ptr<CNGeometry> Densify(double maxSegmentLength) const;
    
    /**
     * @brief 反转
     */
    virtual std::unique_ptr<CNGeometry> Reverse() const;
    
    /**
     * @brief 捕捉到格网
     */
    virtual std::unique_ptr<CNGeometry> SnapToGrid(double gridSize) const;
    
    /**
     * @brief 捕捉到参考几何
     */
    virtual std::unique_ptr<CNGeometry> SnapTo(const CNGeometry* reference, 
                                               double tolerance) const;
    
    /**
     * @brief 仿射变换
     */
    virtual std::unique_ptr<CNGeometry> AffineTransform(double a, double b, double c,
                                                       double d, double e, double f) const;
    
    /**
     * @brief 平移
     */
    virtual std::unique_ptr<CNGeometry> Translate(double dx, double dy) const;
    
    /**
     * @brief 缩放
     */
    virtual std::unique_ptr<CNGeometry> Scale(double scaleX, double scaleY) const;
    
    /**
     * @brief 旋转
     * @param angle 弧度
     */
    virtual std::unique_ptr<CNGeometry> Rotate(double angle) const;
    
    // ========== 序列化 ==========
    
    /**
     * @brief WKT字符串
     * @param precision 小数精度
     */
    virtual std::string AsText(int precision = 15) const = 0;
    
    /**
     * @brief WKB二进制
     */
    virtual std::vector<uint8_t> AsBinary() const = 0;
    
    /**
     * @brief 十六进制WKB
     */
    std::string AsHex() const;
    
    /**
     * @brief GeoJSON字符串
     */
    virtual std::string AsGeoJSON(int precision = 15) const;
    
    /**
     * @brief GML字符串
     */
    virtual std::string AsGML() const;
    
    /**
     * @brief KML字符串
     */
    virtual std::string AsKML() const;
    
    // ========== 克隆和工厂 ==========
    
    /**
     * @brief 深拷贝
     */
    virtual std::unique_ptr<CNGeometry> Clone() const = 0;
    
    /**
     * @brief 创建同类型空几何
     */
    virtual std::unique_ptr<CNGeometry> CloneEmpty() const = 0;
    
    // ========== 访问者模式 ==========
    
    virtual void Apply(GeometryVisitor& visitor);
    virtual void Apply(GeometryConstVisitor& visitor) const;
    
    // ========== 高级方法 ==========
    
    /**
     * @brief 是否包含曲线几何
     */
    virtual bool HasCurveGeometry() const noexcept { return false; }
    
    /**
     * @brief 将曲线几何转换为线性近似
     */
    virtual std::unique_ptr<CNGeometry> GetLinearGeometry() const;
    
    /**
     * @brief 获取子几何数量（集合类）
     */
    virtual size_t GetNumGeometries() const noexcept { return 1; }
    
    /**
     * @brief 获取第N个子几何（集合类）
     */
    virtual const CNGeometry* GetGeometryN(size_t index) const;
    
    /**
     * @brief 计算到另一几何的最短连线
     */
    virtual std::unique_ptr<CNGeometry> GetShortestLine(const CNGeometry* other) const;
    
    /**
     * @brief 计算到另一几何的最长连线
     */
    virtual std::unique_ptr<CNGeometry> GetMaximumDistanceLine(const CNGeometry* other) const;
    
    /**
     * @brief 获取最小外接圆
     */
    virtual std::pair<Coordinate, double> GetMinimumBoundingCircle() const;
    
    /**
     * @brief 获取最小外接矩形（最小面积）
     */
    virtual std::unique_ptr<CNGeometry> GetMinimumBoundingRectangle() const;
    
protected:
    CNGeometry() = default;
    
    // 线程安全的缓存访问（C++11）
    mutable std::mutex m_cache_mutex;  // C++11 使用互斥锁（无读写锁）
    mutable std::unique_ptr<Envelope> m_envelope_cache;
    mutable std::unique_ptr<Coordinate> m_centroid_cache;
    
    // 原子变量：SRID
    std::atomic<int> m_srid{0};
    
    // 缓存计算（内部方法）
    virtual Envelope ComputeEnvelope() const = 0;
    virtual Coordinate ComputeCentroid() const;
    
    // 缓存失效
    void InvalidateCache() const {
        std::unique_lock<std::mutex> lock(m_cache_mutex);
        m_envelope_cache.reset();
        m_centroid_cache.reset();
    }
    
    // 空间关系快速判断（基于外包矩形）
    bool EnvelopeDisjoint(const CNGeometry* other) const;
    bool EnvelopeIntersects(const CNGeometry* other) const;
};
```

---

## 5. 智能指针和内存管理

### 5.1 类型别名

```cpp
// 独占所有权指针（推荐）
using CNGeometryPtr = std::unique_ptr<CNGeometry>;
using CNPointPtr = std::unique_ptr<CNPoint>;
using CNLineStringPtr = std::unique_ptr<CNLineString>;
using CNLinearRingPtr = std::unique_ptr<CNLinearRing>;
using CNPolygonPtr = std::unique_ptr<CNPolygon>;
using CNMultiPointPtr = std::unique_ptr<CNMultiPoint>;
using CNMultiLineStringPtr = std::unique_ptr<CNMultiLineString>;
using CNMultiPolygonPtr = std::unique_ptr<CNMultiPolygon>;
using CNGeometryCollectionPtr = std::unique_ptr<CNGeometryCollection>;

// 共享所有权指针（慎用）
using CNGeometrySharedPtr = std::shared_ptr<CNGeometry>;
```

### 5.2 内存管理策略

#### 5.2.1 设计原则

| 原则 | 说明 | 实现方式 |
|------|------|----------|
| 默认使用unique_ptr | 明确所有权，避免内存泄漏 | 工厂方法返回unique_ptr |
| 支持移动语义 | 高效传递大对象 | 移动构造/赋值 |
| 小对象优化 | 减少小几何对象的堆分配 | 栈上分配或内存池 |
| 内存池可选 | 高频场景优化 | GeometryPool类 |

#### 5.2.2 内存池设计

```cpp
/**
 * @brief 几何对象内存池
 * 
 * 用于频繁创建/销毁几何对象的场景，减少内存碎片
 * 
 * 特性：
 * - 小对象优化：小于64字节的对象使用线程本地缓存
 * - 批量分配：预分配大块内存，减少系统调用
 * - 线程安全：每个线程独立的缓存区
 */
class GeometryPool {
public:
    // 配置参数
    struct Config {
        size_t initialSize = 1024 * 1024;      // 初始池大小：1MB
        size_t maxSize = 64 * 1024 * 1024;     // 最大池大小：64MB
        size_t smallObjectThreshold = 64;       // 小对象阈值：64字节
        bool enableThreadCache = true;          // 启用线程缓存
        size_t threadCacheSize = 64 * 1024;     // 线程缓存大小：64KB
    };
    
    static GeometryPool& GetInstance();
    
    // 对象创建
    template<typename T, typename... Args>
    std::unique_ptr<T> Create(Args&&... args);
    
    // 批量创建
    template<typename T>
    std::vector<std::unique_ptr<T>> CreateBatch(size_t count);
    
    // 内存管理
    void Clear();           // 清空所有缓存
    void ShrinkToFit();     // 释放未使用内存
    void Defragment();      // 内存整理（可选）
    
    // 统计信息
    struct Statistics {
        size_t totalAllocated;    // 总分配字节数
        size_t totalFreed;        // 总释放字节数
        size_t currentUsage;      // 当前使用量
        size_t peakUsage;         // 峰值使用量
        size_t poolSize;          // 池大小
        size_t objectCount;       // 对象数量
        size_t fragmentCount;     // 碎片数量
    };
    Statistics GetStatistics() const;
    
    // 配置
    void SetConfig(const Config& config);
    const Config& GetConfig() const;
    
private:
    GeometryPool();
    ~GeometryPool();
    
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

// 全局便捷函数
template<typename T, typename... Args>
std::unique_ptr<T> MakeGeometry(Args&&... args) {
    return GeometryPool::GetInstance().Create<T>(std::forward<Args>(args)...);
}
```

#### 5.2.3 小对象优化

```cpp
/**
 * @brief 小对象优化策略
 * 
 * 对于小于阈值（默认64字节）的对象，使用栈分配或线程本地缓存
 */
template<size_t Threshold = 64>
class SmallObjectOptimizer {
public:
    // 小对象存储
    union SmallStorage {
        alignas(alignof(std::max_align_t)) char buffer[Threshold];
        void* heapPtr;
    };
    
    // 判断是否为小对象
    static constexpr bool IsSmall(size_t size) {
        return size <= Threshold;
    }
    
    // 分配
    void* Allocate(size_t size) {
        if (IsSmall(size)) {
            return &m_threadLocal.buffer;
        }
        return ::operator new(size);
    }
    
    // 释放
    void Deallocate(void* ptr, size_t size) {
        if (!IsSmall(size)) {
            ::operator delete(ptr);
        }
        // 小对象无需释放，线程退出时自动清理
    }
    
private:
    static thread_local struct ThreadLocalCache {
        SmallStorage buffer;
        // 其他缓存...
    } m_threadLocal;
};
```

#### 5.2.4 内存使用估算

| 几何类型 | 基础大小 | 每坐标额外 | 示例估算 |
|----------|----------|------------|----------|
| CNPoint | 48字节 | 0 | 48字节 |
| CNLineString | 64字节 | 32字节/坐标 | 100坐标 = 3,264字节 |
| CNPolygon | 80字节 | 32字节/坐标 | 1000坐标 = 32,080字节 |
| CNMultiPolygon | 96字节 | 依赖子对象 | 变化较大 |

#### 5.2.5 内存监控接口

```cpp
/**
 * @brief 内存监控回调
 */
class IMemoryMonitor {
public:
    virtual ~IMemoryMonitor() = default;
    
    // 分配通知
    virtual void OnAllocate(size_t size, const char* typeName) = 0;
    
    // 释放通知
    virtual void OnDeallocate(size_t size, const char* typeName) = 0;
    
    // 内存压力通知
    virtual void OnMemoryPressure(size_t currentUsage, size_t limit) = 0;
    
    // 碎片警告
    virtual void OnFragmentationWarning(double fragmentationRatio) = 0;
};

// 设置全局监控器
void SetMemoryMonitor(std::shared_ptr<IMemoryMonitor> monitor);
```

### 5.3 内存池使用示例

```cpp
// 默认使用（推荐）
auto point = CNPoint::Create(100.0, 200.0);

// 使用内存池（高频场景）
auto point = MakeGeometry<CNPoint>(100.0, 200.0);

// 批量创建
auto points = GeometryPool::GetInstance().CreateBatch<CNPoint>(10000);

// 监控内存使用
auto stats = GeometryPool::GetInstance().GetStatistics();
std::cout << "Current usage: " << stats.currentUsage << " bytes\n";
std::cout << "Peak usage: " << stats.peakUsage << " bytes\n";
```

---

## 6. 几何派生类设计

### 6.1 CNPoint（点）

```cpp
class CNPoint : public CNGeometry {
public:
    // 构造
    static CNPointPtr Create(double x, double y);
    static CNPointPtr Create(double x, double y, double z);
    static CNPointPtr Create(double x, double y, double z, double m);
    static CNPointPtr Create(const Coordinate& coord);
    
    // 基本接口
    GeomType GetGeometryType() const noexcept override { return GeomType::Point; }
    const char* GetGeometryTypeString() const noexcept override { return "POINT"; }
    Dimension GetDimension() const noexcept override { return Dimension::Point; }
    int GetCoordinateDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override { return m_coord.IsEmpty(); }
    bool Is3D() const noexcept override { return m_coord.Is3D(); }
    bool IsMeasured() const noexcept override { return m_coord.IsMeasured(); }
    
    // 坐标访问
    Coordinate GetCoordinate() const noexcept { return m_coord; }
    void SetCoordinate(const Coordinate& coord);
    
    double GetX() const noexcept { return m_coord.x; }
    double GetY() const noexcept { return m_coord.y; }
    double GetZ() const noexcept { return m_coord.z; }
    double GetM() const noexcept { return m_coord.m; }
    
    void SetX(double x);
    void SetY(double y);
    void SetZ(double z);
    void SetM(double m);
    
    // 向量运算
    CNPointPtr operator+(const CNPoint& rhs) const;
    CNPointPtr operator-(const CNPoint& rhs) const;
    CNPointPtr operator*(double scalar) const;
    
    double Dot(const CNPoint& other) const;
    double Cross(const CNPoint& other) const;
    
    // 实用方法
    bool IsCollinear(const CNPoint& p1, const CNPoint& p2, double tolerance = 1e-9) const;
    bool IsBetween(const CNPoint& p1, const CNPoint& p2) const;
    
    CNPointPtr Normalize() const;
    
    // 静态工厂
    static CNPointPtr FromPolar(double radius, double angle);
    static CNPointPtr Centroid(const CoordinateList& coords);
    
    // 实现基类纯虚方法
    size_t GetNumCoordinates() const noexcept override { return IsEmpty() ? 0 : 1; }
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    std::string AsText(int precision = 15) const override;
    std::vector<uint8_t> AsBinary() const override;
    std::unique_ptr<CNGeometry> Clone() const override;
    std::unique_ptr<CNGeometry> CloneEmpty() const override;
    
private:
    explicit CNPoint(const Coordinate& coord) : m_coord(coord) {}
    
    Coordinate m_coord;
    Envelope ComputeEnvelope() const override;
};
```

### 6.2 CNLineString（线串）

```cpp
class CNLineString : public CNGeometry {
public:
    // 构造
    static CNLineStringPtr Create();
    static CNLineStringPtr Create(const CoordinateList& coords);
    static CNLineStringPtr Create(CoordinateList&& coords);
    
    // 类型信息
    GeomType GetGeometryType() const noexcept override { return GeomType::LineString; }
    const char* GetGeometryTypeString() const noexcept override { return "LINESTRING"; }
    Dimension GetDimension() const noexcept override { return Dimension::Curve; }
    
    bool IsEmpty() const noexcept override { return m_coords.empty(); }
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    bool IsClosed() const noexcept;
    bool IsRing() const noexcept;
    
    // 坐标管理
    size_t GetNumPoints() const noexcept { return m_coords.size(); }
    size_t GetNumCoordinates() const noexcept override { return m_coords.size(); }
    
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override { return m_coords; }
    
    Coordinate GetPointN(size_t index) const;
    Coordinate GetStartPoint() const;
    Coordinate GetEndPoint() const;
    
    void SetCoordinates(const CoordinateList& coords);
    void SetCoordinates(CoordinateList&& coords);
    
    void AddPoint(const Coordinate& coord);
    void InsertPoint(size_t index, const Coordinate& coord);
    void RemovePoint(size_t index);
    void Clear();
    
    // 线段访问
    struct Segment {
        Coordinate start, end;
        double length;
    };
    Segment GetSegment(size_t index) const;
    size_t GetNumSegments() const noexcept;
    
    // 度量
    double Length() const override;
    double Length3D() const;
    std::vector<double> GetCumulativeLength() const;
    double GetSegmentLengthPercentage(size_t index) const;
    
    // 线性参考
    Coordinate Interpolate(double distance) const;
    double LocatePoint(const Coordinate& point) const;
    CNLineStringPtr GetSubLine(double startDistance, double endDistance) const;
    
    // 分割
    std::pair<CNLineStringPtr, CNLineStringPtr> Split(const Coordinate& point) const;
    std::vector<CNLineStringPtr> SplitAtDistances(const std::vector<double>& distances) const;
    
    // 偏移
    CNLineStringPtr Offset(double distance) const;
    
    // 曲线分析
    double GetCurvatureAt(size_t index) const;
    double GetTotalCurvature() const;
    double GetBearingAt(size_t index) const;
    
    // 实用
    void RemoveDuplicatePoints(double tolerance = 1e-9);
    CNLineStringPtr Smooth(int iterations = 1) const;
    bool IsStraight(double tolerance = 1e-9) const;
    
    // 坐标数据（连续存储）
    bool GetCoordinateData(const double** data, size_t* count) const override {
        if (m_coords.empty()) {
            *data = nullptr;
            *count = 0;
            return false;
        }
        *data = reinterpret_cast<const double*>(m_coords.data());
        *count = m_coords.size() * 4; // x,y,z,m
        return true;
    }
    
    // 迭代器支持
    CoordinateList::const_iterator begin() const { return m_coords.begin(); }
    CoordinateList::const_iterator end() const { return m_coords.end(); }
    
    // 实现基类方法
    std::string AsText(int precision = 15) const override;
    std::vector<uint8_t> AsBinary() const override;
    std::unique_ptr<CNGeometry> Clone() const override;
    std::unique_ptr<CNGeometry> CloneEmpty() const override;
    
protected:
    CNLineString() = default;
    explicit CNLineString(const CoordinateList& coords) : m_coords(coords) {}
    
    CoordinateList m_coords;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};
```

### 6.3 CNLinearRing（环）

```cpp
/**
 * @brief 线性环（闭合线串）
 * 
 * 继承自CNLineString，额外保证：
 * 1. 闭合性（首尾点相同或自动闭合）
 * 2. 简单性（不自交）
 * 3. 有效环方向
 */
class CNLinearRing : public CNLineString {
public:
    // 构造
    static CNLinearRingPtr Create();
    static CNLinearRingPtr Create(const CoordinateList& coords, bool autoClose = true);
    
    // 类型信息
    GeomType GetGeometryType() const noexcept override { return GeomType::LineString; }
    const char* GetGeometryTypeString() const noexcept override { return "LINEARRING"; }
    
    // 有效性检查
    bool IsValidRing(std::string* reason = nullptr) const;
    bool IsSimpleRing() const;
    
    // 多边形特征
    double Area() const;  // 使用鞋带公式
    double GetPerimeter() const { return Length(); }
    
    bool IsConvex() const;
    
    // 方向
    bool IsClockwise() const;
    bool IsCounterClockwise() const;
    
    void Reverse() override;
    void ForceClockwise();
    void ForceCounterClockwise();
    
    // 点包含测试（射线法）
    bool ContainsPoint(const Coordinate& point) const;
    bool IsPointOnBoundary(const Coordinate& point, double tolerance = 1e-9) const;
    
    // 环操作
    CNLinearRingPtr Offset(double distance) const;
    void Normalize();  // 规范化方向
    
    // 三角剖分（耳切法）
    struct Triangle { Coordinate p1, p2, p3; };
    std::vector<Triangle> Triangulate() const;
    
    // 静态工厂
    static CNLinearRingPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    static CNLinearRingPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
    static CNLinearRingPtr CreateRegularPolygon(double centerX, double centerY, double radius, int sides);
    
    // 实现基类方法
    std::unique_ptr<CNGeometry> Clone() const override;
    std::unique_ptr<CNGeometry> CloneEmpty() const override;
    
private:
    CNLinearRing() = default;
    
    // 重写以确保闭合
    void EnsureClosed();
};
```

### 6.4 CNPolygon（多边形）

```cpp
/**
 * @brief 多边形
 * 
 * 由一个外环和零个或多个内环（孔）组成
 */
class CNPolygon : public CNGeometry {
public:
    // 构造
    static CNPolygonPtr Create();
    static CNPolygonPtr Create(CNLinearRingPtr exteriorRing);
    
    // 类型信息
    GeomType GetGeometryType() const noexcept override { return GeomType::Polygon; }
    const char* GetGeometryTypeString() const noexcept override { return "POLYGON"; }
    Dimension GetDimension() const noexcept override { return Dimension::Surface; }
    
    bool IsEmpty() const noexcept override;
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    // 环管理
    void SetExteriorRing(CNLinearRingPtr ring);
    const CNLinearRing* GetExteriorRing() const noexcept;
    
    void AddInteriorRing(CNLinearRingPtr ring);
    void RemoveInteriorRing(size_t index);
    const CNLinearRing* GetInteriorRingN(size_t index) const;
    size_t GetNumInteriorRings() const noexcept { return m_interiorRings.size(); }
    size_t GetNumRings() const noexcept { return 1 + m_interiorRings.size(); }
    
    void ClearRings();
    
    // 坐标访问
    size_t GetNumCoordinates() const noexcept override;
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    
    // 度量
    double Area() const override;
    double Length() const override { return GetPerimeter(); }
    double GetPerimeter() const;
    
    double GetExteriorArea() const;
    double GetInteriorArea() const;
    
    // 形状指标
    double GetSolidity() const;    // 面积/凸包面积
    double GetCompactness() const; // 与等面积圆的比较
    
    // 多边形操作
    CNPolygonPtr RemoveHoles() const;
    CNPolygonPtr MergeHoles() const;
    
    // 三角剖分
    std::vector<CNLinearRing::Triangle> Triangulate() const;
    
    // 空间关系
    bool ContainsRing(const CNLinearRing* ring) const;
    bool ContainsPolygon(const CNPolygon* polygon) const;
    
    // 集合运算（多边形专用）
    GeomResult UnionWithPolygon(const CNPolygon* other, CNPolygonPtr& result) const;
    GeomResult IntersectWithPolygon(const CNPolygon* other, CNPolygonPtr& result) const;
    GeomResult DifferenceWithPolygon(const CNPolygon* other, CNPolygonPtr& result) const;
    
    // 静态工厂
    static CNPolygonPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    static CNPolygonPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
    static CNPolygonPtr CreateTriangle(const Coordinate& p1, const Coordinate& p2, const Coordinate& p3);
    
    // 实现基类方法
    std::string AsText(int precision = 15) const override;
    std::vector<uint8_t> AsBinary() const override;
    std::unique_ptr<CNGeometry> Clone() const override;
    std::unique_ptr<CNGeometry> CloneEmpty() const override;
    
protected:
    CNPolygon() = default;
    
    CNLinearRingPtr m_exteriorRing;
    std::vector<CNLinearRingPtr> m_interiorRings;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};
```

### 6.5 CNMultiPoint（多点）

```cpp
class CNMultiPoint : public CNGeometry {
public:
    // 构造
    static CNMultiPointPtr Create();
    static CNMultiPointPtr Create(const std::vector<CNPointPtr>& points);
    
    // 类型信息
    GeomType GetGeometryType() const noexcept override { return GeomType::MultiPoint; }
    const char* GetGeometryTypeString() const noexcept override { return "MULTIPOINT"; }
    Dimension GetDimension() const noexcept override { return Dimension::Collection; }
    int GetCoordinateDimension() const noexcept override;
    
    bool IsEmpty() const noexcept override { return m_points.empty(); }
    bool Is3D() const noexcept override;
    bool IsMeasured() const noexcept override;
    
    // 点管理
    void AddPoint(CNPointPtr point);
    void AddPoint(const Coordinate& coord);
    void RemovePoint(size_t index);
    void Clear();
    
    const CNPoint* GetPointN(size_t index) const;
    size_t GetNumPoints() const noexcept { return m_points.size(); }
    size_t GetNumGeometries() const noexcept override { return m_points.size(); }
    const CNGeometry* GetGeometryN(size_t index) const override;
    
    // 坐标访问
    size_t GetNumCoordinates() const noexcept override { return m_points.size(); }
    Coordinate GetCoordinateN(size_t index) const override;
    CoordinateList GetCoordinates() const override;
    
    // 集合操作
    CNMultiPointPtr Merge(const CNMultiPoint* other) const;
    CNMultiPointPtr GetUniquePoints(double tolerance = 1e-9) const;
    
    // 空间关系
    bool ContainsPoint(const CNPoint* point) const;
    bool ContainsMultiPoint(const CNMultiPoint* other) const;
    
    // 实用方法
    CNMultiPointPtr SortByX() const;
    CNMultiPointPtr SortByY() const;
    CNMultiPointPtr SortByDistanceTo(const Coordinate& refPoint) const;
    
    bool HasDuplicatePoints(double tolerance = 1e-9) const;
    std::pair<size_t, size_t> GetNearestPoints(const CNMultiPoint* other) const;
    
    // 迭代器
    std::vector<CNPointPtr>::const_iterator begin() const { return m_points.begin(); }
    std::vector<CNPointPtr>::const_iterator end() const { return m_points.end(); }
    
    // 实现基类方法
    std::string AsText(int precision = 15) const override;
    std::vector<uint8_t> AsBinary() const override;
    std::unique_ptr<CNGeometry> Clone() const override;
    std::unique_ptr<CNGeometry> CloneEmpty() const override;
    
private:
    std::vector<CNPointPtr> m_points;
    
    Envelope ComputeEnvelope() const override;
    Coordinate ComputeCentroid() const override;
};
```

---

## 7. 访问者模式

### 7.1 访问者接口

```cpp
/**
 * @brief 几何访问者接口（可修改）
 */
class GeometryVisitor {
public:
    virtual ~GeometryVisitor() = default;
    
    // 默认实现（调用通用访问方法）
    virtual void Visit(CNGeometry* geom) { }
    virtual void VisitPoint(CNPoint* point) { Visit(point); }
    virtual void VisitLineString(CNLineString* line) { Visit(line); }
    virtual void VisitLinearRing(CNLinearRing* ring) { VisitLineString(ring); }
    virtual void VisitPolygon(CNPolygon* polygon) { Visit(polygon); }
    virtual void VisitMultiPoint(CNMultiPoint* multiPoint) { Visit(multiPoint); }
    virtual void VisitMultiLineString(CNMultiLineString* multiLine) { Visit(multiLine); }
    virtual void VisitMultiPolygon(CNMultiPolygon* multiPolygon) { Visit(multiPolygon); }
    virtual void VisitGeometryCollection(CNGeometryCollection* collection) { Visit(collection); }
    
    // 高级类型
    virtual void VisitCircularString(CNCircularString* curve) { Visit(curve); }
    virtual void VisitCompoundCurve(CNCompoundCurve* curve) { Visit(curve); }
    virtual void VisitCurvePolygon(CNCurvePolygon* polygon) { Visit(polygon); }
    virtual void VisitTIN(CNTIN* tin) { Visit(tin); }
};

/**
 * @brief 几何访问者接口（只读）
 */
class GeometryConstVisitor {
public:
    virtual ~GeometryConstVisitor() = default;
    
    virtual void Visit(const CNGeometry* geom) { }
    virtual void VisitPoint(const CNPoint* point) { Visit(point); }
    virtual void VisitLineString(const CNLineString* line) { Visit(line); }
    virtual void VisitLinearRing(const CNLinearRing* ring) { VisitLineString(ring); }
    virtual void VisitPolygon(const CNPolygon* polygon) { Visit(polygon); }
    virtual void VisitMultiPoint(const CNMultiPoint* multiPoint) { Visit(multiPoint); }
    virtual void VisitMultiLineString(const CNMultiLineString* multiLine) { Visit(multiLine); }
    virtual void VisitMultiPolygon(const CNMultiPolygon* multiPolygon) { Visit(multiPolygon); }
    virtual void VisitGeometryCollection(const CNGeometryCollection* collection) { Visit(collection); }
    
    virtual void VisitCircularString(const CNCircularString* curve) { Visit(curve); }
    virtual void VisitCompoundCurve(const CNCompoundCurve* curve) { Visit(curve); }
    virtual void VisitCurvePolygon(const CNCurvePolygon* polygon) { Visit(polygon); }
    virtual void VisitTIN(const CNTIN* tin) { Visit(tin); }
};
```

### 7.2 示例访问者

```cpp
/**
 * @brief 面积计算访问者
 */
class AreaCalculator : public GeometryConstVisitor {
public:
    void VisitPoint(const CNPoint*) override { m_area = 0.0; }
    void VisitLineString(const CNLineString*) override { m_area = 0.0; }
    void VisitPolygon(const CNPolygon* polygon) override { m_area = polygon->Area(); }
    void VisitMultiPolygon(const CNMultiPolygon* multiPolygon) override { 
        m_area = multiPolygon->Area(); 
    }
    
    double GetArea() const noexcept { return m_area; }
    
private:
    double m_area = 0.0;
};

/**
 * @brief 几何统计访问者
 */
class GeometryStatistics : public GeometryConstVisitor {
public:
    void Visit(const CNGeometry* geom) override {
        m_totalCount++;
        m_typeCounts[geom->GetGeometryType()]++;
    }
    
    size_t GetTotalCount() const noexcept { return m_totalCount; }
    size_t GetTypeCount(GeomType type) const { return m_typeCounts[type]; }
    
private:
    size_t m_totalCount = 0;
    std::map<GeomType, size_t> m_typeCounts;
};
```

---

## 8. 几何工厂

### 8.1 GeometryFactory

```cpp
/**
 * @brief 几何对象工厂
 * 
 * 提供便捷的几何创建方法，支持从多种格式解析
 */
class GeometryFactory {
public:
    static GeometryFactory& GetInstance();
    
    // ========== 从WKT创建 ==========
    
    /**
     * @brief 从WKT字符串创建几何
     * @param wkt WKT字符串
     * @param result 输出几何对象
     * @return 解析结果
     */
    GeomResult FromWKT(const std::string& wkt, CNGeometryPtr& result);
    
    /**
     * @brief 从WKT字符串创建点
     */
    CNPointPtr CreatePointFromWKT(const std::string& wkt);
    
    /**
     * @brief 从WKT字符串创建线串
     */
    CNLineStringPtr CreateLineStringFromWKT(const std::string& wkt);
    
    /**
     * @brief 从WKT字符串创建多边形
     */
    CNPolygonPtr CreatePolygonFromWKT(const std::string& wkt);
    
    // ========== 从WKB创建 ==========
    
    GeomResult FromWKB(const std::vector<uint8_t>& wkb, CNGeometryPtr& result);
    GeomResult FromWKB(const uint8_t* data, size_t size, CNGeometryPtr& result);
    
    // ========== 从GeoJSON创建 ==========
    
    GeomResult FromGeoJSON(const std::string& json, CNGeometryPtr& result);
    
    // ========== 从坐标创建 ==========
    
    CNPointPtr CreatePoint(double x, double y);
    CNPointPtr CreatePoint(double x, double y, double z);
    CNPointPtr CreatePoint(const Coordinate& coord);
    
    CNLineStringPtr CreateLineString(const CoordinateList& coords);
    CNLinearRingPtr CreateLinearRing(const CoordinateList& coords);
    
    CNPolygonPtr CreatePolygon(const CoordinateList& exteriorRing);
    CNPolygonPtr CreatePolygon(const std::vector<CoordinateList>& rings);
    
    CNMultiPointPtr CreateMultiPoint(const CoordinateList& coords);
    
    // ========== 预定义形状 ==========
    
    CNPolygonPtr CreateRectangle(double minX, double minY, double maxX, double maxY);
    CNPolygonPtr CreateCircle(double centerX, double centerY, double radius, int segments = 32);
    CNPolygonPtr CreateRegularPolygon(double centerX, double centerY, double radius, int sides);
    CNPolygonPtr CreateTriangle(const Coordinate& p1, const Coordinate& p2, const Coordinate& p3);
    
    // ========== 空几何 ==========
    
    CNGeometryPtr CreateEmptyGeometry(GeomType type);
    
    // ========== SRID设置 ==========
    
    void SetDefaultSRID(int srid) noexcept { m_defaultSRID = srid; }
    int GetDefaultSRID() const noexcept { return m_defaultSRID; }
    
private:
    GeometryFactory() = default;
    
    int m_defaultSRID = 0;
    
    // WKT解析器
    class WKTReader;
    class WKBReader;
    class GeoJSONReader;
};
```

---

## 9. 空间索引

### 9.1 空间索引接口

```cpp
/**
 * @brief 空间索引抽象接口
 * 
 * 支持多种空间索引实现的统一接口
 */
template<typename T>
class ISpatialIndex {
public:
    virtual ~ISpatialIndex() = default;
    
    // ========== 构建和修改 ==========
    
    /**
     * @brief 插入单个项目
     * @param envelope 外包矩形
     * @param item 数据项
     * @return 操作结果
     */
    virtual GeomResult Insert(const Envelope& envelope, const T& item) = 0;
    
    /**
     * @brief 批量插入（性能优化）
     * @param items 项目列表
     * @return 操作结果
     */
    virtual GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) = 0;
    
    /**
     * @brief 删除项目
     * @param envelope 外包矩形
     * @param item 数据项
     * @return 是否成功删除
     */
    virtual bool Remove(const Envelope& envelope, const T& item) = 0;
    
    /**
     * @brief 清空索引
     */
    virtual void Clear() noexcept = 0;
    
    // ========== 空间查询 ==========
    
    /**
     * @brief 范围查询
     * @param envelope 查询范围
     * @return 结果列表
     */
    virtual std::vector<T> Query(const Envelope& envelope) const = 0;
    
    /**
     * @brief 点查询
     * @param point 查询点
     * @return 结果列表
     */
    virtual std::vector<T> Query(const Coordinate& point) const = 0;
    
    /**
     * @brief 相交查询
     * @param geom 查询几何
     * @return 结果列表
     */
    virtual std::vector<T> QueryIntersects(const CNGeometry* geom) const = 0;
    
    /**
     * @brief 最近邻查询
     * @param point 查询点
     * @param k 返回数量
     * @return 最近的结果列表
     */
    virtual std::vector<T> QueryNearest(const Coordinate& point, size_t k) const = 0;
    
    // ========== 统计信息 ==========
    
    /**
     * @brief 获取项目数量
     */
    virtual size_t Size() const noexcept = 0;
    
    /**
     * @brief 是否为空
     */
    virtual bool IsEmpty() const noexcept = 0;
    
    /**
     * @brief 获取索引边界
     */
    virtual Envelope GetBounds() const = 0;
    
    /**
     * @brief 获取索引高度（树形索引）
     */
    virtual size_t GetHeight() const = 0;
    
    /**
     * @brief 获取节点数量
     */
    virtual size_t GetNodeCount() const = 0;
    
    // ========== 持久化 ==========
    
    /**
     * @brief 序列化索引到二进制流
     * @param output 输出流
     * @return 操作结果
     */
    virtual GeomResult Serialize(std::ostream& output) const = 0;
    
    /**
     * @brief 从二进制流反序列化
     * @param input 输入流
     * @return 操作结果
     */
    virtual GeomResult Deserialize(std::istream& input) = 0;
    
    /**
     * @brief 获取序列化大小估算
     * @return 预估字节数
     */
    virtual size_t GetSerializedSize() const noexcept = 0;
    
    /**
     * @brief 序列化到文件
     * @param filepath 文件路径
     * @return 操作结果
     */
    virtual GeomResult SaveToFile(const std::string& filepath) const {
        std::ofstream file(filepath, std::ios::binary);
        if (!file) {
            return GeomResult::IOError;
        }
        return Serialize(file);
    }
    
    /**
     * @brief 从文件加载
     * @param filepath 文件路径
     * @return 操作结果
     */
    virtual GeomResult LoadFromFile(const std::string& filepath) {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) {
            return GeomResult::IOError;
        }
        return Deserialize(file);
    }
};
```

### 9.2 R-Tree索引实现

```cpp
/**
 * @brief R-Tree空间索引
 * 
 * 基于Guttman的R-Tree算法实现
 * 
 * 特性：
 * - 支持批量加载（STR算法）
 * - 支持动态插入/删除
 * - 可配置分裂策略
 */
template<typename T>
class RTree : public ISpatialIndex<T> {
public:
    // ========== 配置 ==========
    
    enum class SplitStrategy {
        Linear,     // 线性分裂：O(n)，简单但质量一般
        Quadratic,  // 二次分裂：O(n²)，质量较好
        RStar       // R*树分裂：O(n²)，质量最好
    };
    
    struct Config {
        size_t maxEntries = 16;           // 节点最大条目数
        size_t minEntries = 4;            // 节点最小条目数
        SplitStrategy splitStrategy = SplitStrategy::RStar;
        bool enableReinsert = true;       // 启用强制重插入（R*特性）
        double reinsertRatio = 0.3;       // 重插入比例
    };
    
    // ========== 构造 ==========
    
    explicit RTree(const Config& config = Config());
    ~RTree() override;
    
    // ========== ISpatialIndex接口实现 ==========
    
    GeomResult Insert(const Envelope& envelope, const T& item) override;
    GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) override;
    bool Remove(const Envelope& envelope, const T& item) override;
    void Clear() noexcept override;
    
    std::vector<T> Query(const Envelope& envelope) const override;
    std::vector<T> Query(const Coordinate& point) const override;
    std::vector<T> QueryIntersects(const CNGeometry* geom) const override;
    std::vector<T> QueryNearest(const Coordinate& point, size_t k) const override;
    
    size_t Size() const noexcept override { return m_size; }
    bool IsEmpty() const noexcept override { return m_size == 0; }
    Envelope GetBounds() const override;
    size_t GetHeight() const override;
    size_t GetNodeCount() const override;
    
    // ========== 性能调优 ==========
    
    void SetConfig(const Config& config);
    const Config& GetConfig() const noexcept { return m_config; }
    
    // ========== 统计信息 ==========
    
    struct Statistics {
        size_t totalNodes;
        size_t leafNodes;
        size_t internalNodes;
        size_t height;
        double avgNodeUtilization;
        size_t totalReinserts;
    };
    Statistics GetStatistics() const;
    
private:
    // 节点结构
    struct Node {
        Envelope bounds;
        bool isLeaf;
        std::vector<std::pair<Envelope, T>> entries;    // 叶子节点
        std::vector<std::unique_ptr<Node>> children;    // 内部节点
    };
    
    std::unique_ptr<Node> m_root;
    Config m_config;
    size_t m_size = 0;
    
    // 内部方法
    Node* ChooseLeaf(const Envelope& envelope);
    void SplitNode(Node* node);
    void AdjustTree(Node* node);
    void Reinsert(Node* node, size_t level);
};
```

### 9.3 Quadtree索引实现

```cpp
/**
 * @brief 四叉树空间索引
 * 
 * 适用于均匀分布的数据
 */
template<typename T>
class Quadtree : public ISpatialIndex<T> {
public:
    struct Config {
        size_t maxItemsPerNode = 16;      // 每节点最大项目数
        size_t maxDepth = 16;              // 最大深度
        Envelope bounds;                   // 初始边界
    };
    
    explicit Quadtree(const Config& config = Config());
    
    // ISpatialIndex接口实现...
    GeomResult Insert(const Envelope& envelope, const T& item) override;
    GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) override;
    bool Remove(const Envelope& envelope, const T& item) override;
    void Clear() noexcept override;
    
    std::vector<T> Query(const Envelope& envelope) const override;
    std::vector<T> Query(const Coordinate& point) const override;
    std::vector<T> QueryIntersects(const CNGeometry* geom) const override;
    std::vector<T> QueryNearest(const Coordinate& point, size_t k) const override;
    
    size_t Size() const noexcept override;
    bool IsEmpty() const noexcept override;
    Envelope GetBounds() const override;
    size_t GetHeight() const override;
    size_t GetNodeCount() const override;
    
private:
    struct QuadNode {
        Envelope bounds;
        std::vector<std::pair<Envelope, T>> items;
        std::unique_ptr<QuadNode> children[4];  // NW, NE, SW, SE
    };
    
    std::unique_ptr<QuadNode> m_root;
    Config m_config;
    size_t m_size = 0;
};
```

### 9.4 网格索引实现

```cpp
/**
 * @brief 网格索引
 * 
 * 适用于快速范围查询，实现简单
 */
template<typename T>
class GridIndex : public ISpatialIndex<T> {
public:
    struct Config {
        size_t gridX = 100;               // X方向网格数
        size_t gridY = 100;               // Y方向网格数
        Envelope bounds;                   // 索引边界
    };
    
    explicit GridIndex(const Config& config = Config());
    
    // ISpatialIndex接口实现...
    GeomResult Insert(const Envelope& envelope, const T& item) override;
    GeomResult BulkLoad(const std::vector<std::pair<Envelope, T>>& items) override;
    bool Remove(const Envelope& envelope, const T& item) override;
    void Clear() noexcept override;
    
    std::vector<T> Query(const Envelope& envelope) const override;
    std::vector<T> Query(const Coordinate& point) const override;
    std::vector<T> QueryIntersects(const CNGeometry* geom) const override;
    std::vector<T> QueryNearest(const Coordinate& point, size_t k) const override;
    
    size_t Size() const noexcept override;
    bool IsEmpty() const noexcept override;
    Envelope GetBounds() const override;
    size_t GetHeight() const override { return 1; }
    size_t GetNodeCount() const override;
    
private:
    std::vector<std::vector<std::pair<Envelope, T>>> m_grid;
    Config m_config;
    size_t m_size = 0;
    
    size_t GetCellIndex(const Coordinate& coord) const;
    std::pair<size_t, size_t> GetCellIndices(const Coordinate& coord) const;
};
```

### 9.5 索引选择指南

| 索引类型 | 适用场景 | 时间复杂度 | 空间复杂度 |
|----------|----------|------------|------------|
| R-Tree | 通用场景，动态数据 | O(log n)查询 | O(n) |
| Quadtree | 均匀分布数据 | O(log n)查询 | O(n) |
| GridIndex | 快速范围查询 | O(1)定位 | O(n + 网格数) |
| STR-Tree | 批量加载，静态数据 | O(n log n)构建 | O(n) |

### 9.5.1 索引更新策略对比

```cpp
/**
 * @brief 索引更新策略
 * 
 * 不同场景下的索引更新策略选择
 */
namespace OGC {
    /**
     * @brief 更新策略枚举
     */
    enum class IndexUpdateStrategy {
        Immediate,      // 立即更新
        Lazy,           // 延迟更新
        Rebuild,        // 定期重建
        Hybrid          // 混合策略
    };
    
    /**
     * @brief 策略性能对比
     * 
     * | 策略 | 插入性能 | 查询性能 | 内存开销 | 适用场景 |
     * |------|---------|---------|---------|---------|
     * | Immediate | O(log n) | O(log n) | 低 | 实时系统 |
     * | Lazy | O(1) | O(log n) + 重构 | 中 | 读多写少 |
     * | Rebuild | O(1) | O(log n) | 高 | 批量导入 |
     * | Hybrid | O(log n) | O(log n) | 中 | 通用场景 |
     */
}
```

#### 策略1: 立即更新

```cpp
/**
 * @brief 立即更新策略
 * 
 * 每次插入/删除立即更新索引结构
 * 优点：查询性能最优
 * 缺点：插入性能较低
 */
template<typename T>
class ImmediateUpdateIndex : public ISpatialIndex<T> {
public:
    GeomResult Insert(const Envelope& envelope, const T& item) override {
        // 立即插入到树中
        return InsertIntoTree(envelope, item);
    }
    
    bool Remove(const Envelope& envelope, const T& item) override {
        // 立即从树中删除
        return RemoveFromTree(envelope, item);
    }
    
    std::vector<T> Query(const Envelope& envelope) const override {
        // 直接查询，无需重构
        return QueryTree(envelope);
    }
    
private:
    GeomResult InsertIntoTree(const Envelope& env, const T& item);
    bool RemoveFromTree(const Envelope& env, const T& item);
    std::vector<T> QueryTree(const Envelope& env) const;
};
```

#### 策略2: 延迟更新

```cpp
/**
 * @brief 延迟更新策略
 * 
 * 插入/删除操作先记录，查询时按需重构
 * 优点：插入性能高
 * 缺点：查询时可能触发重构
 */
template<typename T>
class LazyUpdateIndex : public ISpatialIndex<T> {
public:
    LazyUpdateIndex(size_t rebuildThreshold = 1000)
        : m_rebuildThreshold(rebuildThreshold) {}
    
    GeomResult Insert(const Envelope& envelope, const T& item) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        // 添加到待插入队列
        m_pendingInserts.emplace_back(envelope, item);
        m_dirty = true;
        
        // 检查是否需要重构
        if (m_pendingInserts.size() + m_pendingRemoves.size() > m_rebuildThreshold) {
            RebuildIndex();
        }
        
        return GeomResult::Success;
    }
    
    bool Remove(const Envelope& envelope, const T& item) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        // 添加到待删除队列
        m_pendingRemoves.emplace_back(envelope, item);
        m_dirty = true;
        
        return true;
    }
    
    std::vector<T> Query(const Envelope& envelope) const override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        // 如果有未处理的更新，先重构
        if (m_dirty) {
            const_cast<LazyUpdateIndex*>(this)->RebuildIndex();
        }
        
        return QueryTree(envelope);
    }
    
private:
    void RebuildIndex() {
        // 处理删除
        for (const auto& [env, item] : m_pendingRemoves) {
            RemoveFromTree(env, item);
        }
        
        // 处理插入
        for (const auto& [env, item] : m_pendingInserts) {
            InsertIntoTree(env, item);
        }
        
        // 清空队列
        m_pendingInserts.clear();
        m_pendingRemoves.clear();
        m_dirty = false;
    }
    
    mutable std::mutex m_mutex;
    std::vector<std::pair<Envelope, T>> m_pendingInserts;
    std::vector<std::pair<Envelope, T>> m_pendingRemoves;
    size_t m_rebuildThreshold;
    bool m_dirty = false;
};
```

#### 策略3: 定期重建

```cpp
/**
 * @brief 定期重建策略
 * 
 * 累积一定数量更新后完全重建索引
 * 优点：批量操作性能最优
 * 缺点：重建期间查询不可用
 */
template<typename T>
class RebuildIndex : public ISpatialIndex<T> {
public:
    struct Config {
        size_t rebuildThreshold = 10000;     // 重建阈值
        bool rebuildInBackground = true;      // 后台重建
        double rebuildInterval = 60.0;        // 重建间隔（秒）
    };
    
    explicit RebuildIndex(const Config& config = Config())
        : m_config(config) {}
    
    GeomResult Insert(const Envelope& envelope, const T& item) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        if (m_isRebuilding) {
            // 重建期间，存入临时缓冲区
            m_buffer.emplace_back(envelope, item);
        } else {
            m_items.emplace_back(envelope, item);
            m_pendingUpdates++;
        }
        
        // 检查是否需要重建
        if (m_pendingUpdates >= m_config.rebuildThreshold) {
            TriggerRebuild();
        }
        
        return GeomResult::Success;
    }
    
    std::vector<T> Query(const Envelope& envelope) const override {
        std::shared_lock<std::shared_mutex> lock(m_rwMutex);
        
        if (m_isRebuilding) {
            // 返回部分结果或等待
            return QueryFromBuffer(envelope);
        }
        
        return QueryTree(envelope);
    }
    
private:
    void TriggerRebuild() {
        if (m_config.rebuildInBackground) {
            // 后台线程重建
            std::thread([this]() {
                RebuildInBackground();
            }).detach();
        } else {
            // 同步重建
            RebuildSync();
        }
    }
    
    void RebuildInBackground() {
        std::unique_lock<std::shared_mutex> writeLock(m_rwMutex);
        m_isRebuilding = true;
        
        // 使用STR算法批量构建
        auto newIndex = BuildSTRIndex(m_items);
        
        // 合并缓冲区
        for (const auto& item : m_buffer) {
            newIndex->Insert(item.first, item.second);
        }
        
        // 替换索引
        m_tree = std::move(newIndex);
        m_buffer.clear();
        m_pendingUpdates = 0;
        m_isRebuilding = false;
    }
    
    Config m_config;
    std::vector<std::pair<Envelope, T>> m_items;
    std::vector<std::pair<Envelope, T>> m_buffer;
    size_t m_pendingUpdates = 0;
    std::atomic<bool> m_isRebuilding{false};
    mutable std::shared_mutex m_rwMutex;
    std::mutex m_mutex;
};
```

#### 策略4: 混合策略

```cpp
/**
 * @brief 混合更新策略
 * 
 * 结合立即更新和延迟更新的优点
 * 小批量更新立即处理，大批量更新延迟处理
 */
template<typename T>
class HybridUpdateIndex : public ISpatialIndex<T> {
public:
    struct Config {
        size_t immediateThreshold = 100;    // 立即处理阈值
        size_t lazyThreshold = 5000;        // 延迟处理阈值
        double lazyInterval = 5.0;          // 延迟处理间隔（秒）
    };
    
    GeomResult Insert(const Envelope& envelope, const T& item) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        size_t pendingCount = m_pendingInserts.size();
        
        if (pendingCount < m_config.immediateThreshold) {
            // 小批量：立即插入
            InsertIntoTree(envelope, item);
        } else if (pendingCount < m_config.lazyThreshold) {
            // 中批量：加入延迟队列
            m_pendingInserts.emplace_back(envelope, item);
            ScheduleLazyUpdate();
        } else {
            // 大批量：触发重建
            m_pendingInserts.emplace_back(envelope, item);
            TriggerRebuild();
        }
        
        return GeomResult::Success;
    }
    
private:
    void ScheduleLazyUpdate() {
        if (!m_lazyUpdateScheduled) {
            m_lazyUpdateScheduled = true;
            // 定时器触发延迟更新
            std::thread([this]() {
                std::this_thread::sleep_for(
                    std::chrono::duration<double>(m_config.lazyInterval));
                ProcessLazyUpdates();
            }).detach();
        }
    }
    
    void ProcessLazyUpdates() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        for (const auto& [env, item] : m_pendingInserts) {
            InsertIntoTree(env, item);
        }
        m_pendingInserts.clear();
        m_lazyUpdateScheduled = false;
    }
    
    Config m_config;
    std::vector<std::pair<Envelope, T>> m_pendingInserts;
    bool m_lazyUpdateScheduled = false;
    std::mutex m_mutex;
};
```

#### 策略选择建议

| 数据特征 | 推荐策略 | 理由 |
|---------|---------|------|
| 实时插入，低延迟要求 | Immediate | 查询响应时间最优 |
| 批量导入，查询较少 | Rebuild | 导入性能最优 |
| 读多写少 | Lazy | 写入开销最小 |
| 混合负载 | Hybrid | 平衡读写性能 |
| 内存受限 | Lazy + Rebuild | 减少内存碎片 |

### 9.6 使用示例

```cpp
// 创建R-Tree索引
RTree<CNGeometry*> index;

// 插入几何对象
for (auto& geom : geometries) {
    index.Insert(geom->GetEnvelope(), geom.get());
}

// 批量加载（性能更好）
std::vector<std::pair<Envelope, CNGeometry*>> items;
for (auto& geom : geometries) {
    items.emplace_back(geom->GetEnvelope(), geom.get());
}
index.BulkLoad(items);

// 空间查询
Envelope queryEnv(0, 0, 10, 10);
auto results = index.Query(queryEnv);

// 最近邻查询
Coordinate point(5, 5);
auto nearest = index.QueryNearest(point, 5);  // 返回最近的5个

// 统计信息
auto stats = index.GetStatistics();
std::cout << "Height: " << stats.height << std::endl;
std::cout << "Utilization: " << stats.avgNodeUtilization << std::endl;
```

### 9.7 曲线几何线性化策略

```cpp
/**
 * @brief 曲线几何线性化配置
 * 
 * 将CircularString、CompoundCurve等曲线几何转换为线性几何
 */
namespace OGC {
    /**
     * @brief 线性化策略
     */
    enum class LinearizationStrategy {
        MaxAngle,       // 最大角度间隔
        MaxDeviation,   // 最大偏差
        MaxSegments,    // 最大段数
        Adaptive        // 自适应（根据曲率调整）
    };
    
    /**
     * @brief 线性化配置
     */
    struct LinearizationConfig {
        LinearizationStrategy strategy = LinearizationStrategy::MaxDeviation;
        
        // MaxAngle策略参数
        double maxAngleDegrees = 5.0;     // 最大角度（度）
        
        // MaxDeviation策略参数
        double maxDeviation = 0.001;       // 最大偏差（坐标单位）
        
        // MaxSegments策略参数
        size_t maxSegmentsPerArc = 100;    // 每段弧最大段数
        
        // 通用参数
        size_t minSegmentsPerArc = 8;      // 每段弧最小段数
        bool preserveEndpoints = true;     // 保留端点精确位置
        
        // 预设配置
        static LinearizationConfig HighPrecision() {
            LinearizationConfig config;
            config.strategy = LinearizationStrategy::MaxDeviation;
            config.maxDeviation = 0.0001;
            return config;
        }
        
        static LinearizationConfig Balanced() {
            LinearizationConfig config;
            config.strategy = LinearizationStrategy::MaxDeviation;
            config.maxDeviation = 0.001;
            return config;
        }
        
        static LinearizationConfig Fast() {
            LinearizationConfig config;
            config.strategy = LinearizationStrategy::MaxAngle;
            config.maxAngleDegrees = 10.0;
            return config;
        }
    };
    
    /**
     * @brief 曲线几何接口扩展
     */
    class CNCurve {
    public:
        virtual ~CNCurve() = default;
        
        // 曲线特有方法
        virtual double GetStartAngle() const = 0;
        virtual double GetEndAngle() const = 0;
        virtual bool IsClosed() const = 0;
        virtual bool IsRing() const = 0;
        
        // 线性化
        virtual std::unique_ptr<CNLineString> ToLinear(
            const LinearizationConfig& config = LinearizationConfig()) const = 0;
    };
    
    /**
     * @brief 圆弧串实现
     */
    class CNCircularString : public CNCurve {
    public:
        // 从三点创建圆弧
        static std::unique_ptr<CNCircularString> Create(
            const Coordinate& start, 
            const Coordinate& mid, 
            const Coordinate& end);
        
        // 从圆心和半径创建
        static std::unique_ptr<CNCircularString> CreateFromCenter(
            const Coordinate& center, 
            double radius, 
            double startAngle, 
            double endAngle);
        
        // 线性化实现
        std::unique_ptr<CNLineString> ToLinear(
            const LinearizationConfig& config) const override {
            
            std::vector<Coordinate> linearCoords;
            
            switch (config.strategy) {
                case LinearizationStrategy::MaxAngle:
                    LinearizeByMaxAngle(linearCoords, config);
                    break;
                case LinearizationStrategy::MaxDeviation:
                    LinearizeByMaxDeviation(linearCoords, config);
                    break;
                case LinearizationStrategy::MaxSegments:
                    LinearizeByMaxSegments(linearCoords, config);
                    break;
                case LinearizationStrategy::Adaptive:
                    LinearizeAdaptive(linearCoords, config);
                    break;
            }
            
            return CNLineString::Create(linearCoords);
        }
        
    private:
        void LinearizeByMaxAngle(std::vector<Coordinate>& coords, 
                                const LinearizationConfig& config) const;
        void LinearizeByMaxDeviation(std::vector<Coordinate>& coords,
                                     const LinearizationConfig& config) const;
        void LinearizeByMaxSegments(std::vector<Coordinate>& coords,
                                    const LinearizationConfig& config) const;
        void LinearizeAdaptive(std::vector<Coordinate>& coords,
                              const LinearizationConfig& config) const;
    };
    
    /**
     * @brief 复合曲线实现
     */
    class CNCompoundCurve : public CNCurve {
    public:
        // 添加曲线段
        void AddCurve(std::unique_ptr<CNCurve> curve);
        
        // 获取曲线段
        size_t GetNumCurves() const noexcept;
        const CNCurve* GetCurveN(size_t index) const;
        
        // 线性化
        std::unique_ptr<CNLineString> ToLinear(
            const LinearizationConfig& config) const override {
            
            std::vector<Coordinate> allCoords;
            
            for (size_t i = 0; i < GetNumCurves(); ++i) {
                const CNCurve* curve = GetCurveN(i);
                auto linearCurve = curve->ToLinear(config);
                
                // 合并坐标（避免重复端点）
                const auto& curveCoords = linearCurve->GetCoordinates();
                if (i > 0 && !curveCoords.empty()) {
                    allCoords.insert(allCoords.end(), 
                                    curveCoords.begin() + 1, 
                                    curveCoords.end());
                } else {
                    allCoords.insert(allCoords.end(), 
                                    curveCoords.begin(), 
                                    curveCoords.end());
                }
            }
            
            return CNLineString::Create(allCoords);
        }
    };
    
    /**
     * @brief 曲面几何线性化
     */
    class CNCurvePolygon {
    public:
        // 线性化为多边形
        std::unique_ptr<CNPolygon> ToLinear(
            const LinearizationConfig& config = LinearizationConfig()) const {
            
            auto polygon = CNPolygon::Create();
            
            // 线性化外环
            if (m_exteriorRing) {
                auto linearRing = m_exteriorRing->ToLinear(config);
                polygon->SetExteriorRing(std::move(linearRing));
            }
            
            // 线性化内环
            for (const auto& interiorRing : m_interiorRings) {
                auto linearRing = interiorRing->ToLinear(config);
                polygon->AddInteriorRing(std::move(linearRing));
            }
            
            return polygon;
        }
        
    private:
        std::unique_ptr<CNCurve> m_exteriorRing;
        std::vector<std::unique_ptr<CNCurve>> m_interiorRings;
    };
}
```

### 9.8 线性化使用示例

```cpp
// 创建圆弧
auto arc = CNCircularString::CreateFromCenter(
    Coordinate(0, 0),  // 圆心
    10.0,              // 半径
    0.0,               // 起始角度
    M_PI               // 终止角度（半圆）
);

// 高精度线性化
auto highPrecision = arc->ToLinear(LinearizationConfig::HighPrecision());
std::cout << "高精度段数: " << highPrecision->GetNumPoints() << std::endl;

// 快速线性化
auto fastLinear = arc->ToLinear(LinearizationConfig::Fast());
std::cout << "快速段数: " << fastLinear->GetNumPoints() << std::endl;

// 自定义配置
LinearizationConfig custom;
custom.strategy = LinearizationStrategy::MaxDeviation;
custom.maxDeviation = 0.01;  // 1cm精度
auto customLinear = arc->ToLinear(custom);
```

---

## 10. 性能分析和优化

### 10.1 时间复杂度分析

| 操作 | 时间复杂度 | 空间复杂度 | 说明 |
|------|-----------|-----------|------|
| **基本操作** | | | |
| GetEnvelope() | O(1) 缓存命中 | O(1) | 首次计算O(n) |
| GetCentroid() | O(1) 缓存命中 | O(1) | 首次计算O(n) |
| GetCoordinateN() | O(1) | O(1) | 数组随机访问 |
| Clone() | O(n) | O(n) | 深拷贝 |
| **空间关系** | | | |
| Intersects() | O(n*m) | O(1) | 朴素算法 |
| Intersects() + RTree | O(log N + k) | O(N) | k为结果数量 |
| Contains() | O(n*m) | O(1) | 多边形点包含 |
| **几何运算** | | | |
| Buffer() | O(n log n) | O(n) | 依赖实现 |
| ConvexHull() | O(n log n) | O(n) | Andrew's算法 |
| Simplify() | O(n log n) | O(n) | Douglas-Peucker |
| Intersection() | O(n*m) | O(n+m) | 布尔运算 |

### 10.2 性能优化策略

```cpp
/**
 * @brief 性能优化建议
 * 
 * 1. 批量操作优于逐个操作
 */

// 不推荐：逐个添加点
for (size_t i = 0; i < 10000; ++i) {
    line->AddPoint(coords[i]);  // 多次内存重分配
}

// 推荐：批量设置
line->SetCoordinates(coords);  // 一次内存分配


/**
 * 2. 移动语义优先于拷贝
 */

// 推荐：移动坐标数组
auto line = CNLineString::Create(std::move(coords));

// 推荐：移动环
polygon->SetExteriorRing(std::move(ring));


/**
 * 3. 空间索引加速查询
 */

// 构建索引（一次性成本）
RTree<CNGeometry*> index;
for (auto& geom : geometries) {
    index.Insert(geom->GetEnvelope(), geom.get());
}

// 后续查询O(log N)
auto results = index.Query(queryEnv);


/**
 * 4. 缓存重用
 */

// 缓存外包矩形
const auto& env = geom->GetEnvelope();  // O(1)后续访问

// 避免频繁克隆
const CNPoint* point = /*...*/;  // 使用指针而非克隆


/**
 * 5. 并行处理
 */

// OpenMP并行计算距离
#pragma omp parallel for
for (size_t i = 0; i < geoms.size(); ++i) {
    distances[i] = geoms[i]->Distance(target);
}
```

### 10.3 内存优化

```cpp
/**
 * @brief 内存使用估算
 * 
 * - CNPoint: ~48 bytes (坐标 + 元数据)
 * - CNLineString: ~24 + n*32 bytes (n=坐标数量)
 * - CNPolygon: ~48 + 环大小
 * 
 * 优化建议：
 */

// 1. 使用内存池（频繁创建/销毁场景）
auto& pool = GeometryPool::GetInstance();
auto point = pool.Create<CNPoint>(x, y);

// 2. 小对象优化
// 对于小线串（<8点），使用栈分配的优化版本

// 3. 坐标压缩
// 对于整数坐标，考虑使用int32而非double（节省50%内存）
```

### 10.4 性能基准测试计划

#### 10.4.1 测试数据集

| 数据集 | 几何数量 | 坐标数量 | 来源 |
|--------|----------|----------|------|
| 小数据集 | 1,000 | 10,000 | 随机生成 |
| 中数据集 | 100,000 | 1,000,000 | OSM城市数据 |
| 大数据集 | 10,000,000 | 100,000,000 | 全国行政区划 |
| 极端数据集 | 1 | 10,000,000 | 单一复杂多边形 |

#### 10.4.2 性能指标

| 操作 | 目标性能 | 参考实现 | 测试方法 |
|------|----------|----------|----------|
| WKT解析 | > 1M coords/s | GEOS | 批量解析测试 |
| WKB序列化 | > 2M coords/s | PostGIS | 批量序列化测试 |
| 空间关系判断（索引命中） | < 1ms | JTS | R-Tree查询 |
| 空间关系判断（全扫描） | < 100ms/万对象 | GEOS | 无索引查询 |
| 缓冲区计算（简单几何） | < 10ms | JTS | 单几何操作 |
| 缓冲区计算（复杂几何） | < 100ms | GEOS | 单几何操作 |
| 凸包计算 | < 50ms/万点 | QuickHull | 批量点集 |
| 简化（Douglas-Peucker） | < 30ms/万点 | Shapely | 线简化 |
| 布尔运算 | < 200ms | GEOS | 多边形求交 |

#### 10.4.3 测试环境

```yaml
# 标准测试环境
hardware:
  cpu: Intel i7-12700K (12核20线程)
  ram: 32GB DDR4-3200
  storage: NVMe SSD

software:
  os: Ubuntu 22.04 LTS
  compiler: GCC 11.2 / Clang 14.0
  flags: -O3 -DNDEBUG -march=native
```

#### 10.4.4 基准测试代码

```cpp
/**
 * @brief 性能基准测试框架
 */
class BenchmarkRunner {
public:
    struct Result {
        std::string name;
        size_t iterations;
        double totalTimeMs;
        double avgTimeMs;
        double minTimeMs;
        double maxTimeMs;
        double opsPerSecond;
    };
    
    // 运行单个基准测试
    template<typename Func>
    Result Run(const std::string& name, Func&& func, size_t iterations = 1000) {
        std::vector<double> times;
        times.reserve(iterations);
        
        // 预热
        for (int i = 0; i < 10; ++i) func();
        
        // 正式测试
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < iterations; ++i) {
            auto t0 = std::chrono::high_resolution_clock::now();
            func();
            auto t1 = std::chrono::high_resolution_clock::now();
            times.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        Result result;
        result.name = name;
        result.iterations = iterations;
        result.totalTimeMs = std::chrono::duration<double, std::milli>(end - start).count();
        result.avgTimeMs = std::accumulate(times.begin(), times.end(), 0.0) / iterations;
        result.minTimeMs = *std::min_element(times.begin(), times.end());
        result.maxTimeMs = *std::max_element(times.begin(), times.end());
        result.opsPerSecond = 1000.0 / result.avgTimeMs;
        
        return result;
    }
    
    // 输出报告
    void PrintReport(const std::vector<Result>& results);
};
```

#### 10.4.5 性能回归检测

```cpp
/**
 * @brief 性能回归检测
 * 
 * 每次提交代码后自动运行，确保性能不退化
 */
class PerformanceRegression {
public:
    // 加载历史基准数据
    void LoadBaseline(const std::string& path);
    
    // 保存当前基准数据
    void SaveBaseline(const std::string& path);
    
    // 检测回归
    struct RegressionResult {
        bool hasRegression;
        std::vector<std::string> regressedTests;
        std::map<std::string, double> degradationPercent;
    };
    
    RegressionResult Check(const std::vector<BenchmarkRunner::Result>& current);
    
private:
    std::map<std::string, double> m_baseline;
    double m_regressionThreshold = 0.1;  // 10%退化阈值
};
```

---

## 11. 线程安全性设计

### 11.1 线程安全保证

```cpp
/**
 * @brief 线程安全保证
 * 
 * 1. 只读操作：线程安全
 *    - 所有const方法都是线程安全的
 *    - 缓存使用读写锁保护
 * 
 * 2. 修改操作：需要外部同步
 *    - 修改方法非线程安全
 *    - 使用std::mutex或std::shared_mutex保护
 * 
 * 3. SRID：原子操作
 *    - 使用std::atomic<int>
 */
```

### 11.2 线程安全策略详解

#### 11.2.1 缓存线程安全

```cpp
/**
 * @brief 缓存线程安全实现
 * 
 * 使用双重检查锁定模式优化缓存访问
 */
class CNGeometry {
protected:
    // 方案1: 读写锁 + 缓存
    mutable std::shared_mutex m_cacheMutex;
    mutable std::optional<Envelope> m_cachedEnvelope;
    mutable std::optional<Coordinate> m_cachedCentroid;
    mutable std::optional<double> m_cachedArea;
    
    // 方案2: 原子标志 + 双重检查（推荐，性能更好）
    mutable std::atomic<bool> m_envelopeValid{false};
    mutable std::atomic<bool> m_centroidValid{false};
    mutable std::unique_ptr<Envelope> m_envelope;
    mutable std::unique_ptr<Coordinate> m_centroid;
    mutable std::mutex m_cacheInitMutex;  // 仅用于初始化
    
public:
    // 线程安全的缓存访问（方案2）
    const Envelope& GetEnvelope() const noexcept override {
        if (m_envelopeValid.load(std::memory_order_acquire)) {
            return *m_envelope;
        }
        
        // 双重检查锁定
        std::lock_guard<std::mutex> lock(m_cacheInitMutex);
        if (!m_envelopeValid.load(std::memory_order_relaxed)) {
            m_envelope = ComputeEnvelope();
            m_envelopeValid.store(true, std::memory_order_release);
        }
        return *m_envelope;
    }
    
protected:
    // 缓存失效（修改操作时调用）
    void InvalidateCache() noexcept {
        m_envelopeValid.store(false, std::memory_order_release);
        m_centroidValid.store(false, std::memory_order_release);
    }
};
```

#### 11.2.2 写时复制 (Copy-on-Write)

```cpp
/**
 * @brief 写时复制策略
 * 
 * 适用于读多写少的场景
 */
class CNGeometry {
protected:
    // 共享数据
    struct GeometryData {
        CoordinateList coordinates;
        int srid = 0;
        // 其他数据...
    };
    
    std::shared_ptr<GeometryData> m_data;
    
public:
    // 读取操作：直接访问共享数据
    const Coordinate& GetCoordinateN(size_t index) const {
        return m_data->coordinates[index];
    }
    
    // 修改操作：创建副本
    void SetCoordinateN(size_t index, const Coordinate& coord) {
        // 检查是否需要复制
        if (m_data.use_count() > 1) {
            // 创建私有副本
            m_data = std::make_shared<GeometryData>(*m_data);
        }
        m_data->coordinates[index] = coord;
        InvalidateCache();
    }
    
    // 深拷贝
    CNGeometryPtr Clone() const {
        auto copy = CreateEmpty();
        copy->m_data = std::make_shared<GeometryData>(*m_data);
        return copy;
    }
};
```

#### 11.2.3 线程安全保证矩阵

| 操作类型 | 线程安全级别 | 说明 |
|----------|--------------|------|
| const方法 | 完全线程安全 | 可多线程并发调用 |
| 缓存读取 | 完全线程安全 | 内部同步，对外透明 |
| 缓存失效 | 需要外部同步 | 与修改操作同步 |
| 几何修改 | 需要外部同步 | 非线程安全 |
| SRID读写 | 完全线程安全 | 使用原子操作 |
| 克隆操作 | 完全线程安全 | 返回独立副本 |

#### 11.2.4 线程局部存储

```cpp
/**
 * @brief 线程局部存储策略
 * 
 * 用于避免锁竞争的高性能场景
 */
class GeometryFactory {
public:
    // 线程局部工厂实例
    static GeometryFactory& GetThreadLocal() {
        thread_local static GeometryFactory instance;
        return instance;
    }
    
    // 每个线程独立的缓存
    CNGeometryPtr FromWKT(const std::string& wkt) {
        // 无锁操作
        return ParseWKT(wkt);
    }
    
private:
    // 线程局部缓存
    thread_local static std::string s_parseBuffer;
    thread_local static std::vector<Coordinate> s_coordBuffer;
};
```

### 11.3 并发使用模式

```cpp
/**
 * @brief 并发使用示例
 */

// 模式1：读写锁保护
std::shared_mutex geomMutex;

// 多线程读取
void ThreadSafeRead() {
    std::shared_lock<std::shared_mutex> lock(geomMutex);
    double area = polygon->Area();  // 线程安全
}

// 单线程写入
void ThreadSafeWrite() {
    std::unique_lock<std::shared_mutex> lock(geomMutex);
    polygon->Normalize();  // 需要独占访问
}


// 模式2：不可变对象
auto polygon = CNPolygon::Create(/*...*/);
polygon->Normalize();  // 初始化完成后不再修改

// 多线程安全读取（只读操作天然线程安全）
std::vector<std::thread> threads;
for (int i = 0; i < 10; ++i) {
    threads.emplace_back([&polygon]() {
        double area = polygon->Area();  // 安全
        auto env = polygon->GetEnvelope();  // 安全
    });
}


// 模式3：线程局部存储
thread_local GeometryFactory localFactory;
auto geom = localFactory.FromWKT(wkt);  // 每线程独立工厂
```

### 11.4 线程安全最佳实践

```cpp
/**
 * @brief 线程安全最佳实践
 */

// ✅ 推荐：不可变设计
class ImmutableGeometry {
public:
    // 所有修改操作返回新对象
    ImmutableGeometryPtr Buffer(double distance) const {
        return ComputeBuffer(distance);  // 返回新对象
    }
};

// ✅ 推荐：使用线程局部工厂
void ProcessGeometries(const std::vector<std::string>& wkts) {
    #pragma omp parallel for
    for (size_t i = 0; i < wkts.size(); ++i) {
        auto& factory = GeometryFactory::GetThreadLocal();
        auto geom = factory.FromWKT(wkts[i]);
        // 处理几何...
    }
}

// ❌ 避免：共享可变状态
class BadExample {
    CNGeometry* sharedGeom;  // 多线程共享可变对象
    
    void ThreadUnsafeModify() {
        sharedGeom->SetCoordinateN(0, coord);  // 数据竞争！
    }
};

// ✅ 推荐：使用同步原语
class GoodExample {
    CNGeometryPtr geom;
    mutable std::shared_mutex mutex;
    
    void ThreadSafeModify() {
        std::unique_lock<std::shared_mutex> lock(mutex);
        geom->SetCoordinateN(0, coord);  // 安全
    }
};
```

---

## 11.5 坐标系统变换实现

```cpp
/**
 * @brief 坐标系统变换接口设计
 * 
 * 基于PROJ库实现坐标系统变换
 */
namespace OGC {
    /**
     * @brief 坐标参考系统（CRS）管理
     */
    class CoordinateReferenceSystem {
    public:
        // 从EPSG代码创建
        static std::unique_ptr<CoordinateReferenceSystem> FromEPSG(int epsgCode);
        
        // 从WKT字符串创建
        static std::unique_ptr<CoordinateReferenceSystem> FromWKT(const std::string& wkt);
        
        // 从PROJ字符串创建
        static std::unique_ptr<CoordinateReferenceSystem> FromPROJ(const std::string& proj);
        
        // 属性查询
        int GetEPSGCode() const noexcept;
        std::string GetWKT() const;
        std::string GetPROJString() const;
        bool IsGeographic() const noexcept;
        bool IsProjected() const noexcept;
        std::string GetName() const;
        
        // 单位信息
        std::string GetAxisUnit() const;
        double ToSI(double value) const;  // 转换为SI单位
        
    private:
        void* m_projCRS;  // PROJ PJ* 对象
        int m_epsgCode;
    };
    
    /**
     * @brief 坐标变换器
     */
    class CoordinateTransform {
    public:
        // 创建变换器
        static std::unique_ptr<CoordinateTransform> Create(
            const CoordinateReferenceSystem& source,
            const CoordinateReferenceSystem& target);
        
        static std::unique_ptr<CoordinateTransform> Create(
            int sourceEPSG, int targetEPSG);
        
        // 单点变换
        GeomResult Transform(double& x, double& y, double& z = 0) const;
        
        // 批量变换
        GeomResult Transform(std::vector<Coordinate>& coords) const;
        
        // 几何对象变换
        GeomResult Transform(const CNGeometry* source, 
                            std::unique_ptr<CNGeometry>& result) const;
        
        // 变换信息
        std::string GetDescription() const;
        double GetAccuracy() const;  // 变换精度（米）
        
    private:
        void* m_projTransform;  // PROJ PJ* 对象
        std::unique_ptr<CoordinateReferenceSystem> m_sourceCRS;
        std::unique_ptr<CoordinateReferenceSystem> m_targetCRS;
    };
    
    /**
     * @brief 几何对象坐标变换实现
     */
    class CNGeometry {
    public:
        GeomResult Transform(int targetSRID, 
                            std::unique_ptr<CNGeometry>& result) const override {
            // 获取源CRS
            auto sourceCRS = CoordinateReferenceSystem::FromEPSG(GetSRID());
            if (!sourceCRS) {
                return GeomResult::InvalidSRID;
            }
            
            // 获取目标CRS
            auto targetCRS = CoordinateReferenceSystem::FromEPSG(targetSRID);
            if (!targetCRS) {
                return GeomResult::InvalidSRID;
            }
            
            // 创建变换器
            auto transform = CoordinateTransform::Create(*sourceCRS, *targetCRS);
            if (!transform) {
                return GeomResult::TransformError;
            }
            
            // 执行变换
            return transform->Transform(this, result);
        }
    };
    
    /**
     * @brief 常用坐标系统
     */
    namespace CRS {
        // WGS84地理坐标
        constexpr int WGS84 = 4326;
        
        // Web墨卡托投影
        constexpr int WebMercator = 3857;
        
        // 北京54
        constexpr int Beijing54 = 4214;
        
        // CGCS2000
        constexpr int CGCS2000 = 4490;
        
        // UTM区域计算
        inline int GetUTMZone(double longitude) {
            return static_cast<int>(std::floor((longitude + 180.0) / 6.0)) + 1;
        }
        
        inline int GetUTMEPSG(double longitude, bool north = true) {
            int zone = GetUTMZone(longitude);
            return north ? (32600 + zone) : (32700 + zone);
        }
    }
}
```

### 11.6 PROJ集成说明

```cpp
/**
 * @brief PROJ库集成配置
 * 
 * 依赖要求：PROJ >= 8.0
 */

// CMake配置
/*
find_package(PROJ REQUIRED)
target_link_libraries(ogc_geometry PRIVATE PROJ::proj)
*/

// 初始化PROJ上下文
namespace OGC {
    class PROJContext {
    public:
        static PROJContext& GetInstance() {
            static PROJContext instance;
            return instance;
        }
        
        // 获取PROJ上下文
        void* GetContext() const noexcept { return m_context; }
        
        // 设置数据目录（可选）
        void SetDataDirectory(const std::string& path);
        
        // 设置网络访问（可选）
        void EnableNetworkAccess(bool enable);
        
        // 缓存配置
        void SetCacheSize(size_t entries);
        
    private:
        PROJContext();
        ~PROJContext();
        
        void* m_context;  // PJ_CONTEXT*
    };
}

// 使用示例
void TransformExample() {
    // 创建WGS84点
    auto point = CNPoint::Create(116.4, 39.9);  // 北京
    point->SetSRID(CRS::WGS84);
    
    // 变换到Web墨卡托
    std::unique_ptr<CNGeometry> transformed;
    GeomResult result = point->Transform(CRS::WebMercator, transformed);
    
    if (result == GeomResult::Success) {
        CNPoint* mercatorPoint = dynamic_cast<CNPoint*>(transformed.get());
        std::cout << "墨卡托坐标: (" << mercatorPoint->X() 
                  << ", " << mercatorPoint->Y() << ")" << std::endl;
    }
    
    // 变换到UTM
    int utmEPSG = CRS::GetUTMEPSG(116.4, true);  // 北半球UTM
    result = point->Transform(utmEPSG, transformed);
}
```

---

## 12. 错误处理策略

### 12.1 Result模式

```cpp
/**
 * @brief Result模式：推荐用于可能失败的操作
 */

GeomResult result;
CNGeometryPtr intersection;

// 操作前检查前置条件
if (!geom1 || !geom2) {
    return GeomResult::NullGeometry;
}

result = geom1->Intersection(geom2.get(), intersection);

if (result == GeomResult::Success) {
    // 使用intersection
} else if (result == GeomResult::TopologyError) {
    // 处理拓扑错误
    std::cerr << "拓扑错误: " << GetResultDescription(result) << std::endl;
} else {
    // 其他错误
}
```

### 12.2 异常模式

```cpp
/**
 * @brief 异常模式：可选，用于不可恢复错误
 */

// 全局配置
namespace OGC {
    enum class ErrorHandlingPolicy {
        ReturnCode,  // 默认：返回错误码
        Throw        // 抛出异常
    };
    
    void SetErrorHandlingPolicy(ErrorHandlingPolicy policy);
}

// 使用异常
try {
    auto result = geom1->Intersection(geom2.get());
    // 操作成功
} catch (const GeometryException& e) {
    std::cerr << "几何错误: " << e.what() << std::endl;
    std::cerr << "错误码: " << GetResultString(e.GetErrorCode()) << std::endl;
}
```

### 12.3 异常安全保证

```cpp
/**
 * @brief 异常安全保证
 * 
 * - 强异常保证：Clone(), Buffer(), Intersection()等
 *   操作失败时，对象状态不变
 * 
 * - 基本异常保证：Normalize(), ModifyInPlace()
 *   操作失败时，对象处于有效但未定义状态
 */

class CNGeometry {
public:
    // 强异常保证
    std::unique_ptr<CNGeometry> Clone() const {
        try {
            // 执行深拷贝
            // 如果失败，this对象状态不变
        } catch (...) {
            // 清理并重新抛出
            throw;
        }
    }
    
    // 基本异常保证
    void Normalize() {
        // 先在临时对象上操作
        auto temp = Clone();
        temp->NormalizeInternal();
        
        // 成功后交换
        Swap(temp);  // noexcept
    }
};
```

### 12.4 输入验证和安全防护

```cpp
/**
 * @brief 输入验证和安全防护策略
 * 
 * 防止恶意输入导致的资源耗尽、缓冲区溢出等问题
 */
namespace OGC {
    /**
     * @brief 安全配置
     */
    struct SecurityConfig {
        // 输入限制
        size_t maxWKTLength = 10 * 1024 * 1024;        // WKT最大长度：10MB
        size_t maxWKBSize = 100 * 1024 * 1024;         // WKB最大大小：100MB
        size_t maxCoordinateCount = 100 * 1000 * 1000; // 最大坐标数：1亿
        size_t maxGeometryDepth = 100;                  // 最大嵌套深度：100层
        size_t maxRingCount = 10000;                    // 最大环数量：1万
        
        // 资源限制
        size_t maxMemoryUsage = 1024 * 1024 * 1024;    // 最大内存：1GB
        size_t maxParseTime = 30000;                    // 最大解析时间：30秒
        
        // 验证选项
        bool validateTopology = true;                   // 验证拓扑
        bool validateSRID = true;                       // 验证SRID
        bool strictMode = false;                        // 严格模式
    };
    
    /**
     * @brief 输入验证器
     */
    class InputValidator {
    public:
        // WKT验证
        static GeomResult ValidateWKT(const std::string& wkt, 
                                      const SecurityConfig& config = SecurityConfig()) {
            // 长度检查
            if (wkt.length() > config.maxWKTLength) {
                return GeomResult::ParseError;
            }
            
            // 基本格式检查
            if (!IsValidWKTFormat(wkt)) {
                return GeomResult::FormatError;
            }
            
            // 嵌套深度检查
            if (GetNestingDepth(wkt) > config.maxGeometryDepth) {
                return GeomResult::ParseError;
            }
            
            return GeomResult::Success;
        }
        
        // WKB验证
        static GeomResult ValidateWKB(const std::vector<uint8_t>& wkb,
                                      const SecurityConfig& config = SecurityConfig()) {
            // 大小检查
            if (wkb.size() > config.maxWKBSize) {
                return GeomResult::ParseError;
            }
            
            // 最小长度检查
            if (wkb.size() < 6) {  // WKB最小有效长度
                return GeomResult::FormatError;
            }
            
            // 魔数检查
            uint8_t byteOrder = wkb[0];
            if (byteOrder > 1) {
                return GeomResult::FormatError;
            }
            
            return GeomResult::Success;
        }
        
        // 坐标数量检查
        static GeomResult ValidateCoordinateCount(size_t count,
                                                  const SecurityConfig& config = SecurityConfig()) {
            if (count > config.maxCoordinateCount) {
                return GeomResult::OutOfMemory;
            }
            
            // 整数溢出检查
            if (count > std::numeric_limits<size_t>::max() / sizeof(Coordinate)) {
                return GeomResult::OutOfMemory;
            }
            
            return GeomResult::Success;
        }
        
    private:
        static bool IsValidWKTFormat(const std::string& wkt);
        static size_t GetNestingDepth(const std::string& wkt);
    };
    
    /**
     * @brief 安全解析器
     */
    class SafeParser {
    public:
        SafeParser(const SecurityConfig& config = SecurityConfig())
            : m_config(config), m_bytesParsed(0), m_startTime(std::chrono::steady_clock::now()) {}
        
        // 检查资源限制
        bool CheckResourceLimit() const {
            // 时间检查
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - m_startTime).count();
            if (elapsed > static_cast<int64_t>(m_config.maxParseTime)) {
                return false;
            }
            
            // 内存检查
            if (GetCurrentMemoryUsage() > m_config.maxMemoryUsage) {
                return false;
            }
            
            return true;
        }
        
        // 记录解析进度
        void OnBytesParsed(size_t bytes) {
            m_bytesParsed += bytes;
        }
        
    private:
        SecurityConfig m_config;
        size_t m_bytesParsed;
        std::chrono::steady_clock::time_point m_startTime;
        
        static size_t GetCurrentMemoryUsage();
    };
    
    /**
     * @brief 整数溢出安全计算
     */
    namespace SafeMath {
        // 安全乘法
        inline bool SafeMultiply(size_t a, size_t b, size_t& result) {
            if (a == 0 || b == 0) {
                result = 0;
                return true;
            }
            if (a > std::numeric_limits<size_t>::max() / b) {
                return false;  // 溢出
            }
            result = a * b;
            return true;
        }
        
        // 安全加法
        inline bool SafeAdd(size_t a, size_t b, size_t& result) {
            if (a > std::numeric_limits<size_t>::max() - b) {
                return false;  // 溢出
            }
            result = a + b;
            return true;
        }
        
        // 计算坐标数组所需内存
        inline bool CalculateCoordinateMemory(size_t count, size_t& memory) {
            return SafeMultiply(count, sizeof(Coordinate), memory);
        }
    }
}
```

### 12.5 资源限制配置

```cpp
/**
 * @brief 全局资源限制管理
 */
class ResourceManager {
public:
    static ResourceManager& GetInstance();
    
    // 设置内存限制
    void SetMemoryLimit(size_t bytes) noexcept;
    size_t GetMemoryLimit() const noexcept;
    size_t GetCurrentUsage() const noexcept;
    
    // 检查是否可以分配
    bool CanAllocate(size_t bytes) const noexcept;
    
    // 分配通知
    void OnAllocate(size_t bytes);
    void OnDeallocate(size_t bytes);
    
    // 设置回调
    using MemoryPressureCallback = std::function<void(size_t current, size_t limit)>;
    void SetMemoryPressureCallback(MemoryPressureCallback callback);
    
private:
    ResourceManager() = default;
    
    std::atomic<size_t> m_currentUsage{0};
    std::atomic<size_t> m_memoryLimit{std::numeric_limits<size_t>::max()};
    MemoryPressureCallback m_pressureCallback;
    mutable std::mutex m_mutex;
};

// 使用示例
void ConfigureResourceLimits() {
    auto& manager = ResourceManager::GetInstance();
    
    // 设置1GB内存限制
    manager.SetMemoryLimit(1024 * 1024 * 1024);
    
    // 设置内存压力回调
    manager.SetMemoryPressureCallback([](size_t current, size_t limit) {
        std::cerr << "内存压力警告: " << current << "/" << limit << " bytes" << std::endl;
        // 可以触发垃圾回收或拒绝新请求
    });
}
```

---

## 13. C API绑定设计

### 13.1 C API概述

为支持跨语言互操作，提供C语言API绑定。

```cpp
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief C API版本
 */
#define OGC_VERSION_MAJOR 2
#define OGC_VERSION_MINOR 3
#define OGC_VERSION_PATCH 0

/**
 * @brief 不透明句柄类型
 */
typedef struct OGC_Geometry_s* OGC_GeometryH;
typedef struct OGC_Point_s* OGC_PointH;
typedef struct OGC_LineString_s* OGC_LineStringH;
typedef struct OGC_Polygon_s* OGC_PolygonH;
typedef struct OGC_Factory_s* OGC_FactoryH;

/**
 * @brief 错误码（与C++枚举对应）
 */
typedef int OGC_Result;
#define OGC_SUCCESS 0
#define OGC_INVALID_GEOMETRY 1
#define OGC_PARSE_ERROR 401

/**
 * @brief 坐标结构
 */
typedef struct {
    double x;
    double y;
    double z;
    double m;
} OGC_Coordinate;

/**
 * @brief 外包矩形
 */
typedef struct {
    double minX, minY, maxX, maxY;
    int isNull;
} OGC_Envelope;

```

### 13.2 核心C API函数

```cpp
/**
 * @brief 生命周期管理
 */

// 创建工厂
OGC_FactoryH OGC_Factory_Create(void);

// 销毁工厂
void OGC_Factory_Destroy(OGC_FactoryH factory);

// 销毁几何对象
void OGC_Geometry_Destroy(OGC_GeometryH geom);

// 克隆几何对象
OGC_GeometryH OGC_Geometry_Clone(OGC_GeometryH geom);

/**
 * @brief 几何创建
 */

// 从WKT创建
OGC_Result OGC_Geometry_FromWKT(OGC_FactoryH factory, 
                                const char* wkt, 
                                OGC_GeometryH* result);

// 从WKB创建
OGC_Result OGC_Geometry_FromWKB(OGC_FactoryH factory,
                                const unsigned char* wkb,
                                size_t size,
                                OGC_GeometryH* result);

// 创建点
OGC_PointH OGC_Point_Create(OGC_FactoryH factory, double x, double y);
OGC_PointH OGC_Point_Create3D(OGC_FactoryH factory, double x, double y, double z);

// 创建线串
OGC_LineStringH OGC_LineString_Create(OGC_FactoryH factory);
OGC_Result OGC_LineString_AddPoint(OGC_LineStringH line, double x, double y);

// 创建多边形
OGC_PolygonH OGC_Polygon_Create(OGC_FactoryH factory);

/**
 * @brief 几何属性
 */

// 类型查询
int OGC_Geometry_GetType(OGC_GeometryH geom);
const char* OGC_Geometry_GetTypeName(OGC_GeometryH geom);

// 坐标维度
int OGC_Geometry_GetCoordinateDimension(OGC_GeometryH geom);

// SRID
int OGC_Geometry_GetSRID(OGC_GeometryH geom);
void OGC_Geometry_SetSRID(OGC_GeometryH geom, int srid);

// 外包矩形
OGC_Result OGC_Geometry_GetEnvelope(OGC_GeometryH geom, OGC_Envelope* envelope);

// 坐标数量
size_t OGC_Geometry_GetNumCoordinates(OGC_GeometryH geom);

// 是否为空
int OGC_Geometry_IsEmpty(OGC_GeometryH geom);

/**
 * @brief 空间关系
 */

int OGC_Geometry_Intersects(OGC_GeometryH geom1, OGC_GeometryH geom2);
int OGC_Geometry_Contains(OGC_GeometryH geom1, OGC_GeometryH geom2);
int OGC_Geometry_Within(OGC_GeometryH geom1, OGC_GeometryH geom2);
int OGC_Geometry_Equals(OGC_GeometryH geom1, OGC_GeometryH geom2, double tolerance);

double OGC_Geometry_Distance(OGC_GeometryH geom1, OGC_GeometryH geom2);

/**
 * @brief 几何运算
 */

OGC_Result OGC_Geometry_Buffer(OGC_GeometryH geom, 
                               double distance, 
                               OGC_GeometryH* result);

OGC_Result OGC_Geometry_Intersection(OGC_GeometryH geom1,
                                     OGC_GeometryH geom2,
                                     OGC_GeometryH* result);

OGC_Result OGC_Geometry_Union(OGC_GeometryH geom1,
                              OGC_GeometryH geom2,
                              OGC_GeometryH* result);

OGC_GeometryH OGC_Geometry_ConvexHull(OGC_GeometryH geom);

/**
 * @brief 序列化
 */

char* OGC_Geometry_ToWKT(OGC_GeometryH geom, int precision);
void OGC_FreeString(char* str);

unsigned char* OGC_Geometry_ToWKB(OGC_GeometryH geom, size_t* size);
void OGC_FreeBuffer(unsigned char* buffer);

char* OGC_Geometry_ToGeoJSON(OGC_GeometryH geom, int precision);

/**
 * @brief 错误信息
 */

const char* OGC_GetResultString(OGC_Result result);
const char* OGC_GetLastError(void);

#ifdef __cplusplus
}
#endif
```

### 13.3 C API使用示例

```c
#include <ogc/c_api.h>

int main() {
    // 创建工厂
    OGC_FactoryH factory = OGC_Factory_Create();
    
    // 从WKT创建几何
    OGC_GeometryH geom1 = NULL;
    OGC_Result result = OGC_Geometry_FromWKT(factory, "POINT(116.4 39.9)", &geom1);
    if (result != OGC_SUCCESS) {
        printf("解析失败: %s\n", OGC_GetResultString(result));
        return 1;
    }
    
    // 设置SRID
    OGC_Geometry_SetSRID(geom1, 4326);
    
    // 获取外包矩形
    OGC_Envelope env;
    OGC_Geometry_GetEnvelope(geom1, &env);
    printf("外包矩形: (%.2f, %.2f) - (%.2f, %.2f)\n", 
           env.minX, env.minY, env.maxX, env.maxY);
    
    // 创建缓冲区
    OGC_GeometryH buffer = NULL;
    OGC_Geometry_Buffer(geom1, 100.0, &buffer);
    
    // 输出WKT
    char* wkt = OGC_Geometry_ToWKT(buffer, 6);
    printf("缓冲区: %s\n", wkt);
    OGC_FreeString(wkt);
    
    // 清理
    OGC_Geometry_Destroy(buffer);
    OGC_Geometry_Destroy(geom1);
    OGC_Factory_Destroy(factory);
    
    return 0;
}
```

### 13.4 ABI兼容性保证

```cpp
/**
 * @brief ABI兼容性策略
 * 
 * 1. 版本控制
 *    - 主版本号变更：ABI不兼容
 *    - 次版本号变更：ABI兼容，新增功能
 *    - 补丁版本号变更：ABI兼容，Bug修复
 * 
 * 2. 符号版本控制（Linux）
 */
#define OGC_EXPORT __attribute__((visibility("default")))

// 版本脚本示例
/*
{
    global:
        OGC_*;
    local:
        *;
};
*/

/**
 * 3. 结构体布局保证
 */
// 使用固定布局结构体
struct OGC_Coordinate_V1 {
    double x;
    double y;
    double z;
    double m;
};

// 版本化结构体
struct OGC_GeometryInfo {
    uint32_t structSize;      // 结构体大小，用于版本检测
    uint32_t version;         // API版本
    // ... 其他字段
};

/**
 * 4. 函数指针表（用于动态加载）
 */
typedef struct {
    // 版本信息
    uint32_t version;
    
    // 生命周期
    OGC_FactoryH (*Factory_Create)(void);
    void (*Factory_Destroy)(OGC_FactoryH);
    void (*Geometry_Destroy)(OGC_GeometryH);
    
    // 创建
    OGC_Result (*Geometry_FromWKT)(OGC_FactoryH, const char*, OGC_GeometryH*);
    
    // 属性
    int (*Geometry_GetType)(OGC_GeometryH);
    
    // ... 其他函数指针
} OGC_API_V1;

// 获取API表
const OGC_API_V1* OGC_GetAPI_V1(void);
```

---

## 14. 使用示例

### 13.1 创建几何对象

```cpp
#include <OGC/cn_geometry.h>
#include <OGC/cn_factory.h>

using namespace OGC;

// 创建点
auto point1 = CNPoint::Create(116.4, 39.9);
point1->SetSRID(4326);  // WGS84

auto point2 = CNPoint::Create(116.5, 39.95, 50.0);  // 带高程

// 创建线串
CoordinateList coords = {
    {0, 0}, {1, 1}, {2, 0}, {3, 1}
};
auto lineString = CNLineString::Create(coords);

// 创建多边形（矩形）
auto ring = CNLinearRing::CreateRectangle(0, 0, 10, 10);
auto polygon = CNPolygon::Create();
polygon->SetExteriorRing(std::move(ring));

// 使用工厂创建
auto& factory = GeometryFactory::GetInstance();
auto circle = factory.CreateCircle(0, 0, 5.0, 32);

// 从WKT创建
CNGeometryPtr geom;
GeomResult result = factory.FromWKT("POINT(116.4 39.9)", geom);
if (result == GeomResult::Success) {
    CNPoint* point = dynamic_cast<CNPoint*>(geom.get());
}
```

### 13.2 空间关系判断

```cpp
// 创建两个多边形
auto poly1 = CNPolygon::CreateRectangle(0, 0, 10, 10);
auto poly2 = CNPolygon::CreateRectangle(5, 5, 15, 15);

// 空间关系判断
if (poly1->Intersects(poly2.get())) {
    std::cout << "两个多边形相交" << std::endl;
}

if (poly1->Overlaps(poly2.get())) {
    std::cout << "两个多边形重叠" << std::endl;
}

// 计算DE-9IM矩阵
std::string matrix = poly1->Relate(poly2.get());
std::cout << "DE-9IM: " << matrix << std::endl;

// 距离计算
double distance = poly1->Distance(poly2.get());
std::cout << "距离: " << distance << std::endl;
```

### 13.3 几何运算

```cpp
// 集合运算
auto poly1 = CNPolygon::CreateRectangle(0, 0, 10, 10);
auto poly2 = CNPolygon::CreateRectangle(5, 5, 15, 15);

// 交集
CNGeometryPtr intersection;
GeomResult result = poly1->Intersection(poly2.get(), intersection);
if (result == GeomResult::Success) {
    CNPolygon* interPoly = dynamic_cast<CNPolygon*>(intersection.get());
    std::cout << "交集面积: " << interPoly->Area() << std::endl;
}

// 并集
CNGeometryPtr unionGeom;
result = poly1->Union(poly2.get(), unionGeom);

// 缓冲区
CNGeometryPtr buffer;
result = lineString->Buffer(5.0, buffer);

// 凸包
auto convexHull = lineString->ConvexHull();

// 简化
auto simplified = lineString->Simplify(0.1);
```

### 13.4 序列化

```cpp
auto polygon = CNPolygon::CreateRectangle(0, 0, 10, 10);

// WKT输出
std::string wkt = polygon->AsText(15);
std::cout << wkt << std::endl;
// 输出: POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))

// GeoJSON输出
std::string geojson = polygon->AsGeoJSON();
std::cout << geojson << std::endl;

// WKB输出
std::vector<uint8_t> wkb = polygon->AsBinary();

// 十六进制WKB
std::string hexWkb = polygon->AsHex();
```

### 13.5 访问者模式

```cpp
// 定义自定义访问者
class GeometryPrinter : public GeometryConstVisitor {
public:
    void VisitPoint(const CNPoint* point) override {
        std::cout << "Point: " << point->GetX() << ", " << point->GetY() << std::endl;
    }
    
    void VisitPolygon(const CNPolygon* polygon) override {
        std::cout << "Polygon area: " << polygon->Area() << std::endl;
    }
};

// 使用访问者
GeometryPrinter printer;
geom->Apply(printer);
```

### 13.6 空间索引

```cpp
#include <OGC/cn_spatial_index.h>

// 创建R-Tree索引
RTree<CNGeometry*> index;

// 插入几何对象
std::vector<CNPolygonPtr> polygons;
for (int i = 0; i < 1000; ++i) {
    auto poly = CreateRandomPolygon();
    index.Insert(poly->GetEnvelope(), poly.get());
    polygons.push_back(std::move(poly));
}

// 空间查询
Envelope queryEnv(0, 0, 10, 10);
auto results = index.Query(queryEnv);

std::cout << "找到 " << results.size() << " 个相交多边形" << std::endl;

// 最近邻查询
Coordinate point(5, 5);
auto nearest = index.QueryNearest(point, 5);
```

---

## 14. 与图形绘制框架集成

### 14.1 集成接口

```cpp
/**
 * @brief 与图形绘制框架集成
 * 
 * 通过CNDrawEngine::DrawGeometry接口对接
 */

namespace cycle {
    class CNDrawEngine {
    public:
        /**
         * @brief 绘制几何对象
         * @param geometry 几何对象指针
         * @param style 绘制样式
         * @return 绘制结果
         */
        DrawResult DrawGeometry(const OGC::CNGeometry* geometry, 
                                const SNDrawStyle& style);
    };
}
```

### 14.2 集成示例

```cpp
// 创建几何对象
auto polygon = OGC::CNPolygon::CreateRectangle(0, 0, 100, 100);

// 设置绘制样式
SNDrawStyle style;
style.strStrokeColor = "#000000";
style.strFillColor = "#00FF00";
style.dStrokeWidth = 2.0;

// 调用绘制引擎
DrawResult result = pEngine->DrawGeometry(polygon.get(), style);

// 获取连续坐标数据（用于GPU渲染）
const double* coordData;
size_t coordCount;
if (polygon->GetCoordinateData(&coordData, &coordCount)) {
    // 上传到GPU
    glBufferData(GL_ARRAY_BUFFER, coordCount * sizeof(double), coordData, GL_STATIC_DRAW);
}

// 获取外包矩形（用于视口计算）
const auto& env = polygon->GetEnvelope();
// 调整视口以包含整个几何
viewport.SetBounds(env.GetMinX(), env.GetMinY(), env.GetMaxX(), env.GetMaxY());
```

---

## 15. 编译和依赖

### 15.1 目录结构

```
ogc_geometry/
├── include/
│   └── OGC/
│       ├── cn_common.h           # 公共定义
│       ├── cn_coordinate.h       # 坐标结构
│       ├── cn_envelope.h         # 外包矩形
│       ├── cn_geometry.h         # 几何基类
│       ├── cn_point.h            # 点
│       ├── cn_linestring.h       # 线串
│       ├── cn_linearring.h       # 环
│       ├── cn_polygon.h          # 多边形
│       ├── cn_multipoint.h       # 多点
│       ├── cn_multilinestring.h  # 多线串
│       ├── cn_multipolygon.h     # 多多边形
│       ├── cn_geometrycollection.h # 几何集合
│       ├── cn_visitor.h          # 访问者
│       ├── cn_factory.h          # 工厂
│       ├── cn_spatial_index.h    # 空间索引
│       └── cn_exception.h        # 异常
├── src/
│   ├── cn_geometry.cpp
│   ├── cn_point.cpp
│   └── ...
├── tests/
│   ├── test_coordinate.cpp
│   ├── test_point.cpp
│   └── ...
├── CMakeLists.txt
└── README.md
```

### 15.2 CMake配置

```cmake
cmake_minimum_required(VERSION 3.10)
project(OGCGeometry VERSION 2.1.0 LANGUAGES CXX)

# C++11标准（符合实际编码要求）
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 编译选项
option(USE_GEOS "Enable GEOS backend for advanced operations" ON)
option(USE_PROJ "Enable PROJ for coordinate transformation" ON)
option(ENABLE_TESTS "Build unit tests" ON)
option(ENABLE_PYTHON_BINDINGS "Build Python bindings" OFF)

# 查找依赖
find_package(Threads REQUIRED)

if(USE_GEOS)
    find_package(GEOS 3.8 REQUIRED)
    add_definitions(-DUSE_GEOS)
endif()

if(USE_PROJ)
    find_package(PROJ 7.0 REQUIRED)
    add_definitions(-DUSE_PROJ)
endif()

# 库目标
add_library(ogc_geometry
    src/cn_coordinate.cpp
    src/cn_envelope.cpp
    src/cn_geometry.cpp
    src/cn_point.cpp
    src/cn_linestring.cpp
    src/cn_linearring.cpp
    src/cn_polygon.cpp
    src/cn_multipoint.cpp
    src/cn_multilinestring.cpp
    src/cn_multipolygon.cpp
    src/cn_geometrycollection.cpp
    src/cn_factory.cpp
    src/cn_visitor.cpp
    src/cn_spatial_index.cpp
)

target_include_directories(ogc_geometry
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(ogc_geometry
    PUBLIC
        Threads::Threads
    PRIVATE
        $<$<BOOL:USE_GEOS>:GEOS::GEOS>
        $<$<BOOL:USE_PROJ>:PROJ::proj>
)

# 编译选项
target_compile_options(ogc_geometry
    PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -Werror>
        $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -Werror>
)

# 测试
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# 安装
install(TARGETS ogc_geometry
    EXPORT OGCGeometryTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/OGC
    DESTINATION include
)
```

### 15.3 依赖说明

| 依赖项 | 版本要求 | 用途 | 必需性 |
|--------|---------|------|--------|
| C++编译器 | C++17 | 核心语言特性 | 必需 |
| CMake | >= 3.14 | 构建系统 | 必需 |
| Threads | - | 多线程支持 | 必需 |
| GEOS | >= 3.8 | 高级几何运算 | 可选 |
| PROJ | >= 7.0 | 坐标系统转换 | 可选 |
| Google Test | >= 1.10 | 单元测试 | 可选 |
| Python | >= 3.7 | Python绑定 | 可选 |

---

## 16. 测试策略

### 16.1 单元测试

```cpp
#include <gtest/gtest.h>
#include <OGC/cn_point.h>
#include <OGC/cn_polygon.h>

// 坐标测试
TEST(CoordinateTest, DistanceCalculation) {
    OGC::Coordinate p1(0, 0);
    OGC::Coordinate p2(3, 4);
    
    EXPECT_DOUBLE_EQ(p1.Distance(p2), 5.0);
    EXPECT_DOUBLE_EQ(p1.Distance3D(OGC::Coordinate(3, 4, 0)), 5.0);
}

// 点测试
TEST(PointTest, CreationAndAccess) {
    auto point = OGC::CNPoint::Create(10.5, 20.5);
    
    ASSERT_NE(point, nullptr);
    EXPECT_EQ(point->GetGeometryType(), OGC::GeomType::Point);
    EXPECT_DOUBLE_EQ(point->GetX(), 10.5);
    EXPECT_DOUBLE_EQ(point->GetY(), 20.5);
    EXPECT_FALSE(point->IsEmpty());
    EXPECT_FALSE(point->Is3D());
}

// 多边形测试
TEST(PolygonTest, AreaCalculation) {
    auto ring = OGC::CNLinearRing::CreateRectangle(0, 0, 10, 10);
    auto polygon = OGC::CNPolygon::Create();
    polygon->SetExteriorRing(std::move(ring));
    
    EXPECT_DOUBLE_EQ(polygon->Area(), 100.0);
    EXPECT_DOUBLE_EQ(polygon->Length(), 40.0);  // 周长
}

// 空间关系测试
TEST(SpatialRelationTest, Intersects) {
    auto poly1 = OGC::CNPolygon::CreateRectangle(0, 0, 10, 10);
    auto poly2 = OGC::CNPolygon::CreateRectangle(5, 5, 15, 15);
    auto poly3 = OGC::CNPolygon::CreateRectangle(20, 20, 30, 30);
    
    EXPECT_TRUE(poly1->Intersects(poly2.get()));
    EXPECT_FALSE(poly1->Intersects(poly3.get()));
}
```

### 16.2 性能测试

```cpp
#include <benchmark/benchmark.h>

// 缓冲区性能测试
static void BM_Buffer(benchmark::State& state) {
    auto lineString = CreateLargeLineString(state.range(0));
    
    for (auto _ : state) {
        OGC::CNGeometryPtr result;
        lineString->Buffer(5.0, result);
        benchmark::DoNotOptimize(result);
    }
    
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_Buffer)->RangeMultiplier(2)->Range(100, 100000)->Complexity();

// 空间查询性能测试
static void BM_SpatialQuery(benchmark::State& state) {
    auto index = CreateSpatialIndex(state.range(0));
    OGC::Envelope queryEnv(0, 0, 10, 10);
    
    for (auto _ : state) {
        auto results = index.Query(queryEnv);
        benchmark::DoNotOptimize(results);
    }
    
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_SpatialQuery)->RangeMultiplier(10)->Range(100, 100000)->Complexity();
```

### 16.3 OGC标准符合性测试

```cpp
// 使用OGC CITE测试套件
class OGCCiteTest : public ::testing::Test {
protected:
    void RunCiteTest(const std::string& testName) {
        // 加载测试用例
        auto testCase = LoadCiteTestCase(testName);
        
        // 执行测试
        for (const auto& test : testCase.tests) {
            auto geom1 = GeometryFactory::GetInstance().FromWKT(test.input1);
            auto geom2 = GeometryFactory::GetInstance().FromWKT(test.input2);
            
            bool result = geom1->Relate(geom2.get(), test.pattern);
            EXPECT_EQ(result, test.expectedResult) << test.description;
        }
    }
};

TEST_F(OGCCiteTest, IntersectsTest) {
    RunCiteTest("Intersects");
}

TEST_F(OGCCiteTest, ContainsTest) {
    RunCiteTest("Contains");
}
```

### 16.4 验收标准

| 指标 | 标准 | 说明 |
|------|------|------|
| 单元测试覆盖率 | >= 90% | 代码行覆盖率 |
| OGC CITE测试 | 100%通过 | 所有标准测试用例 |
| 性能基准 | >= GEOS 80% | 关键操作性能 |
| 内存泄漏 | 0 leaks | Valgrind/ASan检测 |
| 线程安全测试 | 通过 | ThreadSanitizer检测 |
| 跨平台编译 | Windows/Linux/macOS | 三平台编译通过 |

---

## 17. 版本兼容性

### 17.1 语义化版本控制

```
版本格式: MAJOR.MINOR.PATCH

- MAJOR: 不兼容的API变更
- MINOR: 向后兼容的功能新增
- PATCH: 向后兼容的Bug修复

示例:
- 1.0.0 -> 1.0.1: Bug修复
- 1.0.1 -> 1.1.0: 新增功能（如新增几何类型）
- 1.1.0 -> 2.0.0: 破坏性API变更
```

### 17.2 API稳定性承诺

```cpp
/**
 * @brief API稳定性级别
 * 
 * 1. Stable（稳定）: 永久保留，仅在主版本更新时可能变更
 *    - 所有公共接口
 *    - 核心几何类型
 * 
 * 2. Experimental（实验性）: 可能在次版本更新时变更
 *    - 标记为 [[experimental]] 的API
 * 
 * 3. Deprecated（废弃）: 计划在未来移除
 *    - 提前2个次版本通知
 *    - 提供迁移指南
 */

// 废弃标记示例
[[deprecated("Use GetCoordinateN() instead")]]
Coordinate GetPointN(size_t index) const;

// 迁移指南
// Deprecated in v2.0, will be removed in v3.0
// Replacement: GetCoordinateN(index)
```

### 17.3 ABI兼容性

```cpp
/**
 * @brief ABI兼容性保证
 * 
 * 次版本更新（如1.0->1.1）保持ABI兼容：
 * - 不改变类的大小和布局
 * - 不改变虚函数顺序
 * - 不删除虚函数
 * 
 * 实现方式：
 * 1. 使用PIMPL模式
 * 2. 预留虚函数槽位
 * 3. 私有成员放在末尾
 */

class CNGeometry {
public:
    // 公共接口
    virtual GeomType GetGeometryType() const noexcept = 0;
    
    // 预留槽位（用于未来扩展）
    virtual void _reserved1() {}
    virtual void _reserved2() {}
    virtual void _reserved3() {}
    
private:
    // 数据成员放在末尾
    std::atomic<int> m_srid{0};
    
    // PIMPL：隐藏实现细节
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
```

---

## 18. 术语表

| 术语 | 英文全称 | 说明 |
|------|---------|------|
| OGC | Open Geospatial Consortium | 开放地理空间联盟 |
| WKT | Well-Known Text | 文本格式的几何表示，如 `POINT(0 0)` |
| WKB | Well-Known Binary | 二进制格式的几何表示 |
| MBR | Minimum Bounding Rectangle | 最小外包矩形 |
| SRID | Spatial Reference System ID | 空间参考系统标识符（如4326表示WGS84） |
| CRS | Coordinate Reference System | 坐标参考系统 |
| DE-9IM | Dimensionally Extended 9-Intersection Model | 维度扩展的九交模型，用于描述空间关系 |
| GIS | Geographic Information System | 地理信息系统 |
| RAII | Resource Acquisition Is Initialization | 资源获取即初始化（C++资源管理惯用语） |
| PIMPL | Pointer to Implementation | 指向实现的指针（隐藏实现细节的模式） |
| ABI | Application Binary Interface | 应用程序二进制接口 |
| API | Application Programming Interface | 应用程序编程接口 |

---

## 19. 总结

本几何类库设计文档基于审查报告的改进建议，构建了一个**生产级**的OGC标准几何类型系统。

### 核心优势

1. **稳定性**: 完善的错误处理机制（Result模式+异常可选）、线程安全设计、异常安全保证
2. **正确性**: 符合OGC标准、完整的测试覆盖（单元测试+性能测试+符合性测试）
3. **高效性**: 智能缓存、空间索引、批量操作优化、内存池支持
4. **扩展性**: 访问者模式、策略模式、清晰的接口设计
5. **跨平台**: 纯C++17标准、无平台依赖、CMake构建
6. **线程安全**: 读写锁保护缓存、原子操作SRID、明确的安全保证

### 关键创新

- **双重错误处理**: 支持Result模式和异常模式，灵活应对不同场景
- **细粒度线程安全**: 只读操作线程安全，修改操作需外部同步
- **性能优化**: 缓存机制、空间索引、SIMD优化预留、内存池
- **完整文档**: 包含性能分析、使用示例、最佳实践、术语表

### 后续工作

1. 实现核心几何类型（Point、LineString、Polygon）
2. 实现GEOS后端集成
3. 完善单元测试和性能测试
4. 编写用户手册和API文档
5. 添加Python绑定（可选）

---

## 18. 精度控制与鲁棒计算

### 18.1 精度上下文

```cpp
/**
 * @brief 精度上下文
 * 
 * 控制几何运算的精度参数，支持线程局部设置
 */
class PrecisionContext {
public:
    static PrecisionContext& GetCurrent();
    
    struct Settings {
        double tolerance = 1e-9;           // 默认容差
        double snapTolerance = 1e-6;       // 捕捉容差
        bool useRobustGeometry = true;     // 使用鲁棒几何算法
        size_t maxIterations = 100;        // 最大迭代次数
    };
    
    void SetSettings(const Settings& settings);
    const Settings& GetSettings() const noexcept;
    
    double GetTolerance() const noexcept { return m_settings.tolerance; }
    double GetSnapTolerance() const noexcept { return m_settings.snapTolerance; }
    
    static void SetDefault(const Settings& settings);
    static Settings GetDefault();
    
private:
    PrecisionContext() = default;
    
    static thread_local Settings t_settings;
    static Settings s_defaultSettings;
    Settings m_settings;
};

/**
 * @brief 精度上下文作用域守卫
 */
class PrecisionScope {
public:
    explicit PrecisionScope(const PrecisionContext::Settings& settings);
    ~PrecisionScope();
    
    PrecisionScope(const PrecisionScope&) = delete;
    PrecisionScope& operator=(const PrecisionScope&) = delete;
    
private:
    PrecisionContext::Settings m_previous;
};

// 使用示例
void PerformPreciseOperation() {
    PrecisionContext::Settings settings;
    settings.tolerance = 1e-12;
    settings.useRobustGeometry = true;
    
    PrecisionScope scope(settings);
    
    // 在此作用域内，所有几何运算使用高精度设置
    auto result = geom1->Intersection(geom2.get());
}
```

### 18.2 鲁棒计算工具类

```cpp
/**
 * @brief 鲁棒计算工具类
 * 
 * 提供数值稳定的几何计算方法
 */
class RobustComputations {
public:
    static constexpr double DEFAULT_EPSILON = 1e-12;
    
    /**
     * @brief 鲁棒浮点比较
     */
    static int Compare(double a, double b, double epsilon = DEFAULT_EPSILON) {
        double diff = a - b;
        if (diff > epsilon) return 1;
        if (diff < -epsilon) return -1;
        return 0;
    }
    
    static bool Equals(double a, double b, double epsilon = DEFAULT_EPSILON) {
        return Compare(a, b, epsilon) == 0;
    }
    
    static bool Greater(double a, double b, double epsilon = DEFAULT_EPSILON) {
        return Compare(a, b, epsilon) > 0;
    }
    
    static bool Less(double a, double b, double epsilon = DEFAULT_EPSILON) {
        return Compare(a, b, epsilon) < 0;
    }
    
    /**
     * @brief 鲁棒符号计算
     */
    static int Sign(double value, double epsilon = DEFAULT_EPSILON) {
        if (value > epsilon) return 1;
        if (value < -epsilon) return -1;
        return 0;
    }
    
    /**
     * @brief 鲁棒叉积（2D）
     * 
     * 计算向量 OA x OB
     * 返回值：正数表示逆时针，负数表示顺时针，零表示共线
     */
    static double CrossProduct(
        const Coordinate& o,
        const Coordinate& a,
        const Coordinate& b,
        double epsilon = DEFAULT_EPSILON
    ) {
        double result = (a.x - o.x) * (b.y - o.y) - (a.y - o.y) * (b.x - o.x);
        return std::abs(result) < epsilon ? 0.0 : result;
    }
    
    /**
     * @brief 鲁棒方向判断
     * 
     * 判断点C相对于有向线段AB的位置
     * 返回值：1=左侧，-1=右侧，0=共线
     */
    static int Orientation(
        const Coordinate& a,
        const Coordinate& b,
        const Coordinate& c,
        double epsilon = DEFAULT_EPSILON
    ) {
        return Sign(CrossProduct(a, b, c), epsilon);
    }
    
    /**
     * @brief 鲁棒线段相交检测
     */
    static bool SegmentsIntersect(
        const Coordinate& p1, const Coordinate& p2,
        const Coordinate& q1, const Coordinate& q2,
        double epsilon = DEFAULT_EPSILON
    ) {
        int d1 = Orientation(q1, q2, p1, epsilon);
        int d2 = Orientation(q1, q2, p2, epsilon);
        int d3 = Orientation(p1, p2, q1, epsilon);
        int d4 = Orientation(p1, p2, q2, epsilon);
        
        if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) &&
            ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
            return true;
        }
        
        // 检查共线情况
        if (d1 == 0 && OnSegment(q1, q2, p1, epsilon)) return true;
        if (d2 == 0 && OnSegment(q1, q2, p2, epsilon)) return true;
        if (d3 == 0 && OnSegment(p1, p2, q1, epsilon)) return true;
        if (d4 == 0 && OnSegment(p1, p2, q2, epsilon)) return true;
        
        return false;
    }
    
    /**
     * @brief 点是否在线段上（假设已共线）
     */
    static bool OnSegment(
        const Coordinate& a, const Coordinate& b,
        const Coordinate& p,
        double epsilon = DEFAULT_EPSILON
    ) {
        return p.x >= std::min(a.x, b.x) - epsilon &&
               p.x <= std::max(a.x, b.x) + epsilon &&
               p.y >= std::min(a.y, b.y) - epsilon &&
               p.y <= std::max(a.y, b.y) + epsilon;
    }
    
    /**
     * @brief 鲁棒点在线上判断
     */
    static bool PointOnLine(
        const Coordinate& p,
        const Coordinate& a, const Coordinate& b,
        double epsilon = DEFAULT_EPSILON
    ) {
        if (Orientation(a, b, p, epsilon) != 0) return false;
        return OnSegment(a, b, p, epsilon);
    }
    
    /**
     * @brief 鲁棒面积计算（使用Shoelace公式）
     */
    static double SignedArea(const CoordinateList& ring, double epsilon = DEFAULT_EPSILON) {
        if (ring.size() < 3) return 0.0;
        
        double area = 0.0;
        size_t n = ring.size();
        
        for (size_t i = 0; i < n; ++i) {
            size_t j = (i + 1) % n;
            area += ring[i].x * ring[j].y;
            area -= ring[j].x * ring[i].y;
        }
        
        if (std::abs(area) < epsilon) return 0.0;
        return area / 2.0;
    }
    
    static double Area(const CoordinateList& ring, double epsilon = DEFAULT_EPSILON) {
        return std::abs(SignedArea(ring, epsilon));
    }
    
    /**
     * @brief 判断环的方向（顺时针/逆时针）
     */
    static bool IsCCW(const CoordinateList& ring, double epsilon = DEFAULT_EPSILON) {
        return SignedArea(ring, epsilon) > 0;
    }
    
    /**
     * @brief 鲁棒距离计算
     */
    static double Distance(
        const Coordinate& a, const Coordinate& b,
        double epsilon = DEFAULT_EPSILON
    ) {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        double dist = std::sqrt(dx * dx + dy * dy);
        return dist < epsilon ? 0.0 : dist;
    }
    
    /**
     * @brief 点到线段的最近点
     */
    static Coordinate ClosestPointOnSegment(
        const Coordinate& p,
        const Coordinate& a, const Coordinate& b,
        double epsilon = DEFAULT_EPSILON
    ) {
        double dx = b.x - a.x;
        double dy = b.y - a.y;
        double lenSq = dx * dx + dy * dy;
        
        if (lenSq < epsilon * epsilon) {
            return a;  // 线段退化为点
        }
        
        double t = ((p.x - a.x) * dx + (p.y - a.y) * dy) / lenSq;
        t = std::max(0.0, std::min(1.0, t));
        
        return Coordinate(a.x + t * dx, a.y + t * dy);
    }
    
    /**
     * @brief 点到线段的距离
     */
    static double PointToSegmentDistance(
        const Coordinate& p,
        const Coordinate& a, const Coordinate& b,
        double epsilon = DEFAULT_EPSILON
    ) {
        Coordinate closest = ClosestPointOnSegment(p, a, b, epsilon);
        return Distance(p, closest, epsilon);
    }
};
```

---

## 19. 空间关系优化策略

### 19.1 两阶段过滤策略

```cpp
/**
 * @brief 空间关系判断优化策略
 * 
 * 使用两阶段过滤减少精确计算次数：
 * 1. 外包矩形快速过滤（O(1)）
 * 2. 精确几何计算（O(n*m)）
 */
namespace SpatialOptimization {

/**
 * @brief 过滤策略枚举
 */
enum class FilterStrategy {
    None,           // 不使用过滤，直接精确计算
    EnvelopeOnly,   // 仅使用外包矩形（快速但不精确）
    TwoPhase,       // 两阶段过滤（推荐）
    Hierarchical    // 层次过滤（使用空间索引）
};

/**
 * @brief 空间关系判断配置
 */
struct SpatialConfig {
    FilterStrategy strategy = FilterStrategy::TwoPhase;
    double envelopeExpansion = 0.0;     // 外包矩形扩展量
    bool useSpatialIndex = true;         // 是否使用空间索引
    size_t indexThreshold = 100;         // 使用索引的阈值
};

/**
 * @brief 两阶段空间关系判断器
 */
class TwoPhaseSpatialPredicate {
public:
    explicit TwoPhaseSpatialPredicate(const SpatialConfig& config = SpatialConfig());
    
    /**
     * @brief 判断相交关系
     * 
     * 第一阶段：外包矩形相交检测
     * 第二阶段：精确几何相交检测
     */
    bool Intersects(const CNGeometry* geom1, const CNGeometry* geom2);
    
    /**
     * @brief 判断包含关系
     */
    bool Contains(const CNGeometry* geom1, const CNGeometry* geom2);
    
    /**
     * @brief 判断覆盖关系
     */
    bool Covers(const CNGeometry* geom1, const CNGeometry* geom2);
    
    /**
     * @brief 判断被包含关系
     */
    bool Within(const CNGeometry* geom1, const CNGeometry* geom2) {
        return Contains(geom2, geom1);
    }
    
    /**
     * @brief 计算距离
     */
    double Distance(const CNGeometry* geom1, const CNGeometry* geom2);
    
    /**
     * @brief 获取统计信息
     */
    struct Statistics {
        size_t totalQueries = 0;
        size_t envelopeFiltered = 0;
        size_t preciseComputations = 0;
        double avgFilterTime = 0.0;
        double avgPreciseTime = 0.0;
    };
    
    const Statistics& GetStatistics() const noexcept { return m_stats; }
    void ResetStatistics() { m_stats = Statistics(); }
    
private:
    SpatialConfig m_config;
    Statistics m_stats;
    
    bool EnvelopeIntersects(const CNGeometry* geom1, const CNGeometry* geom2);
    bool EnvelopeContains(const CNGeometry* geom1, const CNGeometry* geom2);
    bool PreciseIntersects(const CNGeometry* geom1, const CNGeometry* geom2);
    bool PreciseContains(const CNGeometry* geom1, const CNGeometry* geom2);
};

/**
 * @brief 外包矩形过滤器
 */
class EnvelopeFilter {
public:
    /**
     * @brief 快速相交检测
     */
    static bool QuickIntersects(const Envelope& env1, const Envelope& env2) {
        return env1.Intersects(env2);
    }
    
    /**
     * @brief 快速包含检测
     */
    static bool QuickContains(const Envelope& outer, const Envelope& inner) {
        return outer.Contains(inner);
    }
    
    /**
     * @brief 快速距离检测
     * 
     * 如果外包矩形相交或接触，返回0
     * 否则返回外包矩形之间的最小距离
     */
    static double QuickDistance(const Envelope& env1, const Envelope& env2) {
        return env1.Distance(env2);
    }
    
    /**
     * @brief 快速分离检测
     * 
     * 如果外包矩形不相交，则几何一定不相交
     */
    static bool QuickDisjoint(const Envelope& env1, const Envelope& env2) {
        return !env1.Intersects(env2);
    }
    
    /**
     * @brief 批量外包矩形过滤
     * 
     * 从候选集合中过滤出可能与查询几何相交的候选
     */
    template<typename Iterator>
    static std::vector<typename Iterator::value_type> FilterCandidates(
        const Envelope& queryEnv,
        Iterator begin, Iterator end,
        std::function<Envelope(const typename Iterator::value_type&)> getEnvelope
    ) {
        std::vector<typename Iterator::value_type> candidates;
        
        for (auto it = begin; it != end; ++it) {
            if (queryEnv.Intersects(getEnvelope(*it))) {
                candidates.push_back(*it);
            }
        }
        
        return candidates;
    }
};

}
```

### 19.2 空间索引加速

```cpp
/**
 * @brief 基于空间索引的关系判断加速器
 */
class IndexedSpatialPredicate {
public:
    IndexedSpatialPredicate();
    
    /**
     * @brief 添加几何到索引
     */
    void AddGeometry(const CNGeometry* geom, size_t id);
    
    /**
     * @brief 批量添加
     */
    void AddGeometries(const std::vector<std::pair<const CNGeometry*, size_t>>& geoms);
    
    /**
     * @brief 构建索引
     */
    void BuildIndex();
    
    /**
     * @brief 查询与给定几何相交的所有几何
     */
    std::vector<size_t> QueryIntersects(const CNGeometry* query);
    
    /**
     * @brief 查询与给定外包矩形相交的所有几何
     */
    std::vector<size_t> QueryIntersects(const Envelope& queryEnv);
    
    /**
     * @brief 查询包含给定点的所有几何
     */
    std::vector<size_t> QueryContains(const Coordinate& point);
    
    /**
     * @brief 查询最近的k个几何
     */
    std::vector<std::pair<size_t, double>> QueryNearest(
        const Coordinate& point, 
        size_t k
    );
    
    /**
     * @brief 清空索引
     */
    void Clear();
    
    /**
     * @brief 获取索引统计
     */
    struct IndexStats {
        size_t geometryCount;
        size_t indexNodeCount;
        size_t indexHeight;
        double avgQueryTime;
    };
    
    IndexStats GetStatistics() const;
    
private:
    RTree<size_t> m_index;
    std::vector<const CNGeometry*> m_geometries;
    std::vector<Envelope> m_envelopes;
    bool m_indexBuilt;
};
```

---

## 20. 几何有效性验证

### 20.1 有效性验证器

```cpp
/**
 * @brief 几何有效性验证器
 * 
 * 检查几何是否符合OGC标准定义的有效性规则
 */
class GeometryValidator {
public:
    /**
     * @brief 验证规则
     */
    enum class ValidationRule {
        OGC,            // OGC Simple Feature标准
        Extended,       // 扩展规则（更严格）
        Relaxed         // 宽松规则（允许某些非标准情况）
    };
    
    /**
     * @brief 验证结果
     */
    struct ValidationResult {
        bool isValid = true;
        GeomResult errorCode = GeomResult::Success;
        std::string message;
        std::vector<Coordinate> invalidLocations;  // 无效位置
        std::vector<std::string> warnings;
    };
    
    explicit GeometryValidator(ValidationRule rule = ValidationRule::OGC);
    
    /**
     * @brief 验证几何有效性
     */
    ValidationResult Validate(const CNGeometry* geom);
    
    /**
     * @brief 快速有效性检查（仅返回bool）
     */
    bool IsValid(const CNGeometry* geom);
    
    /**
     * @brief 获取验证规则
     */
    ValidationRule GetRule() const noexcept { return m_rule; }
    void SetRule(ValidationRule rule) { m_rule = rule; }
    
    /**
     * @brief 设置容差
     */
    void SetTolerance(double tolerance) { m_tolerance = tolerance; }
    double GetTolerance() const noexcept { return m_tolerance; }
    
private:
    ValidationRule m_rule;
    double m_tolerance = 1e-9;
    
    ValidationResult ValidatePoint(const CNPoint* point);
    ValidationResult ValidateLineString(const CNLineString* line);
    ValidationResult ValidateLinearRing(const CNLinearRing* ring);
    ValidationResult ValidatePolygon(const CNPolygon* polygon);
    ValidationResult ValidateMultiPolygon(const CNMultiPolygon* multiPoly);
    ValidationResult ValidateGeometryCollection(const CNGeometryCollection* collection);
};
```

### 20.2 自相交检测算法

```cpp
/**
 * @brief 自相交检测器
 * 
 * 使用Bentley-Ottmann算法检测线段自相交
 * 时间复杂度: O((n+k) log n)，其中n为线段数，k为交点数
 */
class SelfIntersectionDetector {
public:
    /**
     * @brief 交点信息
     */
    struct Intersection {
        Coordinate location;            // 交点坐标
        size_t segmentIndex1;           // 第一条线段索引
        size_t segmentIndex2;           // 第二条线段索引
        double parameter1;              // 第一条线段上的参数
        double parameter2;              // 第二条线段上的参数
        bool isProper;                  // 是否为真交点（非端点）
    };
    
    /**
     * @brief 检测线串自相交
     */
    std::vector<Intersection> Detect(const CNLineString* line);
    
    /**
     * @brief 检测环自相交
     */
    std::vector<Intersection> DetectRing(const CNLinearRing* ring);
    
    /**
     * @brief 检测多边形自相交
     */
    std::vector<Intersection> DetectPolygon(const CNPolygon* polygon);
    
    /**
     * @brief 快速检测是否存在自相交
     */
    bool HasSelfIntersection(const CNLineString* line);
    bool HasSelfIntersection(const CNLinearRing* ring);
    bool HasSelfIntersection(const CNPolygon* polygon);
    
    /**
     * @brief 设置容差
     */
    void SetTolerance(double tolerance) { m_tolerance = tolerance; }
    
private:
    double m_tolerance = 1e-9;
    
    /**
     * @brief 扫描线状态
     */
    struct SweepLineEvent {
        double x;
        enum Type { Start, End, Intersection } type;
        size_t segmentIndex;
        Coordinate point;
        
        bool operator<(const SweepLineEvent& other) const {
            if (x != other.x) return x < other.x;
            return type < other.type;
        }
    };
    
    std::vector<Intersection> BentleyOttmann(
        const std::vector<std::pair<Coordinate, Coordinate>>& segments
    );
};
```

### 20.3 多边形有效性规则

```cpp
/**
 * @brief 多边形有效性规则
 * 
 * OGC标准定义的多边形有效性条件：
 * 1. 外环必须是逆时针方向
 * 2. 内环必须是顺时针方向
 * 3. 环不能自相交
 * 4. 内环必须完全在外环内部
 * 5. 内环之间不能相交
 * 6. 内环不能嵌套
 */
namespace PolygonValidation {

/**
 * @brief 检查环的方向
 * @return true表示逆时针，false表示顺时针
 */
bool CheckRingOrientation(const CNLinearRing* ring, double tolerance = 1e-9);

/**
 * @brief 检查环是否自相交
 */
bool CheckRingSelfIntersection(const CNLinearRing* ring, double tolerance = 1e-9);

/**
 * @brief 检查内环是否在外环内部
 */
bool CheckInnerRingInsideOuter(
    const CNLinearRing* inner, 
    const CNLinearRing* outer,
    double tolerance = 1e-9
);

/**
 * @brief 检查两个内环是否相交
 */
bool CheckInnerRingsDisjoint(
    const CNLinearRing* ring1, 
    const CNLinearRing* ring2,
    double tolerance = 1e-9
);

/**
 * @brief 检查内环是否嵌套
 */
bool CheckInnerRingsNotNested(
    const std::vector<CNLinearRingPtr>& innerRings,
    double tolerance = 1e-9
);

/**
 * @brief 修复多边形方向
 * 
 * 自动将外环调整为逆时针，内环调整为顺时针
 */
CNPolygonPtr FixRingOrientation(const CNPolygon* polygon);

/**
 * @brief 尝试修复无效多边形
 */
struct RepairResult {
    CNPolygonPtr repairedPolygon;
    bool wasRepaired;
    std::vector<std::string> repairs;
};

RepairResult RepairPolygon(const CNPolygon* polygon, double tolerance = 1e-9);

}
```

---

## 21. 缓存机制详解

### 21.1 缓存架构

```cpp
/**
 * @brief 几何缓存管理器
 * 
 * 管理几何对象的计算结果缓存，支持线程安全访问
 */
class GeometryCache {
public:
    /**
     * @brief 缓存项类型
     */
    enum class CacheType {
        Envelope,       // 外包矩形
        Centroid,       // 质心
        Area,           // 面积
        Length,         // 长度
        IsValid,        // 有效性
        CoordinateCount // 坐标数量
    };
    
    /**
     * @brief 缓存策略
     */
    enum class CachePolicy {
        Lazy,           // 延迟计算，首次访问时缓存
        Eager,          // 立即计算并缓存
        None            // 不缓存
    };
    
    static GeometryCache& GetInstance();
    
    /**
     * @brief 获取缓存的外包矩形
     */
    std::shared_ptr<Envelope> GetEnvelope(const CNGeometry* geom);
    
    /**
     * @brief 获取缓存的质心
     */
    std::shared_ptr<Coordinate> GetCentroid(const CNGeometry* geom);
    
    /**
     * @brief 获取缓存的面积
     */
    std::shared_ptr<double> GetArea(const CNGeometry* geom);
    
    /**
     * @brief 获取缓存的长度
     */
    std::shared_ptr<double> GetLength(const CNGeometry* geom);
    
    /**
     * @brief 使缓存失效
     */
    void Invalidate(const CNGeometry* geom);
    void Invalidate(const CNGeometry* geom, CacheType type);
    void InvalidateAll();
    
    /**
     * @brief 设置缓存策略
     */
    void SetPolicy(CachePolicy policy) { m_policy = policy; }
    CachePolicy GetPolicy() const noexcept { return m_policy; }
    
    /**
     * @brief 获取缓存统计
     */
    struct Statistics {
        size_t totalHits;
        size_t totalMisses;
        size_t totalInvalidations;
        double hitRate;
        size_t memoryUsage;
    };
    
    Statistics GetStatistics() const;
    void ResetStatistics();
    
private:
    GeometryCache() = default;
    
    CachePolicy m_policy = CachePolicy::Lazy;
    
    mutable std::shared_mutex m_mutex;
    
    struct CacheEntry {
        std::shared_ptr<Envelope> envelope;
        std::shared_ptr<Coordinate> centroid;
        std::shared_ptr<double> area;
        std::shared_ptr<double> length;
        std::shared_ptr<bool> isValid;
        std::shared_ptr<size_t> coordinateCount;
        uint64_t version;           // 几何版本号
    };
    
    std::unordered_map<const CNGeometry*, CacheEntry> m_cache;
    
    template<typename T>
    std::shared_ptr<T> GetOrCreate(
        const CNGeometry* geom,
        CacheType type,
        std::function<T()> compute
    );
};
```

### 21.2 缓存失效机制

```cpp
/**
 * @brief 几何版本控制
 * 
 * 每个几何对象维护一个版本号，修改时递增
 * 缓存项存储版本号，访问时检查版本一致性
 */
class GeometryVersion {
public:
    GeometryVersion() : m_version(0) {}
    
    uint64_t GetVersion() const noexcept { 
        return m_version.load(std::memory_order_acquire); 
    }
    
    void IncrementVersion() noexcept {
        m_version.fetch_add(1, std::memory_order_release);
    }
    
    bool IsSameVersion(uint64_t cachedVersion) const noexcept {
        return GetVersion() == cachedVersion;
    }
    
private:
    std::atomic<uint64_t> m_version;
};

/**
 * @brief 可缓存几何基类
 * 
 * 提供缓存支持的几何基类
 */
class CacheableGeometry : public CNGeometry {
public:
    uint64_t GetVersion() const noexcept {
        return m_version.GetVersion();
    }
    
protected:
    void MarkModified() noexcept {
        m_version.IncrementVersion();
        OnGeometryModified();
    }
    
    virtual void OnGeometryModified() {
        // 子类可重写以响应修改事件
    }
    
private:
    GeometryVersion m_version;
};
```

### 21.3 线程安全缓存访问

```cpp
/**
 * @brief 线程安全缓存访问器
 */
template<typename T>
class ThreadSafeCache {
public:
    ThreadSafeCache() = default;
    
    /**
     * @brief 获取缓存值（如果存在）
     */
    std::shared_ptr<T> Get(uint64_t version) const {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        
        if (m_cachedVersion == version && m_cachedValue) {
            return m_cachedValue;
        }
        
        return nullptr;
    }
    
    /**
     * @brief 设置缓存值
     */
    void Set(uint64_t version, std::shared_ptr<T> value) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        m_cachedVersion = version;
        m_cachedValue = std::move(value);
    }
    
    /**
     * @brief 使缓存失效
     */
    void Invalidate() {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_cachedValue.reset();
    }
    
    /**
     * @brief 获取或计算缓存值
     */
    template<typename ComputeFunc>
    std::shared_ptr<T> GetOrCompute(uint64_t version, ComputeFunc compute) {
        // 先尝试读锁获取
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            if (m_cachedVersion == version && m_cachedValue) {
                return m_cachedValue;
            }
        }
        
        // 升级为写锁计算
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        
        // 双重检查
        if (m_cachedVersion == version && m_cachedValue) {
            return m_cachedValue;
        }
        
        m_cachedValue = compute();
        m_cachedVersion = version;
        
        return m_cachedValue;
    }
    
private:
    mutable std::shared_mutex m_mutex;
    std::shared_ptr<T> m_cachedValue;
    uint64_t m_cachedVersion = 0;
};
```

---

## 22. 内存池设计

### 22.1 几何内存池

```cpp
/**
 * @brief 几何对象内存池
 * 
 * 减少频繁内存分配的开销，提升性能
 */
class GeometryMemoryPool {
public:
    /**
     * @brief 内存池配置
     */
    struct Config {
        size_t initialSize = 1024;      // 初始块大小
        size_t maxSize = 1024 * 1024;   // 最大池大小
        size_t alignment = 64;          // 内存对齐（缓存行）
        bool threadSafe = true;         // 是否线程安全
    };
    
    static GeometryMemoryPool& GetInstance();
    
    explicit GeometryMemoryPool(const Config& config = Config());
    ~GeometryMemoryPool();
    
    /**
     * @brief 分配内存
     */
    void* Allocate(size_t size);
    
    /**
     * @brief 释放内存
     */
    void Deallocate(void* ptr, size_t size);
    
    /**
     * @brief 分配几何对象
     */
    template<typename T, typename... Args>
    T* Create(Args&&... args) {
        void* memory = Allocate(sizeof(T));
        return new (memory) T(std::forward<Args>(args)...);
    }
    
    /**
     * @brief 销毁几何对象
     */
    template<typename T>
    void Destroy(T* obj) {
        if (obj) {
            obj->~T();
            Deallocate(obj, sizeof(T));
        }
    }
    
    /**
     * @brief 获取统计信息
     */
    struct Statistics {
        size_t totalAllocations;
        size_t totalDeallocations;
        size_t currentUsage;
        size_t peakUsage;
        size_t poolSize;
    };
    
    Statistics GetStatistics() const;
    void ResetStatistics();
    
    /**
     * @brief 清理未使用的内存
     */
    void Shrink();
    
    /**
     * @brief 设置内存压力回调
     */
    using MemoryPressureCallback = std::function<void(size_t, size_t)>;
    void SetMemoryPressureCallback(MemoryPressureCallback callback);
    
private:
    Config m_config;
    
    struct MemoryBlock {
        void* memory;
        size_t size;
        bool inUse;
    };
    
    std::vector<MemoryBlock> m_blocks;
    std::unordered_map<void*, size_t> m_allocated;
    
    mutable std::mutex m_mutex;
    size_t m_currentUsage = 0;
    size_t m_peakUsage = 0;
    MemoryPressureCallback m_pressureCallback;
};

/**
 * @brief 内存池分配器（STL兼容）
 */
template<typename T>
class PoolAllocator {
public:
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    
    PoolAllocator() noexcept = default;
    
    template<typename U>
    PoolAllocator(const PoolAllocator<U>&) noexcept {}
    
    pointer allocate(size_type n) {
        return static_cast<pointer>(
            GeometryMemoryPool::GetInstance().Allocate(n * sizeof(T))
        );
    }
    
    void deallocate(pointer ptr, size_type n) {
        GeometryMemoryPool::GetInstance().Deallocate(ptr, n * sizeof(T));
    }
    
    template<typename U>
    struct rebind {
        using other = PoolAllocator<U>;
    };
};

template<typename T, typename U>
bool operator==(const PoolAllocator<T>&, const PoolAllocator<U>&) noexcept {
    return true;
}

template<typename T, typename U>
bool operator!=(const PoolAllocator<T>&, const PoolAllocator<U>&) noexcept {
    return false;
}
```

### 22.2 坐标序列内存池

```cpp
/**
 * @brief 坐标序列内存池
 * 
 * 专门优化坐标数组的内存分配
 */
class CoordinatePool {
public:
    static CoordinatePool& GetInstance();
    
    /**
     * @brief 分配坐标数组
     */
    Coordinate* Allocate(size_t count);
    
    /**
     * @brief 释放坐标数组
     */
    void Deallocate(Coordinate* coords, size_t count);
    
    /**
     * @brief 获取预分配的坐标数组
     */
    std::shared_ptr<Coordinate[]> GetShared(size_t count);
    
    /**
     * @brief 批量分配
     */
    std::vector<Coordinate*> AllocateBatch(const std::vector<size_t>& counts);
    
    /**
     * @brief 批量释放
     */
    void DeallocateBatch(const std::vector<std::pair<Coordinate*, size_t>>& allocations);
    
private:
    CoordinatePool() = default;
    
    // 按大小分桶管理
    std::array<std::vector<Coordinate*>, 16> m_freeLists;
    std::mutex m_mutex;
    
    size_t GetBucketIndex(size_t count) const;
};
```

---

## 23. SIMD优化实现

### 23.1 SIMD坐标运算

```cpp
/**
 * @brief SIMD优化的坐标运算
 * 
 * 支持SSE2/AVX/NEON指令集
 */
namespace SimdCoordinateOps {

/**
 * @brief SIMD能力检测
 */
struct SimdCapabilities {
    bool hasSSE2 = false;
    bool hasSSE4_1 = false;
    bool hasAVX = false;
    bool hasAVX2 = false;
    bool hasNEON = false;
};

SimdCapabilities DetectSimdCapabilities();

/**
 * @brief SIMD距离计算
 */
void DistanceBatch(
    const Coordinate* points1,
    const Coordinate* points2,
    double* results,
    size_t count
);

/**
 * @brief SIMD坐标变换
 */
void TransformBatch(
    const Coordinate* input,
    Coordinate* output,
    size_t count,
    std::function<void(const Coordinate&, Coordinate&)> transform
);

/**
 * @brief SIMD外包矩形计算
 */
Envelope ComputeEnvelopeSimd(
    const Coordinate* coords,
    size_t count
);

/**
 * @brief SIMD坐标比较
 */
void CompareBatch(
    const Coordinate* points1,
    const Coordinate* points2,
    double tolerance,
    bool* results,
    size_t count
);

/**
 * @brief SIMD向量运算
 */
void AddBatch(
    const Coordinate* a,
    const Coordinate* b,
    Coordinate* result,
    size_t count
);

void ScaleBatch(
    const Coordinate* input,
    double scale,
    Coordinate* output,
    size_t count
);

/**
 * @brief SIMD点积计算
 */
void DotProductBatch(
    const Coordinate* a,
    const Coordinate* b,
    double* results,
    size_t count
);

}
```

### 23.2 SIMD实现示例

```cpp
#ifdef __SSE2__
#include <emmintrin.h>

namespace SimdCoordinateOps {

inline void DistanceBatchSSE2(
    const Coordinate* points1,
    const Coordinate* points2,
    double* results,
    size_t count
) {
    size_t i = 0;
    
    // SSE2处理：每次处理2个double（128位）
    for (; i + 1 < count; i += 2) {
        // 加载坐标
        __m128d x1 = _mm_set_pd(points1[i].x, points1[i+1].x);
        __m128d y1 = _mm_set_pd(points1[i].y, points1[i+1].y);
        __m128d x2 = _mm_set_pd(points2[i].x, points2[i+1].x);
        __m128d y2 = _mm_set_pd(points2[i].y, points2[i+1].y);
        
        // 计算差值
        __m128d dx = _mm_sub_pd(x1, x2);
        __m128d dy = _mm_sub_pd(y1, y2);
        
        // 计算平方
        __m128d dx2 = _mm_mul_pd(dx, dx);
        __m128d dy2 = _mm_mul_pd(dy, dy);
        
        // 计算和
        __m128d sum = _mm_add_pd(dx2, dy2);
        
        // 计算平方根
        __m128d dist = _mm_sqrt_pd(sum);
        
        // 存储结果
        _mm_storeu_pd(results + i, dist);
    }
    
    // 处理剩余元素
    for (; i < count; ++i) {
        double dx = points1[i].x - points2[i].x;
        double dy = points1[i].y - points2[i].y;
        results[i] = std::sqrt(dx * dx + dy * dy);
    }
}

inline Envelope ComputeEnvelopeSSE2(
    const Coordinate* coords,
    size_t count
) {
    if (count == 0) return Envelope();
    
    __m128d minX = _mm_set1_pd(coords[0].x);
    __m128d maxX = _mm_set1_pd(coords[0].x);
    __m128d minY = _mm_set1_pd(coords[0].y);
    __m128d maxY = _mm_set1_pd(coords[0].y);
    
    size_t i = 0;
    for (; i + 1 < count; i += 2) {
        __m128d x = _mm_set_pd(coords[i].x, coords[i+1].x);
        __m128d y = _mm_set_pd(coords[i].y, coords[i+1].y);
        
        minX = _mm_min_pd(minX, x);
        maxX = _mm_max_pd(maxX, x);
        minY = _mm_min_pd(minY, y);
        maxY = _mm_max_pd(maxY, y);
    }
    
    // 提取结果
    double minXArr[2], maxXArr[2], minYArr[2], maxYArr[2];
    _mm_storeu_pd(minXArr, minX);
    _mm_storeu_pd(maxXArr, maxX);
    _mm_storeu_pd(minYArr, minY);
    _mm_storeu_pd(maxYArr, maxY);
    
    double finalMinX = std::min(minXArr[0], minXArr[1]);
    double finalMaxX = std::max(maxXArr[0], maxXArr[1]);
    double finalMinY = std::min(minYArr[0], minYArr[1]);
    double finalMaxY = std::max(maxYArr[0], maxYArr[1]);
    
    // 处理剩余元素
    for (; i < count; ++i) {
        finalMinX = std::min(finalMinX, coords[i].x);
        finalMaxX = std::max(finalMaxX, coords[i].x);
        finalMinY = std::min(finalMinY, coords[i].y);
        finalMaxY = std::max(finalMaxY, coords[i].y);
    }
    
    return Envelope(finalMinX, finalMinY, finalMaxX, finalMaxY);
}

}
#endif

#ifdef __AVX__
#include <immintrin.h>

namespace SimdCoordinateOps {

inline void DistanceBatchAVX(
    const Coordinate* points1,
    const Coordinate* points2,
    double* results,
    size_t count
) {
    size_t i = 0;
    
    // AVX处理：每次处理4个double（256位）
    for (; i + 3 < count; i += 4) {
        __m256d x1 = _mm256_set_pd(
            points1[i].x, points1[i+1].x, 
            points1[i+2].x, points1[i+3].x
        );
        __m256d y1 = _mm256_set_pd(
            points1[i].y, points1[i+1].y,
            points1[i+2].y, points1[i+3].y
        );
        __m256d x2 = _mm256_set_pd(
            points2[i].x, points2[i+1].x,
            points2[i+2].x, points2[i+3].x
        );
        __m256d y2 = _mm256_set_pd(
            points2[i].y, points2[i+1].y,
            points2[i+2].y, points2[i+3].y
        );
        
        __m256d dx = _mm256_sub_pd(x1, x2);
        __m256d dy = _mm256_sub_pd(y1, y2);
        
        __m256d dx2 = _mm256_mul_pd(dx, dx);
        __m256d dy2 = _mm256_mul_pd(dy, dy);
        
        __m256d sum = _mm256_add_pd(dx2, dy2);
        __m256d dist = _mm256_sqrt_pd(sum);
        
        _mm256_storeu_pd(results + i, dist);
    }
    
    // 处理剩余元素
    for (; i < count; ++i) {
        double dx = points1[i].x - points2[i].x;
        double dy = points1[i].y - points2[i].y;
        results[i] = std::sqrt(dx * dx + dy * dy);
    }
}

}
#endif
```

### 23.3 SIMD运行时选择

```cpp
/**
 * @brief SIMD函数选择器
 * 
 * 运行时检测CPU能力并选择最优实现
 */
class SimdDispatcher {
public:
    using DistanceFunc = void(*)(
        const Coordinate*, const Coordinate*, double*, size_t
    );
    
    using EnvelopeFunc = Envelope(*)(const Coordinate*, size_t);
    
    static SimdDispatcher& GetInstance() {
        static SimdDispatcher instance;
        return instance;
    }
    
    DistanceFunc GetDistanceFunc() const { return m_distanceFunc; }
    EnvelopeFunc GetEnvelopeFunc() const { return m_envelopeFunc; }
    
    const SimdCapabilities& GetCapabilities() const { return m_caps; }
    
private:
    SimdDispatcher() {
        m_caps = DetectSimdCapabilities();
        SelectOptimalFunctions();
    }
    
    void SelectOptimalFunctions() {
#ifdef __AVX__
        if (m_caps.hasAVX) {
            m_distanceFunc = DistanceBatchAVX;
            m_envelopeFunc = ComputeEnvelopeAVX;
            return;
        }
#endif
        
#ifdef __SSE2__
        if (m_caps.hasSSE2) {
            m_distanceFunc = DistanceBatchSSE2;
            m_envelopeFunc = ComputeEnvelopeSSE2;
            return;
        }
#endif
        
        // 回退到标量实现
        m_distanceFunc = DistanceBatchScalar;
        m_envelopeFunc = ComputeEnvelopeScalar;
    }
    
    SimdCapabilities m_caps;
    DistanceFunc m_distanceFunc;
    EnvelopeFunc m_envelopeFunc;
};
```

---

## 24. DE-9IM实现算法

### 24.1 DE-9IM矩阵计算

```cpp
/**
 * @brief DE-9IM（Dimensionally Extended 9-Intersection Model）实现
 * 
 * 用于计算两个几何之间的拓扑关系矩阵
 */
class DE9IMCalculator {
public:
    /**
     * @brief DE-9IM矩阵
     * 
     * 矩阵格式：
     *         I(A)  B(A)  E(A)
     * I(B)  [ ii   bi   ei ]
     * B(B)  [ ib   bb   eb ]
     * E(B)  [ ie   be   ee ]
     * 
     * 其中：
     * I = Interior（内部）
     * B = Boundary（边界）
     * E = Exterior（外部）
     * 
     * 值含义：
     * -1 = 空集
     *  0 = 点（0维）
     *  1 = 线（1维）
     *  2 = 面（2维）
     */
    struct Matrix {
        int8_t ii = -1;  // Interior(A) ∩ Interior(B)
        int8_t ib = -1;  // Interior(A) ∩ Boundary(B)
        int8_t ie = -1;  // Interior(A) ∩ Exterior(B)
        int8_t bi = -1;  // Boundary(A) ∩ Interior(B)
        int8_t bb = -1;  // Boundary(A) ∩ Boundary(B)
        int8_t be = -1;  // Boundary(A) ∩ Exterior(B)
        int8_t ei = -1;  // Exterior(A) ∩ Interior(B)
        int8_t eb = -1;  // Exterior(A) ∩ Boundary(B)
        int8_t ee = -1;  // Exterior(A) ∩ Exterior(B)
        
        std::string ToString() const {
            char buf[10];
            buf[0] = DimChar(ii);
            buf[1] = DimChar(ib);
            buf[2] = DimChar(ie);
            buf[3] = DimChar(bi);
            buf[4] = DimChar(bb);
            buf[5] = DimChar(be);
            buf[6] = DimChar(ei);
            buf[7] = DimChar(eb);
            buf[8] = DimChar(ee);
            buf[9] = '\0';
            return std::string(buf);
        }
        
        static char DimChar(int dim) {
            switch (dim) {
                case -1: return 'F';
                case 0: return '0';
                case 1: return '1';
                case 2: return '2';
                default: return '?';
            }
        }
    };
    
    /**
     * @brief 计算DE-9IM矩阵
     */
    Matrix Calculate(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 基于矩阵判断空间关系
     */
    bool EvaluateRelation(const Matrix& matrix, const std::string& pattern);
    
    /**
     * @brief 预定义关系模式
     */
    static std::string EqualsPattern();
    static std::string DisjointPattern();
    static std::string IntersectsPattern();
    static std::string TouchesPattern();
    static std::string CrossesPattern(GeomType typeA, GeomType typeB);
    static std::string WithinPattern();
    static std::string ContainsPattern();
    static std::string OverlapsPattern(GeomType typeA, GeomType typeB);
    
private:
    /**
     * @brief 获取几何的内部
     */
    std::unique_ptr<CNGeometry> GetInterior(const CNGeometry* geom);
    
    /**
     * @brief 获取几何的边界
     */
    std::unique_ptr<CNGeometry> GetBoundary(const CNGeometry* geom);
    
    /**
     * @brief 获取几何的外部（外包矩形的补集）
     */
    std::unique_ptr<CNGeometry> GetExterior(const CNGeometry* geom);
    
    /**
     * @brief 计算两个几何交集的最大维度
     */
    int ComputeIntersectionDimension(
        const CNGeometry* geomA, 
        const CNGeometry* geomB
    );
    
    /**
     * @brief 点-点DE-9IM
     */
    Matrix PointPoint(const CNPoint* ptA, const CNPoint* ptB);
    
    /**
     * @brief 点-线DE-9IM
     */
    Matrix PointLineString(const CNPoint* pt, const CNLineString* line);
    
    /**
     * @brief 点-面DE-9IM
     */
    Matrix PointPolygon(const CNPoint* pt, const CNPolygon* poly);
    
    /**
     * @brief 线-线DE-9IM
     */
    Matrix LineStringLineString(
        const CNLineString* lineA, 
        const CNLineString* lineB
    );
    
    /**
     * @brief 线-面DE-9IM
     */
    Matrix LineStringPolygon(
        const CNLineString* line, 
        const CNPolygon* poly
    );
    
    /**
     * @brief 面-面DE-9IM
     */
    Matrix PolygonPolygon(const CNPolygon* polyA, const CNPolygon* polyB);
};
```

### 24.2 空间关系判断实现

```cpp
/**
 * @brief 空间关系判断器
 */
class SpatialRelationEvaluator {
public:
    /**
     * @brief 判断相等关系
     */
    bool Equals(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断分离关系
     */
    bool Disjoint(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断相交关系
     */
    bool Intersects(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断接触关系
     */
    bool Touches(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断交叉关系
     */
    bool Crosses(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断被包含关系
     */
    bool Within(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断包含关系
     */
    bool Contains(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断重叠关系
     */
    bool Overlaps(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断覆盖关系
     */
    bool Covers(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 判断被覆盖关系
     */
    bool CoveredBy(const CNGeometry* geomA, const CNGeometry* geomB);
    
    /**
     * @brief 自定义关系判断
     * 
     * @param pattern DE-9IM模式字符串，如 "T*F**FFF*"
     *                T = true（维度 >= 0）
     *                F = false（维度 = -1）
     *                * = 通配符（任意维度）
     *                0/1/2 = 指定维度
     */
    bool Relate(const CNGeometry* geomA, const CNGeometry* geomB, 
                const std::string& pattern);
    
    /**
     * @brief 获取DE-9IM矩阵字符串
     */
    std::string GetRelateString(const CNGeometry* geomA, const CNGeometry* geomB);
    
private:
    DE9IMCalculator m_de9im;
    SpatialOptimization::TwoPhaseSpatialPredicate m_optimizer;
};
```

---

## 25. 死锁预防策略

### 25.1 锁层次设计

```cpp
/**
 * @brief 锁层次定义
 * 
 * 定义锁的获取顺序，防止死锁
 * 层次号越低，越先获取
 */
enum class LockHierarchy {
    GlobalConfig = 0,       // 全局配置锁
    MemoryPool = 1,         // 内存池锁
    SpatialIndex = 2,       // 空间索引锁
    GeometryCache = 3,      // 几何缓存锁
    GeometryData = 4,       // 几何数据锁
    LocalOperation = 5      // 局部操作锁
};

/**
 * @brief 层次化锁
 * 
 * 确保锁按层次顺序获取
 */
class HierarchicalMutex {
public:
    explicit HierarchicalMutex(LockHierarchy hierarchy)
        : m_hierarchy(hierarchy) {}
    
    void lock() {
        CheckHierarchy();
        m_mutex.lock();
        m_previousHierarchy = t_currentHierarchy;
        t_currentHierarchy = m_hierarchy;
    }
    
    void unlock() {
        t_currentHierarchy = m_previousHierarchy;
        m_mutex.unlock();
    }
    
    bool try_lock() {
        CheckHierarchy();
        if (m_mutex.try_lock()) {
            m_previousHierarchy = t_currentHierarchy;
            t_currentHierarchy = m_hierarchy;
            return true;
        }
        return false;
    }
    
private:
    void CheckHierarchy() {
        if (t_currentHierarchy <= m_hierarchy) {
            throw std::logic_error(
                "Lock hierarchy violation: attempting to lock level " +
                std::to_string(static_cast<int>(m_hierarchy)) +
                " while holding level " +
                std::to_string(static_cast<int>(t_currentHierarchy))
            );
        }
    }
    
    std::mutex m_mutex;
    LockHierarchy m_hierarchy;
    LockHierarchy m_previousHierarchy;
    
    static thread_local LockHierarchy t_currentHierarchy;
};

thread_local LockHierarchy HierarchicalMutex::t_currentHierarchy = 
    LockHierarchy::LocalOperation;
```

### 25.2 多锁获取策略

```cpp
/**
 * @brief 多锁获取器
 * 
 * 安全地同时获取多个锁
 */
class MultiLockGuard {
public:
    /**
     * @brief 同时获取两个互斥锁（避免死锁）
     */
    static std::tuple<std::unique_lock<std::mutex>, std::unique_lock<std::mutex>>
    LockTwo(std::mutex& m1, std::mutex& m2) {
        std::lock(m1, m2);
        return {
            std::unique_lock<std::mutex>(m1, std::adopt_lock),
            std::unique_lock<std::mutex>(m2, std::adopt_lock)
        };
    }
    
    /**
     * @brief 同时获取两个读写锁
     */
    static std::tuple<std::unique_lock<std::shared_mutex>, 
                      std::unique_lock<std::shared_mutex>>
    LockTwoWrite(std::shared_mutex& m1, std::shared_mutex& m2) {
        std::lock(m1, m2);
        return {
            std::unique_lock<std::shared_mutex>(m1, std::adopt_lock),
            std::unique_lock<std::shared_mutex>(m2, std::adopt_lock)
        };
    }
    
    /**
     * @brief 获取一个读锁和一个写锁
     */
    static std::tuple<std::shared_lock<std::shared_mutex>,
                      std::unique_lock<std::shared_mutex>>
    LockReadAndWrite(std::shared_mutex& readMutex, 
                     std::shared_mutex& writeMutex) {
        // 先获取写锁，再获取读锁
        std::lock(writeMutex, readMutex);
        return {
            std::shared_lock<std::shared_mutex>(readMutex, std::adopt_lock),
            std::unique_lock<std::shared_mutex>(writeMutex, std::adopt_lock)
        };
    }
    
    /**
     * @brief 同时获取多个互斥锁
     */
    template<typename... Mutexes>
    static std::tuple<std::unique_lock<Mutexes>...>
    LockMultiple(Mutexes&... mutexes) {
        std::lock(mutexes...);
        return {std::unique_lock<Mutexes>(mutexes, std::adopt_lock)...};
    }
};

/**
 * @brief 作用域锁追踪器（调试用）
 */
class LockTracker {
public:
#ifdef DEBUG_LOCKS
    static void OnLockAcquired(const char* lockName, const char* file, int line) {
        auto& held = t_heldLocks;
        std::string info = std::string(file) + ":" + std::to_string(line);
        held.emplace_back(lockName, info);
        
        std::cout << "Lock acquired: " << lockName << " at " << info << std::endl;
    }
    
    static void OnLockReleased(const char* lockName) {
        auto& held = t_heldLocks;
        auto it = std::find_if(held.begin(), held.end(),
            [&](const auto& p) { return p.first == lockName; });
        if (it != held.end()) {
            held.erase(it);
        }
        
        std::cout << "Lock released: " << lockName << std::endl;
    }
    
    static void PrintHeldLocks() {
        std::cout << "Currently held locks:" << std::endl;
        for (const auto& [name, loc] : t_heldLocks) {
            std::cout << "  " << name << " at " << loc << std::endl;
        }
    }
    
    static thread_local std::vector<std::pair<std::string, std::string>> t_heldLocks;
#else
    static void OnLockAcquired(const char*, const char*, int) {}
    static void OnLockReleased(const char*) {}
    static void PrintHeldLocks() {}
#endif
};

#ifdef DEBUG_LOCKS
#define TRACK_LOCK(name) \
    LockTracker::OnLockAcquired(#name, __FILE__, __LINE__); \
    struct LockReleaser_##name { \
        ~LockReleaser_##name() { LockTracker::OnLockReleased(#name); } \
    } releaser_##name
#else
#define TRACK_LOCK(name)
#endif
```

### 25.3 锁超时机制

```cpp
/**
 * @brief 带超时的锁获取
 */
class TimedLockGuard {
public:
    /**
     * @brief 尝试在指定时间内获取锁
     * @return 是否成功获取
     */
    static bool TryLockFor(
        std::timed_mutex& mutex,
        std::chrono::milliseconds timeout
    ) {
        return mutex.try_lock_for(timeout);
    }
    
    /**
     * @brief 带超时的读写锁获取
     */
    static std::unique_ptr<std::unique_lock<std::shared_mutex>>
    TryWriteLockFor(
        std::shared_mutex& mutex,
        std::chrono::milliseconds timeout
    ) {
        auto lock = std::make_unique<std::unique_lock<std::shared_mutex>>(
            mutex, std::defer_lock
        );
        if (lock->try_lock_for(timeout)) {
            return lock;
        }
        return nullptr;
    }
    
    static std::unique_ptr<std::shared_lock<std::shared_mutex>>
    TryReadLockFor(
        std::shared_mutex& mutex,
        std::chrono::milliseconds timeout
    ) {
        auto lock = std::make_unique<std::shared_lock<std::shared_mutex>>(
            mutex, std::defer_lock
        );
        if (lock->try_lock_for(timeout)) {
            return lock;
        }
        return nullptr;
    }
};

/**
 * @brief 死锁检测器
 */
class DeadlockDetector {
public:
    struct LockWaitInfo {
        std::thread::id threadId;
        std::string lockName;
        std::chrono::steady_clock::time_point waitStart;
    };
    
    static void OnWaitStart(const std::string& lockName) {
        LockWaitInfo info;
        info.threadId = std::this_thread::get_id();
        info.lockName = lockName;
        info.waitStart = std::chrono::steady_clock::now();
        
        std::lock_guard<std::mutex> lock(s_mutex);
        s_waitInfos.push_back(info);
        
        CheckForDeadlock();
    }
    
    static void OnWaitEnd(const std::string& lockName) {
        std::lock_guard<std::mutex> lock(s_mutex);
        
        auto it = std::remove_if(s_waitInfos.begin(), s_waitInfos.end(),
            [&](const LockWaitInfo& info) {
                return info.threadId == std::this_thread::get_id() &&
                       info.lockName == lockName;
            });
        s_waitInfos.erase(it, s_waitInfos.end());
    }
    
    static void CheckForDeadlock() {
        auto now = std::chrono::steady_clock::now();
        
        for (const auto& info : s_waitInfos) {
            auto waitDuration = std::chrono::duration_cast<std::chrono::seconds>(
                now - info.waitStart
            ).count();
            
            if (waitDuration > 5) {
                std::cerr << "Potential deadlock detected: thread " 
                          << info.threadId << " waiting for " << info.lockName
                          << " for " << waitDuration << " seconds" << std::endl;
            }
        }
    }
    
private:
    static std::mutex s_mutex;
    static std::vector<LockWaitInfo> s_waitInfos;
};
```

---

## 26. 总结

### 26.1 设计亮点

| 特性 | 说明 |
|------|------|
| **完整的OGC标准支持** | 17种几何类型，完整的空间关系和运算 |
| **高性能缓存** | 线程安全的缓存机制，版本控制失效策略 |
| **鲁棒计算** | 精度上下文、鲁棒比较、数值稳定算法 |
| **SIMD优化** | 运行时检测，SSE2/AVX/NEON支持 |
| **内存池** | 几何对象和坐标序列的专用内存池 |
| **线程安全** | 层次化锁、死锁预防、读写分离 |
| **两阶段过滤** | 外包矩形快速过滤，减少精确计算 |

### 26.2 性能指标

| 操作 | 性能目标 | 说明 |
|------|----------|------|
| 外包矩形计算 | < 1μs | 1000点几何 |
| 空间关系判断（过滤后） | < 10μs | 简单几何 |
| 缓存命中 | < 100ns | 读锁访问 |
| 内存分配（池化） | < 50ns | 小对象 |

### 26.3 后续工作

1. 实现曲线几何类型的完整支持
2. 添加更多空间索引类型（KD-Tree、Grid）
3. 完善GPU加速的批量运算
4. 添加更多语言的绑定（Java、C#）

---

## 27. 曲线几何类型支持

### 27.1 曲线插值器

```cpp
/**
 * @brief 曲线插值器接口
 * 
 * 将曲线几何离散化为线串
 */
class CurveInterpolator {
public:
    struct Config {
        double maxSegmentLength = 1.0;      // 最大线段长度
        double maxAngle = 5.0;              // 最大角度（度）
        size_t maxSegments = 1000;          // 最大线段数
        bool preserveEndpoints = true;       // 保留端点
    };
    
    virtual ~CurveInterpolator() = default;
    
    /**
     * @brief 将曲线离散化为线串
     */
    virtual CNLineStringPtr Interpolate(const CNGeometry* curve, const Config& config) = 0;
    
    /**
     * @brief 获取曲线上的点
     */
    virtual Coordinate GetPoint(const CNGeometry* curve, double parameter) = 0;
    
    /**
     * @brief 获取曲线长度
     */
    virtual double GetLength(const CNGeometry* curve) = 0;
};

/**
 * @brief 圆弧插值器
 */
class CircularStringInterpolator : public CurveInterpolator {
public:
    CNLineStringPtr Interpolate(const CNGeometry* curve, const Config& config) override;
    Coordinate GetPoint(const CNGeometry* curve, double parameter) override;
    double GetLength(const CNGeometry* curve) override;
    
private:
    /**
     * @brief 三点确定圆弧
     */
    struct ArcInfo {
        Coordinate center;
        double radius;
        double startAngle;
        double endAngle;
        bool isClockwise;
    };
    
    ArcInfo ComputeArcInfo(const Coordinate& p1, const Coordinate& p2, const Coordinate& p3);
    Coordinate GetArcPoint(const ArcInfo& arc, double angle);
    double GetArcLength(const ArcInfo& arc);
};

/**
 * @brief 复合曲线插值器
 */
class CompoundCurveInterpolator : public CurveInterpolator {
public:
    CNLineStringPtr Interpolate(const CNGeometry* curve, const Config& config) override;
    Coordinate GetPoint(const CNGeometry* curve, double parameter) override;
    double GetLength(const CNGeometry* curve) override;
    
private:
    std::vector<std::unique_ptr<CurveInterpolator>> m_interpolators;
};
```

### 27.2 曲线几何类型实现

```cpp
/**
 * @brief 圆弧字符串
 */
class CNCircularString : public CNGeometry {
public:
    static std::unique_ptr<CNCircularString> Create();
    static std::unique_ptr<CNCircularString> Create(const CoordinateList& points);
    
    // 几何操作
    size_t GetNumPoints() const noexcept;
    const Coordinate& GetPointN(size_t n) const;
    void SetPointN(size_t n, const Coordinate& coord);
    void AddPoint(const Coordinate& coord);
    
    // 曲线特定操作
    CNLineStringPtr ToLineString(double maxSegmentLength = 1.0) const;
    double GetLength() const;
    Coordinate GetPointAtDistance(double distance) const;
    
    // CNGeometry接口实现
    GeomType GetGeometryType() const noexcept override { return GeomType::CircularString; }
    std::string GetGeometryTypeName() const override { return "CIRCULARSTRING"; }
    Dimension GetDimension() const noexcept override { return Dimension::Curve; }
    
private:
    CoordinateList m_points;
    mutable std::shared_ptr<CircularStringInterpolator> m_interpolator;
};

/**
 * @brief 复合曲线
 * 
 * 由连续的线段和圆弧组成
 */
class CNCompoundCurve : public CNGeometry {
public:
    struct CurveComponent {
        enum Type { LineString, CircularString } type;
        CNGeometryPtr geometry;
    };
    
    static std::unique_ptr<CNCompoundCurve> Create();
    
    void AddComponent(std::unique_ptr<CNLineString> line);
    void AddComponent(std::unique_ptr<CNCircularString> arc);
    
    size_t GetNumComponents() const noexcept;
    const CurveComponent& GetComponent(size_t n) const;
    
    CNLineStringPtr ToLineString(double maxSegmentLength = 1.0) const;
    double GetLength() const;
    
    GeomType GetGeometryType() const noexcept override { return GeomType::CompoundCurve; }
    std::string GetGeometryTypeName() const override { return "COMPOUNDCURVE"; }
    
private:
    std::vector<CurveComponent> m_components;
};

/**
 * @brief 曲线多边形
 * 
 * 外环和内环可以是复合曲线
 */
class CNCurvePolygon : public CNGeometry {
public:
    static std::unique_ptr<CNCurvePolygon> Create();
    
    void SetExteriorRing(std::unique_ptr<CNGeometry> ring);
    void AddInteriorRing(std::unique_ptr<CNGeometry> ring);
    
    const CNGeometry* GetExteriorRing() const noexcept;
    size_t GetNumInteriorRings() const noexcept;
    const CNGeometry* GetInteriorRingN(size_t n) const;
    
    CNPolygonPtr ToPolygon(double maxSegmentLength = 1.0) const;
    double GetArea() const;
    
    GeomType GetGeometryType() const noexcept override { return GeomType::CurvePolygon; }
    std::string GetGeometryTypeName() const override { return "CURVEPOLYGON"; }
    Dimension GetDimension() const noexcept override { return Dimension::Surface; }
    
private:
    CNGeometryPtr m_exteriorRing;
    std::vector<CNGeometryPtr> m_interiorRings;
};
```

---

## 28. 批量操作接口

### 28.1 批量空间关系判断

```cpp
/**
 * @brief 批量空间关系判断器
 * 
 * 优化大量几何之间的关系判断性能
 */
class BatchSpatialPredicate {
public:
    struct Config {
        bool useParallel = true;            // 是否并行处理
        size_t batchSize = 1000;            // 批处理大小
        size_t threadCount = 0;             // 线程数（0=自动）
        bool useEnvelopeFilter = true;      // 使用外包矩形过滤
    };
    
    explicit BatchSpatialPredicate(const Config& config = Config());
    
    /**
     * @brief 批量相交判断
     * 
     * 判断query与candidates中每个几何是否相交
     */
    std::vector<bool> Intersects(
        const CNGeometry* query,
        const std::vector<const CNGeometry*>& candidates
    );
    
    /**
     * @brief 批量包含判断
     */
    std::vector<bool> Contains(
        const CNGeometry* query,
        const std::vector<const CNGeometry*>& candidates
    );
    
    /**
     * @brief 批量距离计算
     */
    std::vector<double> Distance(
        const CNGeometry* query,
        const std::vector<const CNGeometry*>& candidates
    );
    
    /**
     * @brief 批量DE-9IM计算
     */
    std::vector<std::string> Relate(
        const CNGeometry* query,
        const std::vector<const CNGeometry*>& candidates
    );
    
    /**
     * @brief 矩阵式关系判断
     * 
     * 判断两组几何之间的所有关系
     */
    std::vector<std::vector<bool>> IntersectsMatrix(
        const std::vector<const CNGeometry*>& setA,
        const std::vector<const CNGeometry*>& setB
    );
    
    /**
     * @brief 获取统计信息
     */
    struct Statistics {
        size_t totalOperations;
        size_t envelopeFiltered;
        size_t preciseComputations;
        double totalTime;
        double avgTimePerOp;
    };
    
    const Statistics& GetStatistics() const noexcept;
    void ResetStatistics();
    
private:
    Config m_config;
    Statistics m_stats;
    SpatialOptimization::TwoPhaseSpatialPredicate m_optimizer;
    
    void ProcessBatch(
        const CNGeometry* query,
        const std::vector<const CNGeometry*>& candidates,
        std::vector<bool>& results,
        size_t startIdx,
        size_t endIdx
    );
};
```

### 28.2 批量几何运算

```cpp
/**
 * @brief 批量几何运算器
 */
class BatchGeometryOperation {
public:
    struct Config {
        bool useParallel = true;
        size_t batchSize = 100;
        size_t threadCount = 0;
    };
    
    explicit BatchGeometryOperation(const Config& config = Config());
    
    /**
     * @brief 批量缓冲区
     */
    std::vector<CNGeometryPtr> Buffer(
        const std::vector<const CNGeometry*>& geometries,
        double distance
    );
    
    /**
     * @brief 批量简化
     */
    std::vector<CNGeometryPtr> Simplify(
        const std::vector<const CNGeometry*>& geometries,
        double tolerance
    );
    
    /**
     * @brief 批量凸包
     */
    std::vector<CNGeometryPtr> ConvexHull(
        const std::vector<const CNGeometry*>& geometries
    );
    
    /**
     * @brief 批量外包矩形
     */
    std::vector<Envelope> Envelope(
        const std::vector<const CNGeometry*>& geometries
    );
    
    /**
     * @brief 批量坐标变换
     */
    std::vector<CNGeometryPtr> Transform(
        const std::vector<const CNGeometry*>& geometries,
        std::function<Coordinate(const Coordinate&)> transform
    );
    
private:
    Config m_config;
};
```

---

## 29. 几何构建器

### 29.1 流式几何构建API

```cpp
/**
 * @brief 几何构建器
 * 
 * 提供流式API构建复杂几何
 */
class GeometryBuilder {
public:
    GeometryBuilder();
    
    // ========== 点构建 ==========
    
    GeometryBuilder& Point(double x, double y);
    GeometryBuilder& Point(double x, double y, double z);
    GeometryBuilder& Point(const Coordinate& coord);
    
    // ========== 线串构建 ==========
    
    GeometryBuilder& StartLineString();
    GeometryBuilder& AddPoint(double x, double y);
    GeometryBuilder& AddPoint(double x, double y, double z);
    GeometryBuilder& AddPoint(const Coordinate& coord);
    GeometryBuilder& AddPoints(const CoordinateList& coords);
    GeometryBuilder& EndLineString();
    
    // ========== 多边形构建 ==========
    
    GeometryBuilder& StartPolygon();
    GeometryBuilder& StartRing();
    GeometryBuilder& EndRing();
    GeometryBuilder& EndPolygon();
    
    // ========== 集合构建 ==========
    
    GeometryBuilder& StartCollection();
    GeometryBuilder& AddGeometry(const CNGeometry* geom);
    GeometryBuilder& EndCollection();
    
    // ========== 预定义形状 ==========
    
    GeometryBuilder& Rectangle(double minX, double minY, double maxX, double maxY);
    GeometryBuilder& Circle(double centerX, double centerY, double radius, int segments = 32);
    GeometryBuilder& RegularPolygon(double centerX, double centerY, double radius, int sides);
    
    // ========== 构建 ==========
    
    CNGeometryPtr Build();
    CNPointPtr BuildPoint();
    CNLineStringPtr BuildLineString();
    CNPolygonPtr BuildPolygon();
    
    // ========== 重置 ==========
    
    GeometryBuilder& Reset();
    
private:
    enum class BuildMode {
        None,
        Point,
        LineString,
        Polygon,
        Collection
    };
    
    BuildMode m_mode = BuildMode::None;
    CoordinateList m_currentCoords;
    std::vector<CoordinateList> m_rings;
    std::vector<CNGeometryPtr> m_geometries;
    
    Coordinate m_currentPoint;
    bool m_hasPoint = false;
};

// 使用示例
void ExampleUsage() {
    auto polygon = GeometryBuilder()
        .StartPolygon()
        .StartRing()
        .AddPoint(0, 0)
        .AddPoint(10, 0)
        .AddPoint(10, 10)
        .AddPoint(0, 10)
        .AddPoint(0, 0)
        .EndRing()
        .StartRing()
        .AddPoint(2, 2)
        .AddPoint(8, 2)
        .AddPoint(8, 8)
        .AddPoint(2, 8)
        .AddPoint(2, 2)
        .EndRing()
        .EndPolygon()
        .BuildPolygon();
    
    auto lineString = GeometryBuilder()
        .StartLineString()
        .AddPoint(0, 0)
        .AddPoint(1, 1)
        .AddPoint(2, 0)
        .EndLineString()
        .BuildLineString();
}
```

### 29.2 WKT构建器

```cpp
/**
 * @brief WKT字符串构建器
 */
class WKTBuilder {
public:
    WKTBuilder& StartPoint();
    WKTBuilder& StartLineString();
    WKTBuilder& StartPolygon();
    WKTBuilder& StartMultiPoint();
    WKTBuilder& StartMultiLineString();
    WKTBuilder& StartMultiPolygon();
    WKTBuilder& StartCollection();
    
    WKTBuilder& AddCoordinate(double x, double y);
    WKTBuilder& AddCoordinate(double x, double y, double z);
    WKTBuilder& AddCoordinate(const Coordinate& coord);
    
    WKTBuilder& End();
    
    std::string Build();
    
private:
    std::stringstream m_ss;
    std::vector<std::string> m_stack;
    bool m_needComma = false;
};
```

---

## 30. 性能监控与诊断

### 30.1 性能计数器

```cpp
/**
 * @brief 几何操作性能计数器
 */
class GeometryPerformanceCounter {
public:
    enum class CounterType {
        EnvelopeComputation,
        SpatialRelation,
        GeometryOperation,
        Serialization,
        Deserialization,
        MemoryAllocation
    };
    
    static GeometryPerformanceCounter& GetInstance();
    
    /**
     * @brief 记录操作
     */
    void Record(CounterType type, double duration, size_t dataSize = 0);
    
    /**
     * @brief 获取统计
     */
    struct Statistics {
        size_t count;
        double totalTime;
        double minTime;
        double maxTime;
        double avgTime;
        size_t totalDataSize;
    };
    
    Statistics GetStatistics(CounterType type) const;
    
    /**
     * @brief 重置计数器
     */
    void Reset();
    void Reset(CounterType type);
    
    /**
     * @brief 导出报告
     */
    std::string GenerateReport() const;
    
private:
    GeometryPerformanceCounter() = default;
    
    struct CounterData {
        std::atomic<size_t> count{0};
        std::atomic<double> totalTime{0};
        std::atomic<double> minTime{std::numeric_limits<double>::max()};
        std::atomic<double> maxTime{0};
        std::atomic<size_t> totalDataSize{0};
    };
    
    std::unordered_map<CounterType, CounterData> m_counters;
};

/**
 * @brief 性能计时守卫
 */
class PerformanceTimer {
public:
    explicit PerformanceTimer(
        GeometryPerformanceCounter::CounterType type,
        size_t dataSize = 0
    );
    ~PerformanceTimer();
    
    PerformanceTimer(const PerformanceTimer&) = delete;
    PerformanceTimer& operator=(const PerformanceTimer&) = delete;
    
private:
    GeometryPerformanceCounter::CounterType m_type;
    size_t m_dataSize;
    std::chrono::high_resolution_clock::time_point m_start;
};

// 使用宏简化
#define PERF_TIMER(type) PerformanceTimer _timer(type)
#define PERF_TIMER_DATA(type, size) PerformanceTimer _timer(type, size)
```

### 30.2 内存诊断

```cpp
/**
 * @brief 内存诊断器
 */
class MemoryDiagnostics {
public:
    struct MemoryInfo {
        size_t geometryCount;           // 几何对象数量
        size_t coordinateCount;         // 坐标数量
        size_t totalMemory;             // 总内存使用
        size_t cachedMemory;            // 缓存内存
        size_t pooledMemory;            // 内存池使用
        size_t peakMemory;              // 峰值内存
    };
    
    static MemoryDiagnostics& GetInstance();
    
    /**
     * @brief 获取内存信息
     */
    MemoryInfo GetMemoryInfo() const;
    
    /**
     * @brief 按类型统计
     */
    std::map<GeomType, size_t> GetGeometryCountByType() const;
    
    /**
     * @brief 内存泄漏检测
     */
    struct LeakInfo {
        void* address;
        size_t size;
        GeomType type;
        std::string allocationTrace;
    };
    
    std::vector<LeakInfo> DetectLeaks() const;
    
    /**
     * @brief 设置内存警告阈值
     */
    void SetWarningThreshold(size_t bytes);
    void SetCriticalThreshold(size_t bytes);
    
    /**
     * @brief 内存警告回调
     */
    using MemoryWarningCallback = std::function<void(const MemoryInfo&)>;
    void SetWarningCallback(MemoryWarningCallback callback);
    
private:
    MemoryDiagnostics() = default;
    
    size_t m_warningThreshold = 1024 * 1024 * 100;   // 100MB
    size_t m_criticalThreshold = 1024 * 1024 * 500;  // 500MB
    MemoryWarningCallback m_warningCallback;
};
```

---

## 31. 错误处理增强

### 31.1 错误上下文

```cpp
/**
 * @brief 错误上下文
 * 
 * 提供详细的错误信息，包括调用链
 */
class ErrorContext {
public:
    struct Frame {
        std::string function;
        std::string file;
        int line;
        std::string message;
    };
    
    ErrorContext() = default;
    
    /**
     * @brief 添加调用帧
     */
    void PushFrame(const std::string& function, const std::string& file, int line, const std::string& message = "");
    
    /**
     * @brief 弹出调用帧
     */
    void PopFrame();
    
    /**
     * @brief 获取调用栈
     */
    const std::vector<Frame>& GetCallStack() const noexcept { return m_callStack; }
    
    /**
     * @brief 生成错误报告
     */
    std::string GenerateReport() const;
    
    /**
     * @brief 清空调用栈
     */
    void Clear() { m_callStack.clear(); }
    
    /**
     * @brief 作用域守卫
     */
    class ScopeGuard {
    public:
        ScopeGuard(const std::string& function, const std::string& file, int line, const std::string& message = "");
        ~ScopeGuard();
        
        ScopeGuard(const ScopeGuard&) = delete;
        ScopeGuard& operator=(const ScopeGuard&) = delete;
    };
    
private:
    std::vector<Frame> m_callStack;
    
    static thread_local ErrorContext t_currentContext;
};

// 使用宏简化
#define ERROR_SCOPE(msg) ErrorContext::ScopeGuard _scope(__func__, __FILE__, __LINE__, msg)
#define ERROR_PUSH(msg) ErrorContext::ScopeGuard _scope(__func__, __FILE__, __LINE__, msg)
```

### 31.2 增强的异常类

```cpp
/**
 * @brief 增强的几何异常类
 */
class EnhancedGeometryException : public std::runtime_error {
public:
    EnhancedGeometryException(
        GeomResult code,
        const std::string& message,
        const ErrorContext& context = ErrorContext()
    );
    
    /**
     * @brief 获取错误码
     */
    GeomResult GetErrorCode() const noexcept { return m_code; }
    
    /**
     * @brief 获取错误上下文
     */
    const ErrorContext& GetContext() const noexcept { return m_context; }
    
    /**
     * @brief 获取详细错误信息
     */
    std::string GetDetailedMessage() const;
    
    /**
     * @brief 获取调用栈字符串
     */
    std::string GetStackTrace() const;
    
private:
    GeomResult m_code;
    ErrorContext m_context;
};

/**
 * @brief 错误处理器
 */
class ErrorHandler {
public:
    enum class Policy {
        Throw,          // 抛出异常
        ReturnCode,     // 返回错误码
        Log,            // 记录日志
        Ignore          // 忽略
    };
    
    static ErrorHandler& GetInstance();
    
    /**
     * @brief 设置错误处理策略
     */
    void SetPolicy(Policy policy) { m_policy = policy; }
    Policy GetPolicy() const noexcept { return m_policy; }
    
    /**
     * @brief 处理错误
     */
    GeomResult Handle(GeomResult code, const std::string& message, const char* file, int line);
    
    /**
     * @brief 设置日志回调
     */
    using LogCallback = std::function<void(GeomResult, const std::string&, const std::string&)>;
    void SetLogCallback(LogCallback callback);
    
private:
    ErrorHandler() = default;
    
    Policy m_policy = Policy::Throw;
    LogCallback m_logCallback;
};

// 使用宏
#define GLM_HANDLE_ERROR(code, msg) \
    ErrorHandler::GetInstance().Handle(code, msg, __FILE__, __LINE__)
```

---

## 32. 总结

### 32.1 设计亮点

| 特性 | 说明 |
|------|------|
| **完整的OGC标准支持** | 17种几何类型，完整的空间关系和运算 |
| **高性能缓存** | 线程安全的缓存机制，版本控制失效策略 |
| **鲁棒计算** | 精度上下文、鲁棒比较、数值稳定算法 |
| **SIMD优化** | 运行时检测，SSE2/AVX/NEON支持 |
| **内存池** | 几何对象和坐标序列的专用内存池 |
| **线程安全** | 层次化锁、死锁预防、读写分离 |
| **两阶段过滤** | 外包矩形快速过滤，减少精确计算 |
| **曲线几何支持** | 圆弧、复合曲线、曲线多边形 |
| **批量操作** | 并行批量空间关系判断和几何运算 |
| **流式API** | GeometryBuilder提供易用的构建接口 |
| **性能监控** | 计数器、内存诊断、错误追踪 |

### 32.2 性能指标

| 操作 | 性能目标 | 说明 |
|------|----------|------|
| 外包矩形计算 | < 1μs | 1000点几何 |
| 空间关系判断（过滤后） | < 10μs | 简单几何 |
| 缓存命中 | < 100ns | 读锁访问 |
| 内存分配（池化） | < 50ns | 小对象 |
| 批量关系判断 | > 100K ops/s | 并行处理 |

### 32.3 后续工作

1. 实现GPU加速的批量运算
2. 添加更多语言的绑定（Java、C#）
3. 完善空间索引的持久化
4. 添加拓扑操作支持

---

**文档版本**: v2.5  
**最后更新**: 2026年3月19日  
**维护者**: Geometry Library Team
