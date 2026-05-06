#ifndef OGC_PORTRAYAL_PARSER_SYMBOL_PARSER_H
#define OGC_PORTRAYAL_PARSER_SYMBOL_PARSER_H

#include "ogc/portrayal/parser/element_parser.h"
#include "../portrayal_export.h"

namespace ogc {
namespace portrayal {
namespace parser {

class OGC_PORTRAYAL_API SymbolParser : public ElementParser {
public:
    std::string GetElementName() const override;
    bool CanParse(const std::string& elementName) const override;
    bool Parse(XmlReader& reader,
               model::PortrayalCatalog& catalog) override;

private:
    model::Description ParseDescription(XmlReader& reader);
};

} // namespace parser
} // namespace portrayal
} // namespace ogc

#endif
