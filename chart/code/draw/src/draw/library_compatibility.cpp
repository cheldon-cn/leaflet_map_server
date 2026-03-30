#include "ogc/draw/library_compatibility.h"
#include <algorithm>
#include <sstream>
#include <iostream>

namespace ogc {
namespace draw {

bool LibraryVersion::operator>=(const LibraryVersion& other) const {
    if (major != other.major) return major > other.major;
    if (minor != other.minor) return minor > other.minor;
    return patch >= other.patch;
}

bool LibraryVersion::operator<(const LibraryVersion& other) const {
    if (major != other.major) return major < other.major;
    if (minor != other.minor) return minor < other.minor;
    return patch < other.patch;
}

bool LibraryVersion::operator==(const LibraryVersion& other) const {
    return major == other.major && minor == other.minor && patch == other.patch;
}

std::string LibraryVersion::ToString() const {
    std::ostringstream oss;
    oss << major << "." << minor << "." << patch;
    return oss.str();
}

LibraryVersion LibraryVersion::Parse(const std::string& versionStr, const std::string& name) {
    LibraryVersion version;
    version.name = name;
    
    size_t pos = 0;
    size_t prev = 0;
    int component = 0;
    
    std::string str = versionStr;
    while ((pos = str.find('.', prev)) != std::string::npos && component < 3) {
        std::string part = str.substr(prev, pos - prev);
        if (component == 0) version.major = std::stoi(part);
        else if (component == 1) version.minor = std::stoi(part);
        prev = pos + 1;
        component++;
    }
    
    if (component < 3) {
        std::string part = str.substr(prev);
        if (component == 0) version.major = std::stoi(part);
        else if (component == 1) version.minor = std::stoi(part);
        else if (component == 2) version.patch = std::stoi(part);
    }
    
    return version;
}

std::string CompatibilityIssue::ToString() const {
    std::ostringstream oss;
    oss << libraryName << ": detected " << detected.ToString() 
        << ", required " << required.ToString();
    if (isCritical) {
        oss << " [CRITICAL]";
    }
    if (!description.empty()) {
        oss << " - " << description;
    }
    return oss.str();
}

bool LibraryCompatibility::s_enableLogging = false;
std::map<std::string, LibraryVersion> LibraryCompatibility::s_minimumVersions;
std::map<std::string, LibraryVersion> LibraryCompatibility::s_detectedVersions;

void LibraryCompatibility::InitializeMinimumVersions() {
    s_minimumVersions.clear();
    
    LibraryVersion qtVer;
    qtVer.name = "Qt"; qtVer.major = 5; qtVer.minor = 12; qtVer.patch = 0;
    s_minimumVersions["Qt"] = qtVer;
    
    LibraryVersion cairoVer;
    cairoVer.name = "Cairo"; cairoVer.major = 1; cairoVer.minor = 14; cairoVer.patch = 0;
    s_minimumVersions["Cairo"] = cairoVer;
    
    LibraryVersion podofoVer;
    podofoVer.name = "podofo"; podofoVer.major = 0; podofoVer.minor = 9; podofoVer.patch = 0;
    s_minimumVersions["podofo"] = podofoVer;
    
    LibraryVersion openglVer;
    openglVer.name = "OpenGL"; openglVer.major = 3; openglVer.minor = 3; openglVer.patch = 0;
    s_minimumVersions["OpenGL"] = openglVer;
    
    LibraryVersion freetypeVer;
    freetypeVer.name = "FreeType"; freetypeVer.major = 2; freetypeVer.minor = 10; freetypeVer.patch = 0;
    s_minimumVersions["FreeType"] = freetypeVer;
    
    LibraryVersion zlibVer;
    zlibVer.name = "zlib"; zlibVer.major = 1; zlibVer.minor = 2; zlibVer.patch = 11;
    s_minimumVersions["zlib"] = zlibVer;
    
    LibraryVersion libpngVer;
    libpngVer.name = "libpng"; libpngVer.major = 1; libpngVer.minor = 6; libpngVer.patch = 37;
    s_minimumVersions["libpng"] = libpngVer;
    
    LibraryVersion libjpegVer;
    libjpegVer.name = "libjpeg"; libjpegVer.major = 9; libjpegVer.minor = 0; libjpegVer.patch = 0;
    s_minimumVersions["libjpeg"] = libjpegVer;
}

LibraryVersion LibraryCompatibility::GetQtVersion() {
    return GetLibraryVersion("Qt");
}

LibraryVersion LibraryCompatibility::GetCairoVersion() {
    return GetLibraryVersion("Cairo");
}

LibraryVersion LibraryCompatibility::GetPodofoVersion() {
    return GetLibraryVersion("podofo");
}

LibraryVersion LibraryCompatibility::GetOpenGLVersion() {
    return GetLibraryVersion("OpenGL");
}

LibraryVersion LibraryCompatibility::GetFreetypeVersion() {
    return GetLibraryVersion("FreeType");
}

LibraryVersion LibraryCompatibility::GetZlibVersion() {
    return GetLibraryVersion("zlib");
}

LibraryVersion LibraryCompatibility::GetLibPngVersion() {
    return GetLibraryVersion("libpng");
}

LibraryVersion LibraryCompatibility::GetLibJpegVersion() {
    return GetLibraryVersion("libjpeg");
}

LibraryVersion LibraryCompatibility::GetLibraryVersion(const std::string& libraryName) {
    if (s_minimumVersions.empty()) {
        InitializeMinimumVersions();
    }
    
    auto it = s_detectedVersions.find(libraryName);
    if (it != s_detectedVersions.end()) {
        return it->second;
    }
    
    LibraryVersion version;
    
    if (libraryName == "Qt") {
        version = DetectQtVersion();
    } else if (libraryName == "Cairo") {
        version = DetectCairoVersion();
    } else if (libraryName == "podofo") {
        version = DetectPodofoVersion();
    } else if (libraryName == "OpenGL") {
        version = DetectOpenGLVersion();
    } else if (libraryName == "FreeType") {
        version = DetectFreetypeVersion();
    } else if (libraryName == "zlib") {
        version = DetectZlibVersion();
    } else if (libraryName == "libpng") {
        version = DetectLibPngVersion();
    } else if (libraryName == "libjpeg") {
        version = DetectLibJpegVersion();
    }
    
    s_detectedVersions[libraryName] = version;
    return version;
}

bool LibraryCompatibility::CheckMinimumRequirements() {
    auto issues = GetDetailedCompatibilityIssues();
    for (const auto& issue : issues) {
        if (issue.isCritical) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> LibraryCompatibility::GetCompatibilityIssues() {
    auto detailedIssues = GetDetailedCompatibilityIssues();
    std::vector<std::string> issues;
    for (const auto& issue : detailedIssues) {
        issues.push_back(issue.ToString());
    }
    return issues;
}

std::vector<CompatibilityIssue> LibraryCompatibility::GetDetailedCompatibilityIssues() {
    std::vector<CompatibilityIssue> issues;
    
    if (s_minimumVersions.empty()) {
        InitializeMinimumVersions();
    }
    
    for (const auto& pair : s_minimumVersions) {
        const std::string& name = pair.first;
        const LibraryVersion& required = pair.second;
        LibraryVersion detected = GetLibraryVersion(name);
        
        if (detected.IsValid() && detected < required) {
            CompatibilityIssue issue;
            issue.libraryName = name;
            issue.detected = detected;
            issue.required = required;
            issue.isCritical = true;
            issue.description = name + " version " + detected.ToString() + 
                               " is below minimum required " + required.ToString();
            issues.push_back(issue);
        }
    }
    
    return issues;
}

std::string LibraryCompatibility::GetCompatibilityReport() {
    std::string report = "Library Compatibility Report\n";
    report += "==========================\n\n";
    
    auto issues = GetCompatibilityIssues();
    if (issues.empty()) {
        report += "All library requirements met.\n";
    } else {
        report += "Compatibility Issues:\n";
        for (const auto& issue : issues) {
            report += "  - " + issue + "\n";
        }
    }
    
    report += "\nDetected Library Versions:\n";
    auto available = GetAvailableLibraries();
    for (const auto& lib : available) {
        auto version = GetLibraryVersion(lib);
        report += "  " + lib + ": " + version.ToString() + "\n";
    }
    
    return report;
}

std::string LibraryCompatibility::GetDetailedCompatibilityReport() {
    std::string report = GetCompatibilityReport();
    
    report += "\nMinimum Required Versions:\n";
    for (const auto& pair : s_minimumVersions) {
        report += "  " + pair.first + ": " + pair.second.ToString() + "\n";
    }
    
    report += "\n" + GetSystemInfo();
    
    return report;
}

void LibraryCompatibility::SetMinimumVersion(const std::string& libraryName, const LibraryVersion& version) {
    s_minimumVersions[libraryName] = version;
}

LibraryVersion LibraryCompatibility::GetMinimumVersion(const std::string& libraryName) {
    if (s_minimumVersions.empty()) {
        InitializeMinimumVersions();
    }
    
    auto it = s_minimumVersions.find(libraryName);
    if (it != s_minimumVersions.end()) {
        return it->second;
    }
    return LibraryVersion();
}

void LibraryCompatibility::ResetMinimumVersions() {
    s_minimumVersions.clear();
    s_detectedVersions.clear();
    InitializeMinimumVersions();
}

bool LibraryCompatibility::IsQtAvailable() {
    return GetQtVersion().IsValid();
}

bool LibraryCompatibility::IsCairoAvailable() {
    return GetCairoVersion().IsValid();
}

bool LibraryCompatibility::IsPodofoAvailable() {
    return GetPodofoVersion().IsValid();
}

bool LibraryCompatibility::IsOpenGLAvailable() {
    return GetOpenGLVersion().IsValid();
}

bool LibraryCompatibility::IsFreetypeAvailable() {
    return GetFreetypeVersion().IsValid();
}

std::vector<std::string> LibraryCompatibility::GetAvailableLibraries() {
    std::vector<std::string> available;
    
    if (IsQtAvailable()) available.push_back("Qt");
    if (IsCairoAvailable()) available.push_back("Cairo");
    if (IsPodofoAvailable()) available.push_back("podofo");
    if (IsOpenGLAvailable()) available.push_back("OpenGL");
    if (IsFreetypeAvailable()) available.push_back("FreeType");
    if (GetZlibVersion().IsValid()) available.push_back("zlib");
    if (GetLibPngVersion().IsValid()) available.push_back("libpng");
    if (GetLibJpegVersion().IsValid()) available.push_back("libjpeg");
    
    return available;
}

std::string LibraryCompatibility::GetSystemInfo() {
    std::string info = "System Information:\n";
    
#ifdef _WIN32
    info += "  Platform: Windows\n";
#elif defined(__linux__)
    info += "  Platform: Linux\n";
#elif defined(__APPLE__)
    info += "  Platform: macOS\n";
#else
    info += "  Platform: Unknown\n";
#endif

#ifdef _MSC_VER
    info += "  Compiler: MSVC " + std::to_string(_MSC_VER / 100) + "." + std::to_string(_MSC_VER % 100) + "\n";
#elif defined(__GNUC__)
    info += "  Compiler: GCC " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "\n";
#elif defined(__clang__)
    info += "  Compiler: Clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "\n";
#endif

    info += "  Build: ";
#ifdef NDEBUG
    info += "Release\n";
#else
    info += "Debug\n";
#endif
    
    return info;
}

void LibraryCompatibility::SetEnableLogging(bool enable) {
    s_enableLogging = enable;
}

bool LibraryCompatibility::IsLoggingEnabled() {
    return s_enableLogging;
}

void LibraryCompatibility::Log(const std::string& message) {
    if (s_enableLogging) {
        std::cout << "[LibraryCompatibility] " << message << std::endl;
    }
}

LibraryVersion LibraryCompatibility::DetectQtVersion() {
    LibraryVersion version;
    version.name = "Qt";
    
#ifdef DRAW_HAS_QT
    version.major = QT_VERSION_MAJOR;
    version.minor = QT_VERSION_MINOR;
    version.patch = QT_VERSION_PATCH;
#endif
    
    return version;
}

LibraryVersion LibraryCompatibility::DetectCairoVersion() {
    LibraryVersion version;
    version.name = "Cairo";
    
#ifdef DRAW_HAS_CAIRO
    version.major = CAIRO_VERSION_MAJOR;
    version.minor = CAIRO_VERSION_MINOR;
    version.patch = CAIRO_VERSION_MICRO;
#endif
    
    return version;
}

LibraryVersion LibraryCompatibility::DetectPodofoVersion() {
    LibraryVersion version;
    version.name = "podofo";
    
#ifdef DRAW_HAS_PODOFO
    version.major = PODOFO_VERSION_MAJOR;
    version.minor = PODOFO_VERSION_MINOR;
    version.patch = PODOFO_VERSION_PATCH;
#endif
    
    return version;
}

LibraryVersion LibraryCompatibility::DetectOpenGLVersion() {
    LibraryVersion version;
    version.name = "OpenGL";
    
    return version;
}

LibraryVersion LibraryCompatibility::DetectFreetypeVersion() {
    LibraryVersion version;
    version.name = "FreeType";
    
#ifdef DRAW_HAS_FREETYPE
    version.major = FREETYPE_MAJOR;
    version.minor = FREETYPE_MINOR;
    version.patch = FREETYPE_PATCH;
#endif
    
    return version;
}

LibraryVersion LibraryCompatibility::DetectZlibVersion() {
    LibraryVersion version;
    version.name = "zlib";
    
#ifdef DRAW_HAS_ZLIB
    version.major = ZLIB_VER_MAJOR;
    version.minor = ZLIB_VER_MINOR;
#endif
    
    return version;
}

LibraryVersion LibraryCompatibility::DetectLibPngVersion() {
    LibraryVersion version;
    version.name = "libpng";
    
#ifdef DRAW_HAS_LIBPNG
    version.major = PNG_LIBPNG_VER_MAJOR;
    version.minor = PNG_LIBPNG_VER_MINOR;
    version.patch = PNG_LIBPNG_VER_RELEASE;
#endif
    
    return version;
}

LibraryVersion LibraryCompatibility::DetectLibJpegVersion() {
    LibraryVersion version;
    version.name = "libjpeg";
    
#ifdef DRAW_HAS_LIBJPEG
    version.major = JPEG_LIB_VERSION_MAJOR;
    version.minor = JPEG_LIB_VERSION_MINOR;
#endif
    
    return version;
}

}  
}  
