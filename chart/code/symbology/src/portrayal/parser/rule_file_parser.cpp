#include "ogc/portrayal/parser/rule_file_parser.h"
#include "ogc/portrayal/utils/string_utils.h"

namespace ogc {
namespace portrayal {
namespace parser {

std::string RuleFileParser::GetElementName() const {
    return "ruleFile";
}

bool RuleFileParser::CanParse(const std::string& elementName) const {
    return elementName == "ruleFile";
}

bool RuleFileParser::Parse(XmlReader& reader,
                           model::PortrayalCatalog& catalog) {
    model::RuleFile rule;
    rule.SetId(reader.GetAttribute("id"));

    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        catalog.AddRuleFile(rule);
        return true;
    }

    do {
        std::string name = reader.GetCurrentElementName();
        if (name == "description") {
            rule.SetDescription(ParseDescription(reader));
        } else if (name == "fileName") {
            reader.MoveToFirstChild();
            rule.SetFileName(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileType") {
            reader.MoveToFirstChild();
            rule.SetFileType(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileFormat") {
            reader.MoveToFirstChild();
            rule.SetFileFormat(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "ruleType") {
            reader.MoveToFirstChild();
            rule.SetRuleType(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        }
    } while (reader.MoveToNextSibling());

    catalog.AddRuleFile(rule);
    return true;
}

model::Description RuleFileParser::ParseDescription(XmlReader& reader) {
    model::Description desc;
    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        return desc;
    }

    do {
        std::string name = reader.GetCurrentElementName();
        if (name == "name") {
            reader.MoveToFirstChild();
            desc.SetName(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "description") {
            reader.MoveToFirstChild();
            desc.SetDescription(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "language") {
            reader.MoveToFirstChild();
            desc.SetLanguage(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        }
    } while (reader.MoveToNextSibling());

    return desc;
}

} // namespace parser
} // namespace portrayal
} // namespace ogc
