#ifndef OGC_DRAW_TILE_DEVICE_H
#define OGC_DRAW_TILE_DEVICE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/raster_image_device.h"
#include <vector>
#include <memory>

namespace ogc {
namespace draw {

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

class OGC_DRAW_API TileDevice : public DrawDevice {
public:
    TileDevice(int totalWidth, int totalHeight, int tileSize = 256);
    ~TileDevice() override;

    TileDevice(const TileDevice&) = delete;
    TileDevice& operator=(const TileDevice&) = delete;
    TileDevice(TileDevice&&) = default;
    TileDevice& operator=(TileDevice&&) = default;

    DeviceType GetType() const override { return DeviceType::kTile; }
    std::string GetName() const override { return "TileDevice"; }
    int GetWidth() const override { return m_totalWidth; }
    int GetHeight() const override { return m_totalHeight; }
    double GetDpi() const override { return m_dpi; }
    void SetDpi(double dpi) override { m_dpi = dpi; }
    int GetColorDepth() const override { return 32; }

    DrawResult Initialize() override;
    DrawResult Finalize() override;
    DeviceState GetState() const override { return m_state; }
    bool IsReady() const override { return m_state == DeviceState::kReady; }

    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kTile; }
    void SetPreferredEngineType(EngineType type) override { m_preferredEngineType = type; }

    DeviceCapabilities QueryCapabilities() const override;
    bool IsFeatureAvailable(const std::string& featureName) const override;

    bool IsDeviceLost() const override { return m_state == DeviceState::kLost; }
    DrawResult RecoverDevice() override;

    int GetTileSize() const { return m_tileSize; }
    int GetTileCountX() const { return m_tileCountX; }
    int GetTileCountY() const { return m_tileCountY; }
    int GetTotalTileCount() const { return m_tileCountX * m_tileCountY; }

    TileInfo GetTileInfo(int indexX, int indexY) const;
    TileInfo GetTileInfoAtPixel(int pixelX, int pixelY) const;

    RasterImageDevice* GetTile(int indexX, int indexY);
    const RasterImageDevice* GetTile(int indexX, int indexY) const;

    void SetTileDirty(int indexX, int indexY, bool dirty = true);
    bool IsTileDirty(int indexX, int indexY) const;

    void MarkAllTilesDirty();
    void MarkAllTilesClean();

    std::vector<TileInfo> GetDirtyTiles() const;
    std::vector<TileInfo> GetTilesInRect(int x, int y, int w, int h) const;

    void SetOverlap(int overlap) { m_overlap = overlap; }
    int GetOverlap() const { return m_overlap; }

    bool IsValidTileIndex(int indexX, int indexY) const;

    void Clear(const Color& color);
    void ClearTile(int indexX, int indexY, const Color& color);

    bool SaveAllTiles(const std::string& directory, const std::string& prefix = "tile_",
                      OutputFormat format = OutputFormat::kPNG);

private:
    int TileIndex(int indexX, int indexY) const;
    void CreateTiles();

    int m_totalWidth;
    int m_totalHeight;
    int m_tileSize;
    int m_overlap;
    int m_tileCountX;
    int m_tileCountY;
    double m_dpi;
    DeviceState m_state;
    EngineType m_preferredEngineType;
    std::vector<std::unique_ptr<RasterImageDevice>> m_tiles;
    std::vector<bool> m_dirtyFlags;
};

}  
}  

#endif  
