#ifndef OGC_DRAW_TYPES_H
#define OGC_DRAW_TYPES_H

#include <memory>
#include <vector>
#include <string>

namespace ogc {
namespace draw {

class DrawDevice;
class DrawEngine;
class DrawDriver;
class DrawContext;
class DrawFacade;
class Symbolizer;
class PointSymbolizer;
class LineSymbolizer;
class PolygonSymbolizer;
class TextSymbolizer;
class RasterSymbolizer;
class CompositeSymbolizer;
class IconSymbolizer;
class Filter;
class RuleEngine;
class TileCache;
class TileRenderer;
class LabelEngine;
class LabelPlacement;
class LabelConflict;

template<typename T>
using Ptr = std::shared_ptr<T>;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T>
using Vector = std::vector<T>;

using String = std::string;

using DrawDevicePtr = std::shared_ptr<DrawDevice>;
using DrawEnginePtr = std::shared_ptr<DrawEngine>;
using DrawDriverPtr = std::shared_ptr<DrawDriver>;
using DrawContextPtr = std::shared_ptr<DrawContext>;
using DrawFacadePtr = std::shared_ptr<DrawFacade>;
using SymbolizerPtr = std::shared_ptr<Symbolizer>;
using PointSymbolizerPtr = std::shared_ptr<PointSymbolizer>;
using LineSymbolizerPtr = std::shared_ptr<LineSymbolizer>;
using PolygonSymbolizerPtr = std::shared_ptr<PolygonSymbolizer>;
using TextSymbolizerPtr = std::shared_ptr<TextSymbolizer>;
using RasterSymbolizerPtr = std::shared_ptr<RasterSymbolizer>;
using CompositeSymbolizerPtr = std::shared_ptr<CompositeSymbolizer>;
using IconSymbolizerPtr = std::shared_ptr<IconSymbolizer>;
using FilterPtr = std::shared_ptr<Filter>;
using RuleEnginePtr = std::shared_ptr<RuleEngine>;
using TileCachePtr = std::shared_ptr<TileCache>;
using TileRendererPtr = std::shared_ptr<TileRenderer>;
using LabelEnginePtr = std::shared_ptr<LabelEngine>;
using LabelPlacementPtr = std::shared_ptr<LabelPlacement>;
using LabelConflictPtr = std::shared_ptr<LabelConflict>;

using SymbolizerList = std::vector<SymbolizerPtr>;
using FilterList = std::vector<FilterPtr>;

}  
}  

#endif  
