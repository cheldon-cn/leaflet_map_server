# 渲染架构设计文档

**版本**: v1.5  
**日期**: 2026-03-29  
**变更**: 多角色交叉评审改进（GPU资源管理、Qt许可证指南、错误处理统一、线程安全示例）

## 文档目标

桌面底层绘制系统采用**设备抽象层**设计模式，通过统一的绘制接口隔离不同输出设备的实现细节。根据设备类型的不同，采用不同的渲染策略以实现高效率和高质量的输出。

### 核心目标

| 目标 | 描述 | 达成方式 |
|------|------|----------|
| **高扩展性** | 支持多种设备类型和渲染后端，易于扩展新设备 | Device.CreateEngine()模式 |
| **高性能** | 支持GPU加速、瓦片渲染、多线程渲染 | 引擎池、资源管理器、线程安全包装 |
| **低学习曲线** | 概念简洁，API易用，与Qt设计一致 | 三层架构、状态栈封装 |

### 设计原则

1. **设备抽象**：通过DrawDevice统一接口隔离不同输出设备实现
2. **引擎分离**：DrawEngine负责渲染逻辑，支持多后端切换
3. **上下文封装**：DrawContext提供高层API，管理状态栈
4. **借鉴Qt**：参考QPaintDevice/QPaintEngine/QPainter成熟设计

### 职责边界定义

| 组件 | 职责范围 | 不负责 | 接口层次 |
|------|----------|--------|----------|
| **DrawDevice** | 设备信息、生命周期管理、引擎创建、能力查询 | 绘制操作、坐标变换、裁剪计算 | 设备抽象层 |
| **DrawEngine** | 所有绘制操作（原子操作）、坐标变换、裁剪计算、批处理优化 | 设备管理、业务逻辑 | 渲染引擎层 |
| **DrawContext** | 状态栈管理、API封装、参数验证、引擎切换 | 具体绘制实现、设备管理 | 用户API层 |

**分层调用规则（参照Qt设计）**：
- DrawContext → DrawEngine → DrawDevice（严格单向）
- Device只负责设备抽象和引擎创建，**不包含任何绘制方法**
- Engine定义所有绘制操作（原子操作），执行变换和裁剪
- Context提供用户API，管理状态栈，协调Device和Engine

**与Qt架构对比**：

| 方面 | Qt架构 | 本设计 |
|------|--------|--------|
| 设备层 | QPaintDevice（无绘制方法） | DrawDevice（无绘制方法） |
| 引擎层 | QPaintEngine（所有绘制方法） | DrawEngine（所有绘制方法） |
| 用户API | QPainter（状态管理+API封装） | DrawContext（状态管理+API封装） |
| 引擎创建 | device->paintEngine() | device->CreateEngine() |

---

## 架构总览

### 三层架构设计

```
┌─────────────────────────────────────────────────────────────────┐
│                        应用层                                   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    DrawContext                           │   │
│  │  职责：状态栈管理、API封装、引擎协调                      │   │
│  │  特性：SetEngineType()运行时切换引擎                     │   │
│  │  参照：QPainter                                          │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│                        引擎层                                   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    DrawEngine                            │   │
│  │  职责：所有绘制操作（原子操作）、坐标变换、裁剪           │   │
│  │  实现：Simple2DEngine、GPUAcceleratedEngine、TileEngine  │   │
│  │  参照：QPaintEngine                                      │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│                        设备层                                   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    DrawDevice                            │   │
│  │  职责：设备抽象、引擎创建、能力查询                       │   │
│  │  特性：CreateEngine()创建引擎（无绘制方法）              │   │
│  │  参照：QPaintDevice                                      │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### 类关系图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      类关系图（参照Qt设计）                              │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────┐       SetDevice()        ┌─────────────────┐ │
│  │    DrawContext      │ ─────────────────────────►│   DrawDevice    │ │
│  │   (参照QPainter)    │                           │ (参照QPaintDevice)│
│  │─────────────────────│                           │─────────────────│ │
│  │ - m_device          │                           │ - m_width       │ │
│  │ - m_engine          │      CreateEngine()       │ - m_height      │ │
│  │ - m_transformStack  │ ◄─────────────────────────│ - m_dpi         │ │
│  │ - m_styleStack      │       (创建引擎)          │ - m_state       │ │
│  │ - m_opacityStack    │                           │ (无绘制方法)    │ │
│  └─────────────────────┘                           └─────────────────┘ │
│           │                                                │            │
│           │ 持有                                           │ 继承       │
│           ▼                                                ▼            │
│  ┌─────────────────────┐           ┌─────────────────────────────────┐ │
│  │    DrawEngine       │           │         具体设备实现            │ │
│  │ (参照QPaintEngine)  │           │─────────────────────────────────│ │
│  │─────────────────────│           │ ┌───────────┐ ┌───────────────┐ │ │
│  │ - m_device          │           │ │RasterImage│ │   PdfDevice   │ │ │
│  │ - m_transform       │           │ │  Device   │ │               │ │ │
│  │ - m_clipEnvelope    │           │ └───────────┘ └───────────────┘ │ │
│  │ (所有绘制方法)      │           │ ┌───────────┐ ┌───────────────┐ │ │
│  │  DrawPoints()       │           │ │TileDevice │ │ DisplayDevice │ │ │
│  │  DrawLines()        │           │ └───────────┘ └───────────────┘ │ │
│  │  DrawRects()        │           │ ┌───────────┐ ┌───────────────┐ │ │
│  │  DrawPolygon()      │           │ │ SvgDevice │ │ WebGLDevice   │ │ │
│  │  ...                │           │ └───────────┘ └───────────────┘ │ │
│  └─────────────────────┘           └─────────────────────────────────┘ │
│           │                                                                │
│           │ 继承                                                           │
│           ▼                                                                │
│  ┌─────────────────────────────────────────────────────────────────────┐ │
│  │                        具体引擎实现                                  │ │
│  │─────────────────────────────────────────────────────────────────────│ │
│  │ ┌───────────────┐ ┌─────────────────┐ ┌───────────────────────────┐ │ │
│  │ │Simple2DEngine │ │GPUAccelerated   │ │   TileBasedEngine         │ │ │
│  │ │ (CPU软件渲染) │ │Engine(GPU加速)  │ │   (瓦片渲染)              │ │ │
│  │ └───────────────┘ └─────────────────┘ └───────────────────────────┘ │ │
│  │ ┌───────────────────────────────────┐ ┌───────────────────────────┐ │ │
│  │ │       VectorEngine (基类)         │ │     WebGLEngine           │ │ │
│  │ │      ┌─────────┬─────────┐        │ │   (WebAssembly)           │ │ │
│  │ │      │PdfEngine│SvgEngine│        │ └───────────────────────────┘ │ │
│  │ │      │(podofo) │(cairo)  │        │                             │ │
│  │ │      └─────────┴─────────┘        │                             │ │
│  │ └───────────────────────────────────┘                             │ │
│  └─────────────────────────────────────────────────────────────────────┘ │
│                                                                         │
│  关键关系：                                                              │
│  • DrawDevice.CreateEngine() → 创建DrawEngine实例（核心职责）           │
│  • DrawEngine → 执行所有绘制操作（原子操作）                            │
│  • DrawContext → 用户API层，管理状态栈，协调Device和Engine              │
│                                                                         │
│  注意：DrawDevice不包含任何绘制方法，仅负责设备抽象和引擎创建           │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 设备-引擎绑定关系

```
┌─────────────────────────────────────────────────────────────────────────┐
│              设备与引擎绑定关系（参照Qt QPaintDevice设计）               │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────┐         ┌─────────────────────────────────┐   │
│  │ RasterImageDevice   │ ──────► │ Simple2DEngine (CPU)            │   │
│  │ (无绘制方法)        │ Create  │ GPUAcceleratedEngine (GPU)      │   │
│  └─────────────────────┘ Engine  │ (执行所有绘制操作)              │   │
│                                  └─────────────────────────────────┘   │
│  ┌─────────────────────┐         ┌─────────────────────────────────┐   │
│  │ DisplayDevice       │ ──────► │ GPUAcceleratedEngine (GPU优先)  │   │
│  │ (无绘制方法)        │ Create  │ Simple2DEngine (CPU回退)        │   │
│  └─────────────────────┘ Engine  │ (执行所有绘制操作)              │   │
│                                  └─────────────────────────────────┘   │
│  ┌─────────────────────┐         ┌─────────────────────────────────┐   │
│  │ PdfDevice           │ ──────► │ PdfEngine (podofo)              │   │
│  │ (无绘制方法)        │ Create  │ 继承自VectorEngine              │   │
│  └─────────────────────┘ Engine  └─────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────┐         ┌─────────────────────────────────┐   │
│  │ SvgDevice           │ ──────► │ SvgEngine (cairo)               │   │
│  │ (无绘制方法)        │ Create  │ 继承自VectorEngine              │   │
│  └─────────────────────┘ Engine  └─────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────┐         ┌─────────────────────────────────┐   │
│  │ TileDevice          │ ──────► │ TileBasedEngine (瓦片渲染)      │   │
│  │ (无绘制方法)        │ Create  │ AsyncTileEngine (异步瓦片)      │   │
│  └─────────────────────┘ Engine  │ (执行所有绘制操作)              │   │
│                                  └─────────────────────────────────┘   │
│  ┌─────────────────────┐         ┌─────────────────────────────────┐   │
│  │ WebGLDevice         │ ──────► │ WebGLEngine (WebAssembly)       │   │
│  │ (无绘制方法)        │ Create  │ (执行所有绘制操作)              │   │
│  └─────────────────────┘ Engine  └─────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## DrawDevice 接口定义

### 职责

DrawDevice 是设备抽象层（参照Qt QPaintDevice设计），负责：
- 提供绘制表面信息（尺寸、DPI、颜色深度）
- 设备生命周期管理（初始化、销毁）
- **创建渲染引擎（核心职责）**
- 设备能力查询

**重要**：DrawDevice **不包含任何绘制方法**，所有绘制操作由DrawEngine负责。

### 与Qt QPaintDevice对比

| 方面 | Qt QPaintDevice | 本设计 DrawDevice |
|------|-----------------|-------------------|
| 设备信息 | width(), height(), logicalDpiX() | GetWidth(), GetHeight(), GetDpi() |
| 引擎创建 | paintEngine() | CreateEngine() |
| 绘制方法 | 无 | 无 |
| 职责 | 设备抽象 | 设备抽象 |

### 接口规范

```cpp
class DrawDevice {
public:
    virtual ~DrawDevice() = default;
    
    // ==================== 设备信息 ====================
    
    virtual DeviceType GetType() const = 0;
    virtual std::string GetName() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual double GetDpi() const = 0;
    virtual void SetDpi(double dpi) = 0;
    virtual int GetColorDepth() const = 0;
    
    // ==================== 生命周期 ====================
    
    virtual DrawResult Initialize() = 0;
    virtual DrawResult Finalize() = 0;
    virtual DeviceState GetState() const = 0;
    virtual bool IsReady() const = 0;
    
    // ==================== 核心职责：引擎创建 ====================
    
    virtual std::unique_ptr<DrawEngine> CreateEngine() = 0;
    virtual std::vector<EngineType> GetSupportedEngineTypes() const = 0;
    virtual void SetPreferredEngineType(EngineType type) = 0;
    virtual EngineType GetPreferredEngineType() const = 0;
    
    // ==================== 设备能力查询 ====================
    
    virtual DeviceCapabilities QueryCapabilities() const = 0;
    virtual bool IsFeatureAvailable(const std::string& featureName) const = 0;
    
    // ==================== 设备丢失恢复 ====================
    
    virtual bool IsDeviceLost() const = 0;
    virtual DrawResult RecoverDevice() = 0;
    virtual void OnDeviceLost(std::function<void()> callback) = 0;
    
    // ==================== 工厂方法 ====================
    
    static DrawDevicePtr Create(DeviceType type);

protected:
    EngineType m_preferredEngineType = EngineType::kSimple2D;
};
```

### 设备类型枚举

```cpp
enum class DeviceType {
    kUnknown = 0,      // 未知设备
    kRasterImage = 1,  // 栅格图像设备（PNG/JPEG/BMP等）
    kPdf = 2,          // PDF矢量设备
    kTile = 3,         // 瓦片设备（大幅面分块渲染）
    kDisplay = 4,      // 显示设备（屏幕实时显示）
    kSvg = 5,          // SVG矢量设备
    kPrinter = 6       // 打印设备
};
```

### 设备状态枚举

```cpp
enum class DeviceState {
    kUninitialized,    // 未初始化
    kReady,            // 就绪
    kDrawing,          // 绘制中
    kError             // 错误
};
```

### 设备能力结构体

```cpp
struct DeviceCapabilities {
    bool supportsTransparency = true;
    bool supportsAntialiasing = true;
    bool supportsTextRotation = true;
    bool supportsGPUAcceleration = false;
    int maxTextureSize = 4096;
    int maxRenderTargets = 1;
    size_t maxMemorySize = 0;
    std::vector<ImageFormat> supportedFormats;
};
```

### 具体设备实现示例

```cpp
class RasterImageDevice : public DrawDevice {
public:
    std::unique_ptr<DrawEngine> CreateEngine() override {
        switch (m_preferredEngineType) {
            case EngineType::kGPU:
                return std::make_unique<GPUAcceleratedEngine>(this);
            case EngineType::kSimple2D:
            default:
                return std::make_unique<Simple2DEngine>(this);
        }
    }
    
    std::vector<EngineType> GetSupportedEngineTypes() const override {
        return { EngineType::kSimple2D, EngineType::kGPU };
    }
    
    DeviceCapabilities QueryCapabilities() const override {
        DeviceCapabilities caps;
        caps.supportsTransparency = true;
        caps.supportsAntialiasing = true;
        caps.supportsGPUAcceleration = (m_preferredEngineType == EngineType::kGPU);
        return caps;
    }
    
    bool SaveToFile(const std::string& filepath, ImageFormat format);
    uint8_t* GetData();
    void Resize(int width, int height);
};

class PdfDevice : public DrawDevice {
public:
    std::unique_ptr<DrawEngine> CreateEngine() override {
        return std::make_unique<VectorEngine>(this);
    }
    
    std::vector<EngineType> GetSupportedEngineTypes() const override {
        return { EngineType::kVector };
    }
    
    DeviceCapabilities QueryCapabilities() const override {
        DeviceCapabilities caps;
        caps.supportsTransparency = true;
        caps.supportsAntialiasing = true;
        caps.supportsGPUAcceleration = false;
        return caps;
    }
    
    void SetTitle(const std::string& title);
    void SetAuthor(const std::string& author);
    bool SaveToFile(const std::string& filepath);
};

class TileDevice : public DrawDevice {
public:
    std::unique_ptr<DrawEngine> CreateEngine() override {
        switch (m_preferredEngineType) {
            case EngineType::kTileAsync:
                return std::make_unique<AsyncTileEngine>(this);
            default:
                return std::make_unique<TileBasedEngine>(this);
        }
    }
    
    std::vector<EngineType> GetSupportedEngineTypes() const override {
        return { EngineType::kTile, EngineType::kTileAsync };
    }
    
    void SetTileSize(int size);
    DrawResult BeginTile(int x, int y, int z);
    DrawResult EndTile();
};

class DisplayDevice : public DrawDevice {
public:
    std::unique_ptr<DrawEngine> CreateEngine() override {
        if (m_preferredEngineType == EngineType::kGPU && IsGPUDriverAvailable()) {
            return std::make_unique<GPUAcceleratedEngine>(this);
        }
        return std::make_unique<Simple2DEngine>(this);
    }
    
    std::vector<EngineType> GetSupportedEngineTypes() const override {
        return { EngineType::kSimple2D, EngineType::kGPU };
    }
    
    void SetWindowHandle(void* handle);
    void Present();
    void SetVSync(bool enable);
};
```

---

## DrawEngine 接口定义

### 职责

DrawEngine 是渲染引擎层（参照Qt QPaintEngine设计），负责：
- **所有绘制操作（原子操作定义）**
- 坐标变换（世界坐标 ↔ 屏幕坐标）
- 裁剪区域计算与管理
- 渲染状态管理（变换栈、裁剪栈）
- 性能优化（批处理、缓存）

**重要**：所有绘制方法都在DrawEngine中定义，DrawDevice不包含任何绘制方法。

### 与Qt QPaintEngine对比

| 方面 | Qt QPaintEngine | 本设计 DrawEngine |
|------|-----------------|-------------------|
| 绘制方法 | drawPoints(), drawLines(), drawRects()... | DrawPoints(), DrawLines(), DrawRects()... |
| 变换支持 | updateState(), transform() | SetTransform(), WorldToScreen() |
| 裁剪支持 | setClipPath(), clipPath() | SetClipEnvelope(), GetClipEnvelope() |
| 设备绑定 | paintDevice() | GetDevice() |
| 职责 | 渲染实现 | 渲染实现 |

### 接口规范

```cpp
class DrawEngine {
public:
    virtual ~DrawEngine() = default;
    
    // ==================== 引擎信息 ====================
    
    virtual EngineType GetType() const = 0;
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    
    // ==================== 生命周期 ====================
    
    virtual DrawResult Initialize() = 0;
    virtual DrawResult Finalize() = 0;
    virtual bool IsInitialized() const = 0;
    
    // ==================== 设备绑定 ====================
    
    virtual void SetDevice(DrawDevicePtr device) = 0;
    virtual DrawDevicePtr GetDevice() const = 0;
    virtual bool HasDevice() const = 0;
    
    // ==================== 绘制会话 ====================
    
    virtual DrawResult BeginDraw(const DrawParams& params) = 0;
    virtual DrawResult EndDraw() = 0;
    virtual DrawResult AbortDraw() = 0;
    virtual bool IsDrawing() const = 0;
    
    // ==================== 变换管理 ====================
    
    virtual void SetTransform(const TransformMatrix& transform) = 0;
    virtual TransformMatrix GetTransform() const = 0;
    virtual void ResetTransform() = 0;
    virtual void MultiplyTransform(const TransformMatrix& transform) = 0;
    
    // ==================== 世界坐标变换 ====================
    
    virtual void SetWorldTransform(double offsetX, double offsetY, double scale) = 0;
    virtual void WorldToScreen(double worldX, double worldY, 
                                double& screenX, double& screenY) const = 0;
    virtual void ScreenToWorld(double screenX, double screenY, 
                                double& worldX, double& worldY) const = 0;
    
    // ==================== 裁剪管理 ====================
    
    virtual void SetClipEnvelope(const Envelope& envelope) = 0;
    virtual Envelope GetClipEnvelope() const = 0;
    virtual void ClearClipEnvelope() = 0;
    virtual bool IsClipped() const = 0;
    
    // ==================== 原子绘制操作（参照Qt QPaintEngine） ====================
    
    // 点绘制
    virtual DrawResult DrawPoints(const double* x, const double* y, int count, 
                                   const DrawStyle& style) = 0;
    virtual DrawResult DrawPoint(double x, double y, const DrawStyle& style) {
        return DrawPoints(&x, &y, 1, style);
    }
    
    // 线绘制
    virtual DrawResult DrawLines(const double* x1, const double* y1,
                                  const double* x2, const double* y2,
                                  int count, const DrawStyle& style) = 0;
    virtual DrawResult DrawLine(double x1, double y1, double x2, double y2, 
                                 const DrawStyle& style) {
        return DrawLines(&x1, &y1, &x2, &y2, 1, style);
    }
    
    // 矩形绘制
    virtual DrawResult DrawRects(const double* x, const double* y, 
                                  const double* w, const double* h,
                                  int count, const DrawStyle& style) = 0;
    virtual DrawResult DrawRect(double x, double y, double width, double height, 
                                 const DrawStyle& style) {
        return DrawRects(&x, &y, &width, &height, 1, style);
    }
    
    // 圆/椭圆绘制
    virtual DrawResult DrawEllipse(double cx, double cy, double rx, double ry, 
                                    const DrawStyle& style) = 0;
    virtual DrawResult DrawCircle(double cx, double cy, double radius, 
                                   const DrawStyle& style) {
        return DrawEllipse(cx, cy, radius, radius, style);
    }
    
    // 弧绘制
    virtual DrawResult DrawArc(double cx, double cy, double rx, double ry, 
                                double startAngle, double sweepAngle, 
                                const DrawStyle& style) = 0;
    
    // 多边形绘制
    virtual DrawResult DrawPolygon(const double* x, const double* y, int count, 
                                    const DrawStyle& style) = 0;
    
    // 折线绘制
    virtual DrawResult DrawPolyline(const double* x, const double* y, int count, 
                                     const DrawStyle& style) = 0;
    
    // ==================== 几何绘制（组合操作） ====================
    
    virtual DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) = 0;
    virtual DrawResult DrawGeometries(const std::vector<Geometry*>& geometries, 
                                       const DrawStyle& style) = 0;
    
    // ==================== 文本绘制 ====================
    
    virtual DrawResult DrawText(double x, double y, const std::string& text, 
                                 const Font& font, const Color& color) = 0;
    virtual DrawResult DrawTextWithBackground(double x, double y, const std::string& text,
                                               const Font& font, 
                                               const Color& textColor, const Color& bgColor) = 0;
    virtual DrawResult DrawTextAlongLine(const std::vector<double>& x, const std::vector<double>& y,
                                          const std::string& text, const Font& font, 
                                          const Color& color) = 0;
    
    // ==================== 图像绘制 ====================
    
    virtual DrawResult DrawImage(double x, double y, double width, double height,
                                  const uint8_t* data, int dataWidth, int dataHeight, 
                                  int channels) = 0;
    virtual DrawResult DrawImageRegion(double destX, double destY, double destWidth, double destHeight,
                                        const uint8_t* data, int dataWidth, int dataHeight, int channels,
                                        int srcX, int srcY, int srcWidth, int srcHeight) = 0;
    
    // ==================== 清空与填充 ====================
    
    virtual void Clear(const Color& color) = 0;
    virtual void Fill(const Color& color) = 0;
    virtual void Flush() = 0;
    
    // ==================== 渲染状态 ====================
    
    virtual void SetAntialiasing(bool enable) = 0;
    virtual bool IsAntialiasingEnabled() const = 0;
    virtual void SetOpacity(double opacity) = 0;
    virtual double GetOpacity() const = 0;
    
    // ==================== 视图信息 ====================
    
    virtual Envelope GetVisibleExtent() const = 0;
    virtual double GetScale() const = 0;
    virtual double GetDpi() const = 0;
    
    // ==================== 背景与容差 ====================
    
    virtual void SetBackground(const Color& color) = 0;
    virtual Color GetBackground() const = 0;
    virtual void SetTolerance(double tolerance) = 0;
    virtual double GetTolerance() const = 0;
    
    // ==================== 能力查询 ====================
    
    virtual bool SupportsFeature(const std::string& featureName) const = 0;
};
```

### 引擎类型枚举

```cpp
enum class EngineType {
    kUnknown = 0,      // 未知引擎
    kSimple2D = 1,     // 简单2D引擎（CPU软件渲染）
    kGPU = 2,          // GPU加速引擎
    kTile = 3,         // 瓦片渲染引擎
    kTileAsync = 4,    // 异步瓦片渲染引擎
    kVector = 5,       // 矢量渲染引擎
    kWebGL = 6         // WebGL引擎（WebAssembly）
};
```

### 具体引擎实现

```cpp
class Simple2DEngine : public DrawEngine {
public:
    EngineType GetType() const override { return EngineType::kSimple2D; }
    std::string GetName() const override { return "Simple2DEngine"; }
    
    // 原子操作实现
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override {
        // CPU软件渲染：逐点绘制
        for (int i = 0; i < count; ++i) {
            double sx, sy;
            WorldToScreen(x[i], y[i], sx, sy);
            // 调用底层图形库（如QPainter、GDI+等）绘制点
        }
        return DrawResult::kSuccess;
    }
    
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2,
                          int count, const DrawStyle& style) override {
        // CPU软件渲染：逐线绘制
        for (int i = 0; i < count; ++i) {
            double sx1, sy1, sx2, sy2;
            WorldToScreen(x1[i], y1[i], sx1, sy1);
            WorldToScreen(x2[i], y2[i], sx2, sy2);
            // 调用底层图形库绘制线
        }
        return DrawResult::kSuccess;
    }
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override {
        // 组合操作：根据几何类型调用原子操作
        switch (geometry->GetType()) {
            case GeometryType::kPoint:
                return DrawPoints(...);
            case GeometryType::kLineString:
                return DrawPolyline(...);
            case GeometryType::kPolygon:
                return DrawPolygon(...);
            // ...
        }
    }
};

class GPUAcceleratedEngine : public DrawEngine {
public:
    EngineType GetType() const override { return EngineType::kGPU; }
    std::string GetName() const override { return "GPUAcceleratedEngine"; }
    
    // 原子操作实现（GPU批量渲染）
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override {
        // GPU硬件加速：批量提交顶点缓冲
        auto buffer = m_resourceManager.AcquireVertexBuffer(count);
        // 填充顶点数据
        // 提交GPU绘制指令
        return DrawResult::kSuccess;
    }
    
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2,
                          int count, const DrawStyle& style) override {
        // GPU批量线绘制
        return DrawResult::kSuccess;
    }
    
private:
    GPUResourceManager& m_resourceManager;
};

class TileBasedEngine : public DrawEngine {
public:
    EngineType GetType() const override { return EngineType::kTile; }
    std::string GetName() const override { return "TileBasedEngine"; }
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override {
        // 瓦片渲染实现
        // 1. 计算几何涉及的瓦片
        auto tiles = CalculateInvolvedTiles(geometry);
        // 2. 分瓦片渲染
        for (const auto& tile : tiles) {
            BeginTile(tile.x, tile.y, tile.z);
            // 调用原子操作绘制
            DrawPolygon(...);
            EndTile();
        }
        // 3. 合并结果
        return DrawResult::kSuccess;
    }
};
```

### VectorEngine 继承体系

矢量引擎采用基类+派生类设计，不同矢量格式适配不同的第三方库：

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     VectorEngine 继承体系                               │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    VectorEngine (基类)                           │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ 公共接口：                                                        │   │
│  │ • DrawPoints/DrawLines/DrawPolygon (原子操作)                    │   │
│  │ • DrawText/DrawImage                                             │   │
│  │ • SetTransform/SetClip/SetStyle                                  │   │
│  │                                                                 │   │
│  │ 公共实现：                                                        │   │
│  │ • 坐标变换计算                                                    │   │
│  │ • 裁剪区域计算                                                    │   │
│  │ • 几何简化算法                                                    │   │
│  │                                                                 │   │
│  │ 纯虚函数（子类实现）：                                            │   │
│  │ • WritePath() - 写入路径数据                                     │   │
│  │ • WriteText() - 写入文本                                         │   │
│  │ • FlushPage() - 刷新页面                                         │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│              ┌───────────────┴───────────────┐                         │
│              ▼                               ▼                         │
│  ┌───────────────────────┐     ┌───────────────────────┐              │
│  │     PdfEngine         │     │      SvgEngine        │              │
│  │───────────────────────│     │───────────────────────│              │
│  │ 底层库：podofo        │     │ 底层库：cairo/libsvg  │              │
│  │                       │     │                       │              │
│  │ 特有功能：            │     │ 特有功能：            │              │
│  │ • 多页支持            │     │ • CSS样式             │              │
│  │ • 书签/链接           │     │ • 动画支持            │              │
│  │ • 注释/表单           │     │ • 脚本支持            │              │
│  │ • PDF/A归档           │     │ • 嵌入字体            │              │
│  └───────────────────────┘     └───────────────────────┘              │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

```cpp
enum class VectorFormat {
    kUnknown = 0,
    kPDF = 1,
    kSVG = 2,
    kEMF = 3,
    kEPS = 4
};

struct DocumentMetadata {
    std::string title;
    std::string author;
    std::string subject;
    std::string keywords;
    std::string creator;
    std::string producer;
};

class VectorEngine : public DrawEngine {
public:
    EngineType GetType() const override { return EngineType::kVector; }
    
    virtual VectorFormat GetFormat() const = 0;
    
    virtual void BeginPage(int width, int height) = 0;
    virtual void EndPage() = 0;
    virtual int GetPageCount() const = 0;
    
    virtual void SetMetadata(const DocumentMetadata& metadata) = 0;
    virtual void SetCompression(bool enable) = 0;

protected:
    void TransformCoordinates(double& x, double& y);
    bool IsVisible(const Envelope& bounds);
    void SimplifyGeometry(const Geometry* geometry, double tolerance);
    
    TransformMatrix m_transform;
    Envelope m_clipEnvelope;
    double m_tolerance = 0.1;
};

class PdfEngine : public VectorEngine {
public:
    VectorFormat GetFormat() const override { return VectorFormat::kPDF; }
    std::string GetName() const override { return "PdfEngine"; }
    
    void BeginPage(int width, int height) override;
    void EndPage() override;
    int GetPageCount() const override;
    
    void SetMetadata(const DocumentMetadata& metadata) override;
    void SetCompression(bool enable) override;
    
    void AddBookmark(const std::string& title, int pageIndex);
    void AddLink(double x, double y, double w, double h, const std::string& url);
    void AddAnnotation(double x, double y, double w, double h, 
                       const std::string& content, AnnotationType type);
    void SetPDFVersion(PDFVersion version);
    void EnablePDFA(bool enable);

protected:
    void WritePath(const Geometry* geometry) override;
    void WriteText(double x, double y, const std::string& text, 
                   const Font& font, const Color& color) override;
    void FlushPage() override;

private:
    PoDoFo::PdfMemDocument* m_document = nullptr;
    PoDoFo::PdfPainter* m_painter = nullptr;
    PoDoFo::PdfPage* m_currentPage = nullptr;
    std::vector<PoDoFo::PdfPage*> m_pages;
};

class SvgEngine : public VectorEngine {
public:
    VectorFormat GetFormat() const override { return VectorFormat::kSVG; }
    std::string GetName() const override { return "SvgEngine"; }
    
    void BeginPage(int width, int height) override;
    void EndPage() override;
    int GetPageCount() const override { return 1; }  // SVG单页
    
    void SetMetadata(const DocumentMetadata& metadata) override;
    void SetCompression(bool enable) override;
    
    void SetCSSStyle(const std::string& className, const std::string& style);
    void AddAnimation(const AnimationDef& animation);
    void SetScript(const std::string& script);
    void SetEmbedFonts(bool embed);

protected:
    void WritePath(const Geometry* geometry) override;
    void WriteText(double x, double y, const std::string& text, 
                   const Font& font, const Color& color) override;
    void FlushPage() override;

private:
    cairo_t* m_cairoContext = nullptr;
    cairo_surface_t* m_surface = nullptr;
    std::stringstream m_svgContent;
    std::map<std::string, std::string> m_cssStyles;
};
```

#### 设备与引擎绑定关系

| 设备类型 | 推荐引擎 | 底层库 | 特性支持 |
|----------|----------|--------|----------|
| PdfDevice | PdfEngine | podofo | 多页、书签、链接、注释、PDF/A |
| SvgDevice | SvgEngine | cairo | CSS样式、动画、脚本、嵌入字体 |

---

## DrawContext 接口定义

### 职责

DrawContext 是绘制上下文层（参照Qt QPainter设计），负责：
- 绘制上下文管理与生命周期控制
- 状态栈管理（Transform/Style/Opacity/Clip）
- 绘制API封装与参数验证
- 设备/引擎协调绑定
- **运行时引擎切换（增强版方案A核心）**
- 默认样式、字体、颜色管理

**重要**：DrawContext是用户面向的API层，所有绘制调用都通过Context转发给Engine执行。

### 与Qt QPainter对比

| 方面 | Qt QPainter | 本设计 DrawContext |
|------|-------------|-------------------|
| 状态管理 | save()/restore() | Save(flags)/Restore() |
| 绘制API | drawPoint(), drawLine()... | DrawPoint(), DrawLine()... |
| 设备绑定 | begin(QPaintDevice*) | SetDevice(DrawDevicePtr) |
| 引擎访问 | paintEngine() | GetEngine() |
| 职责 | 用户API封装 | 用户API封装 |

### 接口规范

```cpp
class DrawContext {
public:
    // ==================== 构造与析构 ====================
    
    DrawContext();
    explicit DrawContext(DrawDevicePtr device);
    ~DrawContext();
    
    // ==================== 生命周期 ====================
    
    DrawResult Initialize();
    DrawResult Finalize();
    bool IsInitialized() const;
    
    // ==================== 设备管理（参照QPainter.begin()） ====================
    
    void SetDevice(DrawDevicePtr device) {
        m_device = device;
        if (device) {
            m_engine = device->CreateEngine();  // 自动创建引擎
        }
    }
    
    DrawDevicePtr GetDevice() const;
    bool HasDevice() const;
    
    // ==================== 引擎管理 ====================
    
    void SetEngine(DrawEnginePtr engine);
    DrawEnginePtr GetEngine() const;
    bool HasEngine() const;
    
    // ==================== 运行时引擎切换（增强版特性） ====================
    
    bool SetEngineType(EngineType type) {
        if (!m_device) return false;
        
        auto supported = m_device->GetSupportedEngineTypes();
        if (std::find(supported.begin(), supported.end(), type) == supported.end()) {
            return false;
        }
        
        // 保存当前状态
        StateSerializer::EngineState state;
        if (m_engine) {
            state = StateSerializer::Capture(m_engine.get());
        }
        
        // 切换引擎
        m_device->SetPreferredEngineType(type);
        m_engine = m_device->CreateEngine();
        
        // 恢复状态
        if (m_engine) {
            StateSerializer::Restore(m_engine.get(), state);
        }
        
        return true;
    }
    
    // ==================== 绘制会话 ====================
    
    DrawResult BeginDraw(const DrawParams& params);
    DrawResult EndDraw();
    DrawResult AbortDraw();
    bool IsDrawing() const;
    
    // ==================== 状态栈管理（参照QPainter.save()/restore()） ====================
    
    enum class StateFlag : uint32_t {
        kNone       = 0,
        kTransform  = 1 << 0,   // 变换矩阵
        kStyle      = 1 << 1,   // 绘制样式
        kOpacity    = 1 << 2,   // 透明度
        kClip       = 1 << 3,   // 裁剪区域
        kAll        = 0xFFFFFFFF  // 全部状态
    };
    
    using StateFlags = uint32_t;
    
    void Save(StateFlags flags = StateFlag::kAll);
    void Restore();
    
    // ==================== 变换操作 ====================
    
    void SetTransform(const TransformMatrix& transform);
    TransformMatrix GetTransform() const;
    void MultiplyTransform(const TransformMatrix& transform);
    void ResetTransform();
    void Translate(double tx, double ty);
    void Scale(double sx, double sy);
    void Rotate(double angle);
    
    // ==================== 裁剪操作 ====================
    
    void SetClipRect(double x, double y, double width, double height);
    void ClearClipRect();
    Envelope GetClipRect() const;
    
    // ==================== 样式操作 ====================
    
    void SetStyle(const DrawStyle& style);
    DrawStyle GetStyle() const;
    
    // ==================== 透明度操作 ====================
    
    void SetOpacity(double opacity);
    double GetOpacity() const;
    
    // ==================== 几何绘制（转发给Engine） ====================
    
    DrawResult DrawGeometry(const Geometry* geometry) {
        return m_engine->DrawGeometry(geometry, m_currentStyle);
    }
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) {
        return m_engine->DrawGeometry(geometry, style);
    }
    
    // ==================== 基本图元绘制（转发给Engine） ====================
    
    DrawResult DrawPoint(double x, double y) {
        return m_engine->DrawPoint(x, y, m_currentStyle);
    }
    DrawResult DrawLine(double x1, double y1, double x2, double y2) {
        return m_engine->DrawLine(x1, y1, x2, y2, m_currentStyle);
    }
    DrawResult DrawRect(double x, double y, double width, double height) {
        return m_engine->DrawRect(x, y, width, height, m_currentStyle);
    }
    DrawResult DrawCircle(double cx, double cy, double radius) {
        return m_engine->DrawCircle(cx, cy, radius, m_currentStyle);
    }
    DrawResult DrawPolygon(const double* x, const double* y, int count) {
        return m_engine->DrawPolygon(x, y, count, m_currentStyle);
    }
    DrawResult DrawPolyline(const double* x, const double* y, int count) {
        return m_engine->DrawPolyline(x, y, count, m_currentStyle);
    }
    
    // ==================== 文本绘制（转发给Engine） ====================
    
    DrawResult DrawText(double x, double y, const std::string& text) {
        return m_engine->DrawText(x, y, text, m_defaultFont, m_defaultColor);
    }
    DrawResult DrawText(double x, double y, const std::string& text, 
                         const Font& font, const Color& color) {
        return m_engine->DrawText(x, y, text, font, color);
    }
    
    // ==================== 图像绘制（转发给Engine） ====================
    
    DrawResult DrawImage(double x, double y, double width, double height,
                          const uint8_t* data, int dataWidth, int dataHeight, int channels) {
        return m_engine->DrawImage(x, y, width, height, data, dataWidth, dataHeight, channels);
    }
    
    // ==================== 清空与刷新 ====================
    
    void Clear(const Color& color);
    void Flush();
    
    // ==================== 坐标转换（转发给Engine） ====================
    
    void WorldToScreen(double worldX, double worldY, 
                        double& screenX, double& screenY) const {
        m_engine->WorldToScreen(worldX, worldY, screenX, screenY);
    }
    void ScreenToWorld(double screenX, double screenY, 
                        double& worldX, double& worldY) const {
        m_engine->ScreenToWorld(screenX, screenY, worldX, worldY);
    }
    
    // ==================== 视图信息 ====================
    
    Envelope GetVisibleExtent() const;
    double GetScale() const;
    double GetDpi() const;
    const DrawParams& GetDrawParams() const;
    
    // ==================== 默认值管理 ====================
    
    void SetBackground(const Color& color);
    Color GetBackground() const;
    void SetDefaultStyle(const DrawStyle& style);
    DrawStyle GetDefaultStyle() const;
    void SetDefaultFont(const Font& font);
    Font GetDefaultFont() const;
    void SetDefaultColor(const Color& color);
    Color GetDefaultColor() const;
    
    // ==================== 工厂方法 ====================
    
    static DrawContextPtr Create(DrawDevicePtr device);

private:
    DrawDevicePtr m_device;
    DrawEnginePtr m_engine;
    DrawParams m_params;
    DrawStyle m_defaultStyle;
    Font m_defaultFont;
    Color m_defaultColor;
    Color m_background;
    bool m_initialized;
    bool m_drawing;
    
    std::stack<TransformMatrix> m_transformStack;
    std::stack<DrawStyle> m_styleStack;
    std::stack<double> m_opacityStack;
    int m_clipRectDepth;
};
```

---

## 三者关系详解

### 调用链关系

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      调用链关系（参照Qt设计）                            │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  用户代码                                                               │
│      │                                                                  │
│      ▼                                                                  │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ DrawContext::DrawGeometry(geometry)                             │   │
│  │                                                                 │   │
│  │   // 合并样式                                                   │   │
│  │   DrawStyle effectiveStyle = m_defaultStyle.Merge(style);       │   │
│  │                                                                 │   │
│  │   // 转发给Engine执行（不直接调用Device）                        │   │
│  │   if (m_engine) {                                               │   │
│  │       return m_engine->DrawGeometry(geometry, effectiveStyle);  │   │
│  │   }                                                             │   │
│  │                                                                 │   │
│  │   return DrawResult::kEngineNotReady;                           │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ DrawEngine::DrawGeometry(geometry, style)                       │   │
│  │                                                                 │   │
│  │   // 1. 坐标变换                                                │   │
│  │   WorldToScreen(worldX, worldY, screenX, screenY);              │   │
│  │                                                                 │   │
│  │   // 2. 裁剪检测                                                │   │
│  │   if (IsClipped() && !IsVisible(geometry)) return;              │   │
│  │                                                                 │   │
│  │   // 3. 几何简化                                                │   │
│  │   SimplifyGeometry(geometry, tolerance);                        │   │
│  │                                                                 │   │
│  │   // 4. 调用原子操作绘制（Engine内部实现）                       │   │
│  │   switch (geometry->GetType()) {                                │   │
│  │       case GeometryType::Point:                                 │   │
│  │           DrawPoints(...);                                      │   │
│  │       case GeometryType::LineString:                            │   │
│  │           DrawPolyline(...);                                    │   │
│  │       case GeometryType::Polygon:                               │   │
│  │           DrawPolygon(...);                                     │   │
│  │   }                                                             │   │
│  │                                                                 │   │
│  │   // 注意：Engine不调用Device的绘制方法                         │   │
│  │   // Engine直接执行原子操作，通过底层图形库（QPainter等）实现    │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ DrawDevice（仅提供设备信息和引擎创建）                           │   │
│  │                                                                 │   │
│  │   // Device不参与绘制调用链                                     │   │
│  │   // 职责：提供GetWidth()、GetHeight()、GetDpi()等设备信息       │   │
│  │   // 职责：CreateEngine()创建合适的引擎实例                      │   │
│  │                                                                 │   │
│  │   // Engine通过GetDevice()获取设备信息，但不调用Device绘制       │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  关键说明：                                                              │
│  • DrawContext → DrawEngine（绘制调用）                                 │
│  • DrawEngine → 底层图形库（QPainter/GDI+/GPU API）                     │
│  • DrawDevice → 仅提供设备信息和引擎创建                                │
│  • Device不参与绘制调用链，Engine直接操作底层图形库                     │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 职责边界

| 组件 | 核心职责 | 不负责 |
|------|----------|--------|
| **DrawContext** | 状态栈管理、API封装、参数验证、引擎切换 | 具体绘制逻辑、坐标变换 |
| **DrawEngine** | 所有绘制操作（原子操作）、坐标变换、裁剪计算、几何简化 | 设备管理、业务逻辑 |
| **DrawDevice** | 设备信息、生命周期管理、引擎创建、能力查询 | 绘制操作、坐标变换、裁剪计算 |

### 数据流向

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           数据流向                                       │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  用户数据                                                               │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ Geometry（几何数据）                                             │   │
│  │ DrawStyle（样式数据）                                            │   │
│  │ TransformMatrix（变换矩阵）                                      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ DrawContext                                                      │   │
│  │ • 合并默认样式与用户样式                                         │   │
│  │ • 应用状态栈（变换、透明度、裁剪）                               │   │
│  │ • 选择引擎（CPU/GPU/矢量）                                       │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ DrawEngine                                                       │   │
│  │ • 世界坐标 → 屏幕坐标变换                                        │   │
│  │ • 裁剪区域计算                                                   │   │
│  │ • 几何简化/优化                                                  │   │
│  │ • 执行原子绘制操作（DrawPoints/DrawLines/DrawPolygon等）         │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 底层图形库                                                       │   │
│  │ • QPainter（Qt绑定）                                             │   │
│  │ • GDI+/Direct2D（Windows）                                       │   │
│  │ • OpenGL/Vulkan（GPU加速）                                       │   │
│  │ • podofo/cairo（矢量输出）                                       │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ DrawDevice（设备抽象）                                           │   │
│  │ • 提供设备信息（尺寸、DPI、颜色深度）                            │   │
│  │ • 管理输出目标（文件/窗口/打印）                                 │   │
│  │ • 不参与绘制操作                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  输出结果                                                               │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 图像文件（PNG/JPEG/PDF/SVG）                                     │   │
│  │ 屏幕显示                                                         │   │
│  │ 打印输出                                                         │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 辅助组件设计

### 1. GPUResourceManager（GPU资源管理器）

```cpp
class GPUResourceManager {
public:
    static GPUResourceManager& Instance();
    
    TextureHandle GetOrCreateTexture(const std::string& path);
    BufferHandle GetOrCreateBuffer(size_t size);
    ShaderHandle GetOrCreateShader(const std::string& vertexSrc, 
                                    const std::string& fragmentSrc);
    
    void ClearCache();
    size_t GetMemoryUsage() const;
    
private:
    std::unordered_map<std::string, TextureHandle> m_textureCache;
    std::unordered_map<size_t, BufferHandle> m_bufferPool;
    std::unordered_map<std::string, ShaderHandle> m_shaderCache;
    std::mutex m_mutex;
};
```

### 2. EnginePool（引擎池）

```cpp
class EnginePool {
public:
    static EnginePool& Instance();
    
    std::shared_ptr<DrawEngine> GetOrCreateEngine(
        EngineType type, 
        std::shared_ptr<DrawDevice> device
    );
    
    void ReleaseEngine(EngineType type);
    void Clear();
    
private:
    std::map<std::pair<EngineType, DeviceType>, std::shared_ptr<DrawEngine>> m_pool;
    std::mutex m_mutex;
};
```

### 3. StateSerializer（状态序列化器）

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
    };
    
    static EngineState Capture(const DrawEngine* engine);
    static void Restore(DrawEngine* engine, const EngineState& state);
};
```

### 4. ThreadSafeEngine（线程安全包装）

```cpp
template<typename EngineT>
class ThreadSafeEngine : public DrawEngine {
public:
    explicit ThreadSafeEngine(std::unique_ptr<EngineT> engine)
        : m_engine(std::move(engine)) {}
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_engine->DrawGeometry(geometry, style);
    }
    
    bool IsThreadSafe() const override { return true; }
    
private:
    std::unique_ptr<EngineT> m_engine;
    std::mutex m_mutex;
};
```

---

## 使用示例

### 基本使用

```cpp
auto device = RasterImageDevice::Create(1920, 1080, 4);
auto context = DrawContext::Create(device);

context->Initialize();
context->BeginDraw(DrawParams());
context->Clear(Color(255, 255, 255));
context->DrawRect(100, 100, 500, 300, DrawStyle().SetFillColor(Color(0, 128, 255)));
context->EndDraw();

device->SaveToFile("output.png", ImageFormat::kPNG);
```

### 运行时切换引擎

```cpp
auto device = DisplayDevice::Create(window);
auto context = DrawContext::Create(device);

// 检查支持的引擎类型
auto supported = device->GetSupportedEngineTypes();

// 运行时切换到GPU引擎
if (context->SetEngineType(EngineType::kGPU)) {
    std::cout << "切换到GPU引擎成功" << std::endl;
} else {
    // 回退到CPU引擎
    context->SetEngineType(EngineType::kSimple2D);
}

context->BeginDraw(DrawParams());
// ... 绘制操作
context->EndDraw();
```

### 状态栈使用

```cpp
auto context = DrawContext::Create(device);
context->BeginDraw(DrawParams());

// 方式1：保存全部状态（与Qt QPainter一致）
context->Save();  // 默认保存全部状态
context->Translate(100, 100);
context->Rotate(45);
context->SetOpacity(0.5);
context->DrawRect(0, 0, 200, 100, DrawStyle().SetFillColor(Color(255, 0, 0)));
context->Restore();  // 恢复全部状态

// 方式2：只保存变换状态（性能优化）
context->Save(DrawContext::StateFlag::kTransform);
context->Translate(200, 200);
context->DrawRect(0, 0, 100, 50, DrawStyle().SetFillColor(Color(0, 255, 0)));
context->Restore();  // 只恢复变换，样式和透明度保持不变

// 方式3：保存多个状态（位掩码组合）
context->Save(DrawContext::StateFlag::kTransform | DrawContext::StateFlag::kStyle);
context->Translate(300, 300);
context->SetStyle(DrawStyle().SetStrokeColor(Color(0, 0, 255)));
context->DrawRect(0, 0, 80, 40);
context->Restore();  // 恢复变换和样式

context->EndDraw();
```

### 状态标志说明

| 状态标志 | 说明 | 保存内容 |
|----------|------|----------|
| `StateFlag::kNone` | 不保存任何状态 | - |
| `StateFlag::kTransform` | 变换矩阵 | 平移、缩放、旋转 |
| `StateFlag::kStyle` | 绘制样式 | 填充色、边框色、线宽等 |
| `StateFlag::kOpacity` | 透明度 | 当前透明度值 |
| `StateFlag::kClip` | 裁剪区域 | 当前裁剪矩形 |
| `StateFlag::kAll` | 全部状态 | 以上所有状态 |

---

## 错误处理与异常安全

### 统一错误处理策略

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      统一错误处理策略                                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  原则：                                                                  │
│  1. 所有公共API返回DrawResult枚举，不抛出异常                           │
│  2. 内部实现可使用异常，但在API边界转换为DrawResult                     │
│  3. 提供辅助函数简化错误检查                                            │
│                                                                         │
│  错误处理模式：                                                          │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 模式1: 直接检查                                                  │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ if (context->DrawGeometry(geom, style) != DrawResult::kSuccess)│   │
│  │     HandleError();                                              │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 模式2: 使用DRAW_CHECK宏                                         │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ DRAW_CHECK(context->DrawGeometry(geom, style));                 │   │
│  │ // 自动记录错误位置和上下文                                      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 模式3: 使用DrawScopeGuard                                       │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ DrawScopeGuard guard(context);                                  │   │
│  │ guard.DrawGeometry(geom, style);                                │   │
│  │ if (!guard) { /* 检查是否有错误 */ }                            │   │
│  │ guard.Commit(); // 成功时提交                                   │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 错误码定义

```cpp
enum class DrawResult {
    kSuccess = 0,           // 操作成功
    kDeviceNotReady = 1,    // 设备未就绪
    kEngineNotReady = 2,    // 引擎未就绪
    kInvalidParameter = 3,  // 参数无效
    kOutOfMemory = 4,       // 内存不足
    kGpuDeviceLost = 5,     // GPU设备丢失
    kUnsupportedOperation = 6,  // 不支持的操作
    kCancelled = 7,         // 用户取消
    kTimeout = 8,           // 操作超时
    kResourceExhausted = 9  // 资源耗尽
};

inline bool Succeeded(DrawResult result) {
    return result == DrawResult::kSuccess;
}

inline bool Failed(DrawResult result) {
    return result != DrawResult::kSuccess;
}

std::string GetResultMessage(DrawResult result);

#define DRAW_CHECK(expr) \
    do { \
        DrawResult _result = (expr); \
        if (_result != DrawResult::kSuccess) { \
            DRAW_LOG_ERROR("Draw operation failed: {} at {}:{}", \
                GetResultMessage(_result), __FILE__, __LINE__); \
        } \
    } while(0)

#define DRAW_CHECK_RETURN(expr, retval) \
    do { \
        DrawResult _result = (expr); \
        if (_result != DrawResult::kSuccess) { \
            return retval; \
        } \
    } while(0)
```

### DrawScopeGuard辅助类

```cpp
class DrawScopeGuard {
public:
    explicit DrawScopeGuard(DrawContext* context) 
        : m_context(context), m_committed(false) {}
    
    ~DrawScopeGuard() {
        if (!m_committed && m_context) {
            m_context->AbortDraw();
        }
    }
    
    DrawScopeGuard(const DrawScopeGuard&) = delete;
    DrawScopeGuard& operator=(const DrawScopeGuard&) = delete;
    
    template<typename... Args>
    DrawResult DrawGeometry(Args&&... args) {
        auto result = m_context->DrawGeometry(std::forward<Args>(args)...);
        if (Failed(result)) {
            m_lastError = result;
        }
        return result;
    }
    
    bool Ok() const { return m_lastError == DrawResult::kSuccess; }
    DrawResult GetLastError() const { return m_lastError; }
    
    void Commit() { m_committed = true; }
    
private:
    DrawContext* m_context;
    DrawResult m_lastError = DrawResult::kSuccess;
    bool m_committed;
};
```

### 异常安全保证

| 组件 | 异常安全级别 | 说明 |
|------|-------------|------|
| DrawDevice | 基本异常安全 | 异常后设备状态有效，资源不泄漏 |
| DrawEngine | 强异常安全 | 操作失败时状态回滚 |
| DrawContext | 强异常安全 | 状态栈操作具备事务性 |
| GPUResourceManager | 无异常 | 所有操作返回错误码 |

### 设备丢失恢复

```cpp
class DrawDevice {
public:
    virtual bool IsDeviceLost() const = 0;
    virtual DrawResult RecoverDevice() = 0;
    virtual void OnDeviceLost(std::function<void()> callback) = 0;
};

// GPU设备丢失处理流程
void HandleGpuDeviceLost(DrawContext* context) {
    // 1. 检测设备丢失
    if (context->GetDevice()->IsDeviceLost()) {
        // 2. 释放GPU资源
        GPUResourceManager::Instance().ClearCache();
        // 3. 尝试恢复
        auto result = context->GetDevice()->RecoverDevice();
        // 4. 恢复失败则回退到CPU引擎
        if (result != DrawResult::kSuccess) {
            context->SetEngineType(EngineType::kSimple2D);
        }
    }
}
```

### 资源管理策略

```cpp
// RAII绘制会话
class DrawSession {
public:
    DrawSession(DrawContext* context, const DrawParams& params)
        : m_context(context), m_active(true) {
        m_result = context->BeginDraw(params);
    }
    
    ~DrawSession() {
        if (m_active) {
            m_context->EndDraw();
        }
    }
    
    DrawResult GetResult() const { return m_result; }
    void Cancel() { m_context->AbortDraw(); m_active = false; }
    
private:
    DrawContext* m_context;
    DrawResult m_result;
    bool m_active;
};
```

---

## 性能指标与基准测试

### 目标性能指标

| 场景 | 目标性能 | 测试条件 | 优先级 |
|------|----------|----------|--------|
| 简单几何绘制 | > 100万点/秒 | Simple2DEngine, CPU i7-12700 | 高 |
| GPU加速绘制 | > 1000万点/秒 | GPUAcceleratedEngine, RTX 3080 | 高 |
| 瓦片渲染延迟 | < 100ms/瓦片 | TileBasedEngine, 256×256瓦片 | 高 |
| 内存占用 | < 500MB | 100万几何对象 | 中 |
| 首帧渲染 | < 50ms | 中等复杂度场景 | 中 |
| 引擎切换延迟 | < 10ms | SetEngineType() | 低 |

### 性能对比基准

| 引擎类型 | 点绘制 | 线绘制 | 多边形填充 | 文本渲染 |
|----------|--------|--------|-----------|----------|
| Simple2DEngine | 基准(1x) | 基准(1x) | 基准(1x) | 基准(1x) |
| GPUAcceleratedEngine | 10-50x | 5-20x | 20-100x | 2-5x |
| VectorEngine | 0.8x | 0.9x | 1.2x | 1.0x |

### 性能优化策略

```cpp
// 1. 批处理优化
class BatchRenderer {
public:
    void AddGeometry(const Geometry* geom, const DrawStyle& style);
    void Flush();  // 批量提交
};

// 2. LOD（细节层次）优化
class LODStrategy {
public:
    double GetSimplifyTolerance(double scale) const {
        return m_baseTolerance / scale;
    }
};

// 3. 缓存策略
class RenderCache {
public:
    TextureHandle GetCachedGeometry(const Geometry* geom);
    void Invalidate(const Envelope& extent);
};
```

---

## 线程安全设计

### 线程安全保证级别

| 组件 | 线程安全级别 | 说明 | 使用建议 |
|------|-------------|------|----------|
| DrawContext | 非线程安全 | 无内部同步 | 每线程独立实例 |
| DrawEngine | 可选线程安全 | ThreadSafeEngine包装 | 单引擎多线程需包装 |
| DrawDevice | 非线程安全 | 无内部同步 | 单线程访问或外部同步 |
| GPUResourceManager | 线程安全 | 内部互斥锁 | 全局单例，安全调用 |
| EnginePool | 线程安全 | 内部读写锁 | 全局单例，安全调用 |

### 并发绘制模式

```
┌─────────────────────────────────────────────────────────────────────────┐
│                         并发绘制模式                                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  模式1: 多Context模式（推荐）                                            │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                                │
│  │Thread 1 │  │Thread 2 │  │Thread 3 │                                │
│  │Context 1│  │Context 2│  │Context 3 │                                │
│  └────┬────┘  └────┬────┘  └────┬────┘                                │
│       │            │            │                                       │
│       └────────────┼────────────┘                                       │
│                    ▼                                                    │
│            ┌──────────────┐                                             │
│            │ Shared Device│  ← 需外部同步                               │
│            └──────────────┘                                             │
│                                                                         │
│  模式2: 多Device模式（最高性能）                                         │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                                │
│  │Thread 1 │  │Thread 2 │  │Thread 3 │                                │
│  │Context 1│  │Context 2│  │Context 3 │                                │
│  │Device 1 │  │Device 2 │  │Device 3 │  ← 无共享，无需同步             │
│  └─────────┘  └─────────┘  └─────────┘                                │
│                                                                         │
│  模式3: 任务队列模式（简化同步）                                         │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐                                │
│  │Thread 1 │  │Thread 2 │  │Thread 3 │                                │
│  └────┬────┘  └────┬────┘  └────┬────┘                                │
│       │            │            │                                       │
│       └────────────┼────────────┘                                       │
│                    ▼                                                    │
│            ┌──────────────┐                                             │
│            │ Task Queue   │                                             │
│            └──────┬───────┘                                             │
│                   ▼                                                     │
│            ┌──────────────┐                                             │
│            │Render Thread │  ← 单线程渲染                               │
│            │Context+Device│                                             │
│            └──────────────┘                                             │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 线程安全引擎包装

```cpp
template<typename EngineT>
class ThreadSafeEngine : public DrawEngine {
public:
    explicit ThreadSafeEngine(std::unique_ptr<EngineT> engine)
        : m_engine(std::move(engine)) {}
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_engine->DrawGeometry(geometry, style);
    }
    
    bool IsThreadSafe() const override { return true; }
    
private:
    std::unique_ptr<EngineT> m_engine;
    mutable std::mutex m_mutex;
};
```

### 线程安全使用示例

```cpp
// 示例1: 多线程渲染到同一设备（需要ThreadSafeEngine包装）
void MultiThreadRenderExample() {
    auto device = std::make_unique<RasterImageDevice>(1920, 1080);
    auto engine = device->CreateEngine();
    
    auto threadSafeEngine = std::make_unique<ThreadSafeEngine<DrawEngine>>(std::move(engine));
    
    DrawContext context(device.get());
    context.SetEngine(std::move(threadSafeEngine));
    
    std::vector<std::thread> threads;
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&context, i]() {
            DrawStyle style;
            style.color = Color::FromHSL(i * 90.0, 0.8, 0.5);
            
            for (int j = 0; j < 1000; ++j) {
                double x = (i * 250 + j % 250);
                double y = (j / 250) * 10;
                context.DrawPoint(x, y, style);
            }
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
}

// 示例2: 每线程独立设备（推荐方式，无需同步）
void PerThreadDeviceExample() {
    const int threadCount = 4;
    std::vector<std::unique_ptr<DrawContext>> contexts;
    std::vector<std::unique_ptr<RasterImageDevice>> devices;
    
    for (int i = 0; i < threadCount; ++i) {
        auto device = std::make_unique<RasterImageDevice>(480, 270);
        auto context = std::make_unique<DrawContext>(device.get());
        devices.push_back(std::move(device));
        contexts.push_back(std::move(context));
    }
    
    std::vector<std::thread> threads;
    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&contexts, i]() {
            DrawStyle style;
            style.color = Color::FromHSL(i * 90.0, 0.8, 0.5);
            
            contexts[i]->BeginDraw();
            for (int j = 0; j < 10000; ++j) {
                double x = j % 480;
                double y = j / 480;
                contexts[i]->DrawPoint(x, y, style);
            }
            contexts[i]->EndDraw();
        });
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    // 合并结果
    // MergeTiles(contexts);
}

// 示例3: 生产者-消费者模式（异步渲染）
class AsyncRenderQueue {
public:
    void Submit(std::function<void(DrawContext*)> task) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_tasks.push(std::move(task));
        m_condition.notify_one();
    }
    
    void Start(DrawContext* context, int workerCount = 2) {
        m_running = true;
        for (int i = 0; i < workerCount; ++i) {
            m_workers.emplace_back([this, context]() {
                while (m_running || !m_tasks.empty()) {
                    std::function<void(DrawContext*)> task;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_condition.wait(lock, [this] {
                            return !m_tasks.empty() || !m_running;
                        });
                        if (!m_tasks.empty()) {
                            task = std::move(m_tasks.front());
                            m_tasks.pop();
                        }
                    }
                    if (task) {
                        task(context);
                    }
                }
            });
        }
    }
    
    void Stop() {
        m_running = false;
        m_condition.notify_all();
        for (auto& t : m_workers) {
            t.join();
        }
    }

private:
    std::queue<std::function<void(DrawContext*)>> m_tasks;
    std::vector<std::thread> m_workers;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_running{false};
};

// 使用示例
void AsyncRenderExample() {
    auto device = std::make_unique<RasterImageDevice>(1920, 1080);
    DrawContext context(device.get());
    
    auto threadSafeEngine = std::make_unique<ThreadSafeEngine<DrawEngine>>(
        device->CreateEngine());
    context.SetEngine(std::move(threadSafeEngine));
    
    AsyncRenderQueue queue;
    queue.Start(&context, 4);
    
    for (int i = 0; i < 100; ++i) {
        queue.Submit([i](DrawContext* ctx) {
            DrawStyle style;
            style.color = Color::FromHSL(i * 3.6, 0.8, 0.5);
            ctx->DrawRect(i * 10, i * 10, 50, 50, style);
        });
    }
    
    queue.Stop();
}
```

### 线程安全最佳实践

```
┌─────────────────────────────────────────────────────────────────────────┐
│                      线程安全最佳实践                                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  推荐模式：                                                              │
│                                                                         │
│  1. 每线程独立设备（最佳性能）                                          │
│     ┌─────────────┐  ┌─────────────┐  ┌─────────────┐                 │
│     │ Thread 1    │  │ Thread 2    │  │ Thread 3    │                 │
│     │ Device 1    │  │ Device 2    │  │ Device 3    │                 │
│     │ Engine 1    │  │ Engine 2    │  │ Engine 3    │                 │
│     └─────────────┘  └─────────────┘  └─────────────┘                 │
│            │                │                │                         │
│            └────────────────┼────────────────┘                         │
│                             ▼                                          │
│                    合并渲染结果                                         │
│                                                                         │
│  2. ThreadSafeEngine包装（共享设备）                                    │
│     ┌─────────────────────────────────────────────────────────────┐   │
│     │                    ThreadSafeEngine                          │   │
│     │                    (内部mutex保护)                           │   │
│     │─────────────────────────────────────────────────────────────│   │
│     │                              │                              │   │
│     │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐       │   │
│     │  │Thread 1 │  │Thread 2 │  │Thread 3 │  │Thread 4 │       │   │
│     │  └─────────┘  └─────────┘  └─────────┘  └─────────┘       │   │
│     └─────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  避免的模式：                                                            │
│                                                                         │
│  ✗ 多线程直接操作同一Engine（无保护）                                   │
│  ✗ 在渲染回调中阻塞                                                     │
│  ✗ 长时间持有Engine锁                                                   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 版本兼容性策略

### 接口扩展规则

| 变更类型 | 规则 | 示例 |
|----------|------|------|
| 新增方法 | 添加默认实现，不影响现有代码 | `virtual void NewMethod() {}` |
| 修改方法 | 新增重载版本，保留旧版本 | `Draw(geom)` → `Draw(geom)` + `Draw(geom, style)` |
| 废弃方法 | 标记`[[deprecated]]`，保留2个主版本 | `[[deprecated("Use NewMethod")]]` |
| 删除方法 | 经2个主版本废弃期后删除 | - |

### 版本号规则

```
主版本.次版本.修订号

主版本：不兼容的接口变更（如删除方法、修改签名）
次版本：向后兼容的功能新增（如新增方法、新增枚举值）
修订号：向后兼容的问题修复（如修复Bug、性能优化）

示例：
1.0.0 → 1.0.1  Bug修复
1.0.1 → 1.1.0  新增DrawCurve方法
1.1.0 → 2.0.0  删除废弃的DrawOld方法
```

### 废弃API处理

```cpp
class DrawDevice {
public:
    // 旧版本方法（标记废弃）
    [[deprecated("Use DrawGeometry with DrawStyle parameter instead")]]
    virtual DrawResult DrawGeometry(const Geometry* geometry) {
        return DrawGeometry(geometry, DrawStyle());
    }
    
    // 新版本方法
    virtual DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) = 0;
};
```

### 版本检测

```cpp
struct Version {
    int major;
    int minor;
    int patch;
    
    static Version GetLibraryVersion();
    static bool IsCompatible(int major, int minor);
};
```

---

## 设备能力协商

### 能力查询接口

```cpp
struct DeviceCapabilities {
    bool supportsTransparency = true;
    bool supportsAntialiasing = true;
    bool supportsTextRotation = true;
    bool supportsGPUAcceleration = false;
    int maxTextureSize = 4096;
    int maxRenderTargets = 1;
    size_t maxMemorySize = 0;
    std::vector<ImageFormat> supportedFormats;
};

class DrawDevice {
public:
    virtual DeviceCapabilities QueryCapabilities() const = 0;
    virtual bool IsFeatureAvailable(const std::string& featureName) const = 0;
};
```

### 功能降级策略

```cpp
class CapabilityNegotiator {
public:
    DrawStyle NegotiateStyle(const DrawStyle& requested, const DeviceCapabilities& caps) {
        DrawStyle result = requested;
        
        // 透明度降级
        if (!caps.supportsTransparency && result.GetOpacity() < 1.0) {
            result.SetOpacity(1.0);
            LogWarning("Transparency not supported, opacity set to 1.0");
        }
        
        // 抗锯齿降级
        if (!caps.supportsAntialiasing && result.IsAntialiased()) {
            result.SetAntialiased(false);
            LogWarning("Antialiasing not supported");
        }
        
        return result;
    }
};
```

---

## 异步渲染设计

### 异步渲染接口

```cpp
enum class RenderState {
    kPending = 0,      // 等待执行
    kRunning = 1,      // 正在执行
    kCompleted = 2,    // 已完成
    kFailed = 3,       // 执行失败
    kCancelled = 4     // 已取消
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

class AsyncRenderManager {
public:
    static AsyncRenderManager& Instance();
    
    std::shared_ptr<AsyncRenderTask> SubmitRenderTask(
        DrawDevicePtr device,
        DrawEnginePtr engine,
        const std::vector<Geometry*>& geometries,
        const DrawStyle& style
    );
    
    void SetMaxConcurrentTasks(int maxTasks);
    int GetActiveTaskCount() const;
    void CancelAllTasks();
    void WaitForAll(uint32_t timeoutMs = 0);
    
private:
    std::queue<std::shared_ptr<AsyncRenderTask>> m_pendingQueue;
    std::set<std::shared_ptr<AsyncRenderTask>> m_activeTasks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::vector<std::thread> m_workers;
};
```

### 异步渲染使用示例

```cpp
auto device = TileDevice::Create(10000, 10000, 256);
auto engine = TileBasedEngine::Create();
auto context = DrawContext::Create(device);

auto geometries = LoadLargeDataset();

auto task = AsyncRenderManager::Instance().SubmitRenderTask(
    device, engine, geometries, DrawStyle()
);

task->SetProgressCallback([](float progress, const std::string& stage) {
    std::cout << "Progress: " << (progress * 100) << "% - " << stage << std::endl;
});

task->SetCompletionCallback([](const RenderResult& result) {
    if (result.state == RenderState::kCompleted) {
        std::cout << "Render completed in " << result.elapsedMs << "ms" << std::endl;
    }
});

if (!task->Wait(5000)) {
    task->Cancel();
    std::cout << "Render timeout, cancelled" << std::endl;
}
```

### 瓦片异步渲染流程

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        异步瓦片渲染流程                                  │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  主线程                                                                 │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 1. 创建渲染任务                                                  │   │
│  │    task = AsyncRenderManager::SubmitRenderTask(...)             │   │
│  │                                                                 │   │
│  │ 2. 设置回调                                                      │   │
│  │    task->SetProgressCallback(...)                               │   │
│  │    task->SetCompletionCallback(...)                             │   │
│  │                                                                 │   │
│  │ 3. 继续其他工作（UI响应等）                                      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  渲染线程                                                               │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 4. 计算瓦片范围                                                  │   │
│  │    tiles = CalculateInvolvedTiles(extent)                       │   │
│  │                                                                 │   │
│  │ 5. 分瓦片渲染                                                    │   │
│  │    for (tile : tiles) {                                         │   │
│  │        BeginTile(tile)                                          │   │
│  │        RenderGeometries(geometries, tile)                       │   │
│  │        EndTile()                                                │   │
│  │        ReportProgress(current / total)                          │   │
│  │    }                                                            │   │
│  │                                                                 │   │
│  │ 6. 合并结果                                                      │   │
│  │    MergeTiles()                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  主线程回调                                                             │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ 7. 完成回调                                                      │   │
│  │    CompletionCallback(result)                                   │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 内存池设计

### 渲染内存池

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

### 纹理缓存

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
    TextureHandle GetOrCreateFromData(const std::string& key, 
                                       const uint8_t* data, 
                                       size_t size,
                                       const TextureDesc& desc);
    
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

### 内存管理策略

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        内存管理策略                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     RenderMemoryPool                             │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │                                                                 │   │
│  │  Vertex Buffer Pool          Index Buffer Pool                  │   │
│  │  ┌──────────────────┐        ┌──────────────────┐              │   │
│  │  │ Block 1 (4KB)    │        │ Block 1 (2KB)    │              │   │
│  │  │ Block 2 (16KB)   │        │ Block 2 (8KB)    │              │   │
│  │  │ Block 3 (64KB)   │        │ Block 3 (32KB)   │              │   │
│  │  │ Block 4 (256KB)  │        │ Block 4 (128KB)  │              │   │
│  │  └──────────────────┘        └──────────────────┘              │   │
│  │                                                                 │   │
│  │  分配策略：                                                      │   │
│  │  1. 查找合适大小的空闲块                                         │   │
│  │  2. 若无空闲块，从系统分配新块                                    │   │
│  │  3. 若块过大，分割后返回剩余部分                                  │   │
│  │                                                                 │   │
│  │  回收策略：                                                      │   │
│  │  1. 将块标记为空闲                                               │   │
│  │  2. 合并相邻空闲块                                               │   │
│  │  3. 碎片率>30%时触发整理                                         │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                       TextureCache                               │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │                                                                 │   │
│  │  LRU Eviction Policy:                                           │   │
│  │                                                                 │   │
│  │  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐                      │   │
│  │  │Most │ │     │ │     │ │     │ │Least│                      │   │
│  │  │Recent│ │     │ │     │ │     │ │Used │                      │   │
│  │  └─────┘ └─────┘ └─────┘ └─────┘ └─────┘                      │   │
│  │     │                                   │                       │   │
│  │     ▼                                   ▼                       │   │
│  │  优先保留                           优先淘汰                    │   │
│  │                                                                 │   │
│  │  缓存命中流程：                                                  │   │
│  │  1. 查找key对应的纹理                                            │   │
│  │  2. 若命中，移动到MRU端                                          │   │
│  │  3. 若未命中，加载纹理并插入MRU端                                │   │
│  │  4. 若超出容量，从LRU端淘汰                                      │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### GPU资源RAII包装

```cpp
template<typename HandleType, typename Deleter>
class GPUResource {
public:
    GPUResource() = default;
    explicit GPUResource(HandleType handle) : m_handle(handle) {}
    
    GPUResource(const GPUResource&) = delete;
    GPUResource& operator=(const GPUResource&) = delete;
    
    GPUResource(GPUResource&& other) noexcept 
        : m_handle(other.m_handle) {
        other.m_handle = {};
    }
    
    GPUResource& operator=(GPUResource&& other) noexcept {
        if (this != &other) {
            Reset();
            m_handle = other.m_handle;
            other.m_handle = {};
        }
        return *this;
    }
    
    ~GPUResource() { Reset(); }
    
    HandleType Get() const { return m_handle; }
    bool IsValid() const { return m_handle != HandleType{}; }
    
    void Reset() {
        if (IsValid()) {
            Deleter{}(m_handle);
            m_handle = {};
        }
    }
    
    HandleType Release() {
        HandleType handle = m_handle;
        m_handle = {};
        return handle;
    }

private:
    HandleType m_handle{};
};

using VertexBufferResource = GPUResource<GLuint, VertexBufferDeleter>;
using IndexBufferResource = GPUResource<GLuint, IndexBufferDeleter>;
using TextureResource = GPUResource<GLuint, TextureDeleter>;
using ShaderResource = GPUResource<GLuint, ShaderDeleter>;
using FramebufferResource = GPUResource<GLuint, FramebufferDeleter>;

struct VertexBufferDeleter {
    void operator()(GLuint handle) {
        glDeleteBuffers(1, &handle);
    }
};

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
```

### 多GPU设备选择

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

class GPUDeviceSelector {
public:
    static std::vector<GPUDeviceInfo> EnumerateGPUs();
    
    static GPUDeviceInfo SelectBestGPU(GPUPreference preference = GPUPreference::kAuto) {
        auto gpus = EnumerateGPUs();
        if (gpus.empty()) {
            return {};
        }
        
        switch (preference) {
            case GPUPreference::kAuto:
                return SelectByScore(gpus);
            case GPUPreference::kDiscrete:
                return SelectDiscrete(gpus);
            case GPUPreference::kIntegrated:
                return SelectIntegrated(gpus);
            case GPUPreference::kHighMemory:
                return SelectHighMemory(gpus);
            case GPUPreference::kLowPower:
                return SelectLowPower(gpus);
            default:
                return gpus[0];
        }
    }
    
    static int CalculateGPUScore(const GPUDeviceInfo& gpu) {
        int score = 0;
        score += gpu.dedicatedMemory / (1024 * 1024);
        score += gpu.computeUnits * 100;
        score += gpu.clockFrequency / 10;
        if (gpu.isDiscrete) score += 10000;
        if (gpu.supportsCompute) score += 5000;
        return score;
    }

private:
    static GPUDeviceInfo SelectByScore(const std::vector<GPUDeviceInfo>& gpus) {
        return *std::max_element(gpus.begin(), gpus.end(),
            [](const GPUDeviceInfo& a, const GPUDeviceInfo& b) {
                return CalculateGPUScore(a) < CalculateGPUScore(b);
            });
    }
    
    static GPUDeviceInfo SelectDiscrete(const std::vector<GPUDeviceInfo>& gpus) {
        auto it = std::find_if(gpus.begin(), gpus.end(),
            [](const GPUDeviceInfo& gpu) { return gpu.isDiscrete; });
        return it != gpus.end() ? *it : gpus[0];
    }
};

enum class GPUPreference {
    kAuto,
    kDiscrete,
    kIntegrated,
    kHighMemory,
    kLowPower
};
```

### 瓦片大小动态策略

```cpp
class TileSizeStrategy {
public:
    struct TileConfig {
        int tileSize = 256;
        int overlap = 0;
        int maxTilesInMemory = 100;
    };
    
    static TileConfig CalculateOptimalConfig(
        int totalWidth, 
        int totalHeight,
        size_t availableMemory,
        int targetFPS = 60) {
        
        TileConfig config;
        
        int minTileSize = 128;
        int maxTileSize = 1024;
        int tileSize = 256;
        
        size_t bytesPerPixel = 4;
        size_t tileMemory = tileSize * tileSize * bytesPerPixel;
        
        int maxTiles = static_cast<int>(availableMemory / tileMemory);
        int totalTilesX = (totalWidth + tileSize - 1) / tileSize;
        int totalTilesY = (totalHeight + tileSize - 1) / tileSize;
        int totalTiles = totalTilesX * totalTilesY;
        
        if (totalTiles > maxTiles) {
            tileSize = std::min(maxTileSize,
                static_cast<int>(std::sqrt(
                    static_cast<double>(totalWidth * totalHeight) / maxTiles)));
            tileSize = std::max(minTileSize, tileSize);
        }
        
        if (totalWidth * totalHeight > 10000 * 10000) {
            config.overlap = 2;
        }
        
        config.tileSize = tileSize;
        config.maxTilesInMemory = std::min(maxTiles, 200);
        
        return config;
    }
};
```

---

## 插件扩展机制

### 插件接口定义

```cpp
class DrawEnginePlugin {
public:
    virtual ~DrawEnginePlugin() = default;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    
    virtual std::vector<EngineType> GetSupportedEngineTypes() const = 0;
    virtual std::vector<DeviceType> GetSupportedDeviceTypes() const = 0;
    
    virtual std::unique_ptr<DrawEngine> CreateEngine(EngineType type) = 0;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};

class DrawDevicePlugin {
public:
    virtual ~DrawDevicePlugin() = default;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetVersion() const = 0;
    virtual std::string GetDescription() const = 0;
    
    virtual std::vector<DeviceType> GetSupportedDeviceTypes() const = 0;
    
    virtual std::unique_ptr<DrawDevice> CreateDevice(DeviceType type) = 0;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};
```

### 插件管理器

```cpp
class PluginManager {
public:
    static PluginManager& Instance();
    
    void LoadPlugin(const std::string& path);
    void UnloadPlugin(const std::string& name);
    void UnloadAll();
    
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
};
```

### 插件使用示例

```cpp
void LoadCustomEngine() {
    auto& pm = PluginManager::Instance();
    
    pm.LoadPlugin("plugins/custom_gpu_engine.dll");
    
    auto enginePlugin = pm.GetEnginePlugin("CustomGPUEngine");
    if (enginePlugin) {
        auto engine = enginePlugin->CreateEngine(EngineType::kGPU);
        context->SetEngine(std::move(engine));
    }
}
```

### 插件生命周期

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        插件生命周期                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     PluginManager                                │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │                                                                 │   │
│  │  加载流程：                                                      │   │
│  │  1. LoadPlugin(path)                                            │   │
│  │     ├── dlopen/dlopen(path)                                     │   │
│  │     ├── dlsym("CreatePlugin")                                   │   │
│  │     ├── plugin = CreatePlugin()                                 │   │
│  │     ├── plugin->Initialize()                                    │   │
│  │     └── 注册到m_plugins                                         │   │
│  │                                                                 │   │
│  │  使用流程：                                                      │   │
│  │  2. CreateEngine(type)                                          │   │
│  │     ├── 查找支持该type的插件                                     │   │
│  │     ├── plugin->CreateEngine(type)                              │   │
│  │     └── 返回引擎实例                                             │   │
│  │                                                                 │   │
│  │  卸载流程：                                                      │   │
│  │  3. UnloadPlugin(name)                                          │   │
│  │     ├── plugin->Shutdown()                                      │   │
│  │     ├── 从m_plugins移除                                         │   │
│  │     └── dlclose(handle)                                         │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  插件目录结构：                                                          │
│  plugins/                                                               │
│  ├── custom_gpu_engine.dll/so/dylib                                    │
│  ├── vector_pdf_engine.dll/so/dylib                                    │
│  └── webgl_engine.dll/so/dylib                                         │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 实现路线图

### 阶段划分

| 阶段 | 内容 | 工期 | 依赖 | 优先级 |
|------|------|------|------|--------|
| **阶段1** | 核心框架 | 10天 | 无 | P0 |
| **阶段2** | CPU渲染 | 8天 | 阶段1 | P0 |
| **阶段3** | GPU加速 | 12天 | 阶段1 | P1 |
| **阶段4** | 瓦片渲染 | 10天 | 阶段2 | P1 |
| **阶段5** | 矢量输出 | 8天 | 阶段2 | P2 |
| **阶段6** | 高级特性 | 6天 | 阶段2-5 | P2 |

### 阶段1：核心框架（10天）

| 任务 | 工期 | 产出 |
|------|------|------|
| DrawDevice基类 | 2天 | draw_device.h/cpp |
| DrawEngine基类 | 3天 | draw_engine.h/cpp |
| DrawContext | 4天 | draw_context.h/cpp |
| 基础类型定义 | 1天 | draw_result.h, draw_style.h |

### 阶段2：CPU渲染（8天）

| 任务 | 工期 | 产出 |
|------|------|------|
| RasterImageDevice | 2天 | raster_image_device.h/cpp |
| DisplayDevice | 2天 | display_device.h/cpp |
| Simple2DEngine | 4天 | simple2d_engine.h/cpp |

### 阶段3：GPU加速（12天）

| 任务 | 工期 | 产出 |
|------|------|------|
| GPUResourceManager | 3天 | gpu_resource_manager.h/cpp |
| RenderMemoryPool | 3天 | render_memory_pool.h/cpp |
| GPUAcceleratedEngine | 6天 | gpu_accelerated_engine.h/cpp |

### 阶段4：瓦片渲染（10天）

| 任务 | 工期 | 产出 |
|------|------|------|
| TileDevice | 2天 | tile_device.h/cpp |
| TileBasedEngine | 4天 | tile_based_engine.h/cpp |
| AsyncRenderManager | 4天 | async_render_manager.h/cpp |

### 阶段5：矢量输出（8天）

| 任务 | 工期 | 产出 |
|------|------|------|
| PdfDevice | 2天 | pdf_device.h/cpp |
| SvgDevice | 1天 | svg_device.h/cpp |
| VectorEngine (基类) | 2天 | vector_engine.h/cpp |
| PdfEngine (podofo适配) | 2天 | pdf_engine.h/cpp |
| SvgEngine (cairo适配) | 1天 | svg_engine.h/cpp |

### 阶段6：高级特性（6天）

| 任务 | 工期 | 产出 |
|------|------|------|
| PluginManager | 3天 | plugin_manager.h/cpp |
| 性能优化 | 2天 | 批处理、缓存优化 |
| 测试与文档 | 1天 | 单元测试、API文档 |

### 依赖关系图

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        实现依赖关系                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  阶段1: 核心框架                                                        │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ DrawDevice ──► DrawEngine ──► DrawContext                        │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│              ┌───────────────┼───────────────┐                         │
│              ▼               ▼               ▼                         │
│  阶段2: CPU渲染        阶段3: GPU加速    阶段5: 矢量输出               │
│  ┌───────────────┐    ┌───────────────┐    ┌───────────────┐          │
│  │Simple2DEngine │    │GPUAccelerated │    │ VectorEngine  │          │
│  │RasterImageDev │    │ RenderMemory  │    │ PdfDevice     │          │
│  │ DisplayDevice │    │ TextureCache  │    │ SvgDevice     │          │
│  └───────────────┘    └───────────────┘    └───────────────┘          │
│              │               │                                          │
│              └───────┬───────┘                                          │
│                      ▼                                                  │
│  阶段4: 瓦片渲染                                                        │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ TileDevice ──► TileBasedEngine ──► AsyncRenderManager            │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  阶段6: 高级特性                                                        │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ PluginManager ──► 性能优化 ──► 测试与文档                         │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 平台适配说明

### 支持平台

| 平台 | 底层图形库 | 设备类型 | 引擎类型 | 状态 |
|------|-----------|----------|----------|------|
| **Windows** | GDI+/Direct2D/OpenGL/Qt | DisplayDevice, RasterImageDevice | Simple2DEngine, GPUAcceleratedEngine, QtEngine | ✅ 支持 |
| **Linux** | Cairo/OpenGL/Qt | DisplayDevice, RasterImageDevice | Simple2DEngine, GPUAcceleratedEngine, QtEngine | ✅ 支持 |
| **macOS** | Core Graphics/Metal/Qt | DisplayDevice, RasterImageDevice | Simple2DEngine, GPUAcceleratedEngine, QtEngine | ✅ 支持 |
| **Web** | WebGL/Canvas | WebGLDevice | WebGLEngine | ✅ 支持 |
| **跨平台** | Qt (QPainter) | QtDisplayDevice | QtEngine | ✅ 支持 |

### 图形库选型对比

| 图形库 | 平台覆盖 | 性能 | 功能完整度 | 依赖复杂度 | 推荐场景 |
|--------|----------|------|------------|------------|----------|
| **Qt (QPainter)** | 全平台 | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | 中 (Qt框架) | 跨平台首选、Qt项目 |
| **Direct2D** | Windows | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 低 (系统内置) | Windows高性能 |
| **GDI+** | Windows | ⭐⭐⭐ | ⭐⭐⭐ | 低 (系统内置) | Windows兼容性 |
| **Core Graphics** | macOS/iOS | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 低 (系统内置) | Apple平台原生 |
| **Metal** | macOS/iOS | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | 低 (系统内置) | Apple平台GPU加速 |
| **Cairo** | Linux/跨平台 | ⭐⭐⭐ | ⭐⭐⭐⭐ | 中 (libcairo) | Linux原生、矢量输出 |
| **OpenGL** | 全平台 | ⭐⭐⭐⭐ | ⭐⭐⭐ | 中 (系统/驱动) | GPU加速、跨平台 |
| **WebGL** | Web | ⭐⭐⭐⭐ | ⭐⭐⭐ | 低 (浏览器) | Web平台GPU加速 |

### 三方库宏保护机制

```cpp
#define DRAW_HAS_QT           (defined(QT_VERSION))
#define DRAW_HAS_GDIPLUS      (defined(_WIN32))
#define DRAW_HAS_DIRECT2D     (defined(_WIN32) && NTDDI_VERSION >= NTDDI_WIN7)
#define DRAW_HAS_COREGRAPHICS (defined(__APPLE__))
#define DRAW_HAS_METAL        (defined(__APPLE__) && __MAC_OS_X_VERSION_MAX_ALLOWED >= 101100)
#define DRAW_HAS_CAIRO        (defined(CAIRO_VERSION))
#define DRAW_HAS_OPENGL       (defined(GL_VERSION))
#define DRAW_HAS_VULKAN       (defined(VK_API_VERSION))
#define DRAW_HAS_WEBGL        (defined(__EMSCRIPTEN__))
```

#### 编译期模块可用性检查

```cpp
namespace draw {

enum class EngineAvailability {
    kQtPainter     = DRAW_HAS_QT ? 1 : 0,
    kGDIPlus       = DRAW_HAS_GDIPLUS ? 1 : 0,
    kDirect2D      = DRAW_HAS_DIRECT2D ? 1 : 0,
    kCoreGraphics  = DRAW_HAS_COREGRAPHICS ? 1 : 0,
    kMetal         = DRAW_HAS_METAL ? 1 : 0,
    kCairo         = DRAW_HAS_CAIRO ? 1 : 0,
    kOpenGL        = DRAW_HAS_OPENGL ? 1 : 0,
    kVulkan        = DRAW_HAS_VULKAN ? 1 : 0,
    kWebGL         = DRAW_HAS_WEBGL ? 1 : 0,
};

template<EngineAvailability avail>
struct IsEngineAvailable {
    static constexpr bool value = (avail == EngineAvailability::kQtPainter || 
                                   static_cast<int>(avail) == 1);
};

#if DRAW_HAS_QT
#include <QPainter>
#include <QPaintDevice>

class QtEngine : public Simple2DEngine {
public:
    explicit QtEngine(DrawDevice* device);
    ~QtEngine() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2,
                          int count, const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, 
                         const Font& font, const Color& color) override;

private:
    QPainter* m_painter = nullptr;
};

#endif

#if DRAW_HAS_GDIPLUS
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

class GDIPlusEngine : public Simple2DEngine {
};

#endif

#if DRAW_HAS_DIRECT2D
#include <d2d1.h>
#include <dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

class Direct2DEngine : public GPUAcceleratedEngine {
};

#endif

#if DRAW_HAS_COREGRAPHICS
#include <CoreGraphics/CoreGraphics.h>
#include <CoreText/CoreText.h>

class CoreGraphicsEngine : public Simple2DEngine {
};

#endif

#if DRAW_HAS_METAL
#include <Metal/Metal.h>

class MetalEngine : public GPUAcceleratedEngine {
};

#endif

#if DRAW_HAS_CAIRO
#include <cairo/cairo.h>
#include <pango/pangocairo.h>

class CairoEngine : public Simple2DEngine {
};

#endif

}
```

#### 运行时引擎创建工厂

```cpp
class EngineFactory {
public:
    static std::unique_ptr<DrawEngine> CreateOptimalEngine(DrawDevice* device) {
#if DRAW_HAS_QT
        return std::make_unique<QtEngine>(device);
#elif DRAW_HAS_DIRECT2D
        if (IsDirect2DAvailable()) {
            return std::make_unique<Direct2DEngine>(device);
        }
        return std::make_unique<GDIPlusEngine>(device);
#elif DRAW_HAS_METAL
        if (IsMetalAvailable()) {
            return std::make_unique<MetalEngine>(device);
        }
        return std::make_unique<CoreGraphicsEngine>(device);
#elif DRAW_HAS_CAIRO
        return std::make_unique<CairoEngine>(device);
#elif DRAW_HAS_WEBGL
        return std::make_unique<WebGLEngine>(device);
#else
        static_assert(false, "No rendering backend available");
#endif
    }
    
    static std::vector<std::string> GetAvailableEngines() {
        std::vector<std::string> engines;
#if DRAW_HAS_QT
        engines.push_back("QtPainter");
#endif
#if DRAW_HAS_GDIPLUS
        engines.push_back("GDIPlus");
#endif
#if DRAW_HAS_DIRECT2D
        engines.push_back("Direct2D");
#endif
#if DRAW_HAS_COREGRAPHICS
        engines.push_back("CoreGraphics");
#endif
#if DRAW_HAS_METAL
        engines.push_back("Metal");
#endif
#if DRAW_HAS_CAIRO
        engines.push_back("Cairo");
#endif
#if DRAW_HAS_OPENGL
        engines.push_back("OpenGL");
#endif
#if DRAW_HAS_WEBGL
        engines.push_back("WebGL");
#endif
        return engines;
    }
    
private:
#if DRAW_HAS_DIRECT2D
    static bool IsDirect2DAvailable() {
        ID2D1Factory* factory = nullptr;
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
        if (SUCCEEDED(hr) && factory) {
            factory->Release();
            return true;
        }
        return false;
    }
#endif

#if DRAW_HAS_METAL
    static bool IsMetalAvailable() {
        id<MTLDevice> device = MTLCreateSystemDefaultDevice();
        return device != nil;
    }
#endif
};
```

### 平台特定实现

#### 跨平台Qt实现（推荐）

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        Qt跨平台适配                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  设备实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ QtDisplayDevice                                                  │   │
│  │ ├── QPaintDevice* m_paintDevice                                  │   │
│  │ ├── QWidget* m_widget (可选)                                     │   │
│  │ ├── QImage* m_image (可选)                                       │   │
│  │ └── CreateEngine() → QtEngine                                    │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  引擎实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ QtEngine (Simple2DEngine子类)                                    │   │
│  │ ├── QPainter* m_painter                                          │   │
│  │ ├── QPen m_pen                                                   │   │
│  │ ├── QBrush m_brush                                               │   │
│  │ └── QFont m_font                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  优势：                                                                  │
│  • 全平台一致性API                                                      │
│  • 功能完整（文本、图像、路径、变换）                                    │
│  • 与本设计架构高度一致                                                  │
│  • 支持QImage/QPixmap/QWidget/QPrinter等多种设备                        │
│  • 内置抗锯齿和高质量渲染                                                │
│                                                                         │
│  注意事项：                                                              │
│  • 需要Qt框架依赖                                                       │
│  • LGPL/GPL协议需注意商业使用                                           │
│  • Qt商业版可避免协议限制                                                │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

#### Qt许可证选择指南

```
┌─────────────────────────────────────────────────────────────────────────┐
│                    Qt许可证选择指南                                      │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    开源许可证 (LGPL v3)                          │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │                                                                 │   │
│  │  适用场景：                                                      │   │
│  │  ✓ 开源项目（GPL/LGPL兼容）                                     │   │
│  │  ✓ 动态链接Qt库的闭源商业软件                                   │   │
│  │  ✓ 允许用户替换Qt动态库                                         │   │
│  │                                                                 │   │
│  │  义务要求：                                                      │   │
│  │  • 提供Qt库的源代码或获取方式                                   │   │
│  │  • 允许用户替换Qt动态链接库                                     │   │
│  │  • 在应用中包含LGPL许可证副本                                   │   │
│  │  • 标注使用了Qt库                                               │   │
│  │                                                                 │   │
│  │  限制：                                                          │   │
│  │  ✗ 不能静态链接Qt库到闭源软件                                   │   │
│  │  ✗ 修改Qt源代码后必须开源修改部分                               │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                    商业许可证 (Qt Commercial)                    │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │                                                                 │   │
│  │  适用场景：                                                      │   │
│  │  ✓ 需要静态链接的闭源商业软件                                   │   │
│  │  ✓ 不希望开源任何代码的商业项目                                 │   │
│  │  ✓ 需要Qt官方技术支持                                          │   │
│  │  ✓ 需要使用Qt商业版独有功能                                     │   │
│  │                                                                 │   │
│  │  权益：                                                          │   │
│  │  • 无需开源任何代码                                             │   │
│  │  • 允许静态链接                                                 │   │
│  │  • 官方技术支持                                                 │   │
│  │  • 商业版独有组件                                               │   │
│  │                                                                 │   │
│  │  费用：按开发者席位收费，详情见Qt官网                           │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  决策流程：                                                              │
│                                                                         │
│  项目是否开源？                                                          │
│       │                                                                  │
│    YES├──────► 使用LGPL v3开源许可证                                    │
│       │                                                                  │
│    NO │                                                                  │
│       ▼                                                                  │
│  是否需要静态链接？                                                      │
│       │                                                                  │
│    YES├──────► 必须购买Qt商业许可证                                     │
│       │                                                                  │
│    NO │                                                                  │
│       ▼                                                                  │
│  是否需要官方技术支持？                                                  │
│       │                                                                  │
│    YES├──────► 建议购买Qt商业许可证                                     │
│       │                                                                  │
│    NO └──────► 可使用LGPL v3（动态链接）                                │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

#### 三方库版本兼容性矩阵

| 三方库 | 最低版本 | 推荐版本 | 测试版本 | 兼容性说明 |
|--------|----------|----------|----------|------------|
| **Qt** | 5.12 LTS | 6.5 LTS | 6.6.0 | Qt5/Qt6双版本支持 |
| **Cairo** | 1.16.0 | 1.18.0 | 1.18.0 | Linux默认矢量库 |
| **Pango** | 1.48.0 | 1.52.0 | 1.52.0 | 文本渲染依赖 |
| **podofo** | 0.9.8 | 0.10.0 | 0.10.3 | PDF生成库 |
| **OpenGL** | 3.3 | 4.5 | 4.6 | GPU加速核心 |
| **Vulkan** | 1.2 | 1.3 | 1.3.268 | 下一代GPU API |
| **Direct2D** | Windows 7 | Windows 10 | Windows 11 | Windows GPU加速 |
| **DirectWrite** | Windows 7 | Windows 10 | Windows 11 | Windows文本渲染 |
| **Metal** | macOS 10.11 | macOS 12 | macOS 14 | Apple GPU加速 |
| **CoreGraphics** | macOS 10.12 | macOS 12 | macOS 14 | Apple 2D渲染 |
| **Emscripten** | 3.1.0 | 3.1.50 | 3.1.50 | WebAssembly编译 |
| **WebGL** | 1.0 | 2.0 | 2.0 | Web GPU API |

#### 版本检测与兼容性处理

```cpp
namespace draw {

struct LibraryVersion {
    std::string name;
    int major = 0;
    int minor = 0;
    int patch = 0;
    
    bool operator>=(const LibraryVersion& other) const {
        if (major != other.major) return major > other.major;
        if (minor != other.minor) return minor > other.minor;
        return patch >= other.patch;
    }
};

class LibraryCompatibility {
public:
    static LibraryVersion GetQtVersion() {
#if DRAW_HAS_QT
        return {"Qt", QT_VERSION_MAJOR, QT_VERSION_MINOR, QT_VERSION_PATCH};
#else
        return {"Qt", 0, 0, 0};
#endif
    }
    
    static LibraryVersion GetCairoVersion() {
#if DRAW_HAS_CAIRO
        int major, minor, patch;
        cairo_version(&major, &minor, &patch);
        return {"Cairo", major, minor, patch};
#else
        return {"Cairo", 0, 0, 0};
#endif
    }
    
    static bool CheckMinimumRequirements() {
        std::vector<std::string> issues;
        
#if DRAW_HAS_QT
        auto qtVer = GetQtVersion();
        if (!(qtVer >= LibraryVersion{"Qt", 5, 12, 0})) {
            issues.push_back("Qt version must be >= 5.12");
        }
#endif
        
#if DRAW_HAS_CAIRO
        auto cairoVer = GetCairoVersion();
        if (!(cairoVer >= LibraryVersion{"Cairo", 1, 16, 0})) {
            issues.push_back("Cairo version must be >= 1.16.0");
        }
#endif
        
        return issues.empty();
    }
    
    static std::string GetCompatibilityReport() {
        std::stringstream ss;
        ss << "=== Draw Library Compatibility Report ===\n";
        
#if DRAW_HAS_QT
        auto v = GetQtVersion();
        ss << "Qt:       " << v.major << "." << v.minor << "." << v.patch;
        ss << (v >= LibraryVersion{"Qt", 6, 0, 0} ? " (Recommended)" : " (Legacy)");
        ss << "\n";
#endif
        
#if DRAW_HAS_CAIRO
        auto v = GetCairoVersion();
        ss << "Cairo:    " << v.major << "." << v.minor << "." << v.patch << "\n";
#endif
        
        return ss.str();
    }
};

}
```

```cpp
#if DRAW_HAS_QT

class QtDisplayDevice : public DrawDevice {
public:
    explicit QtDisplayDevice(QPaintDevice* device);
    explicit QtDisplayDevice(QWidget* widget);
    explicit QtDisplayDevice(const QSize& size, QImage::Format format = QImage::Format_ARGB32);
    ~QtDisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kDisplay; }
    EngineType GetPreferredEngineType() const override { return EngineType::kSimple2D; }
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    
    QPaintDevice* GetPaintDevice() const { return m_paintDevice; }
    QImage* GetImage() { return m_image.get(); }
    
private:
    QPaintDevice* m_paintDevice = nullptr;
    std::unique_ptr<QImage> m_image;
    QWidget* m_widget = nullptr;
};

class QtEngine : public Simple2DEngine {
public:
    explicit QtEngine(DrawDevice* device);
    ~QtEngine() override;
    
    std::string GetName() const override { return "QtEngine"; }
    EngineType GetType() const override { return EngineType::kSimple2D; }
    
    bool Begin() override;
    void End() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2,
                          int count, const DrawStyle& style) override;
    DrawResult DrawPolygon(const double* x, const double* y, int count,
                            const DrawStyle& style, bool fill = true) override;
    DrawResult DrawText(double x, double y, const std::string& text, 
                         const Font& font, const Color& color) override;
    DrawResult DrawImage(double x, double y, const Image& image, 
                          double scaleX = 1.0, double scaleY = 1.0) override;
    
    void SetTransform(const TransformMatrix& matrix) override;
    void SetClipRegion(const Region& region) override;
    
    void Save(StateFlags flags = StateFlag::kAll) override;
    void Restore() override;

private:
    QPainter* m_painter = nullptr;
    QPaintDevice* m_device = nullptr;
    QPen m_pen;
    QBrush m_brush;
    QFont m_font;
    std::stack<QPainter::State> m_stateStack;
    
    void ApplyStyle(const DrawStyle& style);
    QPen CreatePen(const DrawStyle& style);
    QBrush CreateBrush(const DrawStyle& style);
    QFont CreateFont(const Font& font);
    QColor ToQColor(const Color& color);
};

#endif
```

#### Windows平台

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        Windows平台适配                                   │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  设备实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ Win32DisplayDevice                                               │   │
│  │ ├── HWND m_hwnd                                                  │   │
│  │ ├── HDC m_hdc                                                    │   │
│  │ └── CreateEngine() → GDIPlusEngine 或 Direct2DEngine            │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  引擎实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ GDIPlusEngine (Simple2DEngine子类)                               │   │
│  │ ├── Gdiplus::Graphics* m_graphics                                │   │
│  │ ├── Gdiplus::Pen* m_pen                                          │   │
│  │ └── Gdiplus::Brush* m_brush                                      │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ Direct2DEngine (GPUAcceleratedEngine子类)                        │   │
│  │ ├── ID2D1Factory* m_factory                                      │   │
│  │ ├── ID2D1HwndRenderTarget* m_renderTarget                        │   │
│  │ └── ID2D1Brush* m_brush                                          │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  特有功能：                                                              │
│  • DirectWrite 高质量文本渲染                                           │
│  • WIC 图像编解码                                                       │
│  • Direct3D 11/12 GPU加速                                              │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

```cpp
class GDIPlusEngine : public Simple2DEngine {
public:
    explicit GDIPlusEngine(DrawDevice* device);
    ~GDIPlusEngine() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override;
    DrawResult DrawLines(const double* x1, const double* y1,
                          const double* x2, const double* y2,
                          int count, const DrawStyle& style) override;

private:
    Gdiplus::Graphics* m_graphics = nullptr;
    std::unique_ptr<Gdiplus::Pen> m_pen;
    std::unique_ptr<Gdiplus::Brush> m_brush;
};

class Direct2DEngine : public GPUAcceleratedEngine {
public:
    explicit Direct2DEngine(DrawDevice* device);
    ~Direct2DEngine() override;
    
    bool Initialize() override;
    void Shutdown() override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;

private:
    ID2D1Factory* m_factory = nullptr;
    ID2D1HwndRenderTarget* m_renderTarget = nullptr;
    ID2D1SolidColorBrush* m_brush = nullptr;
    IDWriteFactory* m_writeFactory = nullptr;
};
```

#### Linux平台

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        Linux平台适配                                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  设备实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ X11DisplayDevice                                                 │   │
│  │ ├── Display* m_display                                           │   │
│  │ ├── Window m_window                                              │   │
│  │ └── CreateEngine() → CairoEngine 或 OpenGLEngine                │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ WaylandDisplayDevice                                             │   │
│  │ ├── wl_display* m_display                                        │   │
│  │ ├── wl_surface* m_surface                                        │   │
│  │ └── CreateEngine() → CairoEngine 或 OpenGLEngine                │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  引擎实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ CairoEngine (Simple2DEngine子类)                                 │   │
│  │ ├── cairo_t* m_cr                                                │   │
│  │ ├── cairo_surface_t* m_surface                                   │   │
│  │ └── PangoLayout* m_layout (文本)                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  特有功能：                                                              │
│  • X11/Wayland 双显示服务器支持                                         │
│  • Pango 高质量文本渲染                                                 │
│  • EGL/OpenGL GPU加速                                                  │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

```cpp
class CairoEngine : public Simple2DEngine {
public:
    explicit CairoEngine(DrawDevice* device);
    ~CairoEngine() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, 
                         const Font& font, const Color& color) override;

private:
    cairo_t* m_cr = nullptr;
    cairo_surface_t* m_surface = nullptr;
    PangoLayout* m_layout = nullptr;
};
```

#### macOS平台

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        macOS平台适配                                     │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  设备实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ CocoaDisplayDevice                                               │   │
│  │ ├── NSView* m_view                                               │   │
│  │ ├── NSGraphicsContext* m_context                                 │   │
│  │ └── CreateEngine() → CoreGraphicsEngine 或 MetalEngine           │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  引擎实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ CoreGraphicsEngine (Simple2DEngine子类)                          │   │
│  │ ├── CGContextRef m_context                                       │   │
│  │ └── CTFontRef m_font                                             │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ MetalEngine (GPUAcceleratedEngine子类)                           │   │
│  │ ├── id<MTLDevice> m_device                                       │   │
│  │ ├── id<MTLCommandQueue> m_commandQueue                           │   │
│  │ └── id<MTLRenderPipelineState> m_pipelineState                   │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  特有功能：                                                              │
│  • Core Animation 高性能动画                                            │
│  • Core Text 高质量文本渲染                                             │
│  • Metal GPU加速                                                       │
│  • Retina显示支持                                                       │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

```cpp
class CoreGraphicsEngine : public Simple2DEngine {
public:
    explicit CoreGraphicsEngine(DrawDevice* device);
    ~CoreGraphicsEngine() override;
    
    DrawResult DrawPoints(const double* x, const double* y, int count, 
                           const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, 
                         const Font& font, const Color& color) override;

private:
    CGContextRef m_context = nullptr;
    CTFontRef m_font = nullptr;
};

class MetalEngine : public GPUAcceleratedEngine {
public:
    explicit MetalEngine(DrawDevice* device);
    ~MetalEngine() override;
    
    bool Initialize() override;
    void Shutdown() override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;

private:
    id<MTLDevice> m_device = nil;
    id<MTLCommandQueue> m_commandQueue = nil;
    id<MTLRenderPipelineState> m_pipelineState = nil;
    CAMetalLayer* m_layer = nil;
};
```

#### Web平台

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        Web平台适配                                       │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  设备实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ WebGLDevice                                                      │   │
│  │ ├── emscripten::val m_canvas                                     │   │
│  │ ├── WebGLRenderingContext* m_glContext                           │   │
│  │ └── CreateEngine() → WebGLEngine                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  引擎实现：                                                              │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │ WebGLEngine (GPUAcceleratedEngine子类)                           │   │
│  │ ├── GLuint m_shaderProgram                                       │   │
│  │ ├── GLuint m_vertexBuffer                                        │   │
│  │ └── GLuint m_indexBuffer                                         │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  特有功能：                                                              │
│  • WebGL 1.0/2.0 支持                                                   │
│  • Canvas 2D 回退                                                       │
│  • WebAssembly SIMD 加速                                               │
│  • OffscreenCanvas 多线程渲染                                          │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

```cpp
class WebGLEngine : public GPUAcceleratedEngine {
public:
    explicit WebGLEngine(DrawDevice* device);
    ~WebGLEngine() override;
    
    bool Initialize() override;
    void Shutdown() override;
    
    DrawResult DrawGeometry(const Geometry* geometry, const DrawStyle& style) override;
    DrawResult DrawText(double x, double y, const std::string& text, 
                         const Font& font, const Color& color) override;

private:
    GLuint m_shaderProgram = 0;
    GLuint m_vertexBuffer = 0;
    GLuint m_indexBuffer = 0;
    emscripten::val m_canvas;
    
    bool InitShaders();
    bool InitBuffers();
    void CompileShader(GLenum type, const char* source);
};
```

### 平台适配层设计

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        平台适配层架构                                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     跨平台接口层                                  │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ DrawDevice / DrawEngine / DrawContext                           │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     平台抽象层                                    │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ PlatformDevice / PlatformEngine / PlatformContext               │   │
│  │                                                                 │   │
│  │ // 平台检测宏                                                    │   │
│  │ #if defined(_WIN32)                                             │   │
│  │     using PlatformDisplayDevice = Win32DisplayDevice;           │   │
│  │     using Platform2DEngine = GDIPlusEngine;                     │   │
│  │ #elif defined(__APPLE__)                                        │   │
│  │     using PlatformDisplayDevice = CocoaDisplayDevice;           │   │
│  │     using Platform2DEngine = CoreGraphicsEngine;                │   │
│  │ #elif defined(__linux__)                                        │   │
│  │     using PlatformDisplayDevice = X11DisplayDevice;             │   │
│  │     using Platform2DEngine = CairoEngine;                       │   │
│  │ #elif defined(__EMSCRIPTEN__)                                   │   │
│  │     using PlatformDisplayDevice = WebGLDevice;                  │   │
│  │     using Platform2DEngine = WebGLEngine;                       │   │
│  │ #endif                                                          │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     平台实现层                                    │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ Windows: GDI+ / Direct2D / Direct3D                              │   │
│  │ Linux:   Cairo / OpenGL / Vulkan                                 │   │
│  │ macOS:   Core Graphics / Metal                                   │   │
│  │ Web:     WebGL / Canvas 2D                                       │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 平台能力差异处理

```cpp
struct PlatformCapabilities {
    bool supportsGPUAcceleration;
    bool supportsTransparency;
    bool supportsTextRotation;
    bool supportsPathSimplification;
    int maxTextureSize;
    int maxAnisotropy;
    std::string preferredEngine;
};

class PlatformDetector {
public:
    static PlatformCapabilities DetectCapabilities() {
        PlatformCapabilities caps;
        
#if DRAW_HAS_QT
        caps.supportsGPUAcceleration = false;
        caps.preferredEngine = "QtPainter";
        caps.maxTextureSize = 16384;
        caps.supportsTransparency = true;
        caps.supportsTextRotation = true;
        
#elif defined(_WIN32)
        caps.supportsGPUAcceleration = CheckDirect2DAvailable();
        caps.preferredEngine = caps.supportsGPUAcceleration ? "Direct2D" : "GDI+";
        caps.maxTextureSize = 16384;
        
#elif defined(__APPLE__)
        caps.supportsGPUAcceleration = CheckMetalAvailability();
        caps.preferredEngine = caps.supportsGPUAcceleration ? "Metal" : "CoreGraphics";
        caps.maxTextureSize = 16384;
        
#elif defined(__linux__)
        caps.supportsGPUAcceleration = CheckOpenGLAvailability();
        caps.preferredEngine = caps.supportsGPUAcceleration ? "OpenGL" : "Cairo";
        caps.maxTextureSize = 8192;
        
#elif defined(__EMSCRIPTEN__)
        caps.supportsGPUAcceleration = CheckWebGLAvailability();
        caps.preferredEngine = caps.supportsGPUAcceleration ? "WebGL" : "Canvas2D";
        caps.maxTextureSize = 4096;
#endif
        
        return caps;
    }
    
private:
#if DRAW_HAS_DIRECT2D
    static bool CheckDirect2DAvailable() {
        ID2D1Factory* factory = nullptr;
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
        if (SUCCEEDED(hr)) {
            factory->Release();
            return true;
        }
        return false;
    }
#endif
};
```

### 编译配置

```cmake
# CMakeLists.txt 平台配置

option(DRAW_USE_QT "Enable Qt rendering backend" ON)
option(DRAW_USE_OPENGL "Enable OpenGL rendering backend" ON)
option(DRAW_USE_CAIRO "Enable Cairo rendering backend" ON)

if(DRAW_USE_QT)
    find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
    if(Qt6_FOUND)
        set(DRAW_HAS_QT ON)
        list(APPEND PLATFORM_SOURCES
            qt_display_device.cpp
            qt_engine.cpp
        )
        list(APPEND PLATFORM_LIBS Qt6::Core Qt6::Gui Qt6::Widgets)
        message(STATUS "Qt rendering backend enabled")
    else()
        find_package(Qt5 COMPONENTS Core Gui Widgets QUIET)
        if(Qt5_FOUND)
            set(DRAW_HAS_QT ON)
            list(APPEND PLATFORM_SOURCES
                qt_display_device.cpp
                qt_engine.cpp
            )
            list(APPEND PLATFORM_LIBS Qt5::Core Qt5::Gui Qt5::Widgets)
            message(STATUS "Qt5 rendering backend enabled")
        else()
            set(DRAW_HAS_QT OFF)
            message(STATUS "Qt not found, Qt rendering backend disabled")
        endif()
    endif()
endif()

if(WIN32)
    set(DRAW_HAS_GDIPLUS ON)
    set(DRAW_HAS_DIRECT2D ON)
    
    list(APPEND PLATFORM_SOURCES
        win32_display_device.cpp
    )
    
    if(NOT DRAW_HAS_QT)
        list(APPEND PLATFORM_SOURCES
            gdiplus_engine.cpp
            direct2d_engine.cpp
        )
    endif()
    
    list(APPEND PLATFORM_LIBS
        gdiplus
        d2d1
        dwrite
        d3d11
    )
    
elseif(APPLE)
    set(DRAW_HAS_COREGRAPHICS ON)
    set(DRAW_HAS_METAL ON)
    
    list(APPEND PLATFORM_SOURCES
        cocoa_display_device.cpp
    )
    
    if(NOT DRAW_HAS_QT)
        list(APPEND PLATFORM_SOURCES
            core_graphics_engine.cpp
            metal_engine.cpp
        )
    endif()
    
    list(APPEND PLATFORM_LIBS
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework Metal"
        "-framework CoreGraphics"
        "-framework CoreText"
    )
    
elseif(UNIX AND NOT APPLE)
    find_package(X11 QUIET)
    if(X11_FOUND)
        set(DRAW_HAS_X11 ON)
        list(APPEND PLATFORM_SOURCES x11_display_device.cpp)
    endif()
    
    find_package(PkgConfig QUIET)
    if(PkgConfig_FOUND)
        if(DRAW_USE_CAIRO)
            pkg_check_modules(CAIRO QUIET cairo)
            if(CAIRO_FOUND)
                set(DRAW_HAS_CAIRO ON)
                if(NOT DRAW_HAS_QT)
                    list(APPEND PLATFORM_SOURCES cairo_engine.cpp)
                endif()
                list(APPEND PLATFORM_INCLUDE_DIRS ${CAIRO_INCLUDE_DIRS})
                list(APPEND PLATFORM_LIBS ${CAIRO_LIBRARIES})
            endif()
            
            pkg_check_modules(PANGO QUIET pango pangocairo)
            if(PANGO_FOUND)
                list(APPEND PLATFORM_INCLUDE_DIRS ${PANGO_INCLUDE_DIRS})
                list(APPEND PLATFORM_LIBS ${PANGO_LIBRARIES})
            endif()
        endif()
    endif()
    
    if(DRAW_USE_OPENGL)
        find_package(OpenGL QUIET)
        if(OpenGL_FOUND)
            set(DRAW_HAS_OPENGL ON)
            list(APPEND PLATFORM_SOURCES opengl_engine.cpp)
            list(APPEND PLATFORM_LIBS OpenGL::GL)
        endif()
    endif()
    
elseif(EMSCRIPTEN)
    set(DRAW_HAS_WEBGL ON)
    list(APPEND PLATFORM_SOURCES
        webgl_device.cpp
        webgl_engine.cpp
    )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1")
endif()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/draw_config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/draw_config.h"
)

message(STATUS "=== Draw Rendering Backend Configuration ===")
message(STATUS "Qt:       ${DRAW_HAS_QT}")
message(STATUS "GDI+:     ${DRAW_HAS_GDIPLUS}")
message(STATUS "Direct2D: ${DRAW_HAS_DIRECT2D}")
message(STATUS "CoreGraphics: ${DRAW_HAS_COREGRAPHICS}")
message(STATUS "Metal:    ${DRAW_HAS_METAL}")
message(STATUS "Cairo:    ${DRAW_HAS_CAIRO}")
message(STATUS "OpenGL:   ${DRAW_HAS_OPENGL}")
message(STATUS "WebGL:    ${DRAW_HAS_WEBGL}")
```

#### 配置头文件模板

```cpp
// draw_config.h.in - 由CMake生成

#ifndef DRAW_CONFIG_H
#define DRAW_CONFIG_H

#cmakedefine DRAW_HAS_QT
#cmakedefine DRAW_HAS_GDIPLUS
#cmakedefine DRAW_HAS_DIRECT2D
#cmakedefine DRAW_HAS_COREGRAPHICS
#cmakedefine DRAW_HAS_METAL
#cmakedefine DRAW_HAS_CAIRO
#cmakedefine DRAW_HAS_OPENGL
#cmakedefine DRAW_HAS_VULKAN
#cmakedefine DRAW_HAS_WEBGL

#endif
```

### 模块依赖关系

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        模块依赖关系图                                    │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     核心模块 (必需)                               │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │ draw_device.h      - 设备抽象层                                  │   │
│  │ draw_engine.h      - 引擎抽象层                                  │   │
│  │ draw_context.h     - 上下文管理层                                │   │
│  │ draw_style.h       - 样式定义                                    │   │
│  │ draw_result.h      - 结果定义                                    │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                              │                                          │
│                              ▼                                          │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     可选模块 (按需编译)                           │   │
│  │─────────────────────────────────────────────────────────────────│   │
│  │                                                                 │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │   │
│  │  │ QtEngine    │  │ GDIPlusEng  │  │ Direct2DEng │             │   │
│  │  │ (DRAW_HAS_QT)│  │(DRAW_HAS_   │  │(DRAW_HAS_   │             │   │
│  │  │             │  │ GDIPLUS)    │  │ DIRECT2D)   │             │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘             │   │
│  │                                                                 │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐             │   │
│  │  │ CairoEngine │  │ MetalEngine │  │ WebGLEngine │             │   │
│  │  │(DRAW_HAS_   │  │(DRAW_HAS_   │  │(DRAW_HAS_   │             │   │
│  │  │ CAIRO)      │  │ METAL)      │  │ WEBGL)      │             │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘             │   │
│  │                                                                 │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
│  编译规则：                                                              │
│  • 核心模块始终编译                                                      │
│  • 可选模块仅在对应宏定义时编译                                          │
│  • 至少需要一个渲染后端                                                  │
│  • Qt后端可作为所有平台的跨平台选项                                      │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

---

## 附录

### A. 相关文件

| 文件路径 | 说明 |
|----------|------|
| `include/ogc/draw/draw_device.h` | DrawDevice基类定义 |
| `include/ogc/draw/draw_engine.h` | DrawEngine接口定义 |
| `include/ogc/draw/draw_context.h` | DrawContext绘制上下文 |
| `include/ogc/draw/device_type.h` | DeviceType枚举定义 |
| `include/ogc/draw/engine_type.h` | EngineType枚举定义 |
| `include/ogc/draw/draw_result.h` | DrawResult结果枚举 |
| `include/ogc/draw/draw_style.h` | DrawStyle样式定义 |
| `include/ogc/draw/draw_params.h` | DrawParams参数定义 |

### B. 参考设计

- Qt QPaintDevice/QPaintEngine/QPainter 架构
- Cairo 图形库设备抽象设计
- Skia 渲染引擎架构
