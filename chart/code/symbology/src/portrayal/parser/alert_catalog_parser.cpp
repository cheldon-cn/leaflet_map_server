#include "ogc/portrayal/parser/alert_catalog_parser.h"
#include "ogc/portrayal/utils/string_utils.h"

namespace ogc {
namespace portrayal {
namespace parser {

std::string AlertCatalogParser::GetElementName() const {
    return "alertCatalog";
}

bool AlertCatalogParser::CanParse(const std::string& elementName) const {
    return elementName == "alertCatalog";
}

bool AlertCatalogParser::Parse(XmlReader& reader,
                               model::PortrayalCatalog& catalog) {
    model::AlertCatalog alertCatalog;

    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        catalog.SetAlertCatalog(alertCatalog);
        return true;
    }

    do {
        std::string name = reader.GetCurrentElementName();
        if (name == "description") {
            alertCatalog.SetDescription(ParseDescription(reader));
        } else if (name == "fileName") {
            reader.MoveToFirstChild();
            alertCatalog.SetFileName(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileType") {
            reader.MoveToFirstChild();
            alertCatalog.SetFileType(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "fileFormat") {
            reader.MoveToFirstChild();
            alertCatalog.SetFileFormat(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        }
    } while (reader.MoveToNextSibling());

    catalog.SetAlertCatalog(alertCatalog);
    return true;
}

model::Description AlertCatalogParser::ParseDescription(XmlReader& reader) {
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
