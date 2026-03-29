# 渲染架构任务计划（2人团队）

**版本**: v4.1  
**日期**: 2026-03-29  
**基于文档**: render_category_design.md v1.5

---

## 参考文档

- **设计文档**: [render_category_design.md](./render_category_design.md)
- 实施时以设计文档中的描述为准
- 如有疑问请查阅对应设计文档

## 技术约束

- **编程语言**: C++11
- **核心框架**: 无外部框架依赖，纯C++11标准库
- **代码规范**: Google C++ Style
- **现代C++11特性**: 智能指针、移动语义、RAII、原子操作、强类型枚举


## 1. 项目概述与资源分配

### 1.1 项目概况

| 项目指标 | 数值 |
|----------|------|
| 总任务数 | 57 |
| 总估算工时 | 350h (PERT期望值) |
| 关键路径时长 | 192h |
| 目标完成周期 | 15周 |
| 建议团队规模 | 2人 |

### 1.2 团队资源分配

| 开发者 | 技能特长 | 分配比例 | 主要负责模块 |
|--------|----------|----------|--------------|
| **Dev A** | C++, 架构设计, GPU编程 | 100% | 核心框架、GPU加速、矢量输出、插件系统 |
| **Dev B** | C++, 图形渲染, 跨平台 | 100% | CPU渲染、瓦片渲染、平台适配、性能优化 |

### 1.3 任务分配矩阵

| 里程碑 | Dev A任务 | Dev B任务 |
|--------|-----------|-----------|
| M1 核心框架 | T1-T5, T53 | T6-T9 |
| M2 CPU渲染 | T10-T12 | T13-T16, T55 |
| M3 GPU加速 | T17-T22, T54, T56 | - |
| M4 瓦片渲染 | - | T23-T27 |
| M5 矢量输出 | T28-T32 | T33 |
| M6 平台适配 | T34-T38 | T39-T46 |
| M7 高级特性 | T47-T50 | T51-T52, T57 |

### 1.4 开发周期规划

```
Week 1-2:   M1 核心框架 (Dev A + Dev B 并行)
Week 3-4:   M2 CPU渲染 (Dev A + Dev B 并行)
Week 5-7:   M3 GPU加速 (Dev A) + M4 瓦片渲染 (Dev B) 并行
Week 8-9:   M5 矢量输出 (Dev A + Dev B 并行)
Week 10-12: M6 平台适配 (Dev A + Dev B 并行)
Week 13-15: M7 高级特性 + 集成测试
```


---

## 2. 任务总览与里程碑规划

### 2.1 里程碑概览

| 里程碑 | 名称 | 任务数 | 工时 | 周期 | 依赖 |
|--------|------|--------|------|------|------|
| **M1** | 核心框架 | 10 | 60h | Week 1-2 | 无 |
| **M2** | CPU渲染 | 8 | 48h | Week 3-4 | M1 |
| **M3** | GPU加速 | 8 | 62h | Week 5-7 | M1 |
| **M4** | 瓦片渲染 | 5 | 40h | Week 5-7 | M2 |
| **M5** | 矢量输出 | 6 | 36h | Week 8-9 | M2 |
| **M6** | 平台适配 | 13 | 56h | Week 10-12 | M2-M5 |
| **M7** | 高级特性 | 7 | 48h | Week 13-15 | M1-M6 |

### 2.2 任务依赖关系图

```
                    ┌─────────────────────────────────────────┐
                    │              M1 核心框架                  │
                    │  T1-T9 (DrawDevice/Engine/Context)       │
                    └──────────────────┬──────────────────────┘
                                       │
           ┌───────────────────────────┼───────────────────────────┐
           │                           │                           │
           ▼                           ▼                           ▼
    ┌──────────────┐          ┌──────────────┐          ┌──────────────┐
    │   M2 CPU渲染  │          │  M3 GPU加速  │          │  M5 矢量输出  │
    │  T10-T16     │          │  T17-T22     │          │  T28-T33     │
    │ Simple2D引擎  │          │ GPU引擎/内存池│          │ Vector引擎   │
    └──────┬───────┘          └──────────────┘          └──────────────┘
           │                                                   
           ▼                                                   
    ┌──────────────┐                                          
    │  M4 瓦片渲染  │                                          
    │  T23-T27     │                                          
    │ Tile引擎/异步 │                                          
    └──────────────┘                                          
           │                                                   
           └───────────────────────┬───────────────────────────┘
                                   │
                                   ▼
                    ┌─────────────────────────────────────────┐
                    │              M6 平台适配                  │
                    │  T34-T46 (Qt/Windows/Linux/macOS/Web)   │
                    └──────────────────┬──────────────────────┘
                                       │
                                       ▼
                    ┌─────────────────────────────────────────┐
                    │              M7 高级特性                  │
                    │  T47-T52 (插件/缓存/优化/测试)            │
                    └─────────────────────────────────────────┘
```

### 2.3 任务优先级分布

| 优先级 | 任务数 | 占比 | 说明 |
|--------|--------|------|------|
| **P0** (关键) | 16 | 28% | 核心功能，必须按时完成 |
| **P1** (重要) | 29 | 51% | 主要功能，影响用户体验 |
| **P2** (一般) | 12 | 21% | 增强功能，可适当延期 |

### 2.4 任务列表总览

| 任务ID | 任务名称 | 里程碑 | 优先级 | 负责人 | 估算工时 | 依赖 | 状态 |
|--------|----------|--------|--------|--------|----------|------|------|
| T1 | DrawDevice基类设计 | M1 | P0 | Dev A | 8h | - | ✅完成 |
| T2 | DrawEngine基类设计 | M1 | P0 | Dev A | 10h | T1 | ✅完成 |
| T3 | DrawContext上下文设计 | M1 | P0 | Dev A | 12h | T2 | ✅完成 |
| T4 | DrawStyle样式系统 | M1 | P0 | Dev A | 6h | T2 | ✅完成 |
| T5 | DrawResult错误处理 | M1 | P1 | Dev A | 4h | T2 | ✅完成 |
| T6 | 基础类型定义 | M1 | P0 | Dev B | 4h | - | ✅完成 |
| T7 | TransformMatrix变换矩阵 | M1 | P0 | Dev B | 4h | T6 | ✅完成 |
| T8 | Geometry几何定义 | M1 | P0 | Dev B | 6h | T6 | ✅完成 |
| T9 | 单元测试框架搭建 | M1 | P1 | Dev B | 8h | T1-T8 | ✅完成 |
| T10 | RasterImageDevice实现 | M2 | P0 | Dev A | 8h | T1 | ✅完成 |
| T11 | DisplayDevice实现 | M2 | P0 | Dev A | 6h | T1 | ✅完成 |
| T12 | Simple2DEngine基类 | M2 | P0 | Dev A | 10h | T2 | ✅完成 |
| T13 | Simple2DEngine点线绘制 | M2 | P0 | Dev B | 8h | T12 | ✅完成 |
| T14 | Simple2DEngine多边形绘制 | M2 | P0 | Dev B | 6h | T12 | ✅完成 |
| T15 | Simple2DEngine文本渲染 | M2 | P1 | Dev B | 8h | T12 | ✅完成 |
| T16 | Simple2DEngine图像绘制 | M2 | P1 | Dev B | 6h | T12 | ✅完成 |
| T17 | GPUResourceManager | M3 | P0 | Dev A | 10h | T2 | ⏳待实施 |
| T18 | RenderMemoryPool | M3 | P0 | Dev A | 10h | T17 | ⏳待实施 |
| T19 | TextureCache纹理缓存 | M3 | P1 | Dev A | 8h | T17 | ⏳待实施 |
| T20 | GPUAcceleratedEngine | M3 | P0 | Dev A | 14h | T17,T18 | ⏳待实施 |
| T21 | GPU资源RAII包装 | M3 | P1 | Dev A | 6h | T17 | ⏳待实施 |
| T22 | 多GPU设备选择 | M3 | P2 | Dev A | 4h | T17 | ⏳待实施 |
| T23 | TileDevice瓦片设备 | M4 | P0 | Dev B | 8h | T1 | ✅完成 |
| T24 | TileBasedEngine | M4 | P0 | Dev B | 12h | T23,T12 | ✅完成 |
| T25 | AsyncRenderTask | M4 | P0 | Dev B | 8h | T24 | ✅完成 |
| T26 | AsyncRenderManager | M4 | P0 | Dev B | 8h | T25 | ✅完成 |
| T27 | 瓦片大小动态策略 | M4 | P1 | Dev B | 4h | T24 | ✅完成 |
| T28 | VectorEngine基类 | M5 | P0 | Dev A | 8h | T2 | ✅完成 |
| T29 | PdfDevice实现 | M5 | P0 | Dev A | 6h | T1 | ⏳待实施 |
| T30 | PdfEngine实现(podofo) | M5 | P0 | Dev A | 8h | T28,T29 | ⏳待实施 |
| T31 | SvgDevice实现 | M5 | P1 | Dev A | 4h | T1 | ✅完成 |
| T32 | SvgEngine实现(cairo) | M5 | P1 | Dev A | 6h | T28,T31 | ✅完成 |
| T33 | 矢量输出测试 | M5 | P1 | Dev B | 4h | T30,T32 | ✅完成 |
| T34 | QtEngine实现 | M6 | P0 | Dev A | 12h | T12 | ⏳待实施 |
| T35 | QtDisplayDevice | M6 | P0 | Dev A | 6h | T34 | ⏳待实施 |
| T36 | GDIPlusEngine | M6 | P1 | Dev A | 8h | T12 | ⏳待实施 |
| T37 | Direct2DEngine | M6 | P1 | Dev A | 10h | T17 | ⏳待实施 |
| T38 | Windows平台集成 | M6 | P1 | Dev A | 4h | T36,T37 | ⏳待实施 |
| T39 | CairoEngine | M6 | P1 | Dev B | 8h | T12 | ⏳待实施 |
| T40 | X11DisplayDevice | M6 | P1 | Dev B | 6h | T39 | ⏳待实施 |
| T41 | CoreGraphicsEngine | M6 | P1 | Dev B | 8h | T12 | ⏳待实施 |
| T42 | MetalEngine | M6 | P2 | Dev B | 10h | T17 | ⏳待实施 |
| T43 | macOS平台集成 | M6 | P1 | Dev B | 4h | T41,T42 | ⏳待实施 |
| T44 | WebGLEngine | M6 | P1 | Dev B | 10h | T17 | ⏳待实施 |
| T45 | WebGLDevice | M6 | P1 | Dev B | 6h | T44 | ⏳待实施 |
| T46 | Web平台集成 | M6 | P1 | Dev B | 4h | T44,T45 | ⏳待实施 |
| T47 | PluginManager | M7 | P1 | Dev A | 10h | T2 | ⏳待实施 |
| T48 | DrawEnginePlugin接口 | M7 | P1 | Dev A | 6h | T47 | ⏳待实施 |
| T49 | DrawDevicePlugin接口 | M7 | P1 | Dev A | 4h | T47 | ⏳待实施 |
| T50 | 版本兼容性完善 | M7 | P2 | Dev A | 6h | T47 | ⏳待实施 |
| T51 | EnginePool引擎池 | M7 | P1 | Dev B | 6h | T3 | ⏳待实施 |
| T52 | 性能优化与测试 | M7 | P0 | Dev B | 12h | T1-T51 | ⏳待实施 |
| T53 | StateSerializer状态序列化器 | M1 | P1 | Dev A | 4h | T2 | ✅完成 |
| T54 | ThreadSafeEngine线程安全包装 | M3 | P1 | Dev A | 4h | T2 | ⏳待实施 |
| T55 | 辅助工具类(DrawScopeGuard/DrawSession) | M2 | P2 | Dev B | 4h | T3 | ⏳待实施 |
| T56 | 设备能力协商与丢失恢复 | M3 | P1 | Dev A | 6h | T1,T17 | ⏳待实施 |
| T57 | 库兼容性检查工具 | M7 | P2 | Dev B | 4h | T6 | ⏳待实施 |


---

## 3. M1 核心框架任务详情

### T1: DrawDevice基类设计

| 属性 | 内容 |
|------|------|
| **任务摘要** | 设计并实现DrawDevice抽象基类，定义设备层接口 |
| **任务描述** | 实现DrawDevice基类，包含设备类型、尺寸、DPI、引擎创建等核心接口。支持设备能力查询和引擎类型协商机制。 |
| **依赖关系** | 无依赖 |
| **里程碑** | M1 核心框架 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 8h (乐观6h, 悲观12h) |

**产出文件**:
- `include/ogc/draw/draw_device.h`
- `src/draw/draw_device.cpp`

**类相关接口**:
```cpp
class DrawDevice {
public:
    virtual ~DrawDevice() = default;
    virtual DeviceType GetType() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual double GetDpi() const = 0;
    virtual std::unique_ptr<DrawEngine> CreateEngine() = 0;
    virtual std::vector<EngineType> GetSupportedEngineTypes() const = 0;
    virtual EngineType GetPreferredEngineType() const = 0;
    virtual DeviceCapabilities GetCapabilities() const = 0;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] DrawDevice基类编译通过
- [ ] 所有纯虚函数定义完整
- [ ] 包含设备能力查询接口
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第1-200行

---

### T2: DrawEngine基类设计

| 属性 | 内容 |
|------|------|
| **任务摘要** | 设计并实现DrawEngine抽象基类，定义引擎层接口 |
| **任务描述** | 实现DrawEngine基类，包含原子绘制操作、坐标变换、状态管理、裁剪等核心接口。支持同步和异步渲染模式。 |
| **依赖关系** | T1 (DrawDevice基类) |
| **里程碑** | M1 核心框架 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/draw_engine.h`
- `src/draw/draw_engine.cpp`

**类相关接口**:
```cpp
class DrawEngine {
public:
    virtual ~DrawEngine() = default;
    virtual std::string GetName() const = 0;
    virtual EngineType GetType() const = 0;
    virtual bool Begin() = 0;
    virtual void End() = 0;
    virtual DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) = 0;
    virtual DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) = 0;
    virtual DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) = 0;
    virtual DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) = 0;
    virtual DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) = 0;
    virtual void SetTransform(const TransformMatrix& matrix) = 0;
    virtual void SetClipRegion(const Region& region) = 0;
    virtual void Save(StateFlags flags = StateFlag::kAll) = 0;
    virtual void Restore() = 0;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] DrawEngine基类编译通过
- [ ] 所有原子绘制接口定义完整
- [ ] 状态管理接口(Save/Restore)实现
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第200-500行

---

### T3: DrawContext上下文设计

| 属性 | 内容 |
|------|------|
| **任务摘要** | 设计并实现DrawContext绘制上下文类 |
| **任务描述** | 实现DrawContext类，管理绘制状态栈、样式栈、裁剪区域栈。提供高级绘制API和状态管理功能。实现Save(flags)/Restore()简化状态管理。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M1 核心框架 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 12h (乐观10h, 悲观16h) |

**产出文件**:
- `include/ogc/draw/draw_context.h`
- `src/draw/draw_context.cpp`

**类相关接口**:
```cpp
class DrawContext {
public:
    enum class StateFlag : uint32_t {
        kNone = 0, kTransform = 1 << 0, kStyle = 1 << 1,
        kOpacity = 1 << 2, kClip = 1 << 3, kAll = 0xFFFFFFFF
    };
    using StateFlags = uint32_t;
    
    static std::unique_ptr<DrawContext> Create(DrawDevice* device);
    
    void Save(StateFlags flags = StateFlag::kAll);
    void Restore();
    
    DrawResult DrawGeometry(const Geometry* geometry);
    DrawResult DrawPoints(const double* x, const double* y, int count);
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count);
    DrawResult DrawPolygon(const double* x, const double* y, int count, bool fill = true);
    
    void SetTransform(const TransformMatrix& matrix);
    void SetStyle(const DrawStyle& style);
    void SetClipRect(double x, double y, double w, double h);
    
private:
    DrawDevice* m_device = nullptr;
    DrawEngine* m_engine = nullptr;
    DrawStyle m_currentStyle;
    std::stack<StateEntry> m_stateStack;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] DrawContext类编译通过
- [ ] StateFlag位掩码机制正确实现
- [ ] 状态栈管理正确（Save/Restore配对）
- [ ] 高级绘制API功能完整
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第500-900行

---

### T4: DrawStyle样式系统

| 属性 | 内容 |
|------|------|
| **任务摘要** | 设计并实现DrawStyle样式定义类 |
| **任务描述** | 实现DrawStyle类，包含画笔、画刷、字体、透明度等样式属性。支持样式继承和组合。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M1 核心框架 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/draw_style.h`
- `src/draw/draw_style.cpp`

**类相关接口**:
```cpp
struct Pen {
    Color color = Color(0, 0, 0);
    double width = 1.0;
    PenStyle style = PenStyle::kSolid;
    LineCap cap = LineCap::kFlat;
    LineJoin join = LineJoin::kMiter;
    std::vector<double> dashPattern;
};

struct Brush {
    Color color = Color(255, 255, 255);
    BrushStyle style = BrushStyle::kSolid;
    std::string texturePath;
};

struct Font {
    std::string family = "Arial";
    double size = 12.0;
    FontWeight weight = FontWeight::kNormal;
    FontStyle style = FontStyle::kNormal;
};

struct DrawStyle {
    Pen pen;
    Brush brush;
    Font font;
    double opacity = 1.0;
    FillRule fillRule = FillRule::kEvenOdd;
    bool antialias = true;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] DrawStyle及相关结构体定义完整
- [ ] 支持样式序列化/反序列化
- [ ] 默认样式值合理
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第900-1100行

---

### T5: DrawResult错误处理

| 属性 | 内容 |
|------|------|
| **任务摘要** | 设计并实现DrawResult错误处理机制 |
| **任务描述** | 实现DrawResult枚举和错误处理工具类，支持错误码、错误消息、错误上下文记录。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M1 核心框架 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/draw_result.h`
- `src/draw/draw_result.cpp`

**类相关接口**:
```cpp
enum class DrawResult {
    kSuccess = 0,
    kInvalidParameter = 1,
    kOutOfMemory = 2,
    kDeviceError = 3,
    kEngineError = 4,
    kNotImplemented = 5,
    kCancelled = 6,
    kTimeout = 7
};

class DrawError {
public:
    DrawResult code;
    std::string message;
    std::string context;
    std::string file;
    int line;
    
    static DrawError FromResult(DrawResult result, const std::string& context = "");
    std::string ToString() const;
};

#define DRAW_RETURN_IF_ERROR(expr) \
    do { DrawResult _r = (expr); if (_r != DrawResult::kSuccess) return _r; } while(0)
```

**第三方库依赖**: 无

**验收标准**:
- [ ] DrawResult枚举覆盖所有错误场景
- [ ] DrawError类提供完整错误上下文
- [ ] 宏定义正确使用
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第1100-1300行

---

### T6: 基础类型定义

| 属性 | 内容 |
|------|------|
| **任务摘要** | 定义基础数据类型和枚举 |
| **任务描述** | 定义DeviceType、EngineType、Color、Point、Rect、Size等基础类型。确保跨平台一致性。 |
| **依赖关系** | 无依赖 |
| **里程碑** | M1 核心框架 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/device_type.h`
- `include/ogc/draw/engine_type.h`
- `include/ogc/draw/draw_types.h`

**类相关接口**:
```cpp
enum class DeviceType {
    kDisplay = 0, kRasterImage = 1, kPdf = 2, kSvg = 3,
    kTile = 4, kWebGL = 5, kPrinter = 6
};

enum class EngineType {
    kSimple2D = 0, kGPU = 1, kVector = 2, kTile = 3, kWebGL = 4
};

struct Color {
    uint8_t r, g, b, a;
    Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
    static Color FromHex(uint32_t hex);
    static Color FromHSV(double h, double s, double v);
};

struct Point { double x, y; };
struct Rect { double x, y, w, h; };
struct Size { double w, h; };
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 所有枚举定义完整
- [ ] 基础类型跨平台一致
- [ ] Color支持多种格式转换
- [ ] 单元测试覆盖率 > 90%

**参考文档**: render_category_design.md 第1-200行

---

### T7: TransformMatrix变换矩阵

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现TransformMatrix变换矩阵类 |
| **任务描述** | 实现2D仿射变换矩阵，支持平移、旋转、缩放、倾斜操作。支持矩阵组合和逆矩阵计算。 |
| **依赖关系** | T6 (基础类型定义) |
| **里程碑** | M1 核心框架 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/transform_matrix.h`
- `src/draw/transform_matrix.cpp`

**类相关接口**:
```cpp
class TransformMatrix {
public:
    double m[3][3];
    
    static TransformMatrix Identity();
    static TransformMatrix Translate(double dx, double dy);
    static TransformMatrix Rotate(double angle);
    static TransformMatrix Scale(double sx, double sy);
    static TransformMatrix Shear(double shx, double shy);
    
    TransformMatrix operator*(const TransformMatrix& other) const;
    TransformMatrix Inverse() const;
    
    void TransformPoint(double x, double y, double& outX, double& outY) const;
    Rect TransformRect(const Rect& rect) const;
    
    bool IsIdentity() const;
    bool IsInvertible() const;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 所有变换操作正确实现
- [ ] 矩阵乘法符合数学定义
- [ ] 逆矩阵计算正确
- [ ] 单元测试覆盖率 > 90%

**参考文档**: render_category_design.md 第1300-1500行

---

### T8: Geometry几何定义

| 属性 | 内容 |
|------|------|
| **任务摘要** | 定义Geometry几何数据结构 |
| **任务描述** | 实现Geometry基类和派生类（PointGeometry、LineGeometry、PolygonGeometry等），支持几何数据存储和变换。 |
| **依赖关系** | T6 (基础类型定义) |
| **里程碑** | M1 核心框架 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/geometry.h`
- `src/draw/geometry.cpp`

**类相关接口**:
```cpp
class Geometry {
public:
    virtual ~Geometry() = default;
    virtual GeometryType GetType() const = 0;
    virtual Rect GetBounds() const = 0;
    virtual std::unique_ptr<Geometry> Clone() const = 0;
    virtual void Transform(const TransformMatrix& matrix) = 0;
};

class PointGeometry : public Geometry { /* ... */ };
class LineGeometry : public Geometry { /* ... */ };
class PolygonGeometry : public Geometry { /* ... */ };
class MultiPointGeometry : public Geometry { /* ... */ };
class MultiLineGeometry : public Geometry { /* ... */ };
class MultiPolygonGeometry : public Geometry { /* ... */ };
```

**第三方库依赖**: 无

**验收标准**:
- [ ] Geometry基类和派生类定义完整
- [ ] 边界计算正确
- [ ] 变换操作正确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第1500-1700行

---

### T9: 单元测试框架搭建

| 属性 | 内容 |
|------|------|
| **任务摘要** | 搭建单元测试框架和CI配置 |
| **任务描述** | 配置GoogleTest测试框架，创建测试目录结构，编写基础测试用例模板，配置CI/CD自动化测试。 |
| **依赖关系** | T1-T8 (所有核心框架任务) |
| **里程碑** | M1 核心框架 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `tests/draw/CMakeLists.txt`
- `tests/draw/test_draw_device.cpp`
- `tests/draw/test_draw_engine.cpp`
- `tests/draw/test_draw_context.cpp`
- `tests/draw/test_transform_matrix.cpp`
- `.github/workflows/test.yml`

**类相关接口**: 无（基础设施）

**第三方库依赖**:
- GoogleTest >= 1.12.0

**验收标准**:
- [ ] 测试框架编译通过
- [ ] 所有核心类有基础测试用例
- [ ] CI配置正确运行
- [ ] 代码覆盖率报告生成

**参考文档**: render_category_design.md 附录


---

## 4. M2 CPU渲染任务详情

### T10: RasterImageDevice实现

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现RasterImageDevice光栅图像设备 |
| **任务描述** | 实现RasterImageDevice类，支持创建和管理光栅图像缓冲区。支持多种像素格式（RGB、RGBA、灰度等），支持图像导出（PNG、JPEG、BMP）。 |
| **依赖关系** | T1 (DrawDevice基类) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/raster_image_device.h`
- `src/draw/raster_image_device.cpp`

**类相关接口**:
```cpp
class RasterImageDevice : public DrawDevice {
public:
    RasterImageDevice(int width, int height, PixelFormat format = PixelFormat::kRGBA8888);
    ~RasterImageDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kRasterImage; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override { return m_dpi; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kSimple2D; }
    
    uint8_t* GetPixelData() { return m_pixelData.get(); }
    const uint8_t* GetPixelData() const { return m_pixelData.get(); }
    int GetStride() const { return m_stride; }
    PixelFormat GetPixelFormat() const { return m_format; }
    
    bool SaveToFile(const std::string& path, ImageFormat format);
    Color GetPixel(int x, int y) const;
    void SetPixel(int x, int y, const Color& color);
    
private:
    int m_width, m_height;
    int m_stride;
    double m_dpi = 96.0;
    PixelFormat m_format;
    std::unique_ptr<uint8_t[]> m_pixelData;
};
```

**第三方库依赖**:
- stb_image_write (图像导出)
- stb_image (图像加载)

**验收标准**:
- [ ] 支持RGBA8888、RGB888、Gray8像素格式
- [ ] 图像导出PNG/JPEG正确
- [ ] 内存管理无泄漏
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第1700-1900行

---

### T11: DisplayDevice实现

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现DisplayDevice显示设备 |
| **任务描述** | 实现DisplayDevice类，管理窗口显示表面。支持双缓冲、垂直同步、脏区域更新。 |
| **依赖关系** | T1 (DrawDevice基类) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/display_device.h`
- `src/draw/display_device.cpp`

**类相关接口**:
```cpp
class DisplayDevice : public DrawDevice {
public:
    explicit DisplayDevice(void* nativeWindow);
    ~DisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kDisplay; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    
    void Present();
    void SetVSync(bool enabled);
    bool IsVSyncEnabled() const;
    
    void InvalidateRect(const Rect& rect);
    void InvalidateAll();
    std::vector<Rect> GetDirtyRects() const;
    
    void* GetNativeWindow() const { return m_nativeWindow; }
    
private:
    void* m_nativeWindow = nullptr;
    bool m_vsync = true;
    std::vector<Rect> m_dirtyRects;
    std::unique_ptr<uint8_t[]> m_backBuffer;
};
```

**第三方库依赖**: 平台相关（Windows: GDI, Linux: X11, macOS: Cocoa）

**验收标准**:
- [ ] 双缓冲正确实现
- [ ] 脏区域更新有效
- [ ] 跨平台窗口适配
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第1900-2000行

---

### T12: Simple2DEngine基类

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现Simple2DEngine CPU渲染引擎基类 |
| **任务描述** | 实现Simple2DEngine类，提供CPU端2D渲染基础功能。包含软件光栅化算法、坐标变换、裁剪等核心功能。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/simple2d_engine.h`
- `src/draw/simple2d_engine.cpp`

**类相关接口**:
```cpp
class Simple2DEngine : public DrawEngine {
public:
    explicit Simple2DEngine(DrawDevice* device);
    ~Simple2DEngine() override;
    
    std::string GetName() const override { return "Simple2DEngine"; }
    EngineType GetType() const override { return EngineType::kSimple2D; }
    
    bool Begin() override;
    void End() override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = StateFlag::kAll) override;
    void Restore() override;
    
protected:
    void RasterizeLine(int x1, int y1, int x2, int y2, const Color& color);
    void RasterizePolygon(const std::vector<Point>& points, const Color& fillColor, const Color& strokeColor);
    void FillScanline(int y, int x1, int x2, const Color& color);
    
    bool ClipLine(double& x1, double& y1, double& x2, double& y2) const;
    bool ClipPolygon(std::vector<Point>& points) const;
    
    DrawDevice* m_device = nullptr;
    TransformMatrix m_transform;
    Region m_clipRegion;
    std::stack<StateEntry> m_stateStack;
    uint8_t* m_framebuffer = nullptr;
    int m_width = 0, m_height = 0, m_stride = 0;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] Bresenham直线算法正确
- [ ] 多边形填充算法正确
- [ ] 坐标变换正确应用
- [ ] 裁剪功能正确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2000-2200行

---

### T13: Simple2DEngine点线绘制

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现Simple2DEngine点和线绘制功能 |
| **任务描述** | 实现DrawPoints和DrawLines接口，支持多种线型（实线、虚线）、线帽、线连接样式。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `src/draw/simple2d_engine_points.cpp`
- `src/draw/simple2d_engine_lines.cpp`

**类相关接口**:
```cpp
DrawResult Simple2DEngine::DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
DrawResult Simple2DEngine::DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;

// 内部辅助方法
void DrawPointMarker(int x, int y, PointMarkerType type, double size, const Color& color);
void DrawDashedLine(int x1, int y1, int x2, int y2, const std::vector<double>& dashPattern, const Color& color);
void ApplyLineCap(int x, int y, double angle, LineCap cap, double width, const Color& color);
void ApplyLineJoin(int x, int y, double angle1, double angle2, LineJoin join, double width, const Color& color);
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 点绘制支持圆形、方形、三角形标记
- [ ] 线绘制支持实线、虚线、点线
- [ ] 线帽样式正确（Flat、Round、Square）
- [ ] 线连接样式正确（Miter、Round、Bevel）
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2200-2400行

---

### T14: Simple2DEngine多边形绘制

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现Simple2DEngine多边形绘制功能 |
| **任务描述** | 实现DrawPolygon接口，支持填充和描边多边形。支持填充规则（EvenOdd、NonZero）、抗锯齿。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `src/draw/simple2d_engine_polygon.cpp`

**类相关接口**:
```cpp
DrawResult Simple2DEngine::DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;

// 内部辅助方法
void FillPolygon(const std::vector<Point>& points, const Color& color, FillRule rule);
void StrokePolygon(const std::vector<Point>& points, const Pen& pen);
void ScanlineFill(const std::vector<Point>& points, int minY, int maxY, const Color& color);
int ComputeWindingNumber(const Point& point, const std::vector<Point>& polygon);
void ApplyAntiAliasing(int x, int y, const Color& color, double coverage);
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 多边形填充正确
- [ ] EvenOdd和NonZero填充规则正确
- [ ] 凹多边形正确处理
- [ ] 抗锯齿效果良好
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2400-2600行

---

### T15: Simple2DEngine文本渲染

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现Simple2DEngine文本渲染功能 |
| **任务描述** | 实现DrawText接口，支持TrueType字体渲染。支持字体大小、粗细、斜体样式。支持文本测量。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `src/draw/simple2d_engine_text.cpp`
- `src/draw/font_manager.cpp`

**类相关接口**:
```cpp
DrawResult Simple2DEngine::DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;

class FontManager {
public:
    static FontManager& Instance();
    
    FontHandle LoadFont(const std::string& family, const Font& font);
    void UnloadFont(FontHandle handle);
    
    void RenderText(const std::string& text, FontHandle font, const Color& color, uint8_t* buffer, int width, int height);
    TextMetrics MeasureText(const std::string& text, FontHandle font);
    
private:
    std::unordered_map<std::string, FontCache> m_fontCache;
};

struct TextMetrics {
    double width;
    double height;
    double ascent;
    double descent;
    double lineHeight;
};
```

**第三方库依赖**:
- FreeType >= 2.12.0 (字体渲染)

**验收标准**:
- [ ] 支持系统字体加载
- [ ] 支持字体大小、粗细、斜体
- [ ] 文本测量准确
- [ ] 支持Unicode文本
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第2600-2800行

---

### T16: Simple2DEngine图像绘制

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现Simple2DEngine图像绘制功能 |
| **任务描述** | 实现DrawImage接口，支持图像缩放、旋转、透明度混合。支持多种图像格式加载。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `src/draw/simple2d_engine_image.cpp`
- `src/draw/image_loader.cpp`

**类相关接口**:
```cpp
DrawResult Simple2DEngine::DrawImage(double x, double y, const Image& image, double scaleX = 1.0, double scaleY = 1.0) override;

class ImageLoader {
public:
    static std::unique_ptr<Image> LoadFromFile(const std::string& path);
    static std::unique_ptr<Image> LoadFromMemory(const uint8_t* data, size_t size);
    static bool SaveToFile(const Image& image, const std::string& path, ImageFormat format);
};

struct Image {
    int width = 0;
    int height = 0;
    int channels = 4;
    std::unique_ptr<uint8_t[]> pixelData;
    
    Color GetPixel(int x, int y) const;
    void SetPixel(int x, int y, const Color& color);
    void Resize(int newWidth, int newHeight);
    void Rotate(double angle);
};
```

**第三方库依赖**:
- stb_image >= 2.27 (图像加载)
- stb_image_resize >= 2.05 (图像缩放)

**验收标准**:
- [ ] 支持PNG、JPEG、BMP、TIFF格式
- [ ] 图像缩放质量良好
- [ ] 透明度混合正确
- [ ] 内存管理无泄漏
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第2800-3000行


---

## 5. M3 GPU加速任务详情

### T17: GPUResourceManager

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现GPU资源管理器 |
| **任务描述** | 实现GPUResourceManager类，管理GPU资源（缓冲区、纹理、着色器、帧缓冲）的创建、销毁和生命周期。支持资源统计和垃圾回收。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/gpu_resource_manager.h`
- `src/draw/gpu_resource_manager.cpp`

**类相关接口**:
```cpp
class GPUResourceManager {
public:
    static GPUResourceManager& Instance();
    
    VertexBufferResource CreateVertexBuffer(size_t size, const void* data = nullptr);
    IndexBufferResource CreateIndexBuffer(size_t size, const void* data = nullptr);
    TextureResource CreateTexture2D(int width, int height, TextureFormat format);
    ShaderResource CreateShader(ShaderType type, const std::string& source);
    FramebufferResource CreateFramebuffer(int width, int height);
    
    void GarbageCollect();
    void ReleaseAll();
    
    size_t GetTotalMemoryUsage() const;
    size_t GetResourceCount() const;
    
private:
    std::atomic<size_t> m_totalMemory{0};
    std::atomic<size_t> m_resourceCount{0};
    std::mutex m_mutex;
};

template<typename HandleType, typename Deleter>
class GPUResource {
public:
    GPUResource() = default;
    explicit GPUResource(HandleType handle);
    
    GPUResource(const GPUResource&) = delete;
    GPUResource& operator=(const GPUResource&) = delete;
    GPUResource(GPUResource&& other) noexcept;
    GPUResource& operator=(GPUResource&& other) noexcept;
    
    ~GPUResource() { Reset(); }
    
    HandleType Get() const;
    bool IsValid() const;
    void Reset();
    HandleType Release();
};
```

**第三方库依赖**:
- OpenGL >= 3.3

**验收标准**:
- [ ] GPU资源正确创建和销毁
- [ ] RAII包装无资源泄漏
- [ ] 内存统计准确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2000-2200行 (GPU资源RAII包装)

---

### T18: RenderMemoryPool

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现渲染内存池 |
| **任务描述** | 实现RenderMemoryPool类，管理GPU缓冲区的分配和回收。支持内存池化、碎片整理、动态扩容。 |
| **依赖关系** | T17 (GPUResourceManager) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/render_memory_pool.h`
- `src/draw/render_memory_pool.cpp`

**类相关接口**:
```cpp
struct BufferDesc {
    size_t size = 0;
    BufferType type = BufferType::kVertex;
    MemoryUsage usage = MemoryUsage::kDynamic;
};

class RenderMemoryPool {
public:
    static RenderMemoryPool& Instance();
    
    BufferHandle AllocateVertexBuffer(size_t size);
    BufferHandle AllocateIndexBuffer(size_t size);
    BufferHandle AllocateUniformBuffer(size_t size);
    
    void Recycle(BufferHandle handle);
    void RecycleAll();
    
    void Compact();
    void Defragment();
    
    size_t GetMemoryUsage() const;
    size_t GetPoolCapacity() const;
    float GetFragmentationRatio() const;
    
    void SetMaxPoolSize(size_t maxSize);
    void SetGrowthFactor(float factor);

private:
    std::map<BufferType, std::vector<BufferBlock>> m_freeBlocks;
    std::unordered_map<BufferHandle, BufferBlock> m_allocatedBlocks;
    size_t m_totalAllocated = 0;
    size_t m_maxPoolSize = 512 * 1024 * 1024;  // 512MB
    float m_growthFactor = 1.5f;
    std::mutex m_mutex;
};
```

**第三方库依赖**:
- OpenGL >= 3.3

**验收标准**:
- [ ] 缓冲区分配和回收正确
- [ ] 碎片整理有效
- [ ] 内存池自动扩容
- [ ] 碎片率统计准确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2200-2400行 (内存池设计)

---

### T19: TextureCache纹理缓存

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现纹理缓存系统 |
| **任务描述** | 实现TextureCache类，管理纹理资源的缓存和淘汰。支持LRU淘汰策略、纹理预加载、内存限制。 |
| **依赖关系** | T17 (GPUResourceManager) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/texture_cache.h`
- `src/draw/texture_cache.cpp`

**类相关接口**:
```cpp
struct TextureDesc {
    int width = 0;
    int height = 0;
    int channels = 4;
    TextureFormat format = TextureFormat::kRGBA8;
    TextureFilter filter = TextureFilter::kLinear;
};

class TextureCache {
public:
    static TextureCache& Instance();
    
    TextureHandle GetOrCreate(const std::string& key, const TextureDesc& desc);
    TextureHandle GetOrCreateFromFile(const std::string& filePath);
    TextureHandle GetOrCreateFromData(const std::string& key, const uint8_t* data, size_t size, const TextureDesc& desc);
    
    void Release(TextureHandle handle);
    void Release(const std::string& key);
    void ReleaseAll();
    
    void SetMaxCacheSize(size_t maxSize);
    size_t GetCacheSize() const;
    size_t GetTextureCount() const;
    
    void SetEvictionPolicy(EvictionPolicy policy);
    void EvictLRU(size_t targetSize);

private:
    std::unordered_map<std::string, TextureEntry> m_cache;
    std::list<std::string> m_lruList;
    size_t m_maxCacheSize = 256 * 1024 * 1024;  // 256MB
    size_t m_currentSize = 0;
    std::mutex m_mutex;
};
```

**第三方库依赖**:
- OpenGL >= 3.3
- stb_image >= 2.27

**验收标准**:
- [ ] 纹理缓存命中和未命中正确处理
- [ ] LRU淘汰策略正确
- [ ] 内存限制有效
- [ ] 线程安全
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2400-2600行 (纹理缓存)

---

### T20: GPUAcceleratedEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现GPU加速渲染引擎 |
| **任务描述** | 实现GPUAcceleratedEngine类，利用GPU进行硬件加速渲染。支持顶点缓冲、着色器程序、批处理渲染。 |
| **依赖关系** | T17 (GPUResourceManager), T18 (RenderMemoryPool) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 14h (乐观10h, 悲观20h) |

**产出文件**:
- `include/ogc/draw/gpu_accelerated_engine.h`
- `src/draw/gpu_accelerated_engine.cpp`
- `shaders/basic_vertex.glsl`
- `shaders/basic_fragment.glsl`
- `shaders/line_vertex.glsl`
- `shaders/line_fragment.glsl`

**类相关接口**:
```cpp
class GPUAcceleratedEngine : public DrawEngine {
public:
    explicit GPUAcceleratedEngine(DrawDevice* device);
    ~GPUAcceleratedEngine() override;
    
    std::string GetName() const override { return "GPUAcceleratedEngine"; }
    EngineType GetType() const override { return EngineType::kGPU; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = StateFlag::kAll) override;
    void Restore() override;
    
private:
    void FlushBatch();
    void SetupShader(ShaderProgram* shader);
    void UpdateUniforms();
    
    DrawDevice* m_device = nullptr;
    ShaderProgram* m_currentShader = nullptr;
    VertexBuffer* m_vertexBuffer = nullptr;
    IndexBuffer* m_indexBuffer = nullptr;
    std::vector<DrawBatch> m_batches;
    TransformMatrix m_transform;
    Region m_clipRegion;
};
```

**第三方库依赖**:
- OpenGL >= 3.3
- GLM (数学库，可选)

**验收标准**:
- [ ] GPU渲染管线正确
- [ ] 着色器编译和链接成功
- [ ] 批处理渲染有效
- [ ] 性能优于CPU渲染
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第2600-2900行 (GPU加速引擎)

---

### T21: GPU资源RAII包装

| 属性 | 内容 |
|------|------|
| **任务摘要** | 完善GPU资源RAII包装类 |
| **任务描述** | 实现完整的GPU资源RAII包装，包括VertexBufferResource、IndexBufferResource、TextureResource、ShaderResource、FramebufferResource。确保异常安全。 |
| **依赖关系** | T17 (GPUResourceManager) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/gpu_resource.h`
- `src/draw/gpu_resource.cpp`

**类相关接口**:
```cpp
struct VertexBufferDeleter {
    void operator()(GLuint handle) { glDeleteBuffers(1, &handle); }
};

struct IndexBufferDeleter {
    void operator()(GLuint handle) { glDeleteBuffers(1, &handle); }
};

struct TextureDeleter {
    void operator()(GLuint handle) { glDeleteTextures(1, &handle); }
};

struct ShaderDeleter {
    void operator()(GLuint handle) { glDeleteShader(handle); }
};

struct FramebufferDeleter {
    void operator()(GLuint handle) { glDeleteFramebuffers(1, &handle); }
};

using VertexBufferResource = GPUResource<GLuint, VertexBufferDeleter>;
using IndexBufferResource = GPUResource<GLuint, IndexBufferDeleter>;
using TextureResource = GPUResource<GLuint, TextureDeleter>;
using ShaderResource = GPUResource<GLuint, ShaderDeleter>;
using FramebufferResource = GPUResource<GLuint, FramebufferDeleter>;
```

**第三方库依赖**:
- OpenGL >= 3.3

**验收标准**:
- [ ] 所有资源类型包装完整
- [ ] 移动语义正确
- [ ] 异常安全（强异常保证）
- [ ] 无资源泄漏
- [ ] 单元测试覆盖率 > 90%

**参考文档**: render_category_design.md 第2000-2200行

---

### T22: 多GPU设备选择

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现多GPU设备选择策略 |
| **任务描述** | 实现GPUDeviceSelector类，支持枚举系统GPU设备、计算GPU评分、选择最优GPU。支持偏好设置（自动、独立显卡、集成显卡、高内存、低功耗）。 |
| **依赖关系** | T17 (GPUResourceManager) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P2 (一般) |
| **负责人** | Dev A |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/gpu_device_selector.h`
- `src/draw/gpu_device_selector.cpp`

**类相关接口**:
```cpp
struct GPUDeviceInfo {
    int index = -1;
    std::string name;
    std::string vendor;
    size_t dedicatedMemory = 0;
    size_t sharedMemory = 0;
    int computeUnits = 0;
    int clockFrequency = 0;
    bool isIntegrated = false;
    bool isDiscrete = false;
    bool supportsCompute = false;
    bool supportsRayTracing = false;
};

enum class GPUPreference {
    kAuto,
    kDiscrete,
    kIntegrated,
    kHighMemory,
    kLowPower
};

class GPUDeviceSelector {
public:
    static std::vector<GPUDeviceInfo> EnumerateGPUs();
    static GPUDeviceInfo SelectBestGPU(GPUPreference preference = GPUPreference::kAuto);
    static int CalculateGPUScore(const GPUDeviceInfo& gpu);
    
private:
    static GPUDeviceInfo SelectByScore(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectDiscrete(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectIntegrated(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectHighMemory(const std::vector<GPUDeviceInfo>& gpus);
    static GPUDeviceInfo SelectLowPower(const std::vector<GPUDeviceInfo>& gpus);
};
```

**第三方库依赖**:
- OpenGL >= 3.3
- 平台相关API (Windows: DXGI, Linux: EGL, macOS: Metal)

**验收标准**:
- [ ] GPU枚举正确
- [ ] GPU评分算法合理
- [ ] 偏好选择正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第2900-3100行 (多GPU设备选择)


---

## 6. M4 瓦片渲染任务详情

### T23: TileDevice瓦片设备

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现TileDevice瓦片渲染设备 |
| **任务描述** | 实现TileDevice类，管理大尺寸图像的瓦片化渲染。支持瓦片大小配置、重叠区域设置、瓦片索引管理。 |
| **依赖关系** | T1 (DrawDevice基类) |
| **里程碑** | M4 瓦片渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/tile_device.h`
- `src/draw/tile_device.cpp`

**类相关接口**:
```cpp
struct TileInfo {
    int indexX = 0;
    int indexY = 0;
    int pixelX = 0;
    int pixelY = 0;
    int width = 256;
    int height = 256;
    bool isValid = false;
    bool isDirty = true;
};

class TileDevice : public DrawDevice {
public:
    TileDevice(int totalWidth, int totalHeight, int tileSize = 256);
    ~TileDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kTile; }
    int GetWidth() const override { return m_totalWidth; }
    int GetHeight() const override { return m_totalHeight; }
    double GetDpi() const override { return m_dpi; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kTile; }
    
    int GetTileSize() const { return m_tileSize; }
    int GetTileCountX() const { return m_tileCountX; }
    int GetTileCountY() const { return m_tileCountY; }
    int GetTotalTileCount() const { return m_tileCountX * m_tileCountY; }
    
    TileInfo GetTile(int indexX, int indexY) const;
    std::vector<TileInfo> GetTilesInRect(const Rect& rect) const;
    std::vector<TileInfo> GetDirtyTiles() const;
    void MarkTileDirty(int indexX, int indexY);
    void MarkAllDirty();
    
    void SetTileOverlap(int overlap) { m_tileOverlap = overlap; }
    int GetTileOverlap() const { return m_tileOverlap; }
    
private:
    int m_totalWidth, m_totalHeight;
    int m_tileSize = 256;
    int m_tileOverlap = 0;
    int m_tileCountX, m_tileCountY;
    double m_dpi = 96.0;
    std::vector<std::vector<TileInfo>> m_tiles;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 瓦片索引计算正确
- [ ] 瓦片边界处理正确
- [ ] 脏区域管理正确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第3100-3300行

---

### T24: TileBasedEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现TileBasedEngine瓦片渲染引擎 |
| **任务描述** | 实现TileBasedEngine类，支持瓦片化渲染。自动计算涉及瓦片、分瓦片渲染、合并结果。支持瓦片间几何裁剪。 |
| **依赖关系** | T23 (TileDevice), T12 (Simple2DEngine基类) |
| **里程碑** | M4 瓦片渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 12h (乐观10h, 悲观16h) |

**产出文件**:
- `include/ogc/draw/tile_based_engine.h`
- `src/draw/tile_based_engine.cpp`

**类相关接口**:
```cpp
class TileBasedEngine : public DrawEngine {
public:
    explicit TileBasedEngine(TileDevice* device);
    ~TileBasedEngine() override;
    
    std::string GetName() const override { return "TileBasedEngine"; }
    EngineType GetType() const override { return EngineType::kTile; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = StateFlag::kAll) override;
    void Restore() override;
    
    void SetProgressCallback(ProgressCallback callback);
    float GetProgress() const;
    
private:
    void BeginTile(const TileInfo& tile);
    void EndTile(const TileInfo& tile);
    std::vector<TileInfo> CalculateInvolvedTiles(const Rect& bounds);
    void ClipGeometryToTile(Geometry* geometry, const TileInfo& tile);
    
    TileDevice* m_device = nullptr;
    std::unique_ptr<Simple2DEngine> m_tileEngine;
    std::unique_ptr<RasterImageDevice> m_tileDevice;
    ProgressCallback m_progressCallback;
    float m_progress = 0.0f;
    int m_completedTiles = 0;
    int m_totalTiles = 0;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 瓦片渲染正确
- [ ] 瓦片间无缝拼接
- [ ] 几何裁剪正确
- [ ] 进度回调正确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第3300-3500行

---

### T25: AsyncRenderTask

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现AsyncRenderTask异步渲染任务 |
| **任务描述** | 实现AsyncRenderTask类，封装异步渲染任务。支持进度查询、取消、等待、回调通知。 |
| **依赖关系** | T24 (TileBasedEngine) |
| **里程碑** | M4 瓦片渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/async_render_task.h`
- `src/draw/async_render_task.cpp`

**类相关接口**:
```cpp
enum class RenderState {
    kPending = 0,
    kRunning = 1,
    kCompleted = 2,
    kFailed = 3,
    kCancelled = 4
};

struct RenderResult {
    RenderState state = RenderState::kPending;
    DrawResult drawResult = DrawResult::kSuccess;
    std::string errorMessage;
    double elapsedMs = 0.0;
    size_t geometryCount = 0;
};

class AsyncRenderTask {
public:
    using ProgressCallback = std::function<void(float progress, const std::string& stage)>;
    using CompletionCallback = std::function<void(const RenderResult& result)>;
    
    virtual ~AsyncRenderTask() = default;
    
    virtual RenderState GetState() const = 0;
    virtual float GetProgress() const = 0;
    virtual std::string GetCurrentStage() const = 0;
    
    virtual void SetProgressCallback(ProgressCallback callback) = 0;
    virtual void SetCompletionCallback(CompletionCallback callback) = 0;
    
    virtual void Cancel() = 0;
    virtual bool Wait(uint32_t timeoutMs = 0) = 0;
    
    virtual RenderResult GetResult() const = 0;
};

class TileAsyncRenderTask : public AsyncRenderTask {
public:
    TileAsyncRenderTask(TileDevice* device, TileBasedEngine* engine, const std::vector<Geometry*>& geometries, const DrawStyle& style);
    ~TileAsyncRenderTask() override;
    
    RenderState GetState() const override;
    float GetProgress() const override;
    std::string GetCurrentStage() const override;
    
    void SetProgressCallback(ProgressCallback callback) override;
    void SetCompletionCallback(CompletionCallback callback) override;
    
    void Cancel() override;
    bool Wait(uint32_t timeoutMs = 0) override;
    
    RenderResult GetResult() const override;
    
    void Execute();

private:
    TileDevice* m_device = nullptr;
    TileBasedEngine* m_engine = nullptr;
    std::vector<Geometry*> m_geometries;
    DrawStyle m_style;
    
    std::atomic<RenderState> m_state{RenderState::kPending};
    std::atomic<float> m_progress{0.0f};
    std::atomic<bool> m_cancelled{false};
    std::string m_currentStage;
    RenderResult m_result;
    
    ProgressCallback m_progressCallback;
    CompletionCallback m_completionCallback;
    std::mutex m_mutex;
    std::condition_variable m_cv;
};
```

**第三方库依赖**:
- C++11 <thread>, <mutex>, <condition_variable>, <atomic>

**验收标准**:
- [ ] 异步执行正确
- [ ] 取消功能有效
- [ ] 等待超时正确
- [ ] 回调通知正确
- [ ] 线程安全
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2000-2200行 (异步渲染设计)

---

### T26: AsyncRenderManager

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现AsyncRenderManager异步渲染管理器 |
| **任务描述** | 实现AsyncRenderManager类，管理异步渲染任务队列。支持并发任务限制、任务优先级、任务调度。 |
| **依赖关系** | T25 (AsyncRenderTask) |
| **里程碑** | M4 瓦片渲染 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/async_render_manager.h`
- `src/draw/async_render_manager.cpp`

**类相关接口**:
```cpp
class AsyncRenderManager {
public:
    static AsyncRenderManager& Instance();
    
    std::shared_ptr<AsyncRenderTask> SubmitRenderTask(
        DrawDevice* device,
        DrawEngine* engine,
        const std::vector<Geometry*>& geometries,
        const DrawStyle& style,
        TaskPriority priority = TaskPriority::kNormal
    );
    
    void SetMaxConcurrentTasks(int maxTasks);
    int GetActiveTaskCount() const;
    int GetPendingTaskCount() const;
    
    void CancelAllTasks();
    void WaitForAll(uint32_t timeoutMs = 0);
    
    void Pause();
    void Resume();
    bool IsPaused() const;
    
    void Shutdown();

private:
    AsyncRenderManager();
    ~AsyncRenderManager();
    
    void WorkerThread();
    void ProcessTask(std::shared_ptr<AsyncRenderTask> task);
    
    std::priority_queue<TaskQueueEntry> m_pendingQueue;
    std::set<std::shared_ptr<AsyncRenderTask>> m_activeTasks;
    std::vector<std::thread> m_workers;
    
    std::mutex m_mutex;
    std::condition_variable m_cv;
    
    int m_maxConcurrentTasks = 4;
    bool m_paused = false;
    bool m_shutdown = false;
};

enum class TaskPriority {
    kLow = 0,
    kNormal = 1,
    kHigh = 2,
    kUrgent = 3
};
```

**第三方库依赖**:
- C++11 <thread>, <mutex>, <condition_variable>

**验收标准**:
- [ ] 任务队列正确管理
- [ ] 并发限制有效
- [ ] 优先级调度正确
- [ ] 线程池正确工作
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第2200-2400行 (异步渲染管理器)

---

### T27: 瓦片大小动态策略

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现瓦片大小动态计算策略 |
| **任务描述** | 实现TileSizeStrategy类，根据图像大小、内存限制、GPU能力动态计算最优瓦片大小。支持不同场景的策略选择。 |
| **依赖关系** | T24 (TileBasedEngine) |
| **里程碑** | M4 瓦片渲染 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/tile_size_strategy.h`
- `src/draw/tile_size_strategy.cpp`

**类相关接口**:
```cpp
struct TileConfig {
    int tileSize = 256;
    int overlap = 0;
    int maxTilesInMemory = 100;
};

enum class TileStrategy {
    kFixed,         // 固定大小
    kAdaptive,      // 自适应
    kMemoryAware,   // 内存感知
    kGPUCapability  // GPU能力感知
};

class TileSizeStrategy {
public:
    static TileConfig CalculateOptimalConfig(
        int totalWidth,
        int totalHeight,
        TileStrategy strategy = TileStrategy::kAdaptive,
        size_t availableMemory = 0
    );
    
    static TileConfig GetFixedConfig(int tileSize);
    static TileConfig GetAdaptiveConfig(int totalWidth, int totalHeight);
    static TileConfig GetMemoryAwareConfig(int totalWidth, int totalHeight, size_t availableMemory);
    static TileConfig GetGPUCapabilityConfig(int totalWidth, int totalHeight);
    
    static int CalculateTileCount(int totalSize, int tileSize);
    static int CalculateOptimalTileSize(int totalWidth, int totalHeight, int targetTileCount);
    
private:
    static size_t EstimateMemoryUsage(int tileSize, int channels = 4);
    static int GetMaxTextureSize();
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 瓦片大小计算合理
- [ ] 内存限制有效
- [ ] GPU能力检测正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3500-3700行


---

## 7. M5 矢量输出任务详情

### T28: VectorEngine基类

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现VectorEngine矢量渲染引擎基类 |
| **任务描述** | 实现VectorEngine基类，继承自DrawEngine，提供矢量格式渲染的通用功能。包含路径构建、矢量状态管理、坐标变换等。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M5 矢量输出 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/vector_engine.h`
- `src/draw/vector_engine.cpp`

**类相关接口**:
```cpp
class VectorEngine : public DrawEngine {
public:
    explicit VectorEngine(DrawDevice* device);
    ~VectorEngine() override;
    
    EngineType GetType() const override { return EngineType::kVector; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = StateFlag::kAll) override;
    void Restore() override;
    
protected:
    virtual void WritePath(const std::vector<Point>& points, bool closed) = 0;
    virtual void WriteFill(const Color& color, FillRule rule) = 0;
    virtual void WriteStroke(const Pen& pen) = 0;
    virtual void WriteText(double x, double y, const std::string& text, const Font& font, const Color& color) = 0;
    virtual void WriteImage(double x, double y, const Image& image, double scaleX, double scaleY) = 0;
    
    void BuildPath(const double* x, const double* y, int count);
    void ApplyTransform();
    
    DrawDevice* m_device = nullptr;
    TransformMatrix m_transform;
    Region m_clipRegion;
    std::stack<VectorStateEntry> m_stateStack;
    std::vector<Point> m_currentPath;
    DrawStyle m_currentStyle;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] VectorEngine基类编译通过
- [ ] 路径构建正确
- [ ] 状态管理正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3700-3900行 (VectorEngine继承系统)

---

### T29: PdfDevice实现

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现PdfDevice PDF输出设备 |
| **任务描述** | 实现PdfDevice类，管理PDF文档的创建和输出。支持多页PDF、页面大小设置、元数据设置。 |
| **依赖关系** | T1 (DrawDevice基类) |
| **里程碑** | M5 矢量输出 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/pdf_device.h`
- `src/draw/pdf_device.cpp`

**类相关接口**:
```cpp
class PdfDevice : public DrawDevice {
public:
    PdfDevice(const std::string& filePath, double width, double height);
    PdfDevice(const std::string& filePath, const std::string& pageSize = "A4");
    ~PdfDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kPdf; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override { return 72.0; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kVector; }
    
    void NewPage(double width, double height);
    void NewPage(const std::string& pageSize);
    int GetPageCount() const;
    
    void SetTitle(const std::string& title);
    void SetAuthor(const std::string& author);
    void SetSubject(const std::string& subject);
    void SetCreator(const std::string& creator);
    
    bool Save();
    std::string GetFilePath() const { return m_filePath; }
    
private:
    std::string m_filePath;
    double m_width, m_height;
    int m_pageCount = 0;
    
    void* m_pdfDoc = nullptr;  // podofo::PdfMemDocument*
    void* m_currentPage = nullptr;
    
    static std::map<std::string, std::pair<double, double>> s_pageSizes;
};
```

**第三方库依赖**:
- podofo >= 0.10.0 (PDF生成库)
- DRAW_HAS_PODOFO宏保护

**验收标准**:
- [ ] PDF文件正确生成
- [ ] 多页PDF支持
- [ ] 元数据正确写入
- [ ] 内存管理无泄漏
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3900-4100行

---

### T30: PdfEngine实现(podofo)

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现PdfEngine PDF渲染引擎 |
| **任务描述** | 实现PdfEngine类，继承自VectorEngine，使用podofo库进行PDF矢量渲染。支持PDF特有的功能（链接、注释、图层等）。 |
| **依赖关系** | T28 (VectorEngine基类), T29 (PdfDevice) |
| **里程碑** | M5 矢量输出 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 8h (乐观6h, 悲观12h) |

**产出文件**:
- `include/ogc/draw/pdf_engine.h`
- `src/draw/pdf_engine.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_PODOFO

class PdfEngine : public VectorEngine {
public:
    explicit PdfEngine(PdfDevice* device);
    ~PdfEngine() override;
    
    std::string GetName() const override { return "PdfEngine"; }
    
    bool Begin() override;
    void End() override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    // PDF特有功能
    void AddLink(double x, double y, double w, double h, const std::string& url);
    void AddBookmark(const std::string& title, int level = 0);
    void SetLayer(const std::string& layerName);
    
protected:
    void WritePath(const std::vector<Point>& points, bool closed) override;
    void WriteFill(const Color& color, FillRule rule) override;
    void WriteStroke(const Pen& pen) override;
    void WriteText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    void WriteImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
private:
    void SetupPage();
    void ApplyCurrentStyle();
    void EmbedFont(const Font& font);
    
    PdfDevice* m_device = nullptr;
    void* m_painter = nullptr;  // podofo::PdfPainter*
    void* m_page = nullptr;     // podofo::PdfPage*
};

#endif
```

**第三方库依赖**:
- podofo >= 0.10.0
- FreeType >= 2.12.0 (字体嵌入)

**验收标准**:
- [ ] PDF矢量图形正确渲染
- [ ] 文本和字体正确嵌入
- [ ] 图像正确嵌入
- [ ] 链接和书签功能正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第4100-4300行

---

### T31: SvgDevice实现

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现SvgDevice SVG输出设备 |
| **任务描述** | 实现SvgDevice类，管理SVG文档的创建和输出。支持SVG版本选择、压缩输出。 |
| **依赖关系** | T1 (DrawDevice基类) |
| **里程碑** | M5 矢量输出 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/svg_device.h`
- `src/draw/svg_device.cpp`

**类相关接口**:
```cpp
class SvgDevice : public DrawDevice {
public:
    enum class SvgVersion {
        k1_1,   // SVG 1.1
        k1_2,   // SVG 1.2 Tiny
        k2_0    // SVG 2.0
    };
    
    SvgDevice(const std::string& filePath, double width, double height);
    ~SvgDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kSvg; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override { return 96.0; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kVector; }
    
    void SetSvgVersion(SvgVersion version);
    void SetTitle(const std::string& title);
    void SetDescription(const std::string& desc);
    
    bool Save();
    bool SaveCompressed();  // SVGZ
    
    std::string GetFilePath() const { return m_filePath; }
    std::string GetSvgContent() const;

private:
    std::string m_filePath;
    double m_width, m_height;
    SvgVersion m_version = SvgVersion::k1_1;
    
    std::stringstream m_content;
    int m_indentLevel = 0;
    
    void WriteHeader();
    void WriteFooter();
    void WriteIndent();
};
```

**第三方库依赖**:
- zlib (SVGZ压缩，可选)

**验收标准**:
- [ ] SVG文件正确生成
- [ ] SVG版本兼容
- [ ] SVGZ压缩正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第4300-4500行

---

### T32: SvgEngine实现(cairo)

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现SvgEngine SVG渲染引擎 |
| **任务描述** | 实现SvgEngine类，继承自VectorEngine，使用cairo库进行SVG矢量渲染。支持SVG特有功能（渐变、滤镜、动画）。 |
| **依赖关系** | T28 (VectorEngine基类), T31 (SvgDevice) |
| **里程碑** | M5 矢量输出 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/svg_engine.h`
- `src/draw/svg_engine.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_CAIRO

class SvgEngine : public VectorEngine {
public:
    explicit SvgEngine(SvgDevice* device);
    ~SvgEngine() override;
    
    std::string GetName() const override { return "SvgEngine"; }
    
    bool Begin() override;
    void End() override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    // SVG特有功能
    void AddGradient(const std::string& id, const Gradient& gradient);
    void AddFilter(const std::string& id, const Filter& filter);
    void AddAnimation(const std::string& targetId, const Animation& animation);
    void SetElementId(const std::string& id);
    
protected:
    void WritePath(const std::vector<Point>& points, bool closed) override;
    void WriteFill(const Color& color, FillRule rule) override;
    void WriteStroke(const Pen& pen) override;
    void WriteText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    void WriteImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
private:
    void ApplyCurrentStyle();
    std::string PathToSvgPath(const std::vector<Point>& points, bool closed);
    std::string ColorToSvg(const Color& color);
    
    SvgDevice* m_device = nullptr;
    cairo_t* m_cr = nullptr;
    cairo_surface_t* m_surface = nullptr;
};

#endif
```

**第三方库依赖**:
- cairo >= 1.18.0
- DRAW_HAS_CAIRO宏保护

**验收标准**:
- [ ] SVG矢量图形正确渲染
- [ ] 渐变和滤镜支持
- [ ] 文本正确渲染
- [ ] 图像正确嵌入
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第4500-4700行

---

### T33: 矢量输出测试

| 属性 | 内容 |
|------|------|
| **任务摘要** | 完成矢量输出功能的集成测试 |
| **任务描述** | 编写PDF和SVG输出的集成测试用例，验证输出文件的正确性和兼容性。 |
| **依赖关系** | T30 (PdfEngine), T32 (SvgEngine) |
| **里程碑** | M5 矢量输出 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `tests/draw/test_pdf_output.cpp`
- `tests/draw/test_svg_output.cpp`
- `tests/draw/test_data/vector_test_cases.h`

**类相关接口**: 无（测试文件）

**第三方库依赖**:
- GoogleTest >= 1.12.0

**验收标准**:
- [ ] PDF输出测试覆盖主要功能
- [ ] SVG输出测试覆盖主要功能
- [ ] 输出文件可被外部工具验证
- [ ] 测试用例覆盖边界情况

**参考文档**: render_category_design.md 第4700-4900行


---

## 8. M6 平台适配任务详情

### T34: QtEngine实现

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现QtEngine跨平台渲染引擎 |
| **任务描述** | 实现QtEngine类，继承自Simple2DEngine，使用QPainter进行跨平台渲染。支持Qt5和Qt6。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M6 平台适配 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 12h (乐观10h, 悲观16h) |

**产出文件**:
- `include/ogc/draw/qt_engine.h`
- `src/draw/qt_engine.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_QT

class QtEngine : public Simple2DEngine {
public:
    explicit QtEngine(DrawDevice* device);
    ~QtEngine() override;
    
    std::string GetName() const override { return "QtEngine"; }
    EngineType GetType() const override { return EngineType::kSimple2D; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    void Save(StateFlags flags = StateFlag::kAll) override;
    void Restore() override;
    
    QPainter* GetPainter() const { return m_painter; }

private:
    void ApplyStyle(const DrawStyle& style);
    QPen CreatePen(const DrawStyle& style);
    QBrush CreateBrush(const DrawStyle& style);
    QFont CreateFont(const Font& font);
    QColor ToQColor(const Color& color);
    
    QPainter* m_painter = nullptr;
    QPaintDevice* m_qtDevice = nullptr;
    QPen m_pen;
    QBrush m_brush;
    QFont m_font;
    std::stack<QPainter::State> m_stateStack;
};

#endif
```

**第三方库依赖**:
- Qt5 >= 5.12 或 Qt6 >= 6.5
- DRAW_HAS_QT宏保护

**验收标准**:
- [ ] Qt5和Qt6兼容
- [ ] 所有绘制功能正确
- [ ] 状态管理正确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第3000-3200行 (Qt跨平台实现)

---

### T35: QtDisplayDevice

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现QtDisplayDevice显示设备 |
| **任务描述** | 实现QtDisplayDevice类，支持QWidget、QImage、QPixmap等多种Qt绘图设备。 |
| **依赖关系** | T34 (QtEngine) |
| **里程碑** | M6 平台适配 |
| **优先级** | P0 (关键) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/qt_display_device.h`
- `src/draw/qt_display_device.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_QT

class QtDisplayDevice : public DrawDevice {
public:
    explicit QtDisplayDevice(QPaintDevice* device);
    explicit QtDisplayDevice(QWidget* widget);
    explicit QtDisplayDevice(const QSize& size, QImage::Format format = QImage::Format_ARGB32);
    ~QtDisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kDisplay; }
    int GetWidth() const override;
    int GetHeight() const override;
    double GetDpi() const override;
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kSimple2D; }
    
    QPaintDevice* GetPaintDevice() const { return m_paintDevice; }
    QImage* GetImage() { return m_image.get(); }
    QPixmap* GetPixmap() { return m_pixmap.get(); }
    
    bool SaveToFile(const std::string& path, const char* format = nullptr, int quality = -1);

private:
    QPaintDevice* m_paintDevice = nullptr;
    std::unique_ptr<QImage> m_image;
    std::unique_ptr<QPixmap> m_pixmap;
    QWidget* m_widget = nullptr;
    bool m_ownsDevice = false;
};

#endif
```

**第三方库依赖**:
- Qt5 >= 5.12 或 Qt6 >= 6.5
- DRAW_HAS_QT宏保护

**验收标准**:
- [ ] 支持QWidget/QImage/QPixmap
- [ ] 图像导出正确
- [ ] DPI计算正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3200-3400行

---

### T36: GDIPlusEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现GDIPlusEngine Windows渲染引擎 |
| **任务描述** | 实现GDIPlusEngine类，使用Windows GDI+进行渲染。作为Windows平台的兼容性后备方案。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/gdiplus_engine.h`
- `src/draw/gdiplus_engine.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_GDIPLUS

class GDIPlusEngine : public Simple2DEngine {
public:
    explicit GDIPlusEngine(DrawDevice* device);
    ~GDIPlusEngine() override;
    
    std::string GetName() const override { return "GDIPlusEngine"; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;

private:
    Gdiplus::Graphics* m_graphics = nullptr;
    std::unique_ptr<Gdiplus::Pen> m_pen;
    std::unique_ptr<Gdiplus::Brush> m_brush;
    std::unique_ptr<Gdiplus::Font> m_font;
    
    Gdiplus::Pen* CreatePen(const DrawStyle& style);
    Gdiplus::Brush* CreateBrush(const DrawStyle& style);
    Gdiplus::Font* CreateFont(const Font& font);
    Gdiplus::Color ToGdiColor(const Color& color);
};

#endif
```

**第三方库依赖**:
- Windows GDI+ (系统内置)
- DRAW_HAS_GDIPLUS宏保护

**验收标准**:
- [ ] Windows平台编译通过
- [ ] 所有绘制功能正确
- [ ] 内存管理无泄漏
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3400-3600行 (Windows平台)

---

### T37: Direct2DEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现Direct2DEngine Windows GPU渲染引擎 |
| **任务描述** | 实现Direct2DEngine类，继承自GPUAcceleratedEngine，使用Windows Direct2D进行GPU加速渲染。 |
| **依赖关系** | T17 (GPUResourceManager) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/direct2d_engine.h`
- `src/draw/direct2d_engine.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_DIRECT2D

class Direct2DEngine : public GPUAcceleratedEngine {
public:
    explicit Direct2DEngine(DrawDevice* device);
    ~Direct2DEngine() override;
    
    std::string GetName() const override { return "Direct2DEngine"; }
    
    bool Initialize() override;
    void Shutdown() override;
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;

private:
    bool CreateDeviceResources();
    void DiscardDeviceResources();
    
    ID2D1Factory* m_factory = nullptr;
    ID2D1HwndRenderTarget* m_renderTarget = nullptr;
    ID2D1SolidColorBrush* m_brush = nullptr;
    IDWriteFactory* m_writeFactory = nullptr;
    IDWriteTextFormat* m_textFormat = nullptr;
};

#endif
```

**第三方库依赖**:
- Windows Direct2D (系统内置)
- Windows DirectWrite (系统内置)
- DRAW_HAS_DIRECT2D宏保护

**验收标准**:
- [ ] Windows 7+平台编译通过
- [ ] GPU加速渲染正确
- [ ] 文本渲染高质量
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3600-3800行

---

### T38: Windows平台集成

| 属性 | 内容 |
|------|------|
| **任务摘要** | 完成Windows平台集成和测试 |
| **任务描述** | 整合GDIPlusEngine和Direct2DEngine，实现Windows平台自动选择最优引擎。编写平台集成测试。 |
| **依赖关系** | T36 (GDIPlusEngine), T37 (Direct2DEngine) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `src/draw/platform/windows/platform_init.cpp`
- `tests/draw/test_windows_platform.cpp`

**类相关接口**:
```cpp
namespace draw {
namespace windows {

class WindowsPlatform {
public:
    static std::unique_ptr<DrawEngine> CreateOptimalEngine(DrawDevice* device) {
#if DRAW_HAS_DIRECT2D
        if (IsDirect2DAvailable()) {
            auto engine = std::make_unique<Direct2DEngine>(device);
            if (engine->Initialize()) {
                return engine;
            }
        }
#endif
#if DRAW_HAS_GDIPLUS
        return std::make_unique<GDIPlusEngine>(device);
#endif
        return nullptr;
    }
    
    static bool IsDirect2DAvailable();
    static PlatformCapabilities GetCapabilities();
};

}
}
```

**第三方库依赖**: Windows SDK

**验收标准**:
- [ ] 引擎自动选择正确
- [ ] 平台能力检测正确
- [ ] 集成测试通过
- [ ] 性能测试达标

**参考文档**: render_category_design.md 第3800-4000行

---

### T39: CairoEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现CairoEngine Linux渲染引擎 |
| **任务描述** | 实现CairoEngine类，使用Cairo库进行Linux平台渲染。支持X11和Wayland显示后端。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/cairo_engine.h`
- `src/draw/cairo_engine.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_CAIRO

class CairoEngine : public Simple2DEngine {
public:
    explicit CairoEngine(DrawDevice* device);
    ~CairoEngine() override;
    
    std::string GetName() const override { return "CairoEngine"; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;

private:
    cairo_t* m_cr = nullptr;
    cairo_surface_t* m_surface = nullptr;
    
    void ApplyStyle(const DrawStyle& style);
    void SetSourceColor(const Color& color);
};

#endif
```

**第三方库依赖**:
- cairo >= 1.18.0
- pango >= 1.52.0 (文本渲染)
- DRAW_HAS_CAIRO宏保护

**验收标准**:
- [ ] Linux平台编译通过
- [ ] 所有绘制功能正确
- [ ] 文本渲染支持Unicode
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第4000-4200行 (Linux平台)

---

### T40: X11DisplayDevice

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现X11DisplayDevice X11显示设备 |
| **任务描述** | 实现X11DisplayDevice类，支持X11窗口渲染。支持双缓冲、XRender扩展。 |
| **依赖关系** | T39 (CairoEngine) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/x11_display_device.h`
- `src/draw/x11_display_device.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_X11

class X11DisplayDevice : public DrawDevice {
public:
    explicit X11DisplayDevice(Display* display, Window window);
    X11DisplayDevice(int width, int height);
    ~X11DisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kDisplay; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override;
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kSimple2D; }
    
    Display* GetDisplay() const { return m_display; }
    Window GetWindow() const { return m_window; }
    
    void Present();
    void SetVSync(bool enabled);

private:
    Display* m_display = nullptr;
    Window m_window = 0;
    int m_width = 0, m_height = 0;
    bool m_ownsDisplay = false;
};

#endif
```

**第三方库依赖**:
- X11
- XRender (可选)
- DRAW_HAS_X11宏保护

**验收标准**:
- [ ] X11窗口渲染正确
- [ ] 双缓冲有效
- [ ] 内存管理无泄漏
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第4200-4400行

---

### T41: CoreGraphicsEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现CoreGraphicsEngine macOS渲染引擎 |
| **任务描述** | 实现CoreGraphicsEngine类，使用Apple Core Graphics进行macOS平台渲染。 |
| **依赖关系** | T12 (Simple2DEngine基类) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 8h (乐观6h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/core_graphics_engine.h`
- `src/draw/core_graphics_engine.mm`

**类相关接口**:
```cpp
#if DRAW_HAS_COREGRAPHICS

class CoreGraphicsEngine : public Simple2DEngine {
public:
    explicit CoreGraphicsEngine(DrawDevice* device);
    ~CoreGraphicsEngine() override;
    
    std::string GetName() const override { return "CoreGraphicsEngine"; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1, const double* x2, const double* y2, int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count, const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, double scaleX, double scaleY) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    CGContextRef GetContext() const { return m_context; }

private:
    CGContextRef m_context = nullptr;
    
    void ApplyStyle(const DrawStyle& style);
    CGColorRef CreateCGColor(const Color& color);
    CGPathRef CreatePath(const double* x, const double* y, int count, bool closed);
};

#endif
```

**第三方库依赖**:
- CoreGraphics (系统内置)
- CoreText (系统内置)
- DRAW_HAS_COREGRAPHICS宏保护

**验收标准**:
- [ ] macOS平台编译通过
- [ ] 所有绘制功能正确
- [ ] 文本渲染高质量
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第4400-4600行 (macOS平台)

---

### T42: MetalEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现MetalEngine macOS GPU渲染引擎 |
| **任务描述** | 实现MetalEngine类，继承自GPUAcceleratedEngine，使用Apple Metal进行GPU加速渲染。 |
| **依赖关系** | T17 (GPUResourceManager) |
| **里程碑** | M6 平台适配 |
| **优先级** | P2 (一般) |
| **负责人** | Dev B |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/metal_engine.h`
- `src/draw/metal_engine.mm`
- `shaders/metal/shader.metal`

**类相关接口**:
```cpp
#if DRAW_HAS_METAL

class MetalEngine : public GPUAcceleratedEngine {
public:
    explicit MetalEngine(DrawDevice* device);
    ~MetalEngine() override;
    
    std::string GetName() const override { return "MetalEngine"; }
    
    bool Initialize() override;
    void Shutdown() override;
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;

private:
    id<MTLDevice> m_device = nil;
    id<MTLCommandQueue> m_commandQueue = nil;
    id<MTLRenderPipelineState> m_pipelineState = nil;
    id<MTLBuffer> m_vertexBuffer = nil;
    MTLRenderPassDescriptor* m_renderPassDescriptor = nil;
};

#endif
```

**第三方库依赖**:
- Metal (系统内置, macOS 10.11+)
- MetalKit (系统内置)
- DRAW_HAS_METAL宏保护

**验收标准**:
- [ ] macOS 10.11+平台编译通过
- [ ] GPU加速渲染正确
- [ ] 性能优于CoreGraphics
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第4600-4800行

---

### T43: macOS平台集成

| 属性 | 内容 |
|------|------|
| **任务摘要** | 完成macOS平台集成和测试 |
| **任务描述** | 整合CoreGraphicsEngine和MetalEngine，实现macOS平台自动选择最优引擎。编写平台集成测试。 |
| **依赖关系** | T41 (CoreGraphicsEngine), T42 (MetalEngine) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `src/draw/platform/macos/platform_init.mm`
- `tests/draw/test_macos_platform.mm`

**类相关接口**:
```cpp
namespace draw {
namespace macos {

class MacOSPlatform {
public:
    static std::unique_ptr<DrawEngine> CreateOptimalEngine(DrawDevice* device) {
#if DRAW_HAS_METAL
        if (IsMetalAvailable()) {
            auto engine = std::make_unique<MetalEngine>(device);
            if (engine->Initialize()) {
                return engine;
            }
        }
#endif
#if DRAW_HAS_COREGRAPHICS
        return std::make_unique<CoreGraphicsEngine>(device);
#endif
        return nullptr;
    }
    
    static bool IsMetalAvailable();
    static PlatformCapabilities GetCapabilities();
};

}
}
```

**第三方库依赖**: macOS SDK

**验收标准**:
- [ ] 引擎自动选择正确
- [ ] 平台能力检测正确
- [ ] 集成测试通过
- [ ] 性能测试达标

**参考文档**: render_category_design.md 第4800-5000行

---

### T44: WebGLEngine

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现WebGLEngine Web渲染引擎 |
| **任务描述** | 实现WebGLEngine类，继承自GPUAcceleratedEngine，使用WebGL进行Web平台GPU加速渲染。 |
| **依赖关系** | T17 (GPUResourceManager) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/webgl_engine.h`
- `src/draw/webgl_engine.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_WEBGL

class WebGLEngine : public GPUAcceleratedEngine {
public:
    explicit WebGLEngine(DrawDevice* device);
    ~WebGLEngine() override;
    
    std::string GetName() const override { return "WebGLEngine"; }
    
    bool Initialize() override;
    void Shutdown() override;
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, const Font& font, const Color& color) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;

private:
    GLuint m_vertexBuffer = 0;
    GLuint m_indexBuffer = 0;
    GLuint m_shaderProgram = 0;
    GLuint m_textureAtlas = 0;
    
    bool InitShaders();
    bool InitBuffers();
    void UpdateTextureAtlas();
};

#endif
```

**第三方库依赖**:
- WebGL 2.0 (浏览器)
- Emscripten >= 3.1.50
- DRAW_HAS_WEBGL宏保护

**验收标准**:
- [ ] WebAssembly编译通过
- [ ] WebGL渲染正确
- [ ] 主流浏览器兼容
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第5000-5200行 (Web平台)

---

### T45: WebGLDevice

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现WebGLDevice Web显示设备 |
| **任务描述** | 实现WebGLDevice类，管理WebGL上下文和Canvas元素。支持Canvas大小调整和设备像素比。 |
| **依赖关系** | T44 (WebGLEngine) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/webgl_device.h`
- `src/draw/webgl_device.cpp`

**类相关接口**:
```cpp
#if DRAW_HAS_WEBGL

class WebGLDevice : public DrawDevice {
public:
    explicit WebGLDevice(const std::string& canvasId);
    WebGLDevice(int width, int height);
    ~WebGLDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kWebGL; }
    int GetWidth() const override { return m_width; }
    int GetHeight() const override { return m_height; }
    double GetDpi() const override;
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kWebGL; }
    
    void Resize(int width, int height);
    void SetDevicePixelRatio(double ratio);
    double GetDevicePixelRatio() const { return m_devicePixelRatio; }
    
    void Present();
    std::string GetCanvasId() const { return m_canvasId; }

private:
    std::string m_canvasId;
    int m_width = 0, m_height = 0;
    double m_devicePixelRatio = 1.0;
    void* m_glContext = nullptr;
};

#endif
```

**第三方库依赖**:
- Emscripten >= 3.1.50
- DRAW_HAS_WEBGL宏保护

**验收标准**:
- [ ] Canvas正确初始化
- [ ] 设备像素比正确处理
- [ ] 大小调整正确响应
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第5200-5400行

---

### T46: Web平台集成

| 属性 | 内容 |
|------|------|
| **任务摘要** | 完成Web平台集成和测试 |
| **任务描述** | 整合WebGLEngine和WebGLDevice，完成Emscripten编译配置。编写Web平台集成测试。 |
| **依赖关系** | T44 (WebGLEngine), T45 (WebGLDevice) |
| **里程碑** | M6 平台适配 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `src/draw/platform/web/platform_init.cpp`
- `tests/draw/test_web_platform.cpp`
- `CMakeLists.txt` (Emscripten配置)

**类相关接口**:
```cpp
namespace draw {
namespace web {

class WebPlatform {
public:
    static std::unique_ptr<DrawEngine> CreateOptimalEngine(DrawDevice* device) {
#if DRAW_HAS_WEBGL
        auto engine = std::make_unique<WebGLEngine>(device);
        if (engine->Initialize()) {
            return engine;
        }
#endif
        return nullptr;
    }
    
    static PlatformCapabilities GetCapabilities();
    static bool CheckWebGLSupport();
};

}
}
```

**第三方库依赖**:
- Emscripten >= 3.1.50

**验收标准**:
- [ ] Emscripten编译成功
- [ ] 主流浏览器测试通过
- [ ] 性能测试达标
- [ ] 文档完整

**参考文档**: render_category_design.md 第5400-5600行


---

## 9. M7 高级特性任务详情

### T47: PluginManager

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现PluginManager插件管理器 |
| **任务描述** | 实现PluginManager类，支持动态加载和卸载渲染引擎插件。支持插件发现、版本检查、依赖管理。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M7 高级特性 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 10h (乐观8h, 悲观14h) |

**产出文件**:
- `include/ogc/draw/plugin_manager.h`
- `src/draw/plugin_manager.cpp`

**类相关接口**:
```cpp
class PluginManager {
public:
    static PluginManager& Instance();
    
    void LoadPlugin(const std::string& path);
    void UnloadPlugin(const std::string& name);
    void UnloadAll();
    
    void ScanPluginDirectory(const std::string& directory);
    
    std::vector<std::string> GetLoadedPlugins() const;
    
    DrawEnginePlugin* GetEnginePlugin(const std::string& name);
    DrawDevicePlugin* GetDevicePlugin(const std::string& name);
    
    std::vector<DrawEnginePlugin*> GetAvailableEnginePlugins();
    std::vector<DrawDevicePlugin*> GetAvailableDevicePlugins();
    
    std::unique_ptr<DrawEngine> CreateEngine(EngineType type);
    std::unique_ptr<DrawDevice> CreateDevice(DeviceType type);

private:
    struct PluginInfo {
        void* handle;
        std::string path;
        std::string name;
        std::string version;
        std::unique_ptr<DrawEnginePlugin> enginePlugin;
        std::unique_ptr<DrawDevicePlugin> devicePlugin;
    };
    
    std::unordered_map<std::string, PluginInfo> m_plugins;
    std::mutex m_mutex;
    
    bool ValidatePlugin(const PluginInfo& info);
    bool CheckDependencies(const PluginInfo& info);
};
```

**第三方库依赖**:
- Windows: LoadLibrary
- Linux: dlopen
- macOS: dlopen

**验收标准**:
- [ ] 插件动态加载正确
- [ ] 插件版本检查有效
- [ ] 插件依赖管理正确
- [ ] 内存管理无泄漏
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第2500-2700行 (插件扩展机制)

---

### T48: DrawEnginePlugin接口

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现DrawEnginePlugin引擎插件接口 |
| **任务描述** | 实现DrawEnginePlugin抽象接口，定义引擎插件的标准接口。包含插件信息、支持的引擎类型、引擎创建方法。 |
| **依赖关系** | T47 (PluginManager) |
| **里程碑** | M7 高级特性 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/draw_engine_plugin.h`
- `src/draw/draw_engine_plugin.cpp`

**类相关接口**:
```cpp
class DrawEnginePlugin {
public:
    virtual ~DrawEnginePlugin() = default;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual std::string GetAuthor() const = 0;
    
    virtual std::vector<EngineType> GetSupportedEngineTypes() const = 0;
    virtual std::vector<std::string> GetDependencies() const = 0;
    
    virtual std::unique_ptr<DrawEngine> CreateEngine(EngineType type) = 0;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    virtual bool CheckCompatibility() const;
    virtual std::string GetError() const;
};

#define DRAW_EXPORT_ENGINE_PLUGIN(PluginClass) \
    extern "C" { \
        DRAW_API DrawEnginePlugin* CreateEnginePlugin() { \
            return new PluginClass(); \
        } \
        DRAW_API void DestroyEnginePlugin(DrawEnginePlugin* plugin) { \
            delete plugin; \
        } \
    }
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 插件接口定义完整
- [ ] 导出宏正确使用
- [ ] 示例插件可编译
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第2700-2900行

---

### T49: DrawDevicePlugin接口

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现DrawDevicePlugin设备插件接口 |
| **任务描述** | 实现DrawDevicePlugin抽象接口，定义设备插件的标准接口。包含插件信息、支持的设备类型、设备创建方法。 |
| **依赖关系** | T47 (PluginManager) |
| **里程碑** | M7 高级特性 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/draw_device_plugin.h`
- `src/draw/draw_device_plugin.cpp`

**类相关接口**:
```cpp
class DrawDevicePlugin {
public:
    virtual ~DrawDevicePlugin() = default;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    virtual std::string GetAuthor() const = 0;
    
    virtual std::vector<DeviceType> GetSupportedDeviceTypes() const = 0;
    virtual std::vector<std::string> GetDependencies() const = 0;
    
    virtual std::unique_ptr<DrawDevice> CreateDevice(DeviceType type) = 0;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    
    virtual bool CheckCompatibility() const;
    virtual std::string GetError() const;
};

#define DRAW_EXPORT_DEVICE_PLUGIN(PluginClass) \
    extern "C" { \
        DRAW_API DrawDevicePlugin* CreateDevicePlugin() { \
            return new PluginClass(); \
        } \
        DRAW_API void DestroyDevicePlugin(DrawDevicePlugin* plugin) { \
            delete plugin; \
        } \
    }
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 插件接口定义完整
- [ ] 导出宏正确使用
- [ ] 示例插件可编译
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第2900-3100行

---

### T50: 版本兼容性完善

| 属性 | 内容 |
|------|------|
| **任务摘要** | 完善版本兼容性机制 |
| **任务描述** | 实现版本兼容性检查、废弃API处理、版本号规则。确保API向后兼容，提供平滑升级路径。 |
| **依赖关系** | T47 (PluginManager) |
| **里程碑** | M7 高级特性 |
| **优先级** | P2 (一般) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/draw_version.h`
- `src/draw/draw_version.cpp`

**类相关接口**:
```cpp
namespace draw {

struct Version {
    int major = 0;
    int minor = 0;
    int patch = 0;
    
    static Version Current() { return {1, 0, 0}; }
    static Version FromString(const std::string& str);
    
    std::string ToString() const;
    bool operator==(const Version& other) const;
    bool operator<(const Version& other) const;
    bool IsCompatible(const Version& other) const;
};

class VersionManager {
public:
    static VersionManager& Instance();
    
    Version GetVersion() const;
    Version GetApiVersion() const;
    
    bool IsApiCompatible(int major, int minor) const;
    
    void SetDeprecationWarningHandler(std::function<void(const std::string&)> handler);
    void ReportDeprecatedApi(const std::string& api, const std::string& replacement);
    
    std::vector<std::string> GetDeprecatedApis() const;
};

#define DRAW_DEPRECATED(msg) [[deprecated(msg)]]
#define DRAW_VERSION_CHECK(major, minor) \
    static_assert(DRAW_VERSION_MAJOR > (major) || \
                  (DRAW_VERSION_MAJOR == (major) && DRAW_VERSION_MINOR >= (minor)), \
                  "Version mismatch")

}
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 版本号规则明确
- [ ] 废弃API警告正确
- [ ] 兼容性检查有效
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3100-3300行

---

### T51: EnginePool引擎池

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现EnginePool引擎实例池 |
| **任务描述** | 实现EnginePool类，管理引擎实例的创建和复用。支持引擎预热、实例限制、自动回收。 |
| **依赖关系** | T3 (DrawContext) |
| **里程碑** | M7 高级特性 |
| **优先级** | P1 (重要) |
| **负责人** | Dev B |
| **估算工时** | 6h (乐观4h, 悲观8h) |

**产出文件**:
- `include/ogc/draw/engine_pool.h`
- `src/draw/engine_pool.cpp`

**类相关接口**:
```cpp
class EnginePool {
public:
    static EnginePool& Instance();
    
    void Initialize(EngineType type, int poolSize = 4);
    void Shutdown();
    
    std::shared_ptr<DrawEngine> Acquire(EngineType type);
    void Release(std::shared_ptr<DrawEngine> engine);
    
    void SetMaxPoolSize(EngineType type, int maxSize);
    int GetPoolSize(EngineType type) const;
    int GetAvailableCount(EngineType type) const;
    
    void Warmup(EngineType type);
    void Clear(EngineType type);
    
    class EngineGuard {
    public:
        EngineGuard(std::shared_ptr<DrawEngine> engine, EnginePool* pool);
        ~EngineGuard();
        
        DrawEngine* get() const { return m_engine.get(); }
        DrawEngine* operator->() const { return m_engine.get(); }
        
    private:
        std::shared_ptr<DrawEngine> m_engine;
        EnginePool* m_pool = nullptr;
    };
    
    EngineGuard AcquireGuard(EngineType type);

private:
    std::map<EngineType, std::queue<std::shared_ptr<DrawEngine>>> m_pools;
    std::map<EngineType, int> m_maxSizes;
    std::map<EngineType, std::set<std::shared_ptr<DrawEngine>>> m_inUse;
    std::mutex m_mutex;
};
```

**第三方库依赖**: 无

**验收标准**:
- [ ] 引擎实例复用正确
- [ ] 并发访问安全
- [ ] 资源限制有效
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第3300-3500行

---

### T52: 性能优化与测试

| 属性 | 内容 |
|------|------|
| **任务摘要** | 完成性能优化和综合测试 |
| **任务描述** | 进行性能分析和优化，编写综合测试用例，生成性能报告。包括批处理优化、缓存优化、内存优化。 |
| **依赖关系** | T1-T51 (所有前置任务) |
| **里程碑** | M7 高级特性 |
| **优先级** | P0 (关键) |
| **负责人** | Dev B |
| **估算工时** | 12h (乐观10h, 悲观16h) |

**产出文件**:
- `src/draw/optimization/batch_renderer.cpp`
- `src/draw/optimization/render_cache.cpp`
- `src/draw/optimization/lod_strategy.cpp`
- `tests/draw/performance/benchmark_render.cpp`
- `tests/draw/performance/benchmark_memory.cpp`
- `docs/performance_report.md`

**类相关接口**:
```cpp
class BatchRenderer {
public:
    void BeginBatch();
    void AddGeometry(const Geometry* geometry, const DrawStyle& style);
    void EndBatch();
    void Flush();
    
    void SetBatchSize(int size);
    int GetBatchCount() const;
};

class RenderCache {
public:
    static RenderCache& Instance();
    
    CacheHandle GetOrCreate(const Geometry* geometry, const DrawStyle& style);
    void Invalidate(CacheHandle handle);
    void InvalidateAll();
    
    void SetMaxCacheSize(size_t size);
    size_t GetCacheSize() const;
};

class LODStrategy {
public:
    static int CalculateLOD(double distance, double importance);
    static Geometry* Simplify(const Geometry* geometry, int lod);
    static bool ShouldRender(double distance, double importance);
};
```

**第三方库依赖**:
- Google Benchmark (性能测试)

**验收标准**:
- [ ] 批处理优化有效（性能提升>30%）
- [ ] 缓存命中率>80%
- [ ] 内存使用优化（减少>20%）
- [ ] 性能测试报告完整
- [ ] 综合测试覆盖率 > 90%

**参考文档**: render_category_design.md 第3500-3700行

---

### T53: StateSerializer状态序列化器

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现引擎状态序列化器，支持运行时引擎切换 |
| **任务描述** | 实现StateSerializer类，用于在运行时引擎切换时保存和恢复引擎状态。包括变换矩阵、裁剪区域、背景色、透明度等状态的捕获与恢复。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M1 核心框架 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/state_serializer.h`
- `src/draw/state_serializer.cpp`

**类相关接口**:
```cpp
class StateSerializer {
public:
    struct EngineState {
        TransformMatrix transform;
        Envelope clipEnvelope;
        Color background;
        StateFlags savedFlags;
        double tolerance;
        double opacity;
        bool antialiasing;
    };
    
    static EngineState Capture(const DrawEngine* engine);
    static void Restore(DrawEngine* engine, const EngineState& state);
    static void Merge(EngineState& target, const EngineState& source, StateFlags flags);
};
```

**第三方库依赖**:
- 无

**验收标准**:
- [ ] 状态捕获功能正确
- [ ] 状态恢复功能正确
- [ ] 引擎切换后状态一致
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第1000-1100行

---

### T54: ThreadSafeEngine线程安全包装

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现线程安全引擎包装器 |
| **任务描述** | 实现ThreadSafeEngine模板类，为非线程安全的引擎提供线程安全包装。使用互斥锁保护所有绘制操作，支持多线程渲染场景。 |
| **依赖关系** | T2 (DrawEngine基类) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/thread_safe_engine.h`
- `src/draw/thread_safe_engine.cpp`

**类相关接口**:
```cpp
template<typename EngineT>
class ThreadSafeEngine : public DrawEngine {
public:
    explicit ThreadSafeEngine(std::unique_ptr<EngineT> engine);
    ~ThreadSafeEngine() override = default;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2,
                          int count, const DrawStyle& style) override;
    
    bool IsThreadSafe() const override { return true; }
    EngineT* GetWrappedEngine() const { return m_engine.get(); }

private:
    std::unique_ptr<EngineT> m_engine;
    mutable std::mutex m_mutex;
};
```

**第三方库依赖**:
- 无

**验收标准**:
- [ ] 多线程绘制无数据竞争
- [ ] 性能损失 < 10%
- [ ] 线程安全测试通过
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第1100-1200行

---

### T55: 辅助工具类(DrawScopeGuard/DrawSession)

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现异常安全RAII辅助工具类 |
| **任务描述** | 实现DrawScopeGuard和DrawSession两个RAII辅助类。DrawScopeGuard提供异常安全的绘制操作包装，自动处理错误和状态回滚；DrawSession自动管理BeginDraw/EndDraw生命周期。 |
| **依赖关系** | T3 (DrawContext) |
| **里程碑** | M2 CPU渲染 |
| **优先级** | P2 (一般) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/draw_scope_guard.h`
- `include/ogc/draw/draw_session.h`
- `src/draw/draw_scope_guard.cpp`
- `src/draw/draw_session.cpp`

**类相关接口**:
```cpp
class DrawScopeGuard {
public:
    explicit DrawScopeGuard(DrawContext* context);
    ~DrawScopeGuard();
    
    DrawScopeGuard(const DrawScopeGuard&) = delete;
    DrawScopeGuard& operator=(const DrawScopeGuard&) = delete;
    
    template<typename... Args>
    DrawResult DrawGeometry(Args&&... args);
    
    bool Ok() const { return m_lastError == DrawResult::kSuccess; }
    DrawResult GetLastError() const { return m_lastError; }
    void Commit() { m_committed = true; }

private:
    DrawContext* m_context;
    DrawResult m_lastError = DrawResult::kSuccess;
    bool m_committed;
};

class DrawSession {
public:
    DrawSession(DrawContext* context, const DrawParams& params);
    ~DrawSession();
    
    DrawSession(const DrawSession&) = delete;
    DrawSession& operator=(const DrawSession&) = delete;
    
    DrawResult GetResult() const { return m_result; }
    void Cancel();

private:
    DrawContext* m_context;
    DrawResult m_result;
    bool m_active;
};
```

**第三方库依赖**:
- 无

**验收标准**:
- [ ] DrawScopeGuard异常安全
- [ ] DrawSession自动生命周期管理
- [ ] 错误处理正确
- [ ] 单元测试覆盖率 > 85%

**参考文档**: render_category_design.md 第1500-1700行

---

### T56: 设备能力协商与丢失恢复

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现设备能力协商和设备丢失恢复机制 |
| **任务描述** | 实现CapabilityNegotiator类用于根据设备能力自动降级不支持的特性；扩展DrawDevice接口支持设备丢失检测和恢复。处理GPU设备丢失（如驱动崩溃、设备切换）后的自动恢复。 |
| **依赖关系** | T1 (DrawDevice), T17 (GPUResourceManager) |
| **里程碑** | M3 GPU加速 |
| **优先级** | P1 (重要) |
| **负责人** | Dev A |
| **估算工时** | 6h (乐观4h, 悲观10h) |

**产出文件**:
- `include/ogc/draw/capability_negotiator.h`
- `include/ogc/draw/device_recovery.h`
- `src/draw/capability_negotiator.cpp`
- `src/draw/device_recovery.cpp`

**类相关接口**:
```cpp
class CapabilityNegotiator {
public:
    DrawStyle NegotiateStyle(const DrawStyle& requested, const DeviceCapabilities& caps);
    Font NegotiateFont(const Font& requested, const DeviceCapabilities& caps);
    bool IsFeatureSupported(const std::string& feature, const DeviceCapabilities& caps);
    
private:
    void DowngradeTransparency(DrawStyle& style);
    void DowngradeAntialiasing(DrawStyle& style);
    void DowngradeTextRotation(DrawStyle& style);
};

class DeviceRecovery {
public:
    static bool HandleDeviceLost(DrawContext* context);
    static void RegisterRecoveryCallback(std::function<void()> callback);
    
private:
    static bool TryRecoverGPU(DrawDevice* device);
    static void FallbackToCPU(DrawContext* context);
};
```

**第三方库依赖**:
- 无

**验收标准**:
- [ ] 能力协商功能正确
- [ ] 设备丢失检测正确
- [ ] 自动恢复功能正确
- [ ] CPU回退功能正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第1600-2000行

---

### T57: 库兼容性检查工具

| 属性 | 内容 |
|------|------|
| **任务摘要** | 实现第三方库版本兼容性检查工具 |
| **任务描述** | 实现LibraryCompatibility类，用于运行时检测第三方库版本，确保兼容性。支持Qt、Cairo、podofo等库的版本检测和兼容性报告生成。 |
| **依赖关系** | T6 (基础类型定义) |
| **里程碑** | M7 高级特性 |
| **优先级** | P2 (一般) |
| **负责人** | Dev B |
| **估算工时** | 4h (乐观3h, 悲观6h) |

**产出文件**:
- `include/ogc/draw/library_compatibility.h`
- `src/draw/library_compatibility.cpp`

**类相关接口**:
```cpp
struct LibraryVersion {
    std::string name;
    int major = 0;
    int minor = 0;
    int patch = 0;
    
    bool operator>=(const LibraryVersion& other) const;
    std::string ToString() const;
};

class LibraryCompatibility {
public:
    static LibraryVersion GetQtVersion();
    static LibraryVersion GetCairoVersion();
    static LibraryVersion GetPodofoVersion();
    static LibraryVersion GetOpenGLVersion();
    
    static bool CheckMinimumRequirements();
    static std::vector<std::string> GetCompatibilityIssues();
    static std::string GetCompatibilityReport();
    
private:
    static const std::map<std::string, LibraryVersion> s_minimumVersions;
};
```

**第三方库依赖**:
- 无

**验收标准**:
- [ ] 版本检测功能正确
- [ ] 兼容性检查功能正确
- [ ] 报告生成功能正确
- [ ] 单元测试覆盖率 > 80%

**参考文档**: render_category_design.md 第3200-3400行


---

## 10. 风险评估与质量保障

### 10.1 技术风险

| 风险项 | 风险等级 | 影响范围 | 缓解措施 |
|--------|----------|----------|----------|
| GPU驱动兼容性问题 | 高 | M3 GPU加速 | 实现软件回退机制，支持多GPU厂商测试 |
| 跨平台渲染差异 | 中 | M6 平台适配 | 建立跨平台测试矩阵，统一渲染基准测试 |
| 第三方库版本冲突 | 中 | 全项目 | 使用vcpkg/conan管理依赖，版本锁定 |
| 内存泄漏风险 | 中 | 全项目 | 使用RAII，集成内存检测工具(ASan) |
| 线程安全问题 | 高 | M4 异步渲染 | 代码审查，线程安全测试，静态分析 |

### 10.2 进度风险

| 风险项 | 风险等级 | 影响范围 | 缓解措施 |
|--------|----------|----------|----------|
| GPU加速开发延期 | 高 | M3 | 预留缓冲时间，并行开发CPU渲染作为备选 |
| 平台适配复杂度超预期 | 中 | M6 | 优先完成Qt跨平台方案，其他平台按优先级递减 |
| 测试覆盖不足 | 中 | 全项目 | 持续集成，每日测试报告，覆盖率监控 |

### 10.3 质量保障措施

#### 代码质量

| 措施 | 工具 | 频率 |
|------|------|------|
| 静态代码分析 | Clang-Tidy, Cppcheck | 每次提交 |
| 代码格式检查 | ClangFormat | 每次提交 |
| 内存泄漏检测 | AddressSanitizer | 每日构建 |
| 代码覆盖率 | gcov/lcov | 每日构建 |

#### 测试策略

| 测试类型 | 覆盖目标 | 执行频率 |
|----------|----------|----------|
| 单元测试 | >85% | 每次提交 |
| 集成测试 | 关键路径 | 每日构建 |
| 性能测试 | 基准对比 | 每周 |
| 跨平台测试 | 5平台 | 每周 |
| 回归测试 | 全功能 | 每版本 |

#### 测试环境矩阵

| 平台 | 编译器 | 构建类型 | 测试类型 |
|------|--------|----------|----------|
| Windows 10/11 | MSVC 2022 | Debug/Release | 单元/集成/性能 |
| Ubuntu 22.04 | GCC 11 | Debug/Release | 单元/集成 |
| Ubuntu 22.04 | Clang 14 | Debug/Release | 单元/集成 |
| macOS 13 | Clang 14 | Debug/Release | 单元/集成 |
| Web (Chrome/Firefox) | Emscripten | Release | 集成/性能 |

### 10.4 交付标准

#### 代码标准

- [ ] 所有代码通过静态分析
- [ ] 单元测试覆盖率 > 85%
- [ ] 无内存泄漏（ASan检测通过）
- [ ] 无线程安全问题（TSan检测通过）
- [ ] 代码格式符合项目规范

#### 功能标准

- [ ] 所有P0任务完成
- [ ] 所有P1任务完成率 > 90%
- [ ] 核心功能测试通过
- [ ] 跨平台测试通过
- [ ] 性能基准达标

#### 文档标准

- [ ] API文档完整（Doxygen）
- [ ] 架构设计文档更新
- [ ] 使用示例完整
- [ ] 变更日志完整

### 10.5 验收检查清单

#### M1 核心框架验收

- [ ] DrawDevice/DrawEngine/DrawContext基类编译通过
- [ ] 基础类型定义完整
- [ ] 单元测试覆盖率 > 85%
- [ ] 示例代码可运行

#### M2 CPU渲染验收

- [ ] RasterImageDevice/DisplayDevice功能正确
- [ ] Simple2DEngine所有绘制功能正确
- [ ] 图像导出格式支持完整
- [ ] 性能达到基准要求

#### M3 GPU加速验收

- [ ] GPU资源管理正确
- [ ] GPU渲染结果与CPU一致
- [ ] 性能优于CPU渲染 > 50%
- [ ] 多GPU选择功能正确

#### M4 瓦片渲染验收

- [ ] 大尺寸图像渲染正确
- [ ] 异步渲染功能正确
- [ ] 取消和等待功能正确
- [ ] 进度回调正确

#### M5 矢量输出验收

- [ ] PDF输出正确可打开
- [ ] SVG输出正确可显示
- [ ] 文本和字体正确嵌入
- [ ] 图像正确嵌入

#### M6 平台适配验收

- [ ] Windows平台测试通过
- [ ] Linux平台测试通过
- [ ] macOS平台测试通过
- [ ] Web平台测试通过
- [ ] Qt跨平台方案测试通过

#### M7 高级特性验收

- [ ] 插件系统功能正确
- [ ] 引擎池功能正确
- [ ] 性能优化达标
- [ ] 综合测试通过


---

## 11. 附录与参考文档

### 11.1 文件结构规划

```
code/draw/
├── include/ogc/draw/
│   ├── draw_device.h           # T1
│   ├── draw_engine.h           # T2
│   ├── draw_context.h          # T3
│   ├── draw_style.h            # T4
│   ├── draw_result.h           # T5
│   ├── device_type.h           # T6
│   ├── engine_type.h           # T6
│   ├── draw_types.h            # T6
│   ├── transform_matrix.h      # T7
│   ├── geometry.h              # T8
│   ├── state_serializer.h      # T53
│   ├── thread_safe_engine.h    # T54
│   ├── draw_scope_guard.h      # T55
│   ├── draw_session.h          # T55
│   ├── capability_negotiator.h # T56
│   ├── device_recovery.h       # T56
│   ├── library_compatibility.h # T57
│   ├── raster_image_device.h   # T10
│   ├── display_device.h        # T11
│   ├── simple2d_engine.h       # T12
│   ├── gpu_resource_manager.h  # T17
│   ├── render_memory_pool.h    # T18
│   ├── texture_cache.h         # T19
│   ├── gpu_accelerated_engine.h# T20
│   ├── gpu_resource.h          # T21
│   ├── gpu_device_selector.h   # T22
│   ├── tile_device.h           # T23
│   ├── tile_based_engine.h     # T24
│   ├── async_render_task.h     # T25
│   ├── async_render_manager.h  # T26
│   ├── tile_size_strategy.h    # T27
│   ├── vector_engine.h         # T28
│   ├── pdf_device.h            # T29
│   ├── pdf_engine.h            # T30
│   ├── svg_device.h            # T31
│   ├── svg_engine.h            # T32
│   ├── qt_engine.h             # T34
│   ├── qt_display_device.h     # T35
│   ├── gdiplus_engine.h        # T36
│   ├── direct2d_engine.h       # T37
│   ├── cairo_engine.h          # T39
│   ├── x11_display_device.h    # T40
│   ├── core_graphics_engine.h  # T41
│   ├── metal_engine.h          # T42
│   ├── webgl_engine.h          # T44
│   ├── webgl_device.h          # T45
│   ├── plugin_manager.h        # T47
│   ├── draw_engine_plugin.h    # T48
│   ├── draw_device_plugin.h    # T49
│   ├── draw_version.h          # T50
│   ├── engine_pool.h           # T51
│   └── draw_config.h           # CMake生成
│
├── src/draw/
│   ├── core/                   # 核心模块
│   ├── cpu/                    # CPU渲染
│   ├── gpu/                    # GPU加速
│   ├── tile/                   # 瓦片渲染
│   ├── vector/                 # 矢量输出
│   ├── platform/               # 平台适配
│   │   ├── windows/
│   │   ├── linux/
│   │   ├── macos/
│   │   └── web/
│   ├── plugin/                 # 插件系统
│   └── optimization/           # 性能优化
│
├── shaders/
│   ├── glsl/                   # OpenGL着色器
│   └── metal/                  # Metal着色器
│
├── tests/draw/
│   ├── unit/                   # 单元测试
│   ├── integration/            # 集成测试
│   ├── performance/            # 性能测试
│   └── test_data/              # 测试数据
│
└── docs/
    ├── render_category_design.md
    ├── tasks.md
    ├── api/                    # API文档
    └── performance_report.md
```

### 11.2 第三方库依赖汇总

| 库名称 | 最低版本 | 推荐版本 | 用途 | 宏定义 | 许可证 |
|--------|----------|----------|------|--------|--------|
| **Qt** | 5.12 | 6.5 LTS | 跨平台渲染 | DRAW_HAS_QT | LGPL v3 / Commercial |
| **Cairo** | 1.16.0 | 1.18.0 | Linux矢量渲染 | DRAW_HAS_CAIRO | LGPL v2.1 / MPL |
| **podofo** | 0.9.8 | 0.10.3 | PDF生成 | DRAW_HAS_PODOFO | LGPL v2.1 |
| **FreeType** | 2.12.0 | 2.13.0 | 字体渲染 | - | FTL / GPL |
| **OpenGL** | 3.3 | 4.5 | GPU加速 | DRAW_HAS_OPENGL | 系统内置 |
| **Emscripten** | 3.1.0 | 3.1.50 | Web编译 | DRAW_HAS_WEBGL | MIT |
| **GoogleTest** | 1.12.0 | 1.14.0 | 单元测试 | - | BSD-3 |
| **stb_image** | 2.27 | 2.29 | 图像加载 | - | MIT / Public Domain |
| **stb_image_write** | 1.16 | 1.18 | 图像导出 | - | MIT / Public Domain |

### 11.3 编译配置选项

```cmake
# 核心选项
option(DRAW_BUILD_TESTS "Build unit tests" ON)
option(DRAW_BUILD_EXAMPLES "Build examples" ON)
option(DRAW_ENABLE_SANITIZERS "Enable sanitizers" OFF)

# 渲染后端选项
option(DRAW_USE_QT "Enable Qt rendering backend" ON)
option(DRAW_USE_OPENGL "Enable OpenGL rendering backend" ON)
option(DRAW_USE_CAIRO "Enable Cairo rendering backend" ON)
option(DRAW_USE_PODOFO "Enable PDF output support" ON)

# 平台特定选项
option(DRAW_USE_DIRECT2D "Enable Direct2D backend (Windows)" ON)
option(DRAW_USE_METAL "Enable Metal backend (macOS)" ON)
option(DRAW_USE_WEBGL "Enable WebGL backend (Emscripten)" ON)

# 性能选项
option(DRAW_ENABLE_PROFILING "Enable profiling support" OFF)
option(DRAW_ENABLE_LTO "Enable link-time optimization" OFF)
```

### 11.4 参考文档

#### 设计参考

| 文档 | 说明 |
|------|------|
| [render_category_design.md](render_category_design.md) | 渲染架构设计文档 v1.5 |
| Qt QPainter文档 | https://doc.qt.io/qt-6/qpainter.html |
| Cairo图形库文档 | https://www.cairographics.org/manual/ |
| OpenGL规范 | https://www.khronos.org/opengl/wiki/ |
| Metal编程指南 | https://developer.apple.com/metal/ |

#### 代码规范

| 规范 | 说明 |
|------|------|
| C++ Core Guidelines | https://isocpp.github.io/CppCoreGuidelines/ |
| Google C++ Style Guide | https://google.github.io/styleguide/cppguide.html |
| ClangFormat配置 | .clang-format |

#### 工具文档

| 工具 | 说明 |
|------|------|
| CMake文档 | https://cmake.org/documentation/ |
| vcpkg包管理 | https://vcpkg.io/en/getting-started.html |
| GoogleTest文档 | https://google.github.io/googletest/ |

### 11.5 变更日志

| 版本 | 日期 | 变更内容 |
|------|------|----------|
| v4.1 | 2026-03-29 | 补充遗漏任务（T53-T57）：StateSerializer、ThreadSafeEngine、辅助工具类、设备能力协商、库兼容性检查 |
| v4.0 | 2026-03-29 | 重新规划2人团队任务，增加详细任务属性 |
| v3.0 | 2026-03-29 | 2人团队任务计划 |
| v2.0 | 2026-03-29 | 3人团队任务计划 |
| v1.1 | 2026-03-29 | 补充遗漏任务（T53-T56） |
| v1.0 | 2026-03-29 | 初始版本 |

---

**文档结束**

*本文档基于 render_category_design.md v1.5 生成，包含57个详细任务，覆盖7个里程碑，适用于2人开发团队。*

