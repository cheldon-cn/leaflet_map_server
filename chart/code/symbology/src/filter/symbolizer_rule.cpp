#include "ogc/symbology/filter/symbolizer_rule.h"
#include <algorithm>

namespace ogc {
namespace symbology {

struct SymbolizerRule::Impl {
    std::string name;
    std::string title;
    std::string abstract;
    FilterPtr filter;
    double minScaleDenominator = 0.0;
    double maxScaleDenominator = std::numeric_limits<double>::max();
    std::vector<SymbolizerPtr> symbolizers;
    Envelope extent;
    bool isElseFilter = false;
    int priority = 0;
};

SymbolizerRule::SymbolizerRule() : impl_(std::make_unique<Impl>())
{
}

SymbolizerRule::SymbolizerRule(const std::string& name) : impl_(std::make_unique<Impl>())
{
    impl_->name = name;
}

SymbolizerRule::~SymbolizerRule() = default;

const std::string& SymbolizerRule::GetName() const {
    return impl_->name;
}

void SymbolizerRule::SetName(const std::string& name) {
    impl_->name = name;
}

const std::string& SymbolizerRule::GetTitle() const {
    return impl_->title;
}

void SymbolizerRule::SetTitle(const std::string& title) {
    impl_->title = title;
}

const std::string& SymbolizerRule::GetAbstract() const {
    return impl_->abstract;
}

void SymbolizerRule::SetAbstract(const std::string& abstract) {
    impl_->abstract = abstract;
}

FilterPtr SymbolizerRule::GetFilter() const {
    return impl_->filter;
}

void SymbolizerRule::SetFilter(FilterPtr filter) {
    impl_->filter = filter;
}

bool SymbolizerRule::HasFilter() const {
    return impl_->filter != nullptr;
}

bool SymbolizerRule::Evaluate(const CNFeature* feature) const {
    if (impl_->isElseFilter) {
        return true;
    }
    
    if (!impl_->filter) {
        return true;
    }
    
    return impl_->filter->Evaluate(feature);
}

bool SymbolizerRule::Evaluate(const Geometry* geometry) const {
    if (impl_->isElseFilter) {
        return true;
    }
    
    if (!impl_->filter) {
        return true;
    }
    
    return impl_->filter->Evaluate(geometry);
}

double SymbolizerRule::GetMinScaleDenominator() const {
    return impl_->minScaleDenominator;
}

void SymbolizerRule::SetMinScaleDenominator(double scale) {
    impl_->minScaleDenominator = scale;
}

double SymbolizerRule::GetMaxScaleDenominator() const {
    return impl_->maxScaleDenominator;
}

void SymbolizerRule::SetMaxScaleDenominator(double scale) {
    impl_->maxScaleDenominator = scale;
}

bool SymbolizerRule::IsScaleInRange(double scale) const {
    return scale >= impl_->minScaleDenominator && scale <= impl_->maxScaleDenominator;
}

void SymbolizerRule::AddSymbolizer(SymbolizerPtr symbolizer) {
    if (symbolizer) {
        impl_->symbolizers.push_back(symbolizer);
    }
}

void SymbolizerRule::RemoveSymbolizer(SymbolizerPtr symbolizer) {
    auto it = std::find(impl_->symbolizers.begin(), impl_->symbolizers.end(), symbolizer);
    if (it != impl_->symbolizers.end()) {
        impl_->symbolizers.erase(it);
    }
}

void SymbolizerRule::ClearSymbolizers() {
    impl_->symbolizers.clear();
}

const std::vector<SymbolizerPtr>& SymbolizerRule::GetSymbolizers() const {
    return impl_->symbolizers;
}

size_t SymbolizerRule::GetSymbolizerCount() const {
    return impl_->symbolizers.size();
}

SymbolizerPtr SymbolizerRule::GetSymbolizer(size_t index) const {
    if (index < impl_->symbolizers.size()) {
        return impl_->symbolizers[index];
    }
    return nullptr;
}

bool SymbolizerRule::HasSymbolizers() const {
    return !impl_->symbolizers.empty();
}

const Envelope& SymbolizerRule::GetExtent() const {
    return impl_->extent;
}

void SymbolizerRule::SetExtent(const Envelope& extent) {
    impl_->extent = extent;
}

bool SymbolizerRule::HasExtent() const {
    return !impl_->extent.IsNull();
}

bool SymbolizerRule::IsElseFilter() const {
    return impl_->isElseFilter;
}

void SymbolizerRule::SetElseFilter(bool isElse) {
    impl_->isElseFilter = isElse;
}

int SymbolizerRule::GetPriority() const {
    return impl_->priority;
}

void SymbolizerRule::SetPriority(int priority) {
    impl_->priority = priority;
}

SymbolizerRulePtr SymbolizerRule::Clone() const {
    SymbolizerRulePtr rule = std::make_shared<SymbolizerRule>(impl_->name);
    rule->SetTitle(impl_->title);
    rule->SetAbstract(impl_->abstract);
    
    if (impl_->filter) {
        rule->SetFilter(impl_->filter->Clone());
    }
    
    rule->SetMinScaleDenominator(impl_->minScaleDenominator);
    rule->SetMaxScaleDenominator(impl_->maxScaleDenominator);
    
    for (const auto& sym : impl_->symbolizers) {
        if (sym) {
            rule->AddSymbolizer(sym->Clone());
        }
    }
    
    rule->SetExtent(impl_->extent);
    rule->SetElseFilter(impl_->isElseFilter);
    rule->SetPriority(impl_->priority);
    
    return rule;
}

SymbolizerRulePtr SymbolizerRule::Create() {
    return std::make_shared<SymbolizerRule>();
}

SymbolizerRulePtr SymbolizerRule::Create(const std::string& name) {
    return std::make_shared<SymbolizerRule>(name);
}

}
}
