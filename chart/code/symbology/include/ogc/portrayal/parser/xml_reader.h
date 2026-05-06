#ifndef OGC_PORTRAYAL_PARSER_XML_READER_H
#define OGC_PORTRAYAL_PARSER_XML_READER_H

#include <string>

namespace ogc {
namespace portrayal {
namespace parser {

class XmlReader {
public:
    virtual ~XmlReader() = default;

    virtual bool Open(const std::string& filePath) = 0;
    virtual void Close() = 0;
    virtual bool IsOpen() const = 0;
    virtual std::string GetEncoding() const = 0;

    virtual bool ReadNextElement() = 0;
    virtual std::string GetCurrentElementName() const = 0;
    virtual std::string GetCurrentElementText() const = 0;
    virtual std::string GetAttribute(const std::string& name) const = 0;
    virtual bool HasAttribute(const std::string& name) const = 0;

    virtual bool MoveToFirstChild() = 0;
    virtual bool MoveToNextSibling() = 0;
    virtual bool MoveToParent() = 0;
};

} // namespace parser
} // namespace portrayal
} // namespace ogc

#endif
