#ifndef OGC_PORTRAYAL_MODEL_DESCRIPTION_H
#define OGC_PORTRAYAL_MODEL_DESCRIPTION_H

#include <string>
#include "../portrayal_export.h"

namespace ogc {
namespace portrayal {
namespace model {

class OGC_PORTRAYAL_API Description {
public:
    Description() = default;
    Description(const std::string& name,
                const std::string& desc,
                const std::string& lang = "eng");

    const std::string& GetName() const;
    const std::string& GetDescription() const;
    const std::string& GetLanguage() const;

    void SetName(const std::string& name);
    void SetDescription(const std::string& desc);
    void SetLanguage(const std::string& lang);

private:
    std::string name_;
    std::string description_;
    std::string language_;
};

} // namespace model
} // namespace portrayal
} // namespace ogc

#endif
