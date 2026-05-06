#include "ogc/portrayal/parser/area_fill_parser.h"
#include "ogc/portrayal/utils/string_utils.h"

namespace ogc {
namespace portrayal {
namespace parser {

std::string AreaFillParser::GetElementName() const {
    return "areaFill";
}

bool AreaFillParser::CanParse(const std::string& elementName) const {
    return elementName == "areaFill";
}

bool AreaFillParser::Parse(XmlReader& reader,
                           model::PortrayalCatalog& catalog) {
    model::AreaFill fill;
    fill.SetId(reader.GetAttribute("id"));

    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        catalog.AddAreaFill(fill);
        return true;
    }

    do {
        std::string name = reader.GetCurrentElementName();
        if (name == "description") {
            fill.SetDescription(ParseDescription(reader));
        } else if (name == "fileName") {
            reader.MoveToFirstChild();
            fill.SetFileName(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileType") {
            reader.MoveToFirstChild();
            fill.SetFileType(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileFormat") {
            reader.MoveToFirstChild();
            fill.SetFileFormat(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        }
    } while (reader.MoveToNextSibling());

    catalog.AddAreaFill(fill);
    return true;
}

model::Description AreaFillParser::ParseDescription(XmlReader& reader) {
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
