#include "ogc/navi/track/track_manager.h"
#include "ogc/navi/track/track_point.h"
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

namespace ogc {
namespace navi {

struct TrackManager::Impl {
    std::map<std::string, Track*> tracks;
    int track_counter = 0;
    bool initialized = false;
};

TrackManager::TrackManager()
    : impl_(new Impl())
{
}

TrackManager::~TrackManager() {
    Shutdown();
}

TrackManager& TrackManager::Instance() {
    static TrackManager instance;
    return instance;
}

bool TrackManager::Initialize() {
    if (impl_->initialized) {
        return true;
    }
    
    impl_->initialized = true;
    return true;
}

void TrackManager::Shutdown() {
    for (auto& pair : impl_->tracks) {
        if (pair.second) {
            pair.second->ReleaseReference();
        }
    }
    impl_->tracks.clear();
    impl_->initialized = false;
}

Track* TrackManager::CreateTrack(const std::string& name) {
    Track* track = Track::Create();
    track->SetName(name);
    
    impl_->track_counter++;
    std::string id = "track-" + std::to_string(impl_->track_counter);
    track->SetId(id);
    
    impl_->tracks[id] = track;
    return track;
}

Track* TrackManager::LoadTrack(const std::string& track_id) {
    auto it = impl_->tracks.find(track_id);
    if (it != impl_->tracks.end()) {
        return it->second;
    }
    return nullptr;
}

bool TrackManager::SaveTrack(Track* track) {
    if (!track) {
        return false;
    }
    
    const std::string& id = track->GetId();
    if (id.empty()) {
        impl_->track_counter++;
        track->SetId("track-" + std::to_string(impl_->track_counter));
    }
    
    impl_->tracks[track->GetId()] = track;
    return true;
}

bool TrackManager::DeleteTrack(const std::string& track_id) {
    auto it = impl_->tracks.find(track_id);
    if (it == impl_->tracks.end()) {
        return false;
    }
    
    if (it->second) {
        it->second->ReleaseReference();
    }
    impl_->tracks.erase(it);
    return true;
}

int TrackManager::GetTrackCount() const {
    return static_cast<int>(impl_->tracks.size());
}

std::vector<Track*> TrackManager::GetAllTracks() {
    std::vector<Track*> result;
    for (auto& pair : impl_->tracks) {
        result.push_back(pair.second);
    }
    return result;
}

std::vector<Track*> TrackManager::GetTracksByType(TrackType type) {
    std::vector<Track*> result;
    for (auto& pair : impl_->tracks) {
        if (pair.second && pair.second->GetType() == type) {
            result.push_back(pair.second);
        }
    }
    return result;
}

std::vector<Track*> TrackManager::GetTracksInTimeRange(double start_time, double end_time) {
    std::vector<Track*> result;
    for (auto& pair : impl_->tracks) {
        if (pair.second) {
            TrackData data = pair.second->ToData();
            if (!data.points.empty()) {
                double first_time = data.points.front().timestamp;
                double last_time = data.points.back().timestamp;
                
                if (first_time >= start_time && last_time <= end_time) {
                    result.push_back(pair.second);
                }
            }
        }
    }
    return result;
}

Track* TrackManager::GetTrack(const std::string& track_id) const {
    auto it = impl_->tracks.find(track_id);
    if (it != impl_->tracks.end()) {
        return it->second;
    }
    return nullptr;
}

bool TrackManager::ExportTrack(Track* track, const std::string& file_path, const std::string& format) {
    if (!track) {
        return false;
    }
    
    if (format == "gpx" || format == "GPX") {
        return ExportToGPX(track, file_path);
    } else if (format == "kml" || format == "KML") {
        return ExportToKML(track, file_path);
    }
    
    return false;
}

Track* TrackManager::ImportTrack(const std::string& file_path, const std::string& format) {
    if (format == "gpx" || format == "GPX") {
        return ImportFromGPX(file_path);
    } else if (format == "kml" || format == "KML") {
        return ImportFromKML(file_path);
    }
    
    return nullptr;
}

bool TrackManager::ExportToGPX(Track* track, const std::string& file_path) {
    if (!track) {
        return false;
    }
    
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    TrackData data = track->ToData();
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<gpx version=\"1.1\" creator=\"OGC Navigation System\">\n";
    file << "  <trk>\n";
    file << "    <name>" << data.name << "</name>\n";
    file << "    <trkseg>\n";
    
    for (const auto& pt : data.points) {
        file << "      <trkpt lat=\"" << pt.latitude << "\" lon=\"" << pt.longitude << "\">\n";
        file << "        <time>" << pt.timestamp << "</time>\n";
        if (pt.speed > 0) {
            file << "        <speed>" << pt.speed << "</speed>\n";
        }
        if (pt.course >= 0) {
            file << "        <course>" << pt.course << "</course>\n";
        }
        file << "      </trkpt>\n";
    }
    
    file << "    </trkseg>\n";
    file << "  </trk>\n";
    file << "</gpx>\n";
    
    file.close();
    return true;
}

bool TrackManager::ExportToKML(Track* track, const std::string& file_path) {
    if (!track) {
        return false;
    }
    
    std::ofstream file(file_path);
    if (!file.is_open()) {
        return false;
    }
    
    TrackData data = track->ToData();
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    file << "<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n";
    file << "  <Document>\n";
    file << "    <name>" << data.name << "</name>\n";
    file << "    <Placemark>\n";
    file << "      <name>Track</name>\n";
    file << "      <LineString>\n";
    file << "        <coordinates>\n";
    
    for (const auto& pt : data.points) {
        file << "          " << pt.longitude << "," << pt.latitude << "\n";
    }
    
    file << "        </coordinates>\n";
    file << "      </LineString>\n";
    file << "    </Placemark>\n";
    file << "  </Document>\n";
    file << "</kml>\n";
    
    file.close();
    return true;
}

Track* TrackManager::ImportFromGPX(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return nullptr;
    }
    
    TrackData data;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("<name>") != std::string::npos) {
            size_t start = line.find("<name>") + 6;
            size_t end = line.find("</name>");
            if (end != std::string::npos) {
                data.name = line.substr(start, end - start);
            }
        }
        else if (line.find("<trkpt") != std::string::npos) {
            TrackPointData pt;
            
            size_t lat_pos = line.find("lat=\"");
            if (lat_pos != std::string::npos) {
                size_t start = lat_pos + 5;
                size_t end = line.find("\"", start);
                pt.latitude = std::stod(line.substr(start, end - start));
            }
            
            size_t lon_pos = line.find("lon=\"");
            if (lon_pos != std::string::npos) {
                size_t start = lon_pos + 5;
                size_t end = line.find("\"", start);
                pt.longitude = std::stod(line.substr(start, end - start));
            }
            
            data.points.push_back(pt);
        }
    }
    
    file.close();
    
    Track* track = Track::Create(data);
    SaveTrack(track);
    return track;
}

Track* TrackManager::ImportFromKML(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        return nullptr;
    }
    
    TrackData data;
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.find("<name>") != std::string::npos) {
            size_t start = line.find("<name>") + 6;
            size_t end = line.find("</name>");
            if (end != std::string::npos) {
                data.name = line.substr(start, end - start);
            }
        }
        else if (line.find("<coordinates>") != std::string::npos) {
            while (std::getline(file, line)) {
                if (line.find("</coordinates>") != std::string::npos) {
                    break;
                }
                
                size_t comma = line.find(",");
                if (comma != std::string::npos) {
                    TrackPointData pt;
                    pt.longitude = std::stod(line.substr(0, comma));
                    
                    size_t comma2 = line.find(",", comma + 1);
                    if (comma2 != std::string::npos) {
                        pt.latitude = std::stod(line.substr(comma + 1, comma2 - comma - 1));
                    } else {
                        pt.latitude = std::stod(line.substr(comma + 1));
                    }
                    
                    data.points.push_back(pt);
                }
            }
        }
    }
    
    file.close();
    
    Track* track = Track::Create(data);
    SaveTrack(track);
    return track;
}

}
}
