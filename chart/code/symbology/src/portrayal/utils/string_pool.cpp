#include "ogc/portrayal/utils/string_pool.h"

namespace ogc {
namespace portrayal {
namespace utils {

const std::string& StringPool::Intern(const std::string& str) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = pool_.find(str);
    if (it != pool_.end()) {
        return it->second;
    }
    auto result = pool_.emplace(str, str);
    return result.first->second;
}

void StringPool::Clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    pool_.clear();
}

size_t StringPool::Size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return pool_.size();
}

size_t StringPool::GetMemoryUsage() const {
    std::lock_guard<std::mutex> lock(mutex_);
    size_t total = 0;
    for (const auto& pair : pool_) {
        total += pair.first.capacity() + pair.second.capacity();
    }
    return total;
}

} // namespace utils
} // namespace portrayal
} // namespace ogc
