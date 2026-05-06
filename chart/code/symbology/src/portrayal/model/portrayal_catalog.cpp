#include "ogc/portrayal/model/portrayal_catalog.h"

namespace ogc {
namespace portrayal {
namespace model {

const std::string& PortrayalCatalog::GetProductId() const {
    return productId_;
}

const std::string& PortrayalCatalog::GetVersion() const {
    return version_;
}

void PortrayalCatalog::SetProductId(const std::string& productId) {
    productId_ = productId;
}

void PortrayalCatalog::SetVersion(const std::string& version) {
    version_ = version;
}

const AlertCatalog& PortrayalCatalog::GetAlertCatalog() const {
    return alertCatalog_;
}

const std::vector<ColorProfile>& PortrayalCatalog::GetColorProfiles() const {
    return colorProfiles_;
}

const std::vector<Symbol>& PortrayalCatalog::GetSymbols() const {
    return symbols_;
}

const std::vector<StyleSheet>& PortrayalCatalog::GetStyleSheets() const {
    return styleSheets_;
}

const std::vector<LineStyle>& PortrayalCatalog::GetLineStyles() const {
    return lineStyles_;
}

const std::vector<AreaFill>& PortrayalCatalog::GetAreaFills() const {
    return areaFills_;
}

const std::vector<ViewingGroup>& PortrayalCatalog::GetViewingGroups() const {
    return viewingGroups_;
}

const std::vector<RuleFile>& PortrayalCatalog::GetRuleFiles() const {
    return ruleFiles_;
}

void PortrayalCatalog::SetAlertCatalog(const AlertCatalog& catalog) {
    alertCatalog_ = catalog;
}

void PortrayalCatalog::AddColorProfile(const ColorProfile& profile) {
    colorProfiles_.push_back(profile);
}

void PortrayalCatalog::AddSymbol(const Symbol& symbol) {
    symbolIndex_[symbol.GetId()] = symbols_.size();
    symbols_.push_back(symbol);
}

void PortrayalCatalog::AddStyleSheet(const StyleSheet& sheet) {
    styleSheets_.push_back(sheet);
}

void PortrayalCatalog::AddLineStyle(const LineStyle& style) {
    lineStyleIndex_[style.GetId()] = lineStyles_.size();
    lineStyles_.push_back(style);
}

void PortrayalCatalog::AddAreaFill(const AreaFill& fill) {
    areaFillIndex_[fill.GetId()] = areaFills_.size();
    areaFills_.push_back(fill);
}

void PortrayalCatalog::AddViewingGroup(const ViewingGroup& group) {
    viewingGroupIndex_[group.GetId()] = viewingGroups_.size();
    viewingGroups_.push_back(group);
}

void PortrayalCatalog::AddRuleFile(const RuleFile& rule) {
    ruleFileIndex_[rule.GetId()] = ruleFiles_.size();
    ruleFiles_.push_back(rule);
}

const Symbol* PortrayalCatalog::FindSymbol(const std::string& id) const {
    auto it = symbolIndex_.find(id);
    if (it != symbolIndex_.end() && it->second < symbols_.size()) {
        return &symbols_[it->second];
    }
    return nullptr;
}

const LineStyle* PortrayalCatalog::FindLineStyle(const std::string& id) const {
    auto it = lineStyleIndex_.find(id);
    if (it != lineStyleIndex_.end() && it->second < lineStyles_.size()) {
        return &lineStyles_[it->second];
    }
    return nullptr;
}

const AreaFill* PortrayalCatalog::FindAreaFill(const std::string& id) const {
    auto it = areaFillIndex_.find(id);
    if (it != areaFillIndex_.end() && it->second < areaFills_.size()) {
        return &areaFills_[it->second];
    }
    return nullptr;
}

const RuleFile* PortrayalCatalog::FindRuleFile(const std::string& id) const {
    auto it = ruleFileIndex_.find(id);
    if (it != ruleFileIndex_.end() && it->second < ruleFiles_.size()) {
        return &ruleFiles_[it->second];
    }
    return nullptr;
}

const ViewingGroup* PortrayalCatalog::FindViewingGroup(const std::string& id) const {
    auto it = viewingGroupIndex_.find(id);
    if (it != viewingGroupIndex_.end() && it->second < viewingGroups_.size()) {
        return &viewingGroups_[it->second];
    }
    return nullptr;
}

void PortrayalCatalog::SetStringPool(std::shared_ptr<utils::StringPool> pool) {
    stringPool_ = pool;
}

} // namespace model
} // namespace portrayal
} // namespace ogc
