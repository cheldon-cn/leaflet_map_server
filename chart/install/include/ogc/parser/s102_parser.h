#ifndef S102_PARSER_H
#define S102_PARSER_H

#include "s100_parser.h"
#include <vector>

namespace chart {
namespace parser {

struct BathymetryPoint {
    double x;
    double y;
    double depth;
    double uncertainty;
    int quality;
    
    BathymetryPoint() : x(0), y(0), depth(0), uncertainty(0), quality(0) {}
};

struct BathymetryGrid {
    std::string id;
    int rows;
    int cols;
    double minX;
    double minY;
    double maxX;
    double maxY;
    double cellSizeX;
    double cellSizeY;
    std::vector<BathymetryPoint> points;
    
    BathymetryGrid() : rows(0), cols(0), minX(0), minY(0), maxX(0), maxY(0), cellSizeX(0), cellSizeY(0) {}
};

class S102Parser : public S100Parser {
public:
    S102Parser();
    virtual ~S102Parser();
    
    ParseResult ParseChart(const std::string& filePath, const ParseConfig& config = ParseConfig()) override;
    
    std::vector<ChartFormat> GetSupportedFormats() const override;
    
    std::string GetName() const override { return "S102Parser"; }
    std::string GetVersion() const override { return "1.0.0"; }
    
    bool ParseBathymetryGrid(const std::string& filePath, BathymetryGrid& grid);
    bool GetDepthAtPosition(const BathymetryGrid& grid, double x, double y, double& depth);
    
    struct Statistics {
        double minDepth;
        double maxDepth;
        double avgDepth;
        int pointCount;
        int invalidCount;
        
        Statistics() : minDepth(0), maxDepth(0), avgDepth(0), pointCount(0), invalidCount(0) {}
    };
    
    Statistics CalculateStatistics(const BathymetryGrid& grid);

private:
    bool ParseBathymetryLayer(void* dataset, BathymetryGrid& grid);
    bool ParseDepthValues(void* band, BathymetryGrid& grid, int rows, int cols);
};

} // namespace parser
} // namespace chart

#endif // S102_PARSER_H
