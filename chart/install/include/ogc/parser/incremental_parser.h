#ifndef INCREMENTAL_PARSER_H
#define INCREMENTAL_PARSER_H

#include "parse_result.h"
#include "parse_config.h"
#include "parse_cache.h"
#include <string>
#include <vector>
#include <functional>
#include <map>

namespace chart {
namespace parser {

struct FileChangeInfo {
    std::string filePath;
    uint64_t lastModifiedTime;
    uint64_t fileSize;
    std::string fileHash;
    
    bool HasChanged(const FileChangeInfo& other) const {
        return lastModifiedTime != other.lastModifiedTime ||
               fileSize != other.fileSize ||
               fileHash != other.fileHash;
    }
};

struct IncrementalParseResult {
    ParseResult addedFeatures;
    ParseResult modifiedFeatures;
    ParseResult deletedFeatures;
    ParseResult unchangedFeatures;
    
    std::vector<std::string> addedFeatureIds;
    std::vector<std::string> modifiedFeatureIds;
    std::vector<std::string> deletedFeatureIds;
    
    bool hasChanges;
    double parseTimeMs;
    
    IncrementalParseResult() : hasChanges(false), parseTimeMs(0) {}
};

class IncrementalParser {
public:
    using ParserFunc = std::function<ParseResult(const std::string&, const ParseConfig&)>;
    
    static IncrementalParser& Instance();
    
    void SetParser(ParserFunc parser) { m_parser = parser; }
    
    IncrementalParseResult ParseIncremental(
        const std::string& filePath,
        const ParseConfig& config = ParseConfig()
    );
    
    bool HasFileChanged(const std::string& filePath) const;
    
    void MarkFileProcessed(const std::string& filePath);
    void ClearFileState(const std::string& filePath);
    void ClearAllStates();
    
    void SetChangeCallback(std::function<void(const std::string&, const IncrementalParseResult&)> callback) {
        m_changeCallback = callback;
    }
    
    struct State {
        FileChangeInfo fileInfo;
        std::map<std::string, Feature> features;
        std::chrono::system_clock::time_point lastParseTime;
    };
    
    const State* GetFileState(const std::string& filePath) const;
    
private:
    IncrementalParser();
    ~IncrementalParser();
    
    IncrementalParser(const IncrementalParser&) = delete;
    IncrementalParser& operator=(const IncrementalParser&) = delete;
    
    FileChangeInfo GetFileInfo(const std::string& filePath) const;
    std::string ComputeFileHash(const std::string& filePath) const;
    void CompareFeatures(
        const std::map<std::string, Feature>& oldFeatures,
        const std::map<std::string, Feature>& newFeatures,
        IncrementalParseResult& result
    );
    
    bool FeaturesEqual(const Feature& f1, const Feature& f2) const;
    
    ParserFunc m_parser;
    std::map<std::string, State> m_fileStates;
    std::function<void(const std::string&, const IncrementalParseResult&)> m_changeCallback;
};

class IncrementalParseSession {
public:
    IncrementalParseSession();
    ~IncrementalParseSession();
    
    void AddFile(const std::string& filePath);
    void RemoveFile(const std::string& filePath);
    
    IncrementalParseResult UpdateFile(const std::string& filePath, const ParseConfig& config);
    IncrementalParseResult UpdateFile(const std::string& filePath);
    
    void UpdateAll(const ParseConfig& config);
    void UpdateAll();
    
    std::vector<std::string> GetChangedFiles() const;
    std::vector<std::string> GetAllFiles() const;
    
    const ParseResult& GetAggregatedResult() const { return m_aggregatedResult; }
    
    void SetAutoMerge(bool enable) { m_autoMerge = enable; }
    
private:
    void MergeResult(const IncrementalParseResult& incrementalResult);
    
    std::vector<std::string> m_files;
    ParseResult m_aggregatedResult;
    bool m_autoMerge;
};

} // namespace parser
} // namespace chart

#endif // INCREMENTAL_PARSER_H
