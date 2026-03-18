# OGC 几何类库设计文档 v2.2

**版本**: 2.2  
**日期**: 2026年3月18日  
**状态**: 评审通过  
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

---

## 13. 使用示例

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

**文档版本**: v2.1  
**最后更新**: 2026年3月18日  
**维护者**: Geometry Library Team
