#include "ogc/portrayal/parser/viewing_group_parser.h"
#include "ogc/portrayal/utils/string_utils.h"

namespace ogc {
namespace portrayal {
namespace parser {

std::string ViewingGroupParser::GetElementName() const {
    return "viewingGroup";
}

bool ViewingGroupParser::CanParse(const std::string& elementName) const {
    return elementName == "viewingGroup";
}

bool ViewingGroupParser::Parse(XmlReader& reader,
                               model::PortrayalCatalog& catalog) {
    model::ViewingGroup group;
    group.SetId(reader.GetAttribute("id"));

    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        catalog.AddViewingGroup(group);
        return true;
    }

    do {
        std::string name = reader.GetCurrentElementName();
        if (name == "description") {
            group.SetDescription(ParseDescription(reader));
        }
    } while (reader.MoveToNextSibling());

    catalog.AddViewingGroup(group);
    return true;
}

model::Description ViewingGroupParser::ParseDescription(XmlReader& reader) {
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
