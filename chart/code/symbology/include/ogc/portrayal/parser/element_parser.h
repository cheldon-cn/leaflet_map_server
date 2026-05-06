#ifndef OGC_PORTRAYAL_PARSER_ELEMENT_PARSER_H
#define OGC_PORTRAYAL_PARSER_ELEMENT_PARSER_H

#include "ogc/portrayal/parser/xml_reader.h"
#include "ogc/portrayal/model/portrayal_catalog.h"
#include <string>

namespace ogc {
namespace portrayal {
namespace parser {

class ElementParser {
public:
    virtual ~ElementParser() = default;

    virtual std::string GetElementName() const = 0;

    virtual bool CanParse(const std::string& elementName) const {
        return elementName == GetElementName();
    }

    virtual bool Parse(XmlReader& reader,
                       model::PortrayalCatalog& catalog) = 0;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc

#endif
