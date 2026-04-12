#include "parser/incremental_parser.h"
#include "parser/error_handler.h"

#include <fstream>
#include <chrono>
#include <algorithm>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/stat.h>
#endif

namespace chart {
namespace parser {

IncrementalParser& IncrementalParser::Instance() {
    static IncrementalParser instance;
    return instance;
}

IncrementalParser::IncrementalParser() {
}

IncrementalParser::~IncrementalParser() {
}

FileChangeInfo IncrementalParser::GetFileInfo(const std::string& filePath) const {
    FileChangeInfo info;
    info.filePath = filePath;
    
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA fileAttr;
    if (GetFileAttributesExA(filePath.c_str(), GetFileExInfoStandard, &fileAttr)) {
        LARGE_INTEGER fileSize;
        fileSize.HighPart = fileAttr.nFileSizeHigh;
        fileSize.LowPart = fileAttr.nFileSizeLow;
        info.fileSize = static_cast<uint64_t>(fileSize.QuadPart);
        
        LARGE_INTEGER modTime;
        modTime.HighPart = fileAttr.ftLastWriteTime.dwHighDateTime;
        modTime.LowPart = fileAttr.ftLastWriteTime.dwLowDateTime;
        info.lastModifiedTime = static_cast<uint64_t>(modTime.QuadPart);
    }
#else
    struct stat st;
    if (stat(filePath.c_str(), &st) == 0) {
        info.fileSize = static_cast<uint64_t>(st.st_size);
        info.lastModifiedTime = static_cast<uint64_t>(st.st_mtime);
    }
#endif
    
    info.fileHash = ComputeFileHash(filePath);
    
    return info;
}

std::string IncrementalParser::ComputeFileHash(const std::string& filePath) const {
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        return "";
    }
    
    std::stringstream ss;
    ss << filePath << "_" << std::hex;
    
    file.seekg(0, std::ios::end);
    ss << file.tellg();
    
    return ss.str();
}

IncrementalParseResult IncrementalParser::ParseIncremental(
    const std::string& filePath,
    const ParseConfig& config) {
    
    IncrementalParseResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    LOG_INFO("Starting incremental parse for: %s", filePath.c_str());
    
    FileChangeInfo currentInfo = GetFileInfo(filePath);
    
    auto stateIt = m_fileStates.find(filePath);
    bool isFirstParse = (stateIt == m_fileStates.end());
    
    if (isFirstParse) {
        LOG_DEBUG("First parse for file: %s", filePath.c_str());
        
        if (!m_parser) {
            LOG_ERROR("No parser function set");
            return result;
        }
        
        ParseResult parseResult = m_parser(filePath, config);
        
        State state;
        state.fileInfo = currentInfo;
        state.lastParseTime = std::chrono::system_clock::now();
        
        for (const auto& feature : parseResult.features) {
            state.features[feature.id] = feature;
        }
        
        m_fileStates[filePath] = state;
        
        result.addedFeatures = parseResult;
        for (const auto& feature : parseResult.features) {
            result.addedFeatureIds.push_back(feature.id);
        }
        result.hasChanges = true;
        
    } else {
        State& existingState = stateIt->second;
        
        if (!existingState.fileInfo.HasChanged(currentInfo)) {
            LOG_DEBUG("File unchanged: %s", filePath.c_str());
            result.unchangedFeatures.features.reserve(existingState.features.size());
            for (const auto& pair : existingState.features) {
                result.unchangedFeatures.features.push_back(pair.second);
            }
            result.hasChanges = false;
            
        } else {
            LOG_DEBUG("File changed, re-parsing: %s", filePath.c_str());
            
            if (!m_parser) {
                LOG_ERROR("No parser function set");
                return result;
            }
            
            ParseResult parseResult = m_parser(filePath, config);
            
            std::map<std::string, Feature> newFeatures;
            for (const auto& feature : parseResult.features) {
                newFeatures[feature.id] = feature;
            }
            
            CompareFeatures(existingState.features, newFeatures, result);
            
            existingState.fileInfo = currentInfo;
            existingState.features = newFeatures;
            existingState.lastParseTime = std::chrono::system_clock::now();
            
            result.hasChanges = true;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.parseTimeMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    if (m_changeCallback && result.hasChanges) {
        m_changeCallback(filePath, result);
    }
    
    LOG_INFO("Incremental parse completed. Added=%zu, Modified=%zu, Deleted=%zu",
             result.addedFeatureIds.size(),
             result.modifiedFeatureIds.size(),
             result.deletedFeatureIds.size());
    
    return result;
}

bool IncrementalParser::HasFileChanged(const std::string& filePath) const {
    auto it = m_fileStates.find(filePath);
    if (it == m_fileStates.end()) {
        return true;
    }
    
    FileChangeInfo currentInfo = GetFileInfo(filePath);
    return it->second.fileInfo.HasChanged(currentInfo);
}

void IncrementalParser::MarkFileProcessed(const std::string& filePath) {
    if (m_fileStates.find(filePath) == m_fileStates.end()) {
        State state;
        state.fileInfo = GetFileInfo(filePath);
        state.lastParseTime = std::chrono::system_clock::now();
        m_fileStates[filePath] = state;
    }
}

void IncrementalParser::ClearFileState(const std::string& filePath) {
    m_fileStates.erase(filePath);
}

void IncrementalParser::ClearAllStates() {
    m_fileStates.clear();
}

const IncrementalParser::State* IncrementalParser::GetFileState(const std::string& filePath) const {
    auto it = m_fileStates.find(filePath);
    if (it != m_fileStates.end()) {
        return &it->second;
    }
    return nullptr;
}

void IncrementalParser::CompareFeatures(
    const std::map<std::string, Feature>& oldFeatures,
    const std::map<std::string, Feature>& newFeatures,
    IncrementalParseResult& result) {
    
    for (const auto& pair : newFeatures) {
        auto oldIt = oldFeatures.find(pair.first);
        
        if (oldIt == oldFeatures.end()) {
            result.addedFeatures.features.push_back(pair.second);
            result.addedFeatureIds.push_back(pair.first);
        } else if (!FeaturesEqual(oldIt->second, pair.second)) {
            result.modifiedFeatures.features.push_back(pair.second);
            result.modifiedFeatureIds.push_back(pair.first);
        } else {
            result.unchangedFeatures.features.push_back(pair.second);
        }
    }
    
    for (const auto& pair : oldFeatures) {
        if (newFeatures.find(pair.first) == newFeatures.end()) {
            result.deletedFeatures.features.push_back(pair.second);
            result.deletedFeatureIds.push_back(pair.first);
        }
    }
}

bool IncrementalParser::FeaturesEqual(const Feature& f1, const Feature& f2) const {
    if (f1.id != f2.id || f1.type != f2.type || f1.className != f2.className) {
        return false;
    }
    
    if (f1.geometry.type != f2.geometry.type) {
        return false;
    }
    
    if (f1.geometry.points.size() != f2.geometry.points.size()) {
        return false;
    }
    
    for (size_t i = 0; i < f1.geometry.points.size(); ++i) {
        if (std::abs(f1.geometry.points[i].x - f2.geometry.points[i].x) > 1e-9 ||
            std::abs(f1.geometry.points[i].y - f2.geometry.points[i].y) > 1e-9) {
            return false;
        }
    }
    
    return true;
}

IncrementalParseSession::IncrementalParseSession()
    : m_autoMerge(true) {
}

IncrementalParseSession::~IncrementalParseSession() {
}

void IncrementalParseSession::AddFile(const std::string& filePath) {
    if (std::find(m_files.begin(), m_files.end(), filePath) == m_files.end()) {
        m_files.push_back(filePath);
    }
}

void IncrementalParseSession::RemoveFile(const std::string& filePath) {
    m_files.erase(std::remove(m_files.begin(), m_files.end(), filePath), m_files.end());
    IncrementalParser::Instance().ClearFileState(filePath);
}

IncrementalParseResult IncrementalParseSession::UpdateFile(
    const std::string& filePath,
    const ParseConfig& config) {
    
    IncrementalParseResult result = IncrementalParser::Instance().ParseIncremental(filePath, config);
    
    if (m_autoMerge) {
        MergeResult(result);
    }
    
    return result;
}

IncrementalParseResult IncrementalParseSession::UpdateFile(const std::string& filePath) {
    ParseConfig config;
    return UpdateFile(filePath, config);
}

void IncrementalParseSession::UpdateAll(const ParseConfig& config) {
    for (const auto& file : m_files) {
        UpdateFile(file, config);
    }
}

void IncrementalParseSession::UpdateAll() {
    ParseConfig config;
    UpdateAll(config);
}

std::vector<std::string> IncrementalParseSession::GetChangedFiles() const {
    std::vector<std::string> changed;
    for (const auto& file : m_files) {
        if (IncrementalParser::Instance().HasFileChanged(file)) {
            changed.push_back(file);
        }
    }
    return changed;
}

std::vector<std::string> IncrementalParseSession::GetAllFiles() const {
    return m_files;
}

void IncrementalParseSession::MergeResult(const IncrementalParseResult& incrementalResult) {
    for (const auto& feature : incrementalResult.addedFeatures.features) {
        m_aggregatedResult.features.push_back(feature);
    }
    
    for (const auto& feature : incrementalResult.modifiedFeatures.features) {
        auto it = std::find_if(m_aggregatedResult.features.begin(), 
                               m_aggregatedResult.features.end(),
                               [&feature](const Feature& f) { return f.id == feature.id; });
        if (it != m_aggregatedResult.features.end()) {
            *it = feature;
        }
    }
    
    for (const auto& id : incrementalResult.deletedFeatureIds) {
        m_aggregatedResult.features.erase(
            std::remove_if(m_aggregatedResult.features.begin(),
                          m_aggregatedResult.features.end(),
                          [&id](const Feature& f) { return f.id == id; }),
            m_aggregatedResult.features.end());
    }
}

} // namespace parser
} // namespace chart
