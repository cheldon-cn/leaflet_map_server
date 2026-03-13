#ifndef CYCLE_UTILS_FILE_SYSTEM_H
#define CYCLE_UTILS_FILE_SYSTEM_H

#include <string>

namespace cycle {
namespace utils {

// 检查文件或目录是否存在
bool exists(const std::string& path);

// 删除文件
bool remove(const std::string& path);

// 删除目录及其所有内容
bool remove_all(const std::string& path);

// 创建目录（包括父目录）
bool create_directories(const std::string& path);

// 检查路径是否为目录
bool is_directory(const std::string& path);

} // namespace utils
} // namespace cycle

#endif // CYCLE_UTILS_FILE_SYSTEM_H
