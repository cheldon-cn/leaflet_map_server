#include "ogc/portrayal/model/file_reference.h"

namespace ogc {
namespace portrayal {
namespace model {

const std::string& FileReference::GetId() const {
    return id_;
}

const Description& FileReference::GetDescription() const {
    return description_;
}

const std::string& FileReference::GetFileName() const {
    return fileName_;
}

const std::string& FileReference::GetFileType() const {
    return fileType_;
}

const std::string& FileReference::GetFileFormat() const {
    return fileFormat_;
}

void FileReference::SetId(const std::string& id) {
    id_ = id;
}

void FileReference::SetDescription(const Description& desc) {
    description_ = desc;
}

void FileReference::SetFileName(const std::string& fileName) {
    fileName_ = fileName;
}

void FileReference::SetFileType(const std::string& fileType) {
    fileType_ = fileType;
}

void FileReference::SetFileFormat(const std::string& fileFormat) {
    fileFormat_ = fileFormat;
}

} // namespace model
} // namespace portrayal
} // namespace ogc
