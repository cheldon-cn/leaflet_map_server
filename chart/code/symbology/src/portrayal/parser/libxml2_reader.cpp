#include "ogc/portrayal/parser/libxml2_reader.h"
#include <libxml/xmlreader.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <cstring>

namespace ogc {
namespace portrayal {
namespace parser {

class LibXml2Reader::Impl {
public:
    Impl()
        : reader_(nullptr)
        , isOpen_(false)
        , encoding_()
        , currentName_()
        , currentText_()
        , depth_(0)
        , nodeType_(0) {}

    ~Impl() {
        Close();
    }

    bool Open(const std::string& filePath) {
        Close();
        reader_ = xmlReaderForFile(filePath.c_str(), nullptr,
                                   XML_PARSE_NOENT | XML_PARSE_NONET);
        if (!reader_) {
            return false;
        }
        isOpen_ = true;
        return true;
    }

    void Close() {
        if (reader_) {
            xmlFreeTextReader(reader_);
            reader_ = nullptr;
        }
        isOpen_ = false;
        encoding_.clear();
        currentName_.clear();
        currentText_.clear();
    }

    bool IsOpen() const {
        return isOpen_;
    }

    std::string GetEncoding() const {
        return encoding_;
    }

    bool ReadNextElement() {
        if (!reader_) {
            return false;
        }

        int ret = xmlTextReaderRead(reader_);
        if (ret != 1) {
            return false;
        }

        nodeType_ = xmlTextReaderNodeType(reader_);
        depth_ = xmlTextReaderDepth(reader_);

        const xmlChar* name = xmlTextReaderConstName(reader_);
        currentName_ = name ? reinterpret_cast<const char*>(name) : "";

        if (nodeType_ == XML_READER_TYPE_TEXT || nodeType_ == XML_READER_TYPE_CDATA) {
            const xmlChar* value = xmlTextReaderConstValue(reader_);
            currentText_ = value ? reinterpret_cast<const char*>(value) : "";
        } else {
            currentText_.clear();
        }

        if (encoding_.empty()) {
            const xmlChar* enc = xmlTextReaderConstEncoding(reader_);
            if (enc) {
                encoding_ = reinterpret_cast<const char*>(enc);
            }
        }

        return true;
    }

    std::string GetCurrentElementName() const {
        return currentName_;
    }

    std::string GetCurrentElementText() const {
        return currentText_;
    }

    std::string GetAttribute(const std::string& name) const {
        if (!reader_) {
            return "";
        }
        xmlChar* attr = xmlTextReaderGetAttribute(reader_,
                                                   reinterpret_cast<const xmlChar*>(name.c_str()));
        if (!attr) {
            return "";
        }
        std::string result(reinterpret_cast<const char*>(attr));
        xmlFree(attr);
        return result;
    }

    bool HasAttribute(const std::string& name) const {
        if (!reader_) {
            return false;
        }
        xmlChar* attr = xmlTextReaderGetAttribute(reader_,
                                                   reinterpret_cast<const xmlChar*>(name.c_str()));
        if (attr) {
            xmlFree(attr);
            return true;
        }
        return false;
    }

    bool MoveToFirstChild() {
        if (!reader_) {
            return false;
        }
        int currentDepth = xmlTextReaderDepth(reader_);
        int ret = xmlTextReaderRead(reader_);
        if (ret != 1) {
            return false;
        }
        int newDepth = xmlTextReaderDepth(reader_);
        if (newDepth <= currentDepth) {
            return false;
        }

        nodeType_ = xmlTextReaderNodeType(reader_);
        const xmlChar* name = xmlTextReaderConstName(reader_);
        currentName_ = name ? reinterpret_cast<const char*>(name) : "";

        if (nodeType_ == XML_READER_TYPE_TEXT || nodeType_ == XML_READER_TYPE_CDATA) {
            const xmlChar* value = xmlTextReaderConstValue(reader_);
            currentText_ = value ? reinterpret_cast<const char*>(value) : "";
        } else {
            currentText_.clear();
        }
        return true;
    }

    bool MoveToNextSibling() {
        if (!reader_) {
            return false;
        }
        int currentDepth = xmlTextReaderDepth(reader_);

        while (true) {
            int ret = xmlTextReaderRead(reader_);
            if (ret != 1) {
                return false;
            }
            int newDepth = xmlTextReaderDepth(reader_);
            if (newDepth < currentDepth) {
                return false;
            }
            if (newDepth == currentDepth) {
                nodeType_ = xmlTextReaderNodeType(reader_);
                const xmlChar* name = xmlTextReaderConstName(reader_);
                currentName_ = name ? reinterpret_cast<const char*>(name) : "";

                if (nodeType_ == XML_READER_TYPE_TEXT || nodeType_ == XML_READER_TYPE_CDATA) {
                    const xmlChar* value = xmlTextReaderConstValue(reader_);
                    currentText_ = value ? reinterpret_cast<const char*>(value) : "";
                } else {
                    currentText_.clear();
                }
                return true;
            }
        }
        return false;
    }

    bool MoveToParent() {
        if (!reader_) {
            return false;
        }
        int currentDepth = xmlTextReaderDepth(reader_);
        if (currentDepth == 0) {
            return false;
        }

        while (true) {
            int ret = xmlTextReaderRead(reader_);
            if (ret != 1) {
                return false;
            }
            int newDepth = xmlTextReaderDepth(reader_);
            if (newDepth < currentDepth) {
                nodeType_ = xmlTextReaderNodeType(reader_);
                const xmlChar* name = xmlTextReaderConstName(reader_);
                currentName_ = name ? reinterpret_cast<const char*>(name) : "";
                currentText_.clear();
                return true;
            }
        }
        return false;
    }

    xmlTextReaderPtr reader_;
    bool isOpen_;
    std::string encoding_;
    std::string currentName_;
    std::string currentText_;
    int depth_;
    int nodeType_;
};

LibXml2Reader::LibXml2Reader()
    : impl_(new Impl()) {
}

LibXml2Reader::~LibXml2Reader() = default;

bool LibXml2Reader::Open(const std::string& filePath) {
    return impl_->Open(filePath);
}

void LibXml2Reader::Close() {
    impl_->Close();
}

bool LibXml2Reader::IsOpen() const {
    return impl_->IsOpen();
}

std::string LibXml2Reader::GetEncoding() const {
    return impl_->GetEncoding();
}

bool LibXml2Reader::ReadNextElement() {
    return impl_->ReadNextElement();
}

std::string LibXml2Reader::GetCurrentElementName() const {
    return impl_->GetCurrentElementName();
}

std::string LibXml2Reader::GetCurrentElementText() const {
    return impl_->GetCurrentElementText();
}

std::string LibXml2Reader::GetAttribute(const std::string& name) const {
    return impl_->GetAttribute(name);
}

bool LibXml2Reader::HasAttribute(const std::string& name) const {
    return impl_->HasAttribute(name);
}

bool LibXml2Reader::MoveToFirstChild() {
    return impl_->MoveToFirstChild();
}

bool LibXml2Reader::MoveToNextSibling() {
    return impl_->MoveToNextSibling();
}

bool LibXml2Reader::MoveToParent() {
    return impl_->MoveToParent();
}

} // namespace parser
} // namespace portrayal
} // namespace ogc
