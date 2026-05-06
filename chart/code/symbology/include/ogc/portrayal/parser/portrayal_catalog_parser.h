#ifndef OGC_PORTRAYAL_PARSER_PORTRAYAL_CATALOG_PARSER_H
#define OGC_PORTRAYAL_PARSER_PORTRAYAL_CATALOG_PARSER_H

#include "ogc/portrayal/model/portrayal_catalog.h"
#include "ogc/portrayal/parser/xml_reader.h"
#include "../portrayal_export.h"
#include <memory>
#include <string>

namespace ogc {
namespace portrayal {
namespace parser {

class OGC_PORTRAYAL_API PortrayalCatalogParser {
public:
    PortrayalCatalogParser();
    ~PortrayalCatalogParser() = default;

    bool Parse(const std::string& filePath,
               model::PortrayalCatalog& catalog);

    std::string GetLastError() const;

private:
    bool ParseRootElement(XmlReader& reader,
                          model::PortrayalCatalog& catalog);
    bool ParseChildElements(XmlReader& reader,
                            model::PortrayalCatalog& catalog);

    std::string lastError_;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc

#endif
