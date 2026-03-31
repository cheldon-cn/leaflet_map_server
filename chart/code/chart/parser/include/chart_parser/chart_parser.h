#ifndef CHART_PARSER_H
#define CHART_PARSER_H

#include <string>
#include <memory>
#include <vector>
#include "iparser.h"
#include "chart_format.h"
#include "parse_config.h"

namespace chart {
namespace parser {

class ChartParser {
public:
    static ChartParser& Instance();
    
    bool Initialize();
    void Shutdown();
    
    IParser* CreateParser(ChartFormat format);
    std::vector<ChartFormat> GetSupportedFormats() const;
    
    bool IsInitialized() const { return m_initialized; }
    
private:
    ChartParser();
    ~ChartParser();
    
    ChartParser(const ChartParser&) = delete;
    ChartParser& operator=(const ChartParser&) = delete;
    
    bool m_initialized;
};

} // namespace parser
} // namespace chart

#endif // CHART_PARSER_H
