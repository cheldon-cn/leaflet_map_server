#ifndef OGC_PORTRAYAL_UTILS_STRING_UTILS_H
#define OGC_PORTRAYAL_UTILS_STRING_UTILS_H

#include "../portrayal_export.h"
#include <string>
#include <vector>

namespace ogc {
namespace portrayal {
namespace utils {

class OGC_PORTRAYAL_API StringUtils {
public:
    static std::string Trim(const std::string& str);
    static std::string ToLower(const std::string& str);
    static std::string ToUpper(const std::string& str);
    static std::vector<std::string> Split(const std::string& str,
                                          char delimiter);
    static std::string Join(const std::vector<std::string>& parts,
                           const std::string& delimiter);
    static bool StartsWith(const std::string& str,
                          const std::string& prefix);
    static bool EndsWith(const std::string& str,
                        const std::string& suffix);
    static std::string ConvertEncoding(const std::string& input,
                                       const std::string& fromEncoding,
                                       const std::string& toEncoding);
};

} // namespace utils
} // namespace portrayal
} // namespace ogc

#endif
