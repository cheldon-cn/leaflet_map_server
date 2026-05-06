#ifndef OGC_PORTRAYAL_UTILS_FILE_UTILS_H
#define OGC_PORTRAYAL_UTILS_FILE_UTILS_H

#include "../portrayal_export.h"
#include <string>

namespace ogc {
namespace portrayal {
namespace utils {

class OGC_PORTRAYAL_API FileUtils {
public:
    static bool Exists(const std::string& path);
    static bool IsFile(const std::string& path);
    static bool IsDirectory(const std::string& path);
    static std::string GetDirectory(const std::string& filePath);
    static std::string GetFileName(const std::string& filePath);
    static std::string GetFileExtension(const std::string& filePath);
    static std::string CombinePath(const std::string& dir,
                                   const std::string& file);
};

} // namespace utils
} // namespace portrayal
} // namespace ogc

#endif
