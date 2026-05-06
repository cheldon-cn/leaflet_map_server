#include "ogc/portrayal/parser/portrayal_catalog_parser.h"
#include "ogc/portrayal/parser/libxml2_reader.h"
#include "ogc/portrayal/parser/parser_registry.h"
#include "ogc/portrayal/utils/string_utils.h"

namespace ogc {
namespace portrayal {
namespace parser {

PortrayalCatalogParser::PortrayalCatalogParser() {
}

bool PortrayalCatalogParser::Parse(const std::string& filePath,
                                   model::PortrayalCatalog& catalog) {
    std::unique_ptr<XmlReader> reader(new LibXml2Reader());
    if (!reader->Open(filePath)) {
        lastError_ = "Failed to open XML file: " + filePath;
        return false;
    }

    bool result = ParseRootElement(*reader, catalog);
    reader->Close();
    return result;
}

bool PortrayalCatalogParser::ParseRootElement(XmlReader& reader,
                                              model::PortrayalCatalog& catalog) {
    while (reader.ReadNextElement()) {
        std::string name = reader.GetCurrentElementName();
        if (name == "PortrayalCatalogue") {
            return ParseChildElements(reader, catalog);
        }
    }

    lastError_ = "Root element 'PortrayalCatalogue' not found";
    return false;
}

bool PortrayalCatalogParser::ParseChildElements(XmlReader& reader,
                                                model::PortrayalCatalog& catalog) {
    bool hasContent = reader.MoveToFirstChild();
    if (!hasContent) {
        return true;
    }

    ParserRegistry& registry = ParserRegistry::Instance();

    do {
        std::string name = reader.GetCurrentElementName();

        if (name == "productId") {
            reader.MoveToFirstChild();
            catalog.SetProductId(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else if (name == "version") {
            reader.MoveToFirstChild();
            catalog.SetVersion(utils::StringUtils::Trim(reader.GetCurrentElementText()));
            reader.MoveToParent();
        } else {
            ElementParser* elementParser = registry.GetParser(name);
            if (elementParser) {
                if (!elementParser->Parse(reader, catalog)) {
                    lastError_ = "Failed to parse element: " + name;
                    return false;
                }
            }
        }
    } while (reader.MoveToNextSibling());

    return true;
}

std::string PortrayalCatalogParser::GetLastError() const {
    return lastError_;
}

} // namespace parser
} // namespace portrayal
} // namespace ogc
