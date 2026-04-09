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

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>
#include <vector>

using namespace chart::parser;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
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
    return reinterpret_cast<ogc_iparser_t*>(parser);
}

void ogc_iparser_destroy(ogc_iparser_t* parser) {
    delete reinterpret_cast<IParser*>(parser);
}

int ogc_iparser_open(ogc_iparser_t* parser, const char* path) {
    return 0;
}

void ogc_iparser_close(ogc_iparser_t* parser) {
}

ogc_parse_result_t* ogc_iparser_parse(ogc_iparser_t* parser) {
    return nullptr;
}

ogc_parse_result_t* ogc_iparser_parse_incremental(ogc_iparser_t* parser, int batch_size) {
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
    return reinterpret_cast<ogc_s57_parser_t*>(new S57Parser());
}

void ogc_s57_parser_destroy(ogc_s57_parser_t* parser) {
    delete reinterpret_cast<S57Parser*>(parser);
}

int ogc_s57_parser_open(ogc_s57_parser_t* parser, const char* path) {
    return 0;
}

ogc_parse_result_t* ogc_s57_parser_parse(ogc_s57_parser_t* parser) {
    if (parser) {
        ParseResult result = reinterpret_cast<S57Parser*>(parser)->ParseChart("");
        if (result.success) {
            return reinterpret_cast<ogc_parse_result_t*>(new ParseResult(std::move(result)));
        }
    }
    return nullptr;
}

void ogc_s57_parser_set_feature_filter(ogc_s57_parser_t* parser, const char** features, int count) {
}

void ogc_s57_parser_set_spatial_filter(ogc_s57_parser_t* parser, const ogc_envelope_t* bounds) {
}

ogc_incremental_parser_t* ogc_incremental_parser_create(void) {
    return reinterpret_cast<ogc_incremental_parser_t*>(&IncrementalParser::Instance());
}

void ogc_incremental_parser_destroy(ogc_incremental_parser_t* parser) {
}

void ogc_incremental_parser_start(ogc_incremental_parser_t* parser, const char* path, int batch_size) {
    if (parser && path) {
        ParseConfig config;
        reinterpret_cast<IncrementalParser*>(parser)->ParseIncremental(SafeString(path), config);
    }
}

void ogc_incremental_parser_pause(ogc_incremental_parser_t* parser) {
}

void ogc_incremental_parser_resume(ogc_incremental_parser_t* parser) {
}

void ogc_incremental_parser_cancel(ogc_incremental_parser_t* parser) {
    if (parser) {
        reinterpret_cast<IncrementalParser*>(parser)->ClearAllStates();
    }
}

ogc_parse_result_t* ogc_incremental_parser_parse_next(ogc_incremental_parser_t* parser) {
    return nullptr;
}

int ogc_incremental_parser_has_more(const ogc_incremental_parser_t* parser) {
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
