#ifndef OGC_PORTRAYAL_PARSER_LIBXML2_READER_H
#define OGC_PORTRAYAL_PARSER_LIBXML2_READER_H

#include "ogc/portrayal/parser/xml_reader.h"
#include "../portrayal_export.h"
#include <memory>

namespace ogc {
namespace portrayal {
namespace parser {

class OGC_PORTRAYAL_API LibXml2Reader : public XmlReader {
public:
    LibXml2Reader();
    ~LibXml2Reader() override;

    bool Open(const std::string& filePath) override;
    void Close() override;
    bool IsOpen() const override;
    std::string GetEncoding() const override;

    bool ReadNextElement() override;
    std::string GetCurrentElementName() const override;
    std::string GetCurrentElementText() const override;
    std::string GetAttribute(const std::string& name) const override;
    bool HasAttribute(const std::string& name) const override;

    bool MoveToFirstChild() override;
    bool MoveToNextSibling() override;
    bool MoveToParent() override;

private:
    class Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc

#endif
