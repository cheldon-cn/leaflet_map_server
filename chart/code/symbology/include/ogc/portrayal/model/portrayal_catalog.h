#ifndef OGC_PORTRAYAL_MODEL_PORTRAYAL_CATALOG_H
#define OGC_PORTRAYAL_MODEL_PORTRAYAL_CATALOG_H

#include "ogc/portrayal/model/description.h"
#include "ogc/portrayal/model/symbol.h"
#include "ogc/portrayal/model/line_style.h"
#include "ogc/portrayal/model/area_fill.h"
#include "ogc/portrayal/model/style_sheet.h"
#include "ogc/portrayal/model/alert_catalog.h"
#include "ogc/portrayal/model/color_profile.h"
#include "ogc/portrayal/model/rule_file.h"
#include "ogc/portrayal/model/viewing_group.h"
#include "../portrayal_export.h"
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace ogc {
namespace portrayal {
namespace utils {
class StringPool;
} // namespace utils

namespace model {

class OGC_PORTRAYAL_API PortrayalCatalog {
public:
    PortrayalCatalog() = default;

    const std::string& GetProductId() const;
    const std::string& GetVersion() const;
    void SetProductId(const std::string& productId);
    void SetVersion(const std::string& version);

    const AlertCatalog& GetAlertCatalog() const;
    const std::vector<ColorProfile>& GetColorProfiles() const;
    const std::vector<Symbol>& GetSymbols() const;
    const std::vector<StyleSheet>& GetStyleSheets() const;
    const std::vector<LineStyle>& GetLineStyles() const;
    const std::vector<AreaFill>& GetAreaFills() const;
    const std::vector<ViewingGroup>& GetViewingGroups() const;
    const std::vector<RuleFile>& GetRuleFiles() const;

    void SetAlertCatalog(const AlertCatalog& catalog);
    void AddColorProfile(const ColorProfile& profile);
    void AddSymbol(const Symbol& symbol);
    void AddStyleSheet(const StyleSheet& sheet);
    void AddLineStyle(const LineStyle& style);
    void AddAreaFill(const AreaFill& fill);
    void AddViewingGroup(const ViewingGroup& group);
    void AddRuleFile(const RuleFile& rule);

    const Symbol* FindSymbol(const std::string& id) const;
    const LineStyle* FindLineStyle(const std::string& id) const;
    const AreaFill* FindAreaFill(const std::string& id) const;
    const RuleFile* FindRuleFile(const std::string& id) const;
    const ViewingGroup* FindViewingGroup(const std::string& id) const;

    void SetStringPool(std::shared_ptr<utils::StringPool> pool);

private:
    std::string productId_;
    std::string version_;

    AlertCatalog alertCatalog_;
    std::vector<ColorProfile> colorProfiles_;
    std::vector<Symbol> symbols_;
    std::vector<StyleSheet> styleSheets_;
    std::vector<LineStyle> lineStyles_;
    std::vector<AreaFill> areaFills_;
    std::vector<ViewingGroup> viewingGroups_;
    std::vector<RuleFile> ruleFiles_;

    std::map<std::string, size_t> symbolIndex_;
    std::map<std::string, size_t> lineStyleIndex_;
    std::map<std::string, size_t> areaFillIndex_;
    std::map<std::string, size_t> ruleFileIndex_;
    std::map<std::string, size_t> viewingGroupIndex_;

    std::shared_ptr<utils::StringPool> stringPool_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
