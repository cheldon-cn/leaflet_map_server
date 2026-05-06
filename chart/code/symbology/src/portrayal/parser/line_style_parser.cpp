#include "ogc/portrayal/parser/line_style_parser.h"
#include "ogc/portrayal/utils/string_utils.h"

namespace ogc {
namespace portrayal {
namespace parser {

std::string LineStyleParser::GetElementName() const {
    return "lineStyle";
}

bool LineStyleParser::CanParse(const std::string& elementName) const {
    return elementName == "lineStyle";
}

bool LineStyleParser::Parse(XmlReader& reader,
                            model::PortrayalCatalog& catalog) {
    model::LineStyle style;
    style.SetId(reader.GetAttribute("id"));

    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        catalog.AddLineStyle(style);
        return true;
    }

    do {
        std::string name = reader.GetCurrentElementName();
        if (name == "description") {
            style.SetDescription(ParseDescription(reader));
        } else if (name == "fileName") {
            reader.MoveToFirstChild();
            style.SetFileName(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileType") {
            reader.MoveToFirstChild();
            style.SetFileType(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileFormat") {
            reader.MoveToFirstChild();
            style.SetFileFormat(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        }
    } while (reader.MoveToNextSibling());

    catalog.AddLineStyle(style);
    return true;
}

model::Description LineStyleParser::ParseDescription(XmlReader& reader) {
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
