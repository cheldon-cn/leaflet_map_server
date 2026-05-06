#ifndef OGC_PORTRAYAL_MODEL_FILE_REFERENCE_H
#define OGC_PORTRAYAL_MODEL_FILE_REFERENCE_H

#include "ogc/portrayal/model/description.h"
#include "../portrayal_export.h"
#include <string>

namespace ogc {
namespace portrayal {
namespace model {

class OGC_PORTRAYAL_API FileReference {
public:
    FileReference() = default;
    virtual ~FileReference() = default;

    const std::string& GetId() const;
    const Description& GetDescription() const;
    const std::string& GetFileName() const;
    const std::string& GetFileType() const;
    const std::string& GetFileFormat() const;

    void SetId(const std::string& id);
    void SetDescription(const Description& desc);
    void SetFileName(const std::string& fileName);
    void SetFileType(const std::string& fileType);
    void SetFileFormat(const std::string& fileFormat);

protected:
    std::string id_;
    Description description_;
    std::string fileName_;
    std::string fileType_;
    std::string fileFormat_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
