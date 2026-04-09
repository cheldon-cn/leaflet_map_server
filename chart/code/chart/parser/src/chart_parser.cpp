#include "parser/chart_parser.h"
#include "parser/s57_parser.h"
#include "parser/error_handler.h"

#ifdef HAS_LIBXML2
#include "parser/s101_parser.h"
#endif

namespace chart {
namespace parser {

ChartParser& ChartParser::Instance() {
    static ChartParser instance;
    return instance;
}

ChartParser::ChartParser() 
    : m_initialized(false) {
}

ChartParser::~ChartParser() {
    if (m_initialized) {
        Shutdown();
    }
}

bool ChartParser::Initialize() {
    if (m_initialized) {
        return true;
    }
    
    LOG_INFO("Initializing ChartParser module...");
    
    m_initialized = true;
    LOG_INFO("ChartParser module initialized successfully");
    return true;
}

void ChartParser::Shutdown() {
    if (!m_initialized) {
        return;
    }
    
    LOG_INFO("Shutting down ChartParser module...");
    m_initialized = false;
    LOG_INFO("ChartParser module shut down successfully");
}

IParser* ChartParser::CreateParser(ChartFormat format) {
    if (!m_initialized) {
        LOG_ERROR("ChartParser not initialized");
        return nullptr;
    }
    
    switch (format) {
        case ChartFormat::S57:
            return new S57Parser();
        
#ifdef HAS_LIBXML2
        case ChartFormat::S101:
            return new S101Parser();
#endif
        
        default:
            LOG_ERROR("Unsupported chart format: {}", static_cast<int>(format));
            return nullptr;
    }
}

std::vector<ChartFormat> ChartParser::GetSupportedFormats() const {
    std::vector<ChartFormat> formats;
    formats.push_back(ChartFormat::S57);
    
#ifdef HAS_LIBXML2
    formats.push_back(ChartFormat::S101);
#endif
    
    return formats;
}

} // namespace parser
} // namespace chart
