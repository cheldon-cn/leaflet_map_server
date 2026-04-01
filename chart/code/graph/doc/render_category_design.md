# 渲染架构设计文档

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
│  │ ┌───────────────┐ ┌─────────────────┐                              │ │
│  │ │ VectorEngine  │ │  WebGLEngine    │                              │ │
│  │ │ (PDF/SVG矢量) │ │ (WebAssembly)   │                              │ │
│  │ └───────────────┘ └─────────────────┘                              │ │
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
│  │ PdfDevice           │ ──────► │ VectorEngine (PDF矢量)          │   │
│  │ (无绘制方法)        │ Create  │ (执行所有绘制操作)              │   │
│  └─────────────────────┘ Engine  └─────────────────────────────────┘   │
│                                                                         │
│  ┌─────────────────────┐         ┌─────────────────────────────────┐   │
│  │ SvgDevice           │ ──────► │ VectorEngine (SVG矢量)          │   │
│  │ (无绘制方法)        │ Create  │ (执行所有绘制操作)              │   │
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
| 状态管理 | save()/restore() | PushTransform()/PopTransform()等 |
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
    
    // ==================== 变换栈管理（参照QPainter.save()/restore()） ====================
    
    void PushTransform();
    void PopTransform();
    void SetTransform(const TransformMatrix& transform);
    TransformMatrix GetTransform() const;
    void MultiplyTransform(const TransformMatrix& transform);
    void ResetTransform();
    
    // ==================== 变换便捷方法 ====================
    
    void Translate(double tx, double ty);
    void Scale(double sx, double sy);
    void Rotate(double angle);
    
    // ==================== 裁剪栈管理 ====================
    
    void PushClipRect(double x, double y, double width, double height);
    void PopClipRect();
    void ClearClipRect();
    
    // ==================== 样式栈管理 ====================
    
    void PushStyle(const DrawStyle& style);
    void PopStyle();
    void SetStyle(const DrawStyle& style);
    DrawStyle GetStyle() const;
    
    // ==================== 透明度栈管理 ====================
    
    void PushOpacity(double opacity);
    void PopOpacity();
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

context->PushTransform();
context->PushStyle();
context->PushOpacity(0.5);

context->Translate(100, 100);
context->Rotate(45);
context->DrawRect(0, 0, 200, 100, DrawStyle().SetFillColor(Color(255, 0, 0)));

context->PopOpacity();
context->PopStyle();
context->PopTransform();

context->DrawRect(0, 0, 100, 50, DrawStyle().SetFillColor(Color(0, 255, 0)));

context->EndDraw();
```

---

## 错误处理与异常安全

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
