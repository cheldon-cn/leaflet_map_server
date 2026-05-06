#include "ogc/portrayal/model/rule_file.h"

namespace ogc {
namespace portrayal {
namespace model {

const std::string& RuleFile::GetRuleType() const {
    return ruleType_;
}

void RuleFile::SetRuleType(const std::string& ruleType) {
    ruleType_ = ruleType;
}

} // namespace model
} // namespace portrayal
} // namespace ogc
