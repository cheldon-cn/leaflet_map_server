#pragma once

#include "ogc/navi/track/track.h"
#include "ogc/navi/export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace navi {

class OGC_NAVI_API TrackManager {
public:
    static TrackManager& Instance();
    
    bool Initialize();
    void Shutdown();
    
    Track* CreateTrack(const std::string& name);
    Track* LoadTrack(const std::string& track_id);
    bool SaveTrack(Track* track);
    bool DeleteTrack(const std::string& track_id);
    
    int GetTrackCount() const;
    std::vector<Track*> GetAllTracks();
    std::vector<Track*> GetTracksByType(TrackType type);
    std::vector<Track*> GetTracksInTimeRange(double start_time, double end_time);
    
    Track* GetTrack(const std::string& track_id) const;
    
    bool ExportTrack(Track* track, const std::string& file_path, const std::string& format);
    Track* ImportTrack(const std::string& file_path, const std::string& format);
    
    bool ExportToGPX(Track* track, const std::string& file_path);
    bool ExportToKML(Track* track, const std::string& file_path);
    Track* ImportFromGPX(const std::string& file_path);
    Track* ImportFromKML(const std::string& file_path);
    
private:
    TrackManager();
    ~TrackManager();
    TrackManager(const TrackManager&) = delete;
    TrackManager& operator=(const TrackManager&) = delete;
    
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}
}
