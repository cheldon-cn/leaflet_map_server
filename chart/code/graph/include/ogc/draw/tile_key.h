#ifndef OGC_DRAW_TILE_KEY_H
#define OGC_DRAW_TILE_KEY_H

#include "ogc/envelope.h"
#include <cstdint>
#include <string>
#include <functional>
#include <memory>

namespace ogc {
namespace draw {

struct TileKey {
    int x;
    int y;
    int z;
    
    TileKey() : x(0), y(0), z(0) {}
    TileKey(int tileX, int tileY, int tileLevel) : x(tileX), y(tileY), z(tileLevel) {}
    
    bool operator==(const TileKey& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
    
    bool operator!=(const TileKey& other) const {
        return !(*this == other);
    }
    
    bool operator<(const TileKey& other) const {
        if (z != other.z) return z < other.z;
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
    
    std::string ToString() const {
        return std::to_string(z) + "/" + std::to_string(x) + "/" + std::to_string(y);
    }
    
    static TileKey FromString(const std::string& str) {
        TileKey key;
        size_t pos1 = str.find('/');
        size_t pos2 = str.find('/', pos1 + 1);
        if (pos1 != std::string::npos && pos2 != std::string::npos) {
            key.z = std::stoi(str.substr(0, pos1));
            key.x = std::stoi(str.substr(pos1 + 1, pos2 - pos1 - 1));
            key.y = std::stoi(str.substr(pos2 + 1));
        }
        return key;
    }
    
    uint64_t ToIndex() const {
        uint64_t index = static_cast<uint64_t>(z) << 48;
        index |= (static_cast<uint64_t>(x) & 0xFFFFFF) << 24;
        index |= static_cast<uint64_t>(y) & 0xFFFFFF;
        return index;
    }
    
    static TileKey FromIndex(uint64_t index) {
        TileKey key;
        key.z = static_cast<int>((index >> 48) & 0xFFFF);
        key.x = static_cast<int>((index >> 24) & 0xFFFFFF);
        key.y = static_cast<int>(index & 0xFFFFFF);
        return key;
    }
    
    TileKey GetParent() const {
        if (z <= 0) return TileKey();
        return TileKey(x / 2, y / 2, z - 1);
    }
    
    TileKey GetChild(int index) const {
        if (index < 0 || index > 3) return TileKey();
        int childX = x * 2 + (index % 2);
        int childY = y * 2 + (index / 2);
        return TileKey(childX, childY, z + 1);
    }
    
    void GetChildren(TileKey children[4]) const {
        for (int i = 0; i < 4; ++i) {
            children[i] = GetChild(i);
        }
    }
    
    bool IsValid() const {
        if (z < 0) return false;
        int maxTiles = 1 << z;
        return x >= 0 && x < maxTiles && y >= 0 && y < maxTiles;
    }
    
    int GetMaxTiles() const {
        return 1 << z;
    }
};

class TilePyramid;
typedef std::shared_ptr<TilePyramid> TilePyramidPtr;

class TilePyramid {
public:
    TilePyramid();
    explicit TilePyramid(const std::string& name);
    ~TilePyramid() = default;
    
    void SetName(const std::string& name);
    std::string GetName() const;
    
    void SetMinLevel(int level);
    int GetMinLevel() const;
    
    void SetMaxLevel(int level);
    int GetMaxLevel() const;
    
    void SetTileSize(int width, int height);
    void GetTileSize(int& width, int& height) const;
    
    void SetOrigin(double x, double y);
    void GetOrigin(double& x, double& y) const;
    
    void SetResolution(double resolution);
    double GetResolution() const;
    
    void SetExtent(const Envelope& extent);
    Envelope GetExtent() const;
    
    Envelope GetTileExtent(const TileKey& key) const;
    Envelope GetTileExtent(int x, int y, int z) const;
    
    TileKey PointToTile(double x, double y, int z) const;
    
    std::vector<TileKey> GetTilesForExtent(const Envelope& extent, int z) const;
    
    double GetResolution(int z) const;
    
    int GetLevelForResolution(double resolution) const;
    
    int GetTileCount(int z) const;
    
    static TilePyramidPtr Create();
    static TilePyramidPtr CreateWebMercator();
    static TilePyramidPtr CreateGeodetic();
    
private:
    double LonToTileX(double lon, int z) const;
    double LatToTileY(double lat, int z) const;
    double TileXToLon(int x, int z) const;
    double TileYToLat(int y, int z) const;
    
    std::string m_name;
    int m_minLevel;
    int m_maxLevel;
    int m_tileWidth;
    int m_tileHeight;
    double m_originX;
    double m_originY;
    double m_resolution;
    Envelope m_extent;
};

}
}

namespace std {
template<>
struct hash<ogc::draw::TileKey> {
    size_t operator()(const ogc::draw::TileKey& key) const {
        return hash<uint64_t>()(key.ToIndex());
    }
};
}

#endif
