#include "ogc/draw/tile_key.h"
#include <cmath>
#include <algorithm>

namespace ogc {
namespace draw {

TilePyramid::TilePyramid()
    : m_name("default")
    , m_minLevel(0)
    , m_maxLevel(18)
    , m_tileWidth(256)
    , m_tileHeight(256)
    , m_originX(-20037508.34)
    , m_originY(20037508.34)
    , m_resolution(156543.033928)
    , m_extent(-20037508.34, -20037508.34, 20037508.34, 20037508.34) {
}

TilePyramid::TilePyramid(const std::string& name)
    : m_name(name)
    , m_minLevel(0)
    , m_maxLevel(18)
    , m_tileWidth(256)
    , m_tileHeight(256)
    , m_originX(-20037508.34)
    , m_originY(20037508.34)
    , m_resolution(156543.033928)
    , m_extent(-20037508.34, -20037508.34, 20037508.34, 20037508.34) {
}

void TilePyramid::SetName(const std::string& name) {
    m_name = name;
}

std::string TilePyramid::GetName() const {
    return m_name;
}

void TilePyramid::SetMinLevel(int level) {
    m_minLevel = std::max(0, level);
}

int TilePyramid::GetMinLevel() const {
    return m_minLevel;
}

void TilePyramid::SetMaxLevel(int level) {
    m_maxLevel = std::max(m_minLevel, level);
}

int TilePyramid::GetMaxLevel() const {
    return m_maxLevel;
}

void TilePyramid::SetTileSize(int width, int height) {
    m_tileWidth = std::max(1, width);
    m_tileHeight = std::max(1, height);
}

void TilePyramid::GetTileSize(int& width, int& height) const {
    width = m_tileWidth;
    height = m_tileHeight;
}

void TilePyramid::SetOrigin(double x, double y) {
    m_originX = x;
    m_originY = y;
}

void TilePyramid::GetOrigin(double& x, double& y) const {
    x = m_originX;
    y = m_originY;
}

void TilePyramid::SetResolution(double resolution) {
    m_resolution = std::max(0.0001, resolution);
}

double TilePyramid::GetResolution() const {
    return m_resolution;
}

void TilePyramid::SetExtent(const Envelope& extent) {
    m_extent = extent;
}

Envelope TilePyramid::GetExtent() const {
    return m_extent;
}

double TilePyramid::GetResolution(int z) const {
    return m_resolution / std::pow(2.0, z);
}

int TilePyramid::GetLevelForResolution(double resolution) const {
    if (resolution <= 0) {
        return m_maxLevel;
    }
    
    double ratio = m_resolution / resolution;
    int level = static_cast<int>(std::log2(ratio));
    
    return std::max(m_minLevel, std::min(m_maxLevel, level));
}

int TilePyramid::GetTileCount(int z) const {
    int count = 1 << z;
    return count * count;
}

double TilePyramid::LonToTileX(double lon, int z) const {
    int n = 1 << z;
    return (lon + 180.0) / 360.0 * n;
}

double TilePyramid::LatToTileY(double lat, int z) const {
    int n = 1 << z;
    double latRad = lat * 3.14159265358979323846 / 180.0;
    return (1.0 - std::log(std::tan(latRad) + 1.0 / std::cos(latRad)) / 3.14159265358979323846) / 2.0 * n;
}

double TilePyramid::TileXToLon(int x, int z) const {
    int n = 1 << z;
    return x / static_cast<double>(n) * 360.0 - 180.0;
}

double TilePyramid::TileYToLat(int y, int z) const {
    int n = 1 << z;
    double latRad = std::atan(std::sinh(3.14159265358979323846 * (1.0 - 2.0 * y / static_cast<double>(n))));
    return latRad * 180.0 / 3.14159265358979323846;
}

Envelope TilePyramid::GetTileExtent(const TileKey& key) const {
    return GetTileExtent(key.x, key.y, key.z);
}

Envelope TilePyramid::GetTileExtent(int x, int y, int z) const {
    int n = 1 << z;
    
    double minX = TileXToLon(x, z);
    double maxX = TileXToLon(x + 1, z);
    double minY = TileYToLat(y + 1, z);
    double maxY = TileYToLat(y, z);
    
    return Envelope(minX, minY, maxX, maxY);
}

TileKey TilePyramid::PointToTile(double x, double y, int z) const {
    int n = 1 << z;
    
    double tileX = LonToTileX(x, z);
    double tileY = LatToTileY(y, z);
    
    int tileXi = static_cast<int>(std::floor(tileX));
    int tileYi = static_cast<int>(std::floor(tileY));
    
    tileXi = std::max(0, std::min(n - 1, tileXi));
    tileYi = std::max(0, std::min(n - 1, tileYi));
    
    return TileKey(tileXi, tileYi, z);
}

std::vector<TileKey> TilePyramid::GetTilesForExtent(const Envelope& extent, int z) const {
    std::vector<TileKey> tiles;
    
    if (extent.IsNull()) {
        return tiles;
    }
    
    TileKey minTile = PointToTile(extent.GetMinX(), extent.GetMaxY(), z);
    TileKey maxTile = PointToTile(extent.GetMaxX(), extent.GetMinY(), z);
    
    int n = 1 << z;
    
    for (int x = minTile.x; x <= maxTile.x && x < n; ++x) {
        for (int y = minTile.y; y <= maxTile.y && y < n; ++y) {
            tiles.push_back(TileKey(x, y, z));
        }
    }
    
    return tiles;
}

TilePyramidPtr TilePyramid::Create() {
    return std::make_shared<TilePyramid>();
}

TilePyramidPtr TilePyramid::CreateWebMercator() {
    auto pyramid = std::make_shared<TilePyramid>("WebMercator");
    pyramid->SetMinLevel(0);
    pyramid->SetMaxLevel(22);
    pyramid->SetTileSize(256, 256);
    pyramid->SetOrigin(-20037508.34, 20037508.34);
    pyramid->SetResolution(156543.033928);
    pyramid->SetExtent(Envelope(-20037508.34, -20037508.34, 20037508.34, 20037508.34));
    return pyramid;
}

TilePyramidPtr TilePyramid::CreateGeodetic() {
    auto pyramid = std::make_shared<TilePyramid>("Geodetic");
    pyramid->SetMinLevel(0);
    pyramid->SetMaxLevel(22);
    pyramid->SetTileSize(256, 256);
    pyramid->SetOrigin(-180.0, 90.0);
    pyramid->SetResolution(0.703125);
    pyramid->SetExtent(Envelope(-180.0, -90.0, 180.0, 90.0));
    return pyramid;
}

}
}
