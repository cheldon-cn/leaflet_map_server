#include "ogc/draw/s52_style_manager.h"
#include <algorithm>
#include <sstream>
#include <fstream>

namespace ogc {
namespace draw {

const std::map<std::string, int> DisplayPriorityCalculator::s_typePriorities = {
    {"SOUNDG", 100},
    {"WRECKS", 95},
    {"OBSTRN", 90},
    {"BCNSPP", 85},
    {"LIGHTS", 80},
    {"BOYCAR", 75},
    {"DEPCNT", 70},
    {"NAVLNE", 65},
    {"SLCONS", 60},
    {"LNDARE", 50},
    {"DRGARE", 40}
};

ColorSchemeManager& ColorSchemeManager::Instance()
{
    static ColorSchemeManager instance;
    return instance;
}

void ColorSchemeManager::SetDayNightMode(DayNightMode mode)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_mode = mode;
}

Color ColorSchemeManager::GetColor(const std::string& token) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    switch (m_mode) {
        case DayNightMode::kDay:
            return GetDayColor(token);
        case DayNightMode::kNight:
            return GetNightColor(token);
        default:
            return GetDayColor(token);
    }
}

Color ColorSchemeManager::GetDayColor(const std::string& token) const
{
    auto it = m_dayColors.find(token);
    if (it != m_dayColors.end()) {
        return it->second;
    }
    return Color(0, 0, 0, 255);
}

Color ColorSchemeManager::GetNightColor(const std::string& token) const
{
    auto it = m_nightColors.find(token);
    if (it != m_nightColors.end()) {
        return it->second;
    }
    return Color(0, 0, 0, 255);
}

void ColorSchemeManager::RegisterColor(const std::string& token, 
                                       const Color& dayColor, 
                                       const Color& nightColor)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_dayColors[token] = dayColor;
    m_nightColors[token] = nightColor;
}

std::vector<std::string> ColorSchemeManager::GetColorTokens() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> tokens;
    tokens.reserve(m_dayColors.size());
    for (const auto& pair : m_dayColors) {
        tokens.push_back(pair.first);
    }
    return tokens;
}

ColorSchemeManager::ColorSchemeManager()
    : m_mode(DayNightMode::kDay)
{
    InitializeDefaultColors();
}

ColorSchemeManager::~ColorSchemeManager()
{
}

void ColorSchemeManager::InitializeDefaultColors()
{
    RegisterColor("CHGRF", Color(0, 0, 0, 255), Color(139, 139, 0, 255));
    RegisterColor("CHGRD", Color(190, 190, 190, 255), Color(95, 95, 0, 255));
    RegisterColor("LANDF", Color(240, 230, 140, 255), Color(95, 95, 0, 255));
    RegisterColor("LANDA", Color(245, 222, 179, 255), Color(63, 63, 0, 255));
    RegisterColor("DEPVS", Color(0, 255, 255, 255), Color(0, 139, 139, 255));
    RegisterColor("DEPDW", Color(0, 0, 255, 255), Color(0, 0, 139, 255));
    RegisterColor("DEPMS", Color(0, 128, 255, 255), Color(0, 64, 139, 255));
    RegisterColor("DEPMD", Color(0, 191, 255, 255), Color(0, 95, 139, 255));
    RegisterColor("RESBL", Color(0, 0, 0, 255), Color(255, 255, 255, 255));
    RegisterColor("RESTR", Color(255, 255, 255, 255), Color(0, 0, 0, 255));
    RegisterColor("CHBLK", Color(0, 0, 0, 255), Color(255, 255, 255, 255));
    RegisterColor("CHWHT", Color(255, 255, 255, 255), Color(0, 0, 0, 255));
    RegisterColor("CHRED", Color(255, 0, 0, 255), Color(139, 0, 0, 255));
    RegisterColor("CHGRN", Color(0, 255, 0, 255), Color(0, 139, 0, 255));
    RegisterColor("CHBLU", Color(0, 0, 255, 255), Color(0, 0, 139, 255));
    RegisterColor("CHYLW", Color(255, 255, 0, 255), Color(139, 139, 0, 255));
    RegisterColor("CHBRN", Color(165, 42, 42, 255), Color(139, 69, 19, 255));
    RegisterColor("CHCOR", Color(255, 127, 80, 255), Color(139, 69, 19, 255));
    RegisterColor("CHPINK", Color(255, 192, 203, 255), Color(139, 0, 139, 255));
    RegisterColor("SNDG1", Color(0, 128, 255, 255), Color(0, 64, 139, 255));
    RegisterColor("SNDG2", Color(0, 191, 255, 255), Color(0, 95, 139, 255));
    RegisterColor("DNGHL", Color(255, 0, 0, 255), Color(139, 0, 0, 255));
    RegisterColor("DNGTR", Color(255, 165, 0, 255), Color(139, 69, 19, 255));
    RegisterColor("ISODNG", Color(255, 0, 0, 255), Color(139, 0, 0, 255));
    RegisterColor("TRFCD", Color(255, 255, 0, 255), Color(139, 139, 0, 255));
}

Color ColorSchemeManager::ParseColor(const std::string& colorStr) const
{
    if (colorStr.empty()) {
        return Color(0, 0, 0, 255);
    }
    
    if (colorStr[0] == '#') {
        std::string hex = colorStr.substr(1);
        if (hex.length() == 6) {
            int r = std::stoi(hex.substr(0, 2), nullptr, 16);
            int g = std::stoi(hex.substr(2, 2), nullptr, 16);
            int b = std::stoi(hex.substr(4, 2), nullptr, 16);
            return Color(r, g, b, 255);
        } else if (hex.length() == 8) {
            int r = std::stoi(hex.substr(0, 2), nullptr, 16);
            int g = std::stoi(hex.substr(2, 2), nullptr, 16);
            int b = std::stoi(hex.substr(4, 2), nullptr, 16);
            int a = std::stoi(hex.substr(6, 2), nullptr, 16);
            return Color(r, g, b, a);
        }
    }
    
    return Color(0, 0, 0, 255);
}

SymbolLibrary& SymbolLibrary::Instance()
{
    static SymbolLibrary instance;
    return instance;
}

bool SymbolLibrary::LoadFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        std::istringstream iss(line);
        SymbolDefinition symbol;
        if (iss >> symbol.symbolId >> symbol.description) {
            RegisterSymbol(symbol);
        }
    }
    
    return true;
}

bool SymbolLibrary::LoadFromDirectory(const std::string& dirPath)
{
    return true;
}

const SymbolDefinition* SymbolLibrary::GetSymbol(const std::string& symbolId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_symbols.find(symbolId);
    if (it != m_symbols.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<std::string> SymbolLibrary::GetSymbolIds() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::string> ids;
    ids.reserve(m_symbols.size());
    for (const auto& pair : m_symbols) {
        ids.push_back(pair.first);
    }
    return ids;
}

std::vector<const SymbolDefinition*> SymbolLibrary::GetSymbolsByFeatureType(const std::string& featureType) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<const SymbolDefinition*> result;
    for (const auto& pair : m_symbols) {
        result.push_back(&pair.second);
    }
    return result;
}

void SymbolLibrary::RegisterSymbol(const SymbolDefinition& symbol)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_symbols[symbol.symbolId] = symbol;
}

bool SymbolLibrary::RemoveSymbol(const std::string& symbolId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_symbols.erase(symbolId) > 0;
}

bool SymbolLibrary::HasSymbol(const std::string& symbolId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_symbols.find(symbolId) != m_symbols.end();
}

size_t SymbolLibrary::GetSymbolCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_symbols.size();
}

SymbolLibrary::SymbolLibrary()
{
    InitializeDefaultSymbols();
}

SymbolLibrary::~SymbolLibrary()
{
}

void SymbolLibrary::InitializeDefaultSymbols()
{
    SymbolDefinition sndmrk;
    sndmrk.symbolId = "SNDMRK01";
    sndmrk.description = "Sounding marker";
    sndmrk.type = SymbolType::kVector;
    sndmrk.displayPriority = 100;
    RegisterSymbol(sndmrk);
    
    SymbolDefinition wreck;
    wreck.symbolId = "WRECKS01";
    wreck.description = "Wreck";
    wreck.type = SymbolType::kVector;
    wreck.displayPriority = 95;
    RegisterSymbol(wreck);
    
    SymbolDefinition obstrn;
    obstrn.symbolId = "OBSTRN01";
    obstrn.description = "Obstruction";
    obstrn.type = SymbolType::kVector;
    obstrn.displayPriority = 90;
    RegisterSymbol(obstrn);
    
    SymbolDefinition beacon;
    beacon.symbolId = "BCNSPP01";
    beacon.description = "Beacon";
    beacon.type = SymbolType::kVector;
    beacon.displayPriority = 85;
    RegisterSymbol(beacon);
    
    SymbolDefinition light;
    light.symbolId = "LIGHTS01";
    light.description = "Light";
    light.type = SymbolType::kVector;
    light.displayPriority = 80;
    RegisterSymbol(light);
    
    SymbolDefinition buoy;
    buoy.symbolId = "BOYCAR01";
    buoy.description = "Buoy";
    buoy.type = SymbolType::kVector;
    buoy.displayPriority = 75;
    RegisterSymbol(buoy);
}

StyleRuleEngine& StyleRuleEngine::Instance()
{
    static StyleRuleEngine instance;
    return instance;
}

bool StyleRuleEngine::LoadRulesFromFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }
    
    return true;
}

bool StyleRuleEngine::LoadRulesFromDirectory(const std::string& dirPath)
{
    return true;
}

const StyleRule* StyleRuleEngine::MatchRule(const std::string& featureType,
                                            const std::map<std::string, std::string>& attributes,
                                            double scale) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    
    const StyleRule* bestMatch = nullptr;
    int bestPriority = -1;
    
    for (const auto& rule : m_rules) {
        if (rule.featureType != featureType) {
            continue;
        }
        
        if (scale > 0) {
            if (rule.minScale > 0 && scale < rule.minScale) {
                continue;
            }
            if (rule.maxScale > 0 && scale > rule.maxScale) {
                continue;
            }
        }
        
        bool allConditionsMatch = true;
        for (const auto& cond : rule.conditions) {
            if (!EvaluateCondition(attributes, cond)) {
                allConditionsMatch = false;
                break;
            }
        }
        
        if (allConditionsMatch) {
            if (rule.displayPriority > bestPriority) {
                bestMatch = &rule;
                bestPriority = rule.displayPriority;
            }
        }
    }
    
    return bestMatch;
}

std::vector<const StyleRule*> StyleRuleEngine::GetRulesByFeatureType(const std::string& featureType) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<const StyleRule*> result;
    for (const auto& rule : m_rules) {
        if (rule.featureType == featureType) {
            result.push_back(&rule);
        }
    }
    return result;
}

std::vector<const StyleRule*> StyleRuleEngine::GetAllRules() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<const StyleRule*> result;
    result.reserve(m_rules.size());
    for (const auto& rule : m_rules) {
        result.push_back(&rule);
    }
    return result;
}

void StyleRuleEngine::SetDisplayCategory(DisplayCategory category)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_displayCategory = category;
}

void StyleRuleEngine::RegisterRule(const StyleRule& rule)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_rules.push_back(rule);
}

bool StyleRuleEngine::RemoveRule(const std::string& ruleId)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find_if(m_rules.begin(), m_rules.end(),
        [&ruleId](const StyleRule& rule) { return rule.ruleId == ruleId; });
    if (it != m_rules.end()) {
        m_rules.erase(it);
        return true;
    }
    return false;
}

bool StyleRuleEngine::HasRule(const std::string& ruleId) const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = std::find_if(m_rules.begin(), m_rules.end(),
        [&ruleId](const StyleRule& rule) { return rule.ruleId == ruleId; });
    return it != m_rules.end();
}

size_t StyleRuleEngine::GetRuleCount() const
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_rules.size();
}

StyleRuleEngine::StyleRuleEngine()
    : m_displayCategory(DisplayCategory::kStandard)
{
    InitializeDefaultRules();
}

StyleRuleEngine::~StyleRuleEngine()
{
}

void StyleRuleEngine::InitializeDefaultRules()
{
    StyleRule soundingRule;
    soundingRule.ruleId = "SOUNDG_DEFAULT";
    soundingRule.featureType = "SOUNDG";
    soundingRule.lineColor = Color(0, 128, 255, 255);
    soundingRule.displayPriority = 100;
    soundingRule.displayCategory = DisplayCategory::kBase;
    m_rules.push_back(soundingRule);
    
    StyleRule wreckRule;
    wreckRule.ruleId = "WRECKS_DEFAULT";
    wreckRule.featureType = "WRECKS";
    wreckRule.symbolId = "WRECKS01";
    wreckRule.lineColor = Color(255, 0, 0, 255);
    wreckRule.displayPriority = 95;
    wreckRule.displayCategory = DisplayCategory::kBase;
    m_rules.push_back(wreckRule);
    
    StyleRule obstrnRule;
    obstrnRule.ruleId = "OBSTRN_DEFAULT";
    obstrnRule.featureType = "OBSTRN";
    obstrnRule.symbolId = "OBSTRN01";
    obstrnRule.lineColor = Color(255, 165, 0, 255);
    obstrnRule.displayPriority = 90;
    obstrnRule.displayCategory = DisplayCategory::kBase;
    m_rules.push_back(obstrnRule);
    
    StyleRule beaconRule;
    beaconRule.ruleId = "BCNSPP_DEFAULT";
    beaconRule.featureType = "BCNSPP";
    beaconRule.symbolId = "BCNSPP01";
    beaconRule.lineColor = Color(255, 255, 0, 255);
    beaconRule.displayPriority = 85;
    beaconRule.displayCategory = DisplayCategory::kStandard;
    m_rules.push_back(beaconRule);
    
    StyleRule lightRule;
    lightRule.ruleId = "LIGHTS_DEFAULT";
    lightRule.featureType = "LIGHTS";
    lightRule.symbolId = "LIGHTS01";
    lightRule.lineColor = Color(255, 255, 0, 255);
    lightRule.displayPriority = 80;
    lightRule.displayCategory = DisplayCategory::kStandard;
    m_rules.push_back(lightRule);
    
    StyleRule buoyRule;
    buoyRule.ruleId = "BOYCAR_DEFAULT";
    buoyRule.featureType = "BOYCAR";
    buoyRule.symbolId = "BOYCAR01";
    buoyRule.lineColor = Color(255, 255, 0, 255);
    buoyRule.displayPriority = 75;
    buoyRule.displayCategory = DisplayCategory::kStandard;
    m_rules.push_back(buoyRule);
    
    StyleRule depthContourRule;
    depthContourRule.ruleId = "DEPCNT_DEFAULT";
    depthContourRule.featureType = "DEPCNT";
    depthContourRule.lineColor = Color(0, 255, 255, 255);
    depthContourRule.lineWidth = 0.5;
    depthContourRule.displayPriority = 70;
    depthContourRule.displayCategory = DisplayCategory::kStandard;
    m_rules.push_back(depthContourRule);
    
    StyleRule landAreaRule;
    landAreaRule.ruleId = "LNDARE_DEFAULT";
    landAreaRule.featureType = "LNDARE";
    landAreaRule.fillColor = Color(245, 222, 179, 255);
    landAreaRule.lineColor = Color(240, 230, 140, 255);
    landAreaRule.lineWidth = 0.5;
    landAreaRule.displayPriority = 50;
    landAreaRule.displayCategory = DisplayCategory::kBase;
    m_rules.push_back(landAreaRule);
    
    StyleRule waterAreaRule;
    waterAreaRule.ruleId = "DRGARE_DEFAULT";
    waterAreaRule.featureType = "DRGARE";
    waterAreaRule.fillColor = Color(0, 128, 255, 255);
    waterAreaRule.displayPriority = 45;
    waterAreaRule.displayCategory = DisplayCategory::kStandard;
    m_rules.push_back(waterAreaRule);
}

bool StyleRuleEngine::EvaluateCondition(const std::map<std::string, std::string>& attributes,
                                        const StyleCondition& cond) const
{
    auto it = attributes.find(cond.attributeName);
    if (it == attributes.end()) {
        return false;
    }
    
    const std::string& attrValue = it->second;
    
    switch (cond.op) {
        case ConditionOperator::kEqual:
            return attrValue == cond.value;
        case ConditionOperator::kNotEqual:
            return attrValue != cond.value;
        case ConditionOperator::kGreaterThan:
            try {
                return std::stod(attrValue) > std::stod(cond.value);
            } catch (...) {
                return false;
            }
        case ConditionOperator::kLessThan:
            try {
                return std::stod(attrValue) < std::stod(cond.value);
            } catch (...) {
                return false;
            }
        case ConditionOperator::kGreaterEqual:
            try {
                return std::stod(attrValue) >= std::stod(cond.value);
            } catch (...) {
                return false;
            }
        case ConditionOperator::kLessEqual:
            try {
                return std::stod(attrValue) <= std::stod(cond.value);
            } catch (...) {
                return false;
            }
        case ConditionOperator::kContains:
            return attrValue.find(cond.value) != std::string::npos;
        case ConditionOperator::kInRange:
            try {
                double val = std::stod(attrValue);
                double minVal = std::stod(cond.value);
                double maxVal = std::stod(cond.valueRangeEnd);
                return val >= minVal && val <= maxVal;
            } catch (...) {
                return false;
            }
        default:
            return false;
    }
}

int DisplayPriorityCalculator::CalculatePriority(const std::string& featureType,
                                                  const StyleRule* rule,
                                                  const std::map<std::string, std::string>& attributes)
{
    int priority = 0;
    
    if (rule) {
        priority = rule->displayPriority;
    }
    
    priority += GetFeatureTypePriority(featureType);
    priority += GetImportancePriority(attributes);
    
    return priority;
}

int DisplayPriorityCalculator::GetFeatureTypePriority(const std::string& featureType)
{
    auto it = s_typePriorities.find(featureType);
    if (it != s_typePriorities.end()) {
        return it->second;
    }
    return 0;
}

int DisplayPriorityCalculator::GetImportancePriority(const std::map<std::string, std::string>& attributes)
{
    int importance = 0;
    
    auto catIt = attributes.find("CAT");
    if (catIt != attributes.end()) {
        if (catIt->second == "1" || catIt->second == "2") {
            importance += 20;
        }
    }
    
    auto expoIt = attributes.find("EXPO");
    if (expoIt != attributes.end() && expoIt->second == "1") {
        importance += 15;
    }
    
    return importance;
}

S52StyleManager& S52StyleManager::Instance()
{
    static S52StyleManager instance;
    return instance;
}

bool S52StyleManager::Initialize()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_initialized) {
        return true;
    }
    
    m_initialized = true;
    return true;
}

bool S52StyleManager::LoadS52Data(const std::string& dataPath)
{
    return true;
}

void S52StyleManager::SetDayNightMode(DayNightMode mode)
{
    ColorSchemeManager::Instance().SetDayNightMode(mode);
}

DayNightMode S52StyleManager::GetDayNightMode() const
{
    return ColorSchemeManager::Instance().GetDayNightMode();
}

void S52StyleManager::SetDisplayCategory(DisplayCategory category)
{
    StyleRuleEngine::Instance().SetDisplayCategory(category);
}

DisplayCategory S52StyleManager::GetDisplayCategory() const
{
    return StyleRuleEngine::Instance().GetDisplayCategory();
}

const StyleRule* S52StyleManager::GetStyle(const std::string& featureType,
                                           const std::map<std::string, std::string>& attributes,
                                           double scale) const
{
    return StyleRuleEngine::Instance().MatchRule(featureType, attributes, scale);
}

const SymbolDefinition* S52StyleManager::GetSymbol(const std::string& symbolId) const
{
    return SymbolLibrary::Instance().GetSymbol(symbolId);
}

Color S52StyleManager::GetColor(const std::string& token) const
{
    return ColorSchemeManager::Instance().GetColor(token);
}

int S52StyleManager::CalculateDisplayPriority(const std::string& featureType,
                                              const StyleRule* rule,
                                              const std::map<std::string, std::string>& attributes) const
{
    return DisplayPriorityCalculator::CalculatePriority(featureType, rule, attributes);
}

ColorSchemeManager& S52StyleManager::GetColorSchemeManager()
{
    return ColorSchemeManager::Instance();
}

SymbolLibrary& S52StyleManager::GetSymbolLibrary()
{
    return SymbolLibrary::Instance();
}

StyleRuleEngine& S52StyleManager::GetRuleEngine()
{
    return StyleRuleEngine::Instance();
}

S52StyleManager::S52StyleManager()
    : m_initialized(false)
{
}

S52StyleManager::~S52StyleManager()
{
}

}
}
