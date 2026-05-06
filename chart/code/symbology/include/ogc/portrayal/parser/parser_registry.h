#ifndef OGC_PORTRAYAL_PARSER_PARSER_REGISTRY_H
#define OGC_PORTRAYAL_PARSER_PARSER_REGISTRY_H

#include "ogc/portrayal/parser/element_parser.h"
#include "../portrayal_export.h"
#include <memory>
#include <string>
#include <vector>

namespace ogc {
namespace portrayal {
namespace parser {

class OGC_PORTRAYAL_API ParserRegistry {
public:
    static ParserRegistry& Instance();

    void Register(std::unique_ptr<ElementParser> parser);
    ElementParser* GetParser(const std::string& elementName) const;
    bool HasParser(const std::string& elementName) const;
    std::vector<std::string> GetRegisteredElementNames() const;
    void Clear();

private:
    ParserRegistry();
    ~ParserRegistry();

    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc

#endif
