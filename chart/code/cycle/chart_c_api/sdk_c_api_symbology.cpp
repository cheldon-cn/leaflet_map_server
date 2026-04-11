/**
 * @file sdk_c_api_symbology.cpp
 * @brief OGC Chart SDK C API - Symbology Module Implementation
 * @version v1.0
 * @date 2026-04-09
 */

#include "sdk_c_api.h"

#include <ogc/feature/feature.h>
#include <ogc/symbology/filter/filter.h>
#include <ogc/symbology/filter/comparison_filter.h>
#include <ogc/symbology/symbolizer/symbolizer.h>
#include <ogc/symbology/filter/symbolizer_rule.h>

#include <cstring>
#include <cstdlib>
#include <memory>
#include <string>

using namespace ogc;
using namespace ogc::symbology;

#ifdef __cplusplus
extern "C" {
#endif

namespace { static std::string SafeString(const char* str) {
    return str ? std::string(str) : std::string();
}

SymbolizerType ToSymbolizerType(ogc_symbolizer_type_e type) {
    switch (type) {
        case OGC_SYMBOLIZER_POINT: return SymbolizerType::kPoint;
        case OGC_SYMBOLIZER_LINE: return SymbolizerType::kLine;
        case OGC_SYMBOLIZER_POLYGON: return SymbolizerType::kPolygon;
        case OGC_SYMBOLIZER_TEXT: return SymbolizerType::kText;
        case OGC_SYMBOLIZER_RASTER: return SymbolizerType::kRaster;
        case OGC_SYMBOLIZER_COMPOSITE: return SymbolizerType::kComposite;
        case OGC_SYMBOLIZER_ICON: return SymbolizerType::kIcon;
        default: return SymbolizerType::kPoint;
    }
}

ogc_symbolizer_type_e FromSymbolizerType(SymbolizerType type) {
    switch (type) {
        case SymbolizerType::kPoint: return OGC_SYMBOLIZER_POINT;
        case SymbolizerType::kLine: return OGC_SYMBOLIZER_LINE;
        case SymbolizerType::kPolygon: return OGC_SYMBOLIZER_POLYGON;
        case SymbolizerType::kText: return OGC_SYMBOLIZER_TEXT;
        case SymbolizerType::kRaster: return OGC_SYMBOLIZER_RASTER;
        case SymbolizerType::kComposite: return OGC_SYMBOLIZER_COMPOSITE;
        case SymbolizerType::kIcon: return OGC_SYMBOLIZER_ICON;
        default: return OGC_SYMBOLIZER_POINT;
    }
}

}  

ogc_filter_t* ogc_filter_create(void) {
    return reinterpret_cast<ogc_filter_t*>(new FilterPtr(Filter::CreateAll()));
}

void ogc_filter_destroy(ogc_filter_t* filter) {
    if (filter) {
        FilterPtr* ptr = reinterpret_cast<FilterPtr*>(filter);
        delete ptr;
    }
}

ogc_filter_type_e ogc_filter_get_type(const ogc_filter_t* filter) {
    if (filter) {
        const FilterPtr* ptr = reinterpret_cast<const FilterPtr*>(filter);
        return static_cast<ogc_filter_type_e>((*ptr)->GetType());
    }
    return OGC_FILTER_TYPE_NONE;
}

int ogc_filter_evaluate(const ogc_filter_t* filter, const ogc_feature_t* feature) {
    if (filter && feature) {
        const FilterPtr* ptr = reinterpret_cast<const FilterPtr*>(filter);
        return (*ptr)->Evaluate(
            reinterpret_cast<const CNFeature*>(feature)) ? 1 : 0;
    }
    return 0;
}

char* ogc_filter_to_string(const ogc_filter_t* filter) {
    if (filter) {
        const FilterPtr* ptr = reinterpret_cast<const FilterPtr*>(filter);
        std::string str = (*ptr)->ToString();
        char* result = static_cast<char*>(malloc(str.size() + 1));
        if (result) {
            strcpy(result, str.c_str());
        }
        return result;
    }
    return nullptr;
}

ogc_filter_t* ogc_comparison_filter_create(const char* property, ogc_comparison_operator_e op, const char* value) {
    return reinterpret_cast<ogc_filter_t*>(new FilterPtr(Filter::CreateAll()));
}

ogc_filter_t* ogc_logical_filter_create(ogc_logical_operator_e op) {
    return reinterpret_cast<ogc_filter_t*>(new FilterPtr(Filter::CreateAll()));
}

void ogc_logical_filter_add_filter(ogc_filter_t* logical, ogc_filter_t* filter) {
}

ogc_symbolizer_t* ogc_symbolizer_create(ogc_symbolizer_type_e type) {
    return reinterpret_cast<ogc_symbolizer_t*>(new SymbolizerPtr(Symbolizer::Create(ToSymbolizerType(type))));
}

void ogc_symbolizer_destroy(ogc_symbolizer_t* symbolizer) {
    if (symbolizer) {
        SymbolizerPtr* ptr = reinterpret_cast<SymbolizerPtr*>(symbolizer);
        delete ptr;
    }
}

ogc_symbolizer_type_e ogc_symbolizer_get_type(const ogc_symbolizer_t* symbolizer) {
    if (symbolizer) {
        const SymbolizerPtr* ptr = reinterpret_cast<const SymbolizerPtr*>(symbolizer);
        return FromSymbolizerType((*ptr)->GetType());
    }
    return OGC_SYMBOLIZER_POINT;
}

ogc_draw_style_t* ogc_symbolizer_get_style(ogc_symbolizer_t* symbolizer) {
    if (symbolizer) {
        SymbolizerPtr* ptr = reinterpret_cast<SymbolizerPtr*>(symbolizer);
        draw::DrawStyle style = (*ptr)->GetDefaultStyle();
        return reinterpret_cast<ogc_draw_style_t*>(new draw::DrawStyle(style));
    }
    return nullptr;
}

void ogc_symbolizer_set_style(ogc_symbolizer_t* symbolizer, ogc_draw_style_t* style) {
    if (symbolizer && style) {
        SymbolizerPtr* ptr = reinterpret_cast<SymbolizerPtr*>(symbolizer);
        (*ptr)->SetDefaultStyle(*reinterpret_cast<draw::DrawStyle*>(style));
    }
}

int ogc_symbolizer_symbolize(ogc_symbolizer_t* symbolizer, const ogc_feature_t* feature, ogc_draw_device_t* device) {
    return 0;
}

ogc_symbolizer_rule_t* ogc_symbolizer_rule_create(void) {
    return reinterpret_cast<ogc_symbolizer_rule_t*>(new SymbolizerRulePtr(SymbolizerRule::Create()));
}

void ogc_symbolizer_rule_destroy(ogc_symbolizer_rule_t* rule) {
    if (rule) {
        SymbolizerRulePtr* ptr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        delete ptr;
    }
}

const char* ogc_symbolizer_rule_get_name(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        const SymbolizerRulePtr* ptr = reinterpret_cast<const SymbolizerRulePtr*>(rule);
        return (*ptr)->GetName().c_str();
    }
    return "";
}

void ogc_symbolizer_rule_set_name(ogc_symbolizer_rule_t* rule, const char* name) {
    if (rule) {
        SymbolizerRulePtr* ptr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        (*ptr)->SetName(SafeString(name));
    }
}

double ogc_symbolizer_rule_get_min_scale(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        const SymbolizerRulePtr* ptr = reinterpret_cast<const SymbolizerRulePtr*>(rule);
        return (*ptr)->GetMinScaleDenominator();
    }
    return 0.0;
}

void ogc_symbolizer_rule_set_min_scale(ogc_symbolizer_rule_t* rule, double scale) {
    if (rule) {
        SymbolizerRulePtr* ptr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        (*ptr)->SetMinScaleDenominator(scale);
    }
}

double ogc_symbolizer_rule_get_max_scale(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        const SymbolizerRulePtr* ptr = reinterpret_cast<const SymbolizerRulePtr*>(rule);
        return (*ptr)->GetMaxScaleDenominator();
    }
    return 0.0;
}

void ogc_symbolizer_rule_set_max_scale(ogc_symbolizer_rule_t* rule, double scale) {
    if (rule) {
        SymbolizerRulePtr* ptr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        (*ptr)->SetMaxScaleDenominator(scale);
    }
}

ogc_filter_t* ogc_symbolizer_rule_get_filter(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        const SymbolizerRulePtr* ptr = reinterpret_cast<const SymbolizerRulePtr*>(rule);
        FilterPtr filter = (*ptr)->GetFilter();
        if (filter) {
            return reinterpret_cast<ogc_filter_t*>(new FilterPtr(filter));
        }
    }
    return nullptr;
}

void ogc_symbolizer_rule_set_filter(ogc_symbolizer_rule_t* rule, ogc_filter_t* filter) {
    if (rule && filter) {
        SymbolizerRulePtr* rulePtr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        FilterPtr* filterPtr = reinterpret_cast<FilterPtr*>(filter);
        (*rulePtr)->SetFilter(*filterPtr);
    }
}

size_t ogc_symbolizer_rule_get_symbolizer_count(const ogc_symbolizer_rule_t* rule) {
    if (rule) {
        const SymbolizerRulePtr* ptr = reinterpret_cast<const SymbolizerRulePtr*>(rule);
        return (*ptr)->GetSymbolizerCount();
    }
    return 0;
}

ogc_symbolizer_t* ogc_symbolizer_rule_get_symbolizer(const ogc_symbolizer_rule_t* rule, size_t index) {
    if (rule) {
        const SymbolizerRulePtr* ptr = reinterpret_cast<const SymbolizerRulePtr*>(rule);
        SymbolizerPtr symbolizer = (*ptr)->GetSymbolizer(index);
        if (symbolizer) {
            return reinterpret_cast<ogc_symbolizer_t*>(new SymbolizerPtr(symbolizer));
        }
    }
    return nullptr;
}

void ogc_symbolizer_rule_add_symbolizer(ogc_symbolizer_rule_t* rule, ogc_symbolizer_t* symbolizer) {
    if (rule && symbolizer) {
        SymbolizerRulePtr* rulePtr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        SymbolizerPtr* symPtr = reinterpret_cast<SymbolizerPtr*>(symbolizer);
        (*rulePtr)->AddSymbolizer(*symPtr);
    }
}

int ogc_symbolizer_rule_is_applicable(const ogc_symbolizer_rule_t* rule, const ogc_feature_t* feature, double scale) {
    if (rule && feature) {
        const SymbolizerRulePtr* ptr = reinterpret_cast<const SymbolizerRulePtr*>(rule);
        return (*ptr)->Evaluate(reinterpret_cast<const CNFeature*>(feature)) &&
               (*ptr)->IsScaleInRange(scale) ? 1 : 0;
    }
    return 0;
}

void ogc_comparison_filter_destroy(ogc_comparison_filter_t* filter) {
    if (filter) {
        FilterPtr* ptr = reinterpret_cast<FilterPtr*>(filter);
        delete ptr;
    }
}

int ogc_comparison_filter_evaluate(const ogc_comparison_filter_t* filter, const ogc_feature_t* feature) {
    if (filter && feature) {
        const FilterPtr* ptr = reinterpret_cast<const FilterPtr*>(filter);
        return (*ptr)->Evaluate(reinterpret_cast<const CNFeature*>(feature)) ? 1 : 0;
    }
    return 0;
}

ogc_comparison_operator_e ogc_comparison_filter_get_operator(const ogc_comparison_filter_t* filter) {
    if (filter) {
        const FilterPtr* ptr = reinterpret_cast<const FilterPtr*>(filter);
        auto* cf = dynamic_cast<const ComparisonFilter*>(ptr->get());
        if (cf) {
            return static_cast<ogc_comparison_operator_e>(cf->GetOperator());
        }
    }
    return OGC_COMPARISON_EQUAL;
}

void ogc_symbolizer_rule_set_symbolizer(ogc_symbolizer_rule_t* rule, ogc_symbolizer_t* symbolizer) {
    if (rule && symbolizer) {
        SymbolizerRulePtr* rulePtr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        SymbolizerPtr* symPtr = reinterpret_cast<SymbolizerPtr*>(symbolizer);
        (*rulePtr)->AddSymbolizer(*symPtr);
    }
}

void ogc_symbolizer_rule_set_scale_range(ogc_symbolizer_rule_t* rule, double min_scale, double max_scale) {
    if (rule) {
        SymbolizerRulePtr* ptr = reinterpret_cast<SymbolizerRulePtr*>(rule);
        (*ptr)->SetMinScaleDenominator(min_scale);
        (*ptr)->SetMaxScaleDenominator(max_scale);
    }
}

#ifdef __cplusplus
}
#endif
