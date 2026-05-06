#include "ogc/portrayal/parser/parser_registry.h"
#include "ogc/portrayal/parser/symbol_parser.h"
#include "ogc/portrayal/parser/line_style_parser.h"
#include "ogc/portrayal/parser/area_fill_parser.h"
#include "ogc/portrayal/parser/rule_file_parser.h"
#include "ogc/portrayal/parser/viewing_group_parser.h"
#include "ogc/portrayal/parser/alert_catalog_parser.h"
#include "ogc/portrayal/parser/color_profile_parser.h"
#include "ogc/portrayal/parser/style_sheet_parser.h"
#include <map>

namespace ogc {
namespace portrayal {
namespace parser {

class ParserRegistry::Impl {
public:
    std::map<std::string, std::unique_ptr<ElementParser>> parsers_;
};

ParserRegistry::ParserRegistry() : impl_(new Impl()) {
    Register(std::unique_ptr<ElementParser>(new SymbolParser()));
    Register(std::unique_ptr<ElementParser>(new LineStyleParser()));
    Register(std::unique_ptr<ElementParser>(new AreaFillParser()));
    Register(std::unique_ptr<ElementParser>(new RuleFileParser()));
    Register(std::unique_ptr<ElementParser>(new ViewingGroupParser()));
    Register(std::unique_ptr<ElementParser>(new AlertCatalogParser()));
    Register(std::unique_ptr<ElementParser>(new ColorProfileParser()));
    Register(std::unique_ptr<ElementParser>(new StyleSheetParser()));
}

ParserRegistry::~ParserRegistry() = default;

ParserRegistry& ParserRegistry::Instance() {
    static ParserRegistry instance;
    return instance;
}

void ParserRegistry::Register(std::unique_ptr<ElementParser> parser) {
    if (parser && impl_) {
        impl_->parsers_[parser->GetElementName()] = std::move(parser);
    }
}

ElementParser* ParserRegistry::GetParser(const std::string& elementName) const {
    if (!impl_) return nullptr;
    auto it = impl_->parsers_.find(elementName);
    if (it != impl_->parsers_.end()) {
        return it->second.get();
    }
    return nullptr;
}

bool ParserRegistry::HasParser(const std::string& elementName) const {
    if (!impl_) return false;
    return impl_->parsers_.find(elementName) != impl_->parsers_.end();
}

std::vector<std::string> ParserRegistry::GetRegisteredElementNames() const {
    std::vector<std::string> names;
    if (impl_) {
        for (const auto& pair : impl_->parsers_) {
            names.push_back(pair.first);
        }
    }
    return names;
}

void ParserRegistry::Clear() {
    if (impl_) {
        impl_->parsers_.clear();
    }
}

} // namespace parser
} // namespace portrayal
} // namespace ogc
