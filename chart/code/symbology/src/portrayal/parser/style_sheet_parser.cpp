#include "ogc/portrayal/parser/style_sheet_parser.h"
#include "ogc/portrayal/utils/string_utils.h"

namespace ogc {
namespace portrayal {
namespace parser {

std::string StyleSheetParser::GetElementName() const {
    return "styleSheet";
}

bool StyleSheetParser::CanParse(const std::string& elementName) const {
    return elementName == "styleSheet";
}

bool StyleSheetParser::Parse(XmlReader& reader,
                             model::PortrayalCatalog& catalog) {
    model::StyleSheet sheet;
    sheet.SetId(reader.GetAttribute("id"));

    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        catalog.AddStyleSheet(sheet);
        return true;
    }

    do {
        std::string name = reader.GetCurrentElementName();
        if (name == "description") {
            sheet.SetDescription(ParseDescription(reader));
        } else if (name == "fileName") {
            reader.MoveToFirstChild();
            sheet.SetFileName(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileType") {
            reader.MoveToFirstChild();
            sheet.SetFileType(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileFormat") {
            reader.MoveToFirstChild();
            sheet.SetFileFormat(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        }
    } while (reader.MoveToNextSibling());

    catalog.AddStyleSheet(sheet);
    return true;
}

model::Description StyleSheetParser::ParseDescription(XmlReader& reader) {
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
