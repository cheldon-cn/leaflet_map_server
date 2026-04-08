/**
 * @file sdk_c_api_symbology.cpp
 * @brief OGC Chart SDK C API - Symbology Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/symbology/filter.h>
#include <ogc/symbology/symbolizer.h>
#include <ogc/symbology/symbolizer_rule.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc::symbology;

#ifdef __cplusplus
extern "C" {
#endif

namespace {

std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

}  

ogc_filter_t* ogc_filter_create(int type) {
    return reinterpret_cast<ogc_filter_t*>(Filter::Create(static_cast<FilterType>(type)).release());
}

void ogc_filter_destroy(ogc_filter_t* filter) {
    delete reinterpret_cast<Filter*>(filter);
}

int ogc_filter_get_type(const ogc_filter_t* filter) {
    if (filter) {
        return static_cast<int>(reinterpret_cast<const Filter*>(filter)->GetType());
    }
    return 0;
}

int ogc_filter_evaluate(const ogc_filter_t* filter, const ogc_feature_t* feature) {
    if (filter && feature) {
        return reinterpret_cast<const Filter*>(filter)->Evaluate(
            reinterpret_cast<const ogc::feature::Feature*>(feature)) ? 1 : 0;
    }
    return 0;
}

ogc_filter_t* ogc_filter_create_comparison(const char* field_name, int op, const char* value) {
    return reinterpret_cast<ogc_filter_t*>(
        Filter::CreateComparison(SafeString(field_name), 
                                 static_cast<ComparisonOp>(op),
                                 SafeString(value)).release());
}

ogc_filter_t* ogc_filter_create_logical(int op, ogc_filter_t** filters, size_t count) {
    if (filters && count > 0) {
        std::vector<FilterPtr> filterList;
        for (size_t i = 0; i < count; ++i) {
            filterList.push_back(FilterPtr(reinterpret_cast<Filter*>(filters[i])));
        }
        return reinterpret_cast<ogc_filter_t*>(
            Filter::CreateLogical(static_cast<LogicalOp>(op), std::move(filterList)).release());
    }
    return nullptr;
}

ogc_filter_t* ogc_filter_create_spatial(int op, const ogc_geometry_t* geom) {
    if (geom) {
        return reinterpret_cast<ogc_filter_t*>(
            Filter::CreateSpatial(static_cast<SpatialOp>(op),
                                  reinterpret_cast<const ogc::geom::Geometry*>(geom)).release());
    }
    return nullptr;
}

ogc_symbolizer_t* ogc_symbolizer_create(int type) {
    return reinterpret_cast<ogc_symbolizer_t*>(
        Symbolizer::Create(static_cast<SymbolizerType>(type)).release());
}

void ogc_symbolizer_destroy(ogc_symbolizer_t* symbolizer) {
    delete reinterpret_cast<Symbolizer*>(symbolizer);
}

int ogc_symbolizer_get_type(const ogc_symbolizer_t* symbolizer) {
    if (symbolizer) {
        return static_cast<int>(reinterpret_cast<const Symbolizer*>(symbolizer)->GetType());
    }
    return 0;
}

ogc_draw_style_t* ogc_symbolizer_get_style(ogc_symbolizer_t* symbolizer) {
    if (symbolizer) {
        return reinterpret_cast<ogc_draw_style_t*>(
            reinterpret_cast<Symbolizer*>(symbolizer)->GetStyle());
    }
    return nullptr;
}

void ogc_symbolizer_set_style(ogc_symbolizer_t* symbolizer, ogc_draw_style_t* style) {
    if (symbolizer && style) {
        reinterpret_cast<Symbolizer*>(symbolizer)->SetStyle(
            ogc::draw::DrawStylePtr(reinterpret_cast<ogc::draw::DrawStyle*>(style)));
    }
}

int ogc_symbolizer_symbolize(ogc_symbolizer_t* symbolizer, const ogc_feature_t* feature, ogc_draw_device_t* device) {
    if (symbolizer && feature && device) {
        return reinterpret_cast<Symbolizer*>(symbolizer)->Symbolize(
            *reinterpret_cast<const ogc::feature::Feature*>(feature),
            *reinterpret_cast<ogc::draw::DrawDevice*>(device)) ? 1 : 0;
    }
    return 0;
}

ogc_symbolizer_rule_t* ogc_symbolizer_rule_create(void) {
    return reinterpret_cast<ogc_symbolizer_rule_t*>(SymbolizerRule::Create().release());
}

void ogc_symbolizer_rule_destroy(ogc_symbolizer_rule_t* rule) {
    delete reinterpret_cast<SymbolizerRule*>(rule);
}

const char* ogc_symbolizer_rule_get_name(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        return reinterpret_cast<const SymbolizerRule*>(rule)->GetName().c_str();
    }
    return "";
}

void ogc_symbolizer_rule_set_name(ogc_symbolizer_rule_t* rule, const char* name) {
    if (rule) {
        reinterpret_cast<SymbolizerRule*>(rule)->SetName(SafeString(name));
    }
}

double ogc_symbolizer_rule_get_min_scale(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        return reinterpret_cast<const SymbolizerRule*>(rule)->GetMinScale();
    }
    return 0.0;
}

void ogc_symbolizer_rule_set_min_scale(ogc_symbolizer_rule_t* rule, double scale) {
    if (rule) {
        reinterpret_cast<SymbolizerRule*>(rule)->SetMinScale(scale);
    }
}

double ogc_symbolizer_rule_get_max_scale(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        return reinterpret_cast<const SymbolizerRule*>(rule)->GetMaxScale();
    }
    return 0.0;
}

void ogc_symbolizer_rule_set_max_scale(ogc_symbolizer_rule_t* rule, double scale) {
    if (rule) {
        reinterpret_cast<SymbolizerRule*>(rule)->SetMaxScale(scale);
    }
}

ogc_filter_t* ogc_symbolizer_rule_get_filter(ogc_symbolizer_rule_t* rule) {
    if (rule) {
        return reinterpret_cast<ogc_filter_t*>(
            reinterpret_cast<SymbolizerRule*>(rule)->GetFilter());
    }
    return nullptr;
}

void ogc_symbolizer_rule_set_filter(ogc_symbolizer_rule_t* rule, ogc_filter_t* filter) {
    if (rule && filter) {
        reinterpret_cast<SymbolizerRule*>(rule)->SetFilter(
            FilterPtr(reinterpret_cast<Filter*>(filter)));
    }
}

size_t ogc_symbolizer_rule_get_symbolizer_count(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        return reinterpret_cast<const SymbolizerRule*>(rule)->GetSymbolizerCount();
    }
    return 0;
}

ogc_symbolizer_t* ogc_symbolizer_rule_get_symbolizer(ogc_symbolizer_rule_t* rule, size_t index) {
    if (rule) {
        return reinterpret_cast<ogc_symbolizer_t*>(
            reinterpret_cast<SymbolizerRule*>(rule)->GetSymbolizer(index));
    }
    return nullptr;
}

void ogc_symbolizer_rule_add_symbolizer(ogc_symbolizer_rule_t* rule, ogc_symbolizer_t* symbolizer) {
    if (rule && symbolizer) {
        reinterpret_cast<SymbolizerRule*>(rule)->AddSymbolizer(
            SymbolizerPtr(reinterpret_cast<Symbolizer*>(symbolizer)));
    }
}

int ogc_symbolizer_rule_is_applicable(const ogc_symbolizer_rule_t* rule, const ogc_feature_t* feature, double scale) {
    if (rule && feature) {
        return reinterpret_cast<const SymbolizerRule*>(rule)->IsApplicable(
            *reinterpret_cast<const ogc::feature::Feature*>(feature), scale) ? 1 : 0;
    }
    return 0;
}

#ifdef __cplusplus
}
#endif
