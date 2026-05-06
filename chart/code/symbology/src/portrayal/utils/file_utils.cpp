#include "ogc/portrayal/utils/file_utils.h"
#include <sys/stat.h>

namespace ogc {
namespace portrayal {
namespace utils {

bool FileUtils::Exists(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}

bool FileUtils::IsFile(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return (buffer.st_mode & S_IFREG) != 0;
}

bool FileUtils::IsDirectory(const std::string& path) {
    struct stat buffer;
    if (stat(path.c_str(), &buffer) != 0) {
        return false;
    }
    return (buffer.st_mode & S_IFDIR) != 0;
}

std::string FileUtils::GetDirectory(const std::string& filePath) {
    size_t pos = filePath.find_last_of("/\\");
    if (pos == std::string::npos) {
        return "";
    }
    return filePath.substr(0, pos);
}

std::string FileUtils::GetFileName(const std::string& filePath) {
    size_t pos = filePath.find_last_of("/\\");
    if (pos == std::string::npos) {
        return filePath;
    }
    return filePath.substr(pos + 1);
}

std::string FileUtils::GetFileExtension(const std::string& filePath) {
    std::string fileName = GetFileName(filePath);
    size_t pos = fileName.find_last_of('.');
    if (pos == std::string::npos || pos == 0) {
        return "";
    }
    return fileName.substr(pos + 1);
}

std::string FileUtils::CombinePath(const std::string& dir,
                                   const std::string& file) {
    if (dir.empty()) {
        return file;
    }
    if (file.empty()) {
        return dir;
    }
    char lastChar = dir[dir.size() - 1];
    if (lastChar == '/' || lastChar == '\\') {
        return dir + file;
    }
    return dir + "/" + file;
}

} // namespace utils
} // namespace portrayal
} // namespace ogc
