#include "ogc/portrayal/utils/string_utils.h"
#include <algorithm>
#include <cctype>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#else
#include <iconv.h>
#endif

namespace ogc {
namespace portrayal {
namespace utils {

std::string StringUtils::Trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1);
}

std::string StringUtils::ToLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string StringUtils::ToUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return result;
}

std::vector<std::string> StringUtils::Split(const std::string& str,
                                            char delimiter) {
    std::vector<std::string> parts;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        parts.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(delimiter, start);
    }
    parts.push_back(str.substr(start));
    return parts;
}

std::string StringUtils::Join(const std::vector<std::string>& parts,
                              const std::string& delimiter) {
    std::string result;
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) {
            result += delimiter;
        }
        result += parts[i];
    }
    return result;
}

bool StringUtils::StartsWith(const std::string& str,
                             const std::string& prefix) {
    if (prefix.size() > str.size()) {
        return false;
    }
    return str.compare(0, prefix.size(), prefix) == 0;
}

bool StringUtils::EndsWith(const std::string& str,
                           const std::string& suffix) {
    if (suffix.size() > str.size()) {
        return false;
    }
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string StringUtils::ConvertEncoding(const std::string& input,
                                         const std::string& fromEncoding,
                                         const std::string& toEncoding) {
    if (fromEncoding == toEncoding) {
        return input;
    }

#ifdef _WIN32
    UINT fromCodePage = CP_UTF8;
    UINT toCodePage = CP_UTF8;

    if (ToLower(fromEncoding) == "iso-8859-1" || ToLower(fromEncoding) == "latin1") {
        fromCodePage = 28591;
    } else if (ToLower(fromEncoding) == "utf-8") {
        fromCodePage = CP_UTF8;
    }

    if (ToLower(toEncoding) == "iso-8859-1" || ToLower(toEncoding) == "latin1") {
        toCodePage = 28591;
    } else if (ToLower(toEncoding) == "utf-8") {
        toCodePage = CP_UTF8;
    }

    int wideLen = MultiByteToWideChar(fromCodePage, 0, input.c_str(), -1, nullptr, 0);
    if (wideLen <= 0) {
        return input;
    }

    std::vector<wchar_t> wideStr(wideLen);
    MultiByteToWideChar(fromCodePage, 0, input.c_str(), -1, wideStr.data(), wideLen);

    int outLen = WideCharToMultiByte(toCodePage, 0, wideStr.data(), -1, nullptr, 0, nullptr, nullptr);
    if (outLen <= 0) {
        return input;
    }

    std::vector<char> outStr(outLen);
    WideCharToMultiByte(toCodePage, 0, wideStr.data(), -1, outStr.data(), outLen, nullptr, nullptr);

    return std::string(outStr.data());
#else
    iconv_t cd = iconv_open(toEncoding.c_str(), fromEncoding.c_str());
    if (cd == (iconv_t)-1) {
        return input;
    }

    size_t inBytesLeft = input.size();
    size_t outBytesLeft = input.size() * 4;
    std::vector<char> outBuf(outBytesLeft);

    char* inPtr = const_cast<char*>(input.data());
    char* outPtr = outBuf.data();

    size_t result = iconv(cd, &inPtr, &inBytesLeft, &outPtr, &outBytesLeft);
    iconv_close(cd);

    if (result == (size_t)-1) {
        return input;
    }

    return std::string(outBuf.data(), outPtr - outBuf.data());
#endif
}

} // namespace utils
} // namespace portrayal
} // namespace ogc
