#ifndef CYCLE_IRENDER_STRATEGY_H
#define CYCLE_IRENDER_STRATEGY_H

#include "IRenderEngine.h"
#include <memory>
#include <vector>

namespace cycle {

struct Feature {
    int64_t id;
    std::string layerName;
    std::string geometryType;
    std::vector<uint8_t> geometryData;
    std::map<std::string, std::string> attributes;
};

struct LayerStyle {
    std::string fillColor;
    std::string strokeColor;
    double fillOpacity;
    double strokeOpacity;
    double strokeWidth;
    std::string iconPath;
    
    LayerStyle()
        : fillColor("#ffffff")
        , strokeColor("#000000")
        , fillOpacity(1.0)
        , strokeOpacity(1.0)
        , strokeWidth(1.0) {}
};

struct RenderContext {
    BoundingBox bbox;
    int width;
    int height;
    int dpi;
    int zoomLevel;
    std::string backgroundColor;
    std::vector<std::string> layers;
    std::map<std::string, LayerStyle> layerStyles;
    std::string filter;
    
    RenderContext()
        : width(256)
        , height(256)
        , dpi(96)
        , zoomLevel(-1)
        , backgroundColor("#ffffff") {}
    
    void PushState();
    void PopState();
    void Clear();
    
    bool IsValid() const;
    
private:
    std::vector<RenderContext> m_stateStack;
};

class IImageBuffer {
public:
    virtual ~IImageBuffer() = default;
    
    virtual int GetWidth() const = 0;
    virtual int GetHeight() const = 0;
    virtual int GetChannels() const = 0;
    virtual int GetDpi() const = 0;
    
    virtual uint8_t* GetData() = 0;
    virtual const uint8_t* GetData() const = 0;
    virtual size_t GetDataSize() const = 0;
    
    virtual bool SetPixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
    virtual bool GetPixel(int x, int y, uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const = 0;
    
    virtual void Fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) = 0;
    virtual void Clear() = 0;
    
    virtual std::unique_ptr<IImageBuffer> Clone() const = 0;
    
    virtual bool ConvertTo(ImageFormat format, std::vector<uint8_t>& output) const = 0;
    virtual bool Scale(int newWidth, int newHeight, std::unique_ptr<IImageBuffer>& output) const = 0;
    
    virtual bool SaveToFile(const std::string& path, ImageFormat format) const = 0;
};

class IRenderStrategy {
public:
    virtual ~IRenderStrategy() = default;
    
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
    virtual bool IsInitialized() const = 0;
    
    virtual std::unique_ptr<IImageBuffer> CreateBuffer(
        int width, int height, int channels, int dpi) = 0;
    
    virtual bool RenderBackground(
        IImageBuffer* buffer,
        const std::string& backgroundColor) = 0;
    
    virtual bool RenderFeatures(
        IImageBuffer* buffer,
        const std::vector<Feature>& features,
        const LayerStyle& style) = 0;
    
    virtual bool RenderLayer(
        IImageBuffer* buffer,
        const std::string& layerName,
        const RenderContext& context) = 0;
    
    virtual bool EncodeImage(
        const IImageBuffer* buffer,
        ImageFormat format,
        int quality,
        std::vector<uint8_t>& output) = 0;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetDescription() const = 0;
    
    virtual bool SupportsFormat(ImageFormat format) const = 0;
    virtual bool SupportsHighDpi() const = 0;
    virtual int GetMaxImageSize() const = 0;
};

class IRenderStrategyFactory {
public:
    virtual ~IRenderStrategyFactory() = default;
    
    virtual std::unique_ptr<IRenderStrategy> CreateStrategy(
        const std::string& name) = 0;
    
    virtual std::vector<std::string> GetAvailableStrategies() const = 0;
    virtual std::string GetDefaultStrategy() const = 0;
};

} // namespace cycle

#endif // CYCLE_IRENDER_STRATEGY_H
