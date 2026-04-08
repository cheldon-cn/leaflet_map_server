/**
 * @file sdk_c_api_parser.cpp
 * @brief OGC Chart SDK C API - Chart Parser Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/parser/chart_parser.h>
#include <ogc/parser/s57_parser.h>
#include <ogc/parser/incremental_parser.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::parser;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

}  

ogc_chart_parser_t* ogc_chart_parser_create(void) {
    return reinterpret_cast<ogc_chart_parser_t*>(ChartParser::Create().release());
}

void ogc_chart_parser_destroy(ogc_chart_parser_t* parser) {
    delete reinterpret_cast<ChartParser*>(parser);
}

int ogc_chart_parser_open(ogc_chart_parser_t* parser, const char* filepath) {
    if (parser && filepath) {
        return reinterpret_cast<ChartParser*>(parser)->Open(SafeString(filepath)) ? 1 : 0;
    }
    return 0;
}

void ogc_chart_parser_close(ogc_chart_parser_t* parser) {
    if (parser) {
        reinterpret_cast<ChartParser*>(parser)->Close();
    }
}

int ogc_chart_parser_is_open(const ogc_chart_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const ChartParser*>(parser)->IsOpen() ? 1 : 0;
    }
    return 0;
}

const char* ogc_chart_parser_get_format(const ogc_chart_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const ChartParser*>(parser)->GetFormat().c_str();
    }
    return "";
}

size_t ogc_chart_parser_get_layer_count(const ogc_chart_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const ChartParser*>(parser)->GetLayerCount();
    }
    return 0;
}

ogc_layer_t* ogc_chart_parser_get_layer(ogc_chart_parser_t* parser, size_t index) {
    if (parser) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<ChartParser*>(parser)->GetLayer(index));
    }
    return nullptr;
}

ogc_layer_t* ogc_chart_parser_get_layer_by_name(ogc_chart_parser_t* parser, const char* name) {
    if (parser && name) {
        return reinterpret_cast<ogc_layer_t*>(
            reinterpret_cast<ChartParser*>(parser)->GetLayerByName(SafeString(name)));
    }
    return nullptr;
}

ogc_envelope_t* ogc_chart_parser_get_extent(const ogc_chart_parser_t* parser) {
    if (parser) {
        const ogc::geom::Envelope& env = reinterpret_cast<const ChartParser*>(parser)->GetExtent();
        return reinterpret_cast<ogc_envelope_t*>(new ogc::geom::Envelope(env));
    }
    return nullptr;
}

int ogc_chart_parser_read_feature(ogc_chart_parser_t* parser, ogc_feature_t** feature) {
    if (parser && feature) {
        return reinterpret_cast<ChartParser*>(parser)->ReadFeature(
            reinterpret_cast<ogc::feature::Feature**>(feature)) ? 1 : 0;
    }
    return 0;
}

void ogc_chart_parser_reset_reading(ogc_chart_parser_t* parser) {
    if (parser) {
        reinterpret_cast<ChartParser*>(parser)->ResetReading();
    }
}

size_t ogc_chart_parser_get_feature_count(const ogc_chart_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const ChartParser*>(parser)->GetFeatureCount();
    }
    return 0;
}

int ogc_chart_parser_parse_all(ogc_chart_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<ChartParser*>(parser)->ParseAll() ? 1 : 0;
    }
    return 0;
}

ogc_s57_parser_t* ogc_s57_parser_create(void) {
    return reinterpret_cast<ogc_s57_parser_t*>(S57Parser::Create().release());
}

void ogc_s57_parser_destroy(ogc_s57_parser_t* parser) {
    delete reinterpret_cast<S57Parser*>(parser);
}

int ogc_s57_parser_open(ogc_s57_parser_t* parser, const char* filepath) {
    if (parser && filepath) {
        return reinterpret_cast<S57Parser*>(parser)->Open(SafeString(filepath)) ? 1 : 0;
    }
    return 0;
}

void ogc_s57_parser_close(ogc_s57_parser_t* parser) {
    if (parser) {
        reinterpret_cast<S57Parser*>(parser)->Close();
    }
}

int ogc_s57_parser_is_open(const ogc_s57_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const S57Parser*>(parser)->IsOpen() ? 1 : 0;
    }
    return 0;
}

const char* ogc_s57_parser_get_edition(const ogc_s57_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const S57Parser*>(parser)->GetEdition().c_str();
    }
    return "";
}

const char* ogc_s57_parser_get_update_number(const ogc_s57_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const S57Parser*>(parser)->GetUpdateNumber().c_str();
    }
    return "";
}

const char* ogc_s57_parser_get_producer(const ogc_s57_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const S57Parser*>(parser)->GetProducer().c_str();
    }
    return "";
}

size_t ogc_s57_parser_get_object_class_count(const ogc_s57_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const S57Parser*>(parser)->GetObjectClassCount();
    }
    return 0;
}

const char* ogc_s57_parser_get_object_class_name(const ogc_s57_parser_t* parser, size_t index) {
    if (parser) {
        return reinterpret_cast<const S57Parser*>(parser)->GetObjectName(index).c_str();
    }
    return "";
}

ogc_feature_t* ogc_s57_parser_get_feature_by_id(ogc_s57_parser_t* parser, long long id) {
    if (parser) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<S57Parser*>(parser)->GetFeatureById(id));
    }
    return nullptr;
}

ogc_incremental_parser_t* ogc_incremental_parser_create(void) {
    return reinterpret_cast<ogc_incremental_parser_t*>(IncrementalParser::Create().release());
}

void ogc_incremental_parser_destroy(ogc_incremental_parser_t* parser) {
    delete reinterpret_cast<IncrementalParser*>(parser);
}

int ogc_incremental_parser_initialize(ogc_incremental_parser_t* parser, const char* base_path) {
    if (parser && base_path) {
        return reinterpret_cast<IncrementalParser*>(parser)->Initialize(SafeString(base_path)) ? 1 : 0;
    }
    return 0;
}

void ogc_incremental_parser_shutdown(ogc_incremental_parser_t* parser) {
    if (parser) {
        reinterpret_cast<IncrementalParser*>(parser)->Shutdown();
    }
}

int ogc_incremental_parser_add_update_file(ogc_incremental_parser_t* parser, const char* filepath) {
    if (parser && filepath) {
        return reinterpret_cast<IncrementalParser*>(parser)->AddUpdateFile(SafeString(filepath)) ? 1 : 0;
    }
    return 0;
}

int ogc_incremental_parser_remove_update_file(ogc_incremental_parser_t* parser, const char* filepath) {
    if (parser && filepath) {
        return reinterpret_cast<IncrementalParser*>(parser)->RemoveUpdateFile(SafeString(filepath)) ? 1 : 0;
    }
    return 0;
}

size_t ogc_incremental_parser_get_update_count(const ogc_incremental_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<const IncrementalParser*>(parser)->GetUpdateCount();
    }
    return 0;
}

int ogc_incremental_parser_apply_updates(ogc_incremental_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<IncrementalParser*>(parser)->ApplyUpdates() ? 1 : 0;
    }
    return 0;
}

int ogc_incremental_parser_rollback_updates(ogc_incremental_parser_t* parser) {
    if (parser) {
        return reinterpret_cast<IncrementalParser*>(parser)->RollbackUpdates() ? 1 : 0;
    }
    return 0;
}

ogc_feature_t* ogc_incremental_parser_get_updated_feature(ogc_incremental_parser_t* parser, size_t index) {
    if (parser) {
        return reinterpret_cast<ogc_feature_t*>(
            reinterpret_cast<IncrementalParser*>(parser)->GetUpdatedFeature(index));
    }
    return nullptr;
}

#ifdef __cplusplus
}
#endif
