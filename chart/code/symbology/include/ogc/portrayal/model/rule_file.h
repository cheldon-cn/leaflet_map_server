#ifndef OGC_PORTRAYAL_MODEL_RULE_FILE_H
#define OGC_PORTRAYAL_MODEL_RULE_FILE_H

#include "ogc/portrayal/model/file_reference.h"
#include "../portrayal_export.h"
#include <string>

namespace ogc {
namespace portrayal {
namespace model {

class OGC_PORTRAYAL_API RuleFile : public FileReference {
public:
    RuleFile() = default;

    const std::string& GetRuleType() const;
    void SetRuleType(const std::string& ruleType);

private:
    std::string ruleType_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
