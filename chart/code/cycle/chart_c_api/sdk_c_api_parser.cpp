/**
 * @file sdk_c_api_parser.cpp
 * @brief OGC Chart SDK C API - Chart Parser Module Implementation
 * @version v3.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/parser/chart_parser.h>
#include <ogc/parser/s57_parser.h>
#include <ogc/parser/incremental_parser.h>
#include <ogc/parser/iparser.h>
#include <ogc/parser/parse_result.h>
#include <ogc/parser/parse_config.h>
#include <ogc/parser/chart_format.h>
#include <ogc/geom/envelope.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>
#include <map>

using namespace chart::parser;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

ogc_chart_format_e ToCFormat(ChartFormat format) {
    switch (format) {
        case ChartFormat::S57: return OGC_CHART_FORMAT_S57;
        case ChartFormat::S100: return OGC_CHART_FORMAT_S100;
        case ChartFormat::S101: return OGC_CHART_FORMAT_S101;
        case ChartFormat::S102: return OGC_CHART_FORMAT_S102;
        case ChartFormat::S111: return OGC_CHART_FORMAT_UNKNOWN;
        case ChartFormat::S112: return OGC_CHART_FORMAT_UNKNOWN;
        default: return OGC_CHART_FORMAT_UNKNOWN;
    }
}

ChartFormat FromCFormat(ogc_chart_format_e format) {
    switch (format) {
        case OGC_CHART_FORMAT_S57: return ChartFormat::S57;
        case OGC_CHART_FORMAT_S100: return ChartFormat::S100;
        case OGC_CHART_FORMAT_S101: return ChartFormat::S101;
        case OGC_CHART_FORMAT_S102: return ChartFormat::S102;
        case OGC_CHART_FORMAT_GML: return ChartFormat::Unknown;
        default: return ChartFormat::Unknown;
    }
}

struct ParserContext {
    IParser* parser;
    std::string filePath;
    ParseConfig config;
    bool isOpen;
    
    ParserContext() : parser(nullptr), isOpen(false) {}
    ~ParserContext() {
        delete parser;
        parser = nullptr;
    }
};

static std::map<IParser*, ParserContext*> g_parserContexts;

ParserContext* GetContext(ogc_iparser_t* parser) {
    IParser* iparser = reinterpret_cast<IParser*>(parser);
    auto it = g_parserContexts.find(iparser);
    if (it != g_parserContexts.end()) {
        return it->second;
    }
    return nullptr;
}

struct S57ParserContext {
    S57Parser* parser;
    std::string filePath;
    std::vector<std::string> featureFilter;
    ogc::Envelope spatialFilter;
    bool hasSpatialFilter;
    bool isOpen;
    
    S57ParserContext() : parser(nullptr), hasSpatialFilter(false), isOpen(false) {}
    ~S57ParserContext() {
        delete parser;
        parser = nullptr;
    }
};

static std::map<S57Parser*, S57ParserContext*> g_s57ParserContexts;

S57ParserContext* GetS57Context(ogc_s57_parser_t* parser) {
    S57Parser* sparser = reinterpret_cast<S57Parser*>(parser);
    auto it = g_s57ParserContexts.find(sparser);
    if (it != g_s57ParserContexts.end()) {
        return it->second;
    }
    return nullptr;
}

struct IncrementalParserState {
    std::string filePath;
    ParseConfig config;
    int batchSize;
    bool isPaused;
    bool hasMore;
    size_t currentIndex;
    ParseResult lastResult;
    
    IncrementalParserState() : batchSize(1000), isPaused(false), hasMore(false), currentIndex(0) {}
};

static std::map<IncrementalParser*, IncrementalParserState*> g_incrementalStates;

IncrementalParserState* GetIncrementalState(ogc_incremental_parser_t* parser) {
    IncrementalParser* iparser = reinterpret_cast<IncrementalParser*>(parser);
    auto it = g_incrementalStates.find(iparser);
    if (it != g_incrementalStates.end()) {
        return it->second;
    }
    return nullptr;
}

}  

ogc_chart_parser_t* ogc_chart_parser_get_instance(void) {
    return reinterpret_cast<ogc_chart_parser_t*>(&ChartParser::Instance());
}

int ogc_chart_parser_initialize(ogc_chart_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<ChartParser*>(parser)->Initialize() ? 0 : -1;
    }
    return -1;
}

void ogc_chart_parser_shutdown(ogc_chart_parser_t* parser) {
    if (parser) {
        reinterpret_cast<ChartParser*>(parser)->Shutdown();
    }
}

int ogc_chart_parser_get_supported_formats(const ogc_chart_parser_t* parser, ogc_chart_format_e* formats, int max_count) {
    if (parser && formats && max_count > 0) {
        auto kernelFormats = reinterpret_cast<const ChartParser*>(parser)->GetSupportedFormats();
        int count = 0;
        for (const auto& f : kernelFormats) {
            if (count >= max_count) break;
            formats[count++] = ToCFormat(f);
        }
        return count;
    }
    return 0;
}

ogc_iparser_t* ogc_iparser_create(ogc_chart_format_e format) {
    auto& instance = ChartParser::Instance();
    IParser* parser = instance.CreateParser(FromCFormat(format));
    if (parser) {
        ParserContext* ctx = new ParserContext();
        ctx->parser = parser;
        g_parserContexts[parser] = ctx;
        return reinterpret_cast<ogc_iparser_t*>(parser);
    }
    return nullptr;
}

void ogc_iparser_destroy(ogc_iparser_t* parser) {
    IParser* iparser = reinterpret_cast<IParser*>(parser);
    auto it = g_parserContexts.find(iparser);
    if (it != g_parserContexts.end()) {
        ParserContext* ctx = it->second;
        g_parserContexts.erase(it);
        delete ctx;
    } else {
        delete iparser;
    }
}

int ogc_iparser_open(ogc_iparser_t* parser, const char* path) {
    ParserContext* ctx = GetContext(parser);
    if (ctx && ctx->parser && path) {
        ctx->filePath = SafeString(path);
        ctx->isOpen = true;
        return 0;
    }
    return -1;
}

void ogc_iparser_close(ogc_iparser_t* parser) {
    ParserContext* ctx = GetContext(parser);
    if (ctx) {
        ctx->filePath.clear();
        ctx->isOpen = false;
    }
}

ogc_parse_result_t* ogc_iparser_parse(ogc_iparser_t* parser) {
    ParserContext* ctx = GetContext(parser);
    if (ctx && ctx->parser && ctx->isOpen && !ctx->filePath.empty()) {
        ParseResult result = ctx->parser->ParseChart(ctx->filePath, ctx->config);
        if (result.success) {
            return reinterpret_cast<ogc_parse_result_t*>(new ParseResult(std::move(result)));
        }
    }
    return nullptr;
}

ogc_parse_result_t* ogc_iparser_parse_incremental(ogc_iparser_t* parser, int batch_size) {
    ParserContext* ctx = GetContext(parser);
    if (ctx && ctx->parser && ctx->isOpen && !ctx->filePath.empty()) {
        IncrementalParser& incParser = IncrementalParser::Instance();
        IncrementalParseResult incResult = incParser.ParseIncremental(ctx->filePath, ctx->config);
        if (incResult.hasChanges) {
            ParseResult* result = new ParseResult();
            result->success = true;
            result->features = incResult.addedFeatures.features;
            result->features.insert(result->features.end(),
                incResult.modifiedFeatures.features.begin(),
                incResult.modifiedFeatures.features.end());
            return reinterpret_cast<ogc_parse_result_t*>(result);
        }
    }
    return nullptr;
}

ogc_chart_format_e ogc_iparser_get_format(const ogc_iparser_t* parser) {
    if (parser) {
        auto formats = reinterpret_cast<const IParser*>(parser)->GetSupportedFormats();
        if (!formats.empty()) {
            return ToCFormat(formats[0]);
        }
    }
    return OGC_CHART_FORMAT_UNKNOWN;
}

ogc_s57_parser_t* ogc_s57_parser_create(void) {
    S57Parser* parser = new S57Parser();
    if (parser) {
        S57ParserContext* ctx = new S57ParserContext();
        ctx->parser = parser;
        g_s57ParserContexts[parser] = ctx;
        return reinterpret_cast<ogc_s57_parser_t*>(parser);
    }
    return nullptr;
}

void ogc_s57_parser_destroy(ogc_s57_parser_t* parser) {
    S57Parser* sparser = reinterpret_cast<S57Parser*>(parser);
    auto it = g_s57ParserContexts.find(sparser);
    if (it != g_s57ParserContexts.end()) {
        S57ParserContext* ctx = it->second;
        g_s57ParserContexts.erase(it);
        delete ctx;
    } else {
        delete sparser;
    }
}

int ogc_s57_parser_open(ogc_s57_parser_t* parser, const char* path) {
    S57ParserContext* ctx = GetS57Context(parser);
    if (ctx && ctx->parser && path) {
        ctx->filePath = SafeString(path);
        ctx->isOpen = true;
        return 0;
    }
    return -1;
}

ogc_parse_result_t* ogc_s57_parser_parse(ogc_s57_parser_t* parser) {
    S57ParserContext* ctx = GetS57Context(parser);
    if (ctx && ctx->parser && ctx->isOpen && !ctx->filePath.empty()) {
        ParseConfig config;
        config.validateGeometry = true;
        config.validateAttributes = true;
        ParseResult result = ctx->parser->ParseChart(ctx->filePath, config);
        if (result.success) {
            return reinterpret_cast<ogc_parse_result_t*>(new ParseResult(std::move(result)));
        }
    }
    return nullptr;
}

void ogc_s57_parser_set_feature_filter(ogc_s57_parser_t* parser, const char** features, int count) {
    S57ParserContext* ctx = GetS57Context(parser);
    if (ctx && features && count > 0) {
        ctx->featureFilter.clear();
        for (int i = 0; i < count; ++i) {
            if (features[i]) {
                ctx->featureFilter.push_back(SafeString(features[i]));
            }
        }
    }
}

void ogc_s57_parser_set_spatial_filter(ogc_s57_parser_t* parser, const ogc_envelope_t* bounds) {
    S57ParserContext* ctx = GetS57Context(parser);
    if (ctx && bounds) {
        double min_x = ogc_envelope_get_min_x(bounds);
        double min_y = ogc_envelope_get_min_y(bounds);
        double max_x = ogc_envelope_get_max_x(bounds);
        double max_y = ogc_envelope_get_max_y(bounds);
        ctx->spatialFilter = ogc::Envelope(min_x, min_y, max_x, max_y);
        ctx->hasSpatialFilter = true;
    }
}

ogc_incremental_parser_t* ogc_incremental_parser_create(void) {
    IncrementalParser& instance = IncrementalParser::Instance();
    IncrementalParserState* state = new IncrementalParserState();
    g_incrementalStates[&instance] = state;
    return reinterpret_cast<ogc_incremental_parser_t*>(&instance);
}

void ogc_incremental_parser_destroy(ogc_incremental_parser_t* parser) {
    IncrementalParser* iparser = reinterpret_cast<IncrementalParser*>(parser);
    auto it = g_incrementalStates.find(iparser);
    if (it != g_incrementalStates.end()) {
        IncrementalParserState* state = it->second;
        g_incrementalStates.erase(it);
        delete state;
    }
}

void ogc_incremental_parser_start(ogc_incremental_parser_t* parser, const char* path, int batch_size) {
    IncrementalParserState* state = GetIncrementalState(parser);
    if (state && path) {
        state->filePath = SafeString(path);
        state->batchSize = batch_size > 0 ? batch_size : 1000;
        state->isPaused = false;
        state->hasMore = true;
        state->currentIndex = 0;
        IncrementalParser* iparser = reinterpret_cast<IncrementalParser*>(parser);
        IncrementalParseResult result = iparser->ParseIncremental(state->filePath, state->config);
        state->lastResult = result.addedFeatures;
        state->hasMore = result.hasChanges;
    }
}

void ogc_incremental_parser_pause(ogc_incremental_parser_t* parser) {
    IncrementalParserState* state = GetIncrementalState(parser);
    if (state) {
        state->isPaused = true;
    }
}

void ogc_incremental_parser_resume(ogc_incremental_parser_t* parser) {
    IncrementalParserState* state = GetIncrementalState(parser);
    if (state) {
        state->isPaused = false;
    }
}

void ogc_incremental_parser_cancel(ogc_incremental_parser_t* parser) {
    IncrementalParserState* state = GetIncrementalState(parser);
    if (state) {
        state->isPaused = false;
        state->hasMore = false;
        state->filePath.clear();
        state->lastResult = ParseResult();
    }
    if (parser) {
        reinterpret_cast<IncrementalParser*>(parser)->ClearAllStates();
    }
}

ogc_parse_result_t* ogc_incremental_parser_parse_next(ogc_incremental_parser_t* parser) {
    IncrementalParserState* state = GetIncrementalState(parser);
    if (state && !state->isPaused && state->hasMore && !state->filePath.empty()) {
        IncrementalParser* iparser = reinterpret_cast<IncrementalParser*>(parser);
        IncrementalParseResult incResult = iparser->ParseIncremental(state->filePath, state->config);
        if (incResult.hasChanges) {
            ParseResult* result = new ParseResult();
            result->success = true;
            result->features = incResult.addedFeatures.features;
            result->features.insert(result->features.end(),
                incResult.modifiedFeatures.features.begin(),
                incResult.modifiedFeatures.features.end());
            return reinterpret_cast<ogc_parse_result_t*>(result);
        } else {
            state->hasMore = false;
        }
    }
    return nullptr;
}

int ogc_incremental_parser_has_more(const ogc_incremental_parser_t* parser) {
    IncrementalParserState* state = GetIncrementalState(const_cast<ogc_incremental_parser_t*>(parser));
    if (state) {
        return state->hasMore && !state->isPaused ? 1 : 0;
    }
    return 0;
}

float ogc_incremental_parser_get_progress(const ogc_incremental_parser_t* parser) {
    return 0.0f;
}

int ogc_incremental_parser_has_file_changed(const ogc_incremental_parser_t* parser, const char* path) {
    if (parser && path) {
        return reinterpret_cast<const IncrementalParser*>(parser)->HasFileChanged(SafeString(path)) ? 1 : 0;
    }
    return 0;
}

void ogc_incremental_parser_mark_processed(ogc_incremental_parser_t* parser, const char* path) {
    if (parser && path) {
        reinterpret_cast<IncrementalParser*>(parser)->MarkFileProcessed(SafeString(path));
    }
}

void ogc_incremental_parser_clear_state(ogc_incremental_parser_t* parser, const char* path) {
    if (parser && path) {
        reinterpret_cast<IncrementalParser*>(parser)->ClearFileState(SafeString(path));
    }
}

ogc_parse_result_t* ogc_s57_parser_parse_file(ogc_s57_parser_t* parser, const char* path, const ogc_parse_config_t* config) {
    if (parser && path) {
        ParseConfig cfg;
        if (config) {
            cfg.validateGeometry = config->validate_geometry != 0;
            cfg.validateAttributes = config->validate_attributes != 0;
            cfg.strictMode = config->strict_mode != 0;
            cfg.includeMetadata = config->include_metadata != 0;
            cfg.maxFeatureCount = config->max_feature_count;
            cfg.coordinateSystem = SafeString(config->coordinate_system);
            cfg.tolerance = config->tolerance;
        }
        ParseResult result = reinterpret_cast<S57Parser*>(parser)->ParseChart(SafeString(path), cfg);
        if (result.success) {
            return reinterpret_cast<ogc_parse_result_t*>(new ParseResult(std::move(result)));
        }
    }
    return nullptr;
}

ogc_incremental_parse_result_t* ogc_incremental_parser_parse_incremental(ogc_incremental_parser_t* parser, const char* path, const ogc_parse_config_t* config) {
    if (parser && path) {
        ParseConfig cfg;
        if (config) {
            cfg.validateGeometry = config->validate_geometry != 0;
            cfg.validateAttributes = config->validate_attributes != 0;
            cfg.strictMode = config->strict_mode != 0;
            cfg.includeMetadata = config->include_metadata != 0;
            cfg.maxFeatureCount = config->max_feature_count;
            cfg.coordinateSystem = SafeString(config->coordinate_system);
            cfg.tolerance = config->tolerance;
        }
        IncrementalParseResult result = reinterpret_cast<IncrementalParser*>(parser)->ParseIncremental(SafeString(path), cfg);
        ogc_incremental_parse_result_t* c_result = static_cast<ogc_incremental_parse_result_t*>(std::malloc(sizeof(ogc_incremental_parse_result_t)));
        if (c_result) {
            c_result->has_changes = result.hasChanges ? 1 : 0;
            c_result->parse_time_ms = result.parseTimeMs;
        }
        return c_result;
    }
    return nullptr;
}

void ogc_incremental_parse_result_destroy(ogc_incremental_parse_result_t* result) {
    std::free(result);
}

#ifdef __cplusplus
}
#endif
