#include "ogc/draw/tile_device.h"
#include "ogc/draw/simple2d_engine.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace ogc {
namespace draw {

TileDevice::TileDevice(int totalWidth, int totalHeight, int tileSize)
    : m_totalWidth(totalWidth)
    , m_totalHeight(totalHeight)
    , m_tileSize(tileSize)
    , m_overlap(0)
    , m_dpi(96.0)
    , m_state(DeviceState::kUninitialized)
    , m_preferredEngineType(EngineType::kTile) {
    if (m_totalWidth > 0 && m_totalHeight > 0 && m_tileSize > 0) {
        m_tileCountX = (m_totalWidth + m_tileSize - 1) / m_tileSize;
        m_tileCountY = (m_totalHeight + m_tileSize - 1) / m_tileSize;
    } else {
        m_tileCountX = 0;
        m_tileCountY = 0;
    }
}

TileDevice::~TileDevice() {
    Finalize();
}

DrawResult TileDevice::Initialize() {
    if (m_totalWidth <= 0 || m_totalHeight <= 0 || m_tileSize <= 0) {
        m_state = DeviceState::kError;
        return DrawResult::kInvalidParameter;
    }

    CreateTiles();
    m_state = DeviceState::kReady;
    return DrawResult::kSuccess;
}

DrawResult TileDevice::Finalize() {
    m_tiles.clear();
    m_dirtyFlags.clear();
    m_state = DeviceState::kUninitialized;
    return DrawResult::kSuccess;
}

void TileDevice::CreateTiles() {
    int totalTiles = m_tileCountX * m_tileCountY;
    m_tiles.clear();
    m_dirtyFlags.clear();
    m_tiles.reserve(totalTiles);
    m_dirtyFlags.resize(totalTiles, true);

    for (int ty = 0; ty < m_tileCountY; ++ty) {
        for (int tx = 0; tx < m_tileCountX; ++tx) {
            int pixelX = tx * m_tileSize;
            int pixelY = ty * m_tileSize;
            
            int tileWidth = std::min(m_tileSize, m_totalWidth - pixelX);
            int tileHeight = std::min(m_tileSize, m_totalHeight - pixelY);

            auto tile = std::unique_ptr<RasterImageDevice>(
                new RasterImageDevice(tileWidth, tileHeight, PixelFormat::kRGBA8888));
            tile->Initialize();
            m_tiles.push_back(std::move(tile));
        }
    }
}

std::unique_ptr<DrawEngine> TileDevice::CreateEngine() {
    return std::unique_ptr<DrawEngine>(new Simple2DEngine(nullptr));
}

std::vector<EngineType> TileDevice::GetSupportedEngineTypes() const {
    return { EngineType::kTile, EngineType::kSimple2D };
}

DeviceCapabilities TileDevice::QueryCapabilities() const {
    DeviceCapabilities caps;
    caps.supportsGPU = false;
    caps.supportsMultithreading = true;
    caps.supportsVSync = false;
    caps.supportsDoubleBuffer = false;
    caps.supportsPartialUpdate = true;
    caps.supportsAlpha = true;
    caps.supportsAntialiasing = true;
    caps.maxTextureSize = m_tileSize;
    caps.maxRenderTargets = 1;
    caps.supportedFormats = { "RGBA8888" };
    return caps;
}

bool TileDevice::IsFeatureAvailable(const std::string& featureName) const {
    if (featureName == "tiling" || featureName == "partial_update") {
        return true;
    }
    return false;
}

DrawResult TileDevice::RecoverDevice() {
    if (m_state == DeviceState::kLost) {
        CreateTiles();
        m_state = DeviceState::kReady;
        return DrawResult::kSuccess;
    }
    return DrawResult::kDeviceError;
}

int TileDevice::TileIndex(int indexX, int indexY) const {
    return indexY * m_tileCountX + indexX;
}

bool TileDevice::IsValidTileIndex(int indexX, int indexY) const {
    return indexX >= 0 && indexX < m_tileCountX &&
           indexY >= 0 && indexY < m_tileCountY;
}

TileInfo TileDevice::GetTileInfo(int indexX, int indexY) const {
    TileInfo info;
    
    if (!IsValidTileIndex(indexX, indexY)) {
        return info;
    }

    info.indexX = indexX;
    info.indexY = indexY;
    info.pixelX = indexX * m_tileSize;
    info.pixelY = indexY * m_tileSize;
    info.width = std::min(m_tileSize, m_totalWidth - info.pixelX);
    info.height = std::min(m_tileSize, m_totalHeight - info.pixelY);
    info.isValid = true;
    info.isDirty = m_dirtyFlags[TileIndex(indexX, indexY)];

    return info;
}

TileInfo TileDevice::GetTileInfoAtPixel(int pixelX, int pixelY) const {
    int indexX = pixelX / m_tileSize;
    int indexY = pixelY / m_tileSize;
    return GetTileInfo(indexX, indexY);
}

RasterImageDevice* TileDevice::GetTile(int indexX, int indexY) {
    if (!IsValidTileIndex(indexX, indexY)) {
        return nullptr;
    }
    return m_tiles[TileIndex(indexX, indexY)].get();
}

const RasterImageDevice* TileDevice::GetTile(int indexX, int indexY) const {
    if (!IsValidTileIndex(indexX, indexY)) {
        return nullptr;
    }
    return m_tiles[TileIndex(indexX, indexY)].get();
}

void TileDevice::SetTileDirty(int indexX, int indexY, bool dirty) {
    if (IsValidTileIndex(indexX, indexY)) {
        m_dirtyFlags[TileIndex(indexX, indexY)] = dirty;
    }
}

bool TileDevice::IsTileDirty(int indexX, int indexY) const {
    if (!IsValidTileIndex(indexX, indexY)) {
        return false;
    }
    return m_dirtyFlags[TileIndex(indexX, indexY)];
}

void TileDevice::MarkAllTilesDirty() {
    std::fill(m_dirtyFlags.begin(), m_dirtyFlags.end(), true);
}

void TileDevice::MarkAllTilesClean() {
    std::fill(m_dirtyFlags.begin(), m_dirtyFlags.end(), false);
}

std::vector<TileInfo> TileDevice::GetDirtyTiles() const {
    std::vector<TileInfo> dirtyTiles;
    
    for (int ty = 0; ty < m_tileCountY; ++ty) {
        for (int tx = 0; tx < m_tileCountX; ++tx) {
            if (IsTileDirty(tx, ty)) {
                dirtyTiles.push_back(GetTileInfo(tx, ty));
            }
        }
    }
    
    return dirtyTiles;
}

std::vector<TileInfo> TileDevice::GetTilesInRect(int x, int y, int w, int h) const {
    std::vector<TileInfo> tiles;

    int startX = std::max(0, x / m_tileSize);
    int startY = std::max(0, y / m_tileSize);
    int endX = std::min(m_tileCountX - 1, (x + w) / m_tileSize);
    int endY = std::min(m_tileCountY - 1, (y + h) / m_tileSize);

    for (int ty = startY; ty <= endY; ++ty) {
        for (int tx = startX; tx <= endX; ++tx) {
            tiles.push_back(GetTileInfo(tx, ty));
        }
    }

    return tiles;
}

void TileDevice::Clear(const Color& color) {
    for (auto& tile : m_tiles) {
        if (tile) {
            tile->Clear(color);
        }
    }
    MarkAllTilesDirty();
}

void TileDevice::ClearTile(int indexX, int indexY, const Color& color) {
    RasterImageDevice* tile = GetTile(indexX, indexY);
    if (tile) {
        tile->Clear(color);
        SetTileDirty(indexX, indexY, true);
    }
}

bool TileDevice::SaveAllTiles(const std::string& directory, const std::string& prefix,
                               OutputFormat format) {
    bool allSuccess = true;

    for (int ty = 0; ty < m_tileCountY; ++ty) {
        for (int tx = 0; tx < m_tileCountX; ++tx) {
            RasterImageDevice* tile = GetTile(tx, ty);
            if (!tile) {
                allSuccess = false;
                continue;
            }

            std::ostringstream oss;
            oss << directory << "/" << prefix
                << std::setfill('0') << std::setw(4) << tx << "_"
                << std::setfill('0') << std::setw(4) << ty;

            switch (format) {
                case OutputFormat::kPNG:
                    oss << ".png";
                    break;
                case OutputFormat::kJPEG:
                    oss << ".jpg";
                    break;
                case OutputFormat::kBMP:
                    oss << ".bmp";
                    break;
                default:
                    oss << ".png";
                    break;
            }

            if (!tile->SaveToFile(oss.str(), format)) {
                allSuccess = false;
            }
        }
    }

    return allSuccess;
}

}  
}  
