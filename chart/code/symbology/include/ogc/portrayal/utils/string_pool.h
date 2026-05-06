#ifndef OGC_PORTRAYAL_UTILS_STRING_POOL_H
#define OGC_PORTRAYAL_UTILS_STRING_POOL_H

#include "../portrayal_export.h"
#include <string>
#include <unordered_map>
#include <mutex>

namespace ogc {
namespace portrayal {
namespace utils {

class OGC_PORTRAYAL_API StringPool {
public:
    StringPool() = default;

    const std::string& Intern(const std::string& str);
    void Clear();
    size_t Size() const;
    size_t GetMemoryUsage() const;

private:
    std::unordered_map<std::string, std::string> pool_;
    mutable std::mutex mutex_;
};

} // namespace utils
} // namespace portrayal
} // namespace ogc

#endif
