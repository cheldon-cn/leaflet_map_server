#ifndef OGC_SYMBOLOGY_S52_STYLE_MANAGER_H
#define OGC_SYMBOLOGY_S52_STYLE_MANAGER_H

#include "ogc/symbology/export.h"
#include <ogc/draw/draw_style.h>
#include <ogc/draw/color.h>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

namespace ogc {
namespace symbology {

using ogc::draw::Color;
using ogc::draw::PenStyle;

enum class DayNightMode {
    kDay = 0,
    kNight = 1,
    kDusk = 2,
    kTwilight = 3
};

enum class DisplayCategory {
    kBase = 0,
    kStandard = 1,
    kOther = 2,
    kAll = 3
};

enum class SymbolType {
    kVector = 0,
    kRaster = 1,
    kComposite = 2
};

enum class ConditionOperator {
    kEqual = 0,
    kNotEqual = 1,
    kGreaterThan = 2,
    kLessThan = 3,
    kGreaterEqual = 4,
    kLessEqual = 5,
    kContains = 6,
    kInRange = 7
};

struct VectorCommand {
    std::string command;
    std::vector<double> params;
};

struct SymbolDefinition {
    std::string symbolId;
    SymbolType type;
    std::string description;
    
    std::vector<VectorCommand> vectorCommands;
    
    std::string rasterImagePath;
    int width;
    int height;
    
    std::vector<std::string> componentSymbolIds;
    
    double pivotX;
    double pivotY;
    double minScale;
    double maxScale;
    int displayPriority;
    
    SymbolDefinition()
        : type(SymbolType::kVector)
        , width(0)
        , height(0)
        , pivotX(0.0)
        , pivotY(0.0)
        , minScale(0.0)
        , maxScale(0.0)
        , displayPriority(0) {}
};

struct StyleCondition {
    std::string attributeName;
    ConditionOperator op;
    std::string value;
    std::string valueRangeEnd;
    
    StyleCondition() : op(ConditionOperator::kEqual) {}
};

struct StyleRule {
    std::string ruleId;
    std::string featureType;
    std::vector<StyleCondition> conditions;
    
    std::string symbolId;
    Color lineColor;
    Color fillColor;
    double lineWidth;
    PenStyle lineStyle;
    int displayPriority;
    double minScale;
    double maxScale;
    
    bool visibleByDefault;
    DisplayCategory displayCategory;
    
    StyleRule()
        : lineWidth(1.0)
        , lineStyle(PenStyle::kSolid)
        , displayPriority(0)
        , minScale(0.0)
        , maxScale(0.0)
        , visibleByDefault(true)
        , displayCategory(DisplayCategory::kStandard) {}
};

class OGC_SYMBOLOGY_API ColorSchemeManager {
public:
    static ColorSchemeManager& Instance();
    
    void SetDayNightMode(DayNightMode mode);
    DayNightMode GetDayNightMode() const { return m_mode; }
    
    Color GetColor(const std::string& token) const;
    Color GetDayColor(const std::string& token) const;
    Color GetNightColor(const std::string& token) const;
    
    void RegisterColor(const std::string& token, 
                      const Color& dayColor, 
                      const Color& nightColor);
    
    std::vector<std::string> GetColorTokens() const;
    
private:
    ColorSchemeManager();
    ~ColorSchemeManager();
    
    void InitializeDefaultColors();
    Color ParseColor(const std::string& colorStr) const;
    
    std::map<std::string, Color> m_dayColors;
    std::map<std::string, Color> m_nightColors;
    DayNightMode m_mode;
    mutable std::mutex m_mutex;
};

class OGC_SYMBOLOGY_API SymbolLibrary {
public:
    static SymbolLibrary& Instance();
    
    bool LoadFromFile(const std::string& filePath);
    bool LoadFromDirectory(const std::string& dirPath);
    
    const SymbolDefinition* GetSymbol(const std::string& symbolId) const;
    std::vector<std::string> GetSymbolIds() const;
    std::vector<const SymbolDefinition*> GetSymbolsByFeatureType(const std::string& featureType) const;
    
    void RegisterSymbol(const SymbolDefinition& symbol);
    bool RemoveSymbol(const std::string& symbolId);
    bool HasSymbol(const std::string& symbolId) const;
    
    size_t GetSymbolCount() const;
    
private:
    SymbolLibrary();
    ~SymbolLibrary();
    
    void InitializeDefaultSymbols();
    
    std::map<std::string, SymbolDefinition> m_symbols;
    mutable std::mutex m_mutex;
};

class OGC_SYMBOLOGY_API StyleRuleEngine {
public:
    static StyleRuleEngine& Instance();
    
    bool LoadRulesFromFile(const std::string& filePath);
    bool LoadRulesFromDirectory(const std::string& dirPath);
    
    const StyleRule* MatchRule(const std::string& featureType,
                               const std::map<std::string, std::string>& attributes,
                               double scale) const;
    
    std::vector<const StyleRule*> GetRulesByFeatureType(const std::string& featureType) const;
    std::vector<const StyleRule*> GetAllRules() const;
    
    void SetDisplayCategory(DisplayCategory category);
    DisplayCategory GetDisplayCategory() const { return m_displayCategory; }
    
    void RegisterRule(const StyleRule& rule);
    bool RemoveRule(const std::string& ruleId);
    bool HasRule(const std::string& ruleId) const;
    
    size_t GetRuleCount() const;
    
private:
    StyleRuleEngine();
    ~StyleRuleEngine();
    
    void InitializeDefaultRules();
    bool EvaluateCondition(const std::map<std::string, std::string>& attributes,
                          const StyleCondition& cond) const;
    
    std::vector<StyleRule> m_rules;
    DisplayCategory m_displayCategory;
    mutable std::mutex m_mutex;
};

class OGC_SYMBOLOGY_API DisplayPriorityCalculator {
public:
    static int CalculatePriority(const std::string& featureType,
                                 const StyleRule* rule,
                                 const std::map<std::string, std::string>& attributes);
    
    static int GetFeatureTypePriority(const std::string& featureType);
    static int GetImportancePriority(const std::map<std::string, std::string>& attributes);
    
private:
    DisplayPriorityCalculator() = delete;
    
    static const std::map<std::string, int> s_typePriorities;
};

class OGC_SYMBOLOGY_API S52StyleManager {
public:
    static S52StyleManager& Instance();
    
    bool Initialize();
    bool LoadS52Data(const std::string& dataPath);
    
    void SetDayNightMode(DayNightMode mode);
    DayNightMode GetDayNightMode() const;
    
    void SetDisplayCategory(DisplayCategory category);
    DisplayCategory GetDisplayCategory() const;
    
    const StyleRule* GetStyle(const std::string& featureType,
                              const std::map<std::string, std::string>& attributes,
                              double scale) const;
    
    const SymbolDefinition* GetSymbol(const std::string& symbolId) const;
    Color GetColor(const std::string& token) const;
    
    int CalculateDisplayPriority(const std::string& featureType,
                                 const StyleRule* rule,
                                 const std::map<std::string, std::string>& attributes) const;
    
    ColorSchemeManager& GetColorSchemeManager();
    SymbolLibrary& GetSymbolLibrary();
    StyleRuleEngine& GetRuleEngine();
    
private:
    S52StyleManager();
    ~S52StyleManager();
    
    bool m_initialized;
    mutable std::mutex m_mutex;
};

}
}

#endif
