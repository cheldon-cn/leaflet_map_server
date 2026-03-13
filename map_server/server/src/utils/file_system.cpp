#include "file_system.h"

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

namespace cycle {
namespace utils {

bool exists(const std::string& path) {
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(path.c_str());
    return attributes != INVALID_FILE_ATTRIBUTES;
#else
    return access(path.c_str(), F_OK) == 0;
#endif
}

bool remove(const std::string& path) {
#ifdef _WIN32
    if (exists(path)) {
        DWORD attributes = GetFileAttributesA(path.c_str());
        if (attributes != INVALID_FILE_ATTRIBUTES) {
            if (attributes & FILE_ATTRIBUTE_DIRECTORY) {
                return RemoveDirectoryA(path.c_str()) != 0;
            }
        }
        return DeleteFileA(path.c_str()) != 0;
    }
    return false;
#else
    return unlink(path.c_str()) == 0;
#endif
}

static bool remove_directory_recursive(const std::string& path) {
#ifdef _WIN32
    bool success = true;
    WIN32_FIND_DATAA find_data;
    std::string search_path = path + "/*";
    HANDLE handle = FindFirstFileA(search_path.c_str(), &find_data);
    
    if (handle == INVALID_HANDLE_VALUE) {
        return RemoveDirectoryA(path.c_str()) != 0;
    }
    
    do {
        if (strcmp(find_data.cFileName, ".") != 0 && strcmp(find_data.cFileName, "..") != 0) {
            std::string full_path = path + "/" + find_data.cFileName;
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                success &= remove_directory_recursive(full_path);
            } else {
                success &= DeleteFileA(full_path.c_str()) != 0;
            }
        }
    } while (FindNextFileA(handle, &find_data));
    
    FindClose(handle);
    success &= RemoveDirectoryA(path.c_str()) != 0;
    return success;
#else
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        return rmdir(path.c_str()) == 0;
    }
    
    bool success = true;
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
            std::string full_path = path + "/" + entry->d_name;
            struct stat st;
            if (stat(full_path.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    success &= remove_directory_recursive(full_path);
                } else {
                    success &= unlink(full_path.c_str()) == 0;
                }
            }
        }
    }
    closedir(dir);
    success &= rmdir(path.c_str()) == 0;
    return success;
#endif
}

bool remove_all(const std::string& path) {
    if (!exists(path)) {
        return true;
    }
    
    if (!is_directory(path)) {
        return remove(path);
    }
    
    return remove_directory_recursive(path);
}

bool create_directories(const std::string& path) {
#ifdef _WIN32
    if (exists(path)) {
        return is_directory(path);
    }
    
    size_t pos = 0;
    std::string current_path;
    
    if (path.find(":") != std::string::npos) {
        size_t colon_pos = path.find(":") + 2;
        current_path = path.substr(0, colon_pos);
        pos = colon_pos;
    }
    
    while (pos < path.length()) {
        size_t next_pos = path.find('/', pos);
        if (next_pos == std::string::npos) {
            next_pos = path.find('\\', pos);
        }
        if (next_pos == std::string::npos) {
            next_pos = path.length();
        }
        
        current_path += path.substr(pos, next_pos - pos + 1);
        pos = next_pos + 1;
        
        if (!exists(current_path)) {
            if (current_path.length() > 3) {
                _mkdir(current_path.c_str());
            }
        }
    }
    return true;
#else
    if (exists(path)) {
        return is_directory(path);
    }
    
    std::string current_path;
    if (path[0] == '/') {
        current_path = "/";
    }
    
    size_t pos = current_path.length();
    while (pos < path.length()) {
        size_t next_pos = path.find('/', pos);
        if (next_pos == std::string::npos) {
            next_pos = path.length();
        }
        
        current_path += path.substr(pos, next_pos - pos + 1);
        pos = next_pos + 1;
        
        if (!exists(current_path)) {
            mkdir(current_path.c_str(), 0755);
        }
    }
    return true;
#endif
}

bool is_directory(const std::string& path) {
#ifdef _WIN32
    DWORD attributes = GetFileAttributesA(path.c_str());
    if (attributes == INVALID_FILE_ATTRIBUTES) {
        return false;
    }
    return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        return false;
    }
    return S_ISDIR(st.st_mode);
#endif
}

} // namespace utils
} // namespace cycle
