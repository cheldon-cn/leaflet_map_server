#ifndef OGC_LAYER_TESTS_TEST_MOCKS_H
#define OGC_LAYER_TESTS_TEST_MOCKS_H

#include "ogc/layer/connection_pool.h"
#include "ogc/layer/datasource.h"
#include "ogc/layer/driver.h"
#include "ogc/layer/layer_observer.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/layer/vector_layer.h"
#include "ogc/layer/raster_layer.h"
#include "ogc/layer/raster_dataset.h"
#include "ogc/layer/wfs_layer.h"
#include "ogc/layer/shapefile_layer.h"
#include "ogc/layer/geojson_layer.h"
#include "ogc/layer/geopackage_layer.h"
#include "ogc/layer/postgis_layer.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/factory.h"

#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <map>

namespace ogc {
namespace test {

class MockDbConnection : public CNDbConnection {
public:
    MockDbConnection() : connected_(false), handle_(reinterpret_cast<void*>(0x12345678)) {}
    
    bool IsConnected() const override { return connected_; }
    
    bool Execute(const std::string& sql) override {
        if (!connected_) return false;
        last_sql_ = sql;
        return true;
    }
    
    std::unique_ptr<CNDataSource> GetDataSource() override {
        return nullptr;
    }
    
    void* GetHandle() override { return handle_; }
    
    void Connect() { connected_ = true; }
    void Disconnect() { connected_ = false; }
    
    const std::string& GetLastSql() const { return last_sql_; }

private:
    bool connected_;
    std::string last_sql_;
    void* handle_;
};

class MockConnectionPool : public CNConnectionPool {
public:
    MockConnectionPool() : size_(0), idle_count_(0), active_count_(0), closed_(false) {}
    
    std::shared_ptr<CNDbConnection> Acquire(int timeout_ms) override {
        (void)timeout_ms;
        if (closed_) return nullptr;
        
        std::lock_guard<std::mutex> lock(mutex_);
        if (connections_.empty()) {
            auto conn = std::make_shared<MockDbConnection>();
            conn->Connect();
            return conn;
        }
        
        auto conn = connections_.front();
        connections_.pop();
        idle_count_--;
        active_count_++;
        return conn;
    }
    
    void Release(std::shared_ptr<CNDbConnection> conn) override {
        if (!conn) return;
        
        std::lock_guard<std::mutex> lock(mutex_);
        connections_.push(conn);
        idle_count_++;
        active_count_--;
    }
    
    size_t GetSize() const override { return size_; }
    size_t GetIdleCount() const override { return idle_count_; }
    size_t GetActiveCount() const override { return active_count_; }
    
    size_t HealthCheck() override {
        return idle_count_;
    }
    
    void Clear() override {
        std::lock_guard<std::mutex> lock(mutex_);
        while (!connections_.empty()) {
            connections_.pop();
        }
        idle_count_ = 0;
    }
    
    void Close() override {
        closed_ = true;
        Clear();
    }
    
    void SetSize(size_t size) { size_ = size; }

private:
    size_t size_;
    size_t idle_count_;
    size_t active_count_;
    bool closed_;
    std::mutex mutex_;
    std::queue<std::shared_ptr<CNDbConnection>> connections_;
};

class TestDriver : public CNDriver {
public:
    TestDriver() : name_("TestDriver"), description_("Test Driver for Unit Tests") {}
    
    explicit TestDriver(const std::string& name) 
        : name_(name), description_("Test Driver") {}
    
    const char* GetName() const override { return name_.c_str(); }
    const char* GetDescription() const override { return description_.c_str(); }
    std::vector<std::string> GetExtensions() const override { return {".test"}; }
    std::vector<std::string> GetMimeTypes() const override { return {"application/x-test"}; }
    
    bool CanOpen(const std::string& path) const override {
        return path.find(".test") != std::string::npos;
    }
    
    bool CanCreate(const std::string& path) const override {
        return path.find(".test") != std::string::npos;
    }
    
    bool SupportsUpdate() const override { return true; }
    bool SupportsMultipleLayers() const override { return true; }
    bool SupportsTransactions() const override { return false; }
    
    std::unique_ptr<CNDataSource> Open(const std::string& path, bool update) override {
        (void)update;
        if (!CanOpen(path)) return nullptr;
        return std::make_unique<TestDataSource>("test_ds", path);
    }
    
    std::unique_ptr<CNDataSource> Create(const std::string& path,
                                          const std::map<std::string, std::string>& options) override {
        (void)options;
        if (!CanCreate(path)) return nullptr;
        return std::make_unique<TestDataSource>("test_ds", path);
    }

private:
    std::string name_;
    std::string description_;
};

class TestDataSource : public CNDataSource {
public:
    TestDataSource(const std::string& name, const std::string& path)
        : name_(name), path_(path), read_only_(false) {}
    
    const std::string& GetName() const override { return name_; }
    std::string GetPath() const override { return path_; }
    bool IsReadOnly() const override { return read_only_; }
    const char* GetDriverName() const override { return "TestDriver"; }
    
    int GetLayerCount() const override {
        return static_cast<int>(layers_.size());
    }
    
    CNLayer* GetLayer(int index) override {
        if (index >= 0 && index < static_cast<int>(layers_.size())) {
            return layers_[index].get();
        }
        return nullptr;
    }
    
    CNLayer* GetLayerByName(const std::string& name) override {
        for (auto& layer : layers_) {
            if (layer && layer->GetName() == name) {
                return layer.get();
            }
        }
        return nullptr;
    }
    
    CNLayer* CreateLayer(const std::string& name, GeomType geom_type, void* srs) override {
        (void)srs;
        auto layer = std::make_unique<CNMemoryLayer>(name, geom_type);
        layers_.push_back(std::move(layer));
        return layers_.back().get();
    }
    
    CNStatus DeleteLayer(const std::string& name) override {
        for (auto it = layers_.begin(); it != layers_.end(); ++it) {
            if ((*it)->GetName() == name) {
                layers_.erase(it);
                return CNStatus::kSuccess;
            }
        }
        return CNStatus::kFeatureNotFound;
    }
    
    void SetReadOnly(bool read_only) { read_only_ = read_only; }
    
private:
    std::string name_;
    std::string path_;
    bool read_only_;
    std::vector<std::unique_ptr<CNMemoryLayer>> layers_;
};

class TestObserver : public CNLayerObserver {
public:
    void OnLayerChanged(CNLayer* layer, const CNLayerEventArgs& args) override {
        (void)layer;
        last_event_ = args.type;
        event_count_++;
    }
    
    bool OnLayerChanging(CNLayer* layer, const CNLayerEventArgs& args) override {
        (void)layer;
        (void)args;
        return allow_change_;
    }
    
    void Reset() {
        event_count_ = 0;
        last_event_ = CNLayerEventType::kCleared;
    }
    
    void SetAllowChange(bool allow) { allow_change_ = allow; }
    
    int GetEventCount() const { return event_count_; }
    CNLayerEventType GetLastEvent() const { return last_event_; }
    
private:
    int event_count_ = 0;
    CNLayerEventType last_event_ = CNLayerEventType::kCleared;
    bool allow_change_ = true;
};

class TestObservableLayer : public CNObservableLayer {
public:
    TestObservableLayer(const std::string& name, GeomType geom_type)
        : name_(name), geom_type_(geom_type) {
        feature_defn_ = std::make_shared<CNFeatureDefn>(name.c_str());
    }
    
    const std::string& GetName() const override { return name_; }
    CNLayerType GetLayerType() const override { return CNLayerType::kMemory; }
    CNFeatureDefn* GetFeatureDefn() override { return feature_defn_.get(); }
    const CNFeatureDefn* GetFeatureDefn() const override { return feature_defn_.get(); }
    GeomType GetGeomType() const override { return geom_type_; }
    void* GetSpatialRef() override { return nullptr; }
    const void* GetSpatialRef() const override { return nullptr; }
    
    CNStatus GetExtent(Envelope& extent, bool force) const override {
        (void)force;
        extent = extent_;
        return CNStatus::kSuccess;
    }
    
    int64_t GetFeatureCount(bool force) const override {
        (void)force;
        return static_cast<int64_t>(features_.size());
    }
    
    void ResetReading() override { read_cursor_ = 0; }
    
    std::unique_ptr<CNFeature> GetNextFeature() override {
        if (read_cursor_ >= features_.size()) return nullptr;
        return std::unique_ptr<CNFeature>(features_[read_cursor_++]->Clone());
    }
    
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override {
        for (auto& f : features_) {
            if (f->GetFID() == fid) return std::unique_ptr<CNFeature>(f->Clone());
        }
        return nullptr;
    }
    
    void SetSpatialFilter(const CNGeometry* geometry) override { (void)geometry; }
    const CNGeometry* GetSpatialFilter() const override { return nullptr; }
    
    CNStatus SetAttributeFilter(const std::string& query) override {
        (void)query;
        return CNStatus::kNotSupported;
    }
    
    bool TestCapability(CNLayerCapability capability) const override {
        (void)capability;
        return false;
    }
    
    std::unique_ptr<CNLayer> Clone() const override { return nullptr; }
    
    CNStatus CreateFeature(CNFeature* feature) override {
        CNLayerEventArgs args;
        args.type = CNLayerEventType::kFeatureAdded;
        args.fid = feature->GetFID();
        args.feature = feature;
        
        if (!NotifyChanging(args)) {
            return CNStatus::kError;
        }
        
        features_.push_back(std::unique_ptr<CNFeature>(feature->Clone()));
        NotifyObservers(args);
        return CNStatus::kSuccess;
    }
    
private:
    std::string name_;
    GeomType geom_type_;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    std::vector<std::unique_ptr<CNFeature>> features_;
    Envelope extent_;
    size_t read_cursor_ = 0;
};

class TestVectorLayer : public CNVectorLayer {
public:
    TestVectorLayer(const std::string& name, GeomType geom_type)
        : name_(name), geom_type_(geom_type) {
        feature_defn_ = std::make_shared<CNFeatureDefn>(name);
    }
    
    const std::string& GetName() const override { return name_; }
    CNFeatureDefn* GetFeatureDefn() override { return feature_defn_.get(); }
    const CNFeatureDefn* GetFeatureDefn() const override { return feature_defn_.get(); }
    GeomType GetGeomType() const override { return geom_type_; }
    void* GetSpatialRef() override { return nullptr; }
    const void* GetSpatialRef() const override { return nullptr; }
    
    CNStatus GetExtent(Envelope& extent, bool force) const override {
        (void)force;
        extent = extent_;
        return CNStatus::kSuccess;
    }
    
    int64_t GetFeatureCount(bool force) const override {
        (void)force;
        return static_cast<int64_t>(features_.size());
    }
    
    void ResetReading() override { read_cursor_ = 0; }
    
    std::unique_ptr<CNFeature> GetNextFeature() override {
        if (read_cursor_ >= features_.size()) return nullptr;
        return features_[read_cursor_++]->Clone();
    }
    
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override {
        for (auto& f : features_) {
            if (f->GetFID() == fid) return f->Clone();
        }
        return nullptr;
    }
    
    CNStatus CreateFeature(CNFeature* feature) override {
        features_.push_back(feature->Clone());
        return CNStatus::kSuccess;
    }
    
    std::string GetDataSourcePath() const override { return ""; }
    std::string GetFormatName() const override { return "TestVector"; }
    bool IsReadOnly() const override { return false; }
    
    void AddFeature(std::unique_ptr<CNFeature> feature) {
        features_.push_back(std::move(feature));
    }
    
private:
    std::string name_;
    GeomType geom_type_;
    std::shared_ptr<CNFeatureDefn> feature_defn_;
    std::vector<std::unique_ptr<CNFeature>> features_;
    Envelope extent_;
    size_t read_cursor_ = 0;
};

class TestRasterBand : public CNRasterBand {
public:
    TestRasterBand(int width, int height, CNDataType data_type)
        : width_(width), height_(height), data_type_(data_type) {}
    
    int GetWidth() const override { return width_; }
    int GetHeight() const override { return height_; }
    CNDataType GetDataType() const override { return data_type_; }
    
    int GetNoDataValue(double& value) const override {
        value = no_data_value_;
        return has_no_data_ ? 1 : 0;
    }
    
    double GetNoDataValue() const override { return no_data_value_; }
    
    CNStatus ReadRaster(int x_offset, int y_offset, int width, int height,
                        void* buffer, CNDataType buffer_type) override {
        (void)x_offset; (void)y_offset; (void)width; (void)height;
        (void)buffer; (void)buffer_type;
        return CNStatus::kNotSupported;
    }
    
    CNStatus WriteRaster(int x_offset, int y_offset, int width, int height,
                         const void* buffer, CNDataType buffer_type) override {
        (void)x_offset; (void)y_offset; (void)width; (void)height;
        (void)buffer; (void)buffer_type;
        return CNStatus::kNotSupported;
    }
    
    double GetMinimum() const override { return 0.0; }
    double GetMaximum() const override { return 255.0; }
    double GetOffset() const override { return 0.0; }
    double GetScale() const override { return 1.0; }
    std::string GetDescription() const override { return "TestBand"; }
    
private:
    int width_;
    int height_;
    CNDataType data_type_;
    double no_data_value_ = -9999.0;
    bool has_no_data_ = true;
};

class TestRasterLayer : public CNRasterLayer {
public:
    TestRasterLayer(const std::string& name, int width, int height, int band_count)
        : name_(name), width_(width), height_(height), band_count_(band_count) {
        for (int i = 0; i < band_count; ++i) {
            bands_.push_back(std::make_unique<TestRasterBand>(width, height, CNDataType::kByte));
        }
    }
    
    const std::string& GetName() const override { return name_; }
    CNFeatureDefn* GetFeatureDefn() override { return nullptr; }
    const CNFeatureDefn* GetFeatureDefn() const override { return nullptr; }
    GeomType GetGeomType() const override { return GeomType::kUnknown; }
    
    int GetWidth() const override { return width_; }
    int GetHeight() const override { return height_; }
    int GetBandCount() const override { return band_count_; }
    
    double GetPixelWidth() const override { return 1.0; }
    double GetPixelHeight() const override { return 1.0; }
    CNDataType GetDataType() const override { return CNDataType::kByte; }
    
    void GetGeoTransform(double* transform) const override {
        for (int i = 0; i < 6; ++i) transform[i] = geo_transform_[i];
    }
    
    CNRasterBand* GetBand(int band_index) override {
        if (band_index >= 0 && band_index < band_count_) {
            return bands_[band_index].get();
        }
        return nullptr;
    }
    
    CNStatus ReadRaster(int x_offset, int y_offset, int width, int height,
                        int band_count, CNDataType buffer_type, void* buffer) override {
        (void)x_offset; (void)y_offset; (void)width; (void)height;
        (void)band_count; (void)buffer_type; (void)buffer;
        return CNStatus::kNotSupported;
    }
    
    CNStatus WriteRaster(int x_offset, int y_offset, int width, int height,
                         int band_count, CNDataType buffer_type, const void* buffer) override {
        (void)x_offset; (void)y_offset; (void)width; (void)height;
        (void)band_count; (void)buffer_type; (void)buffer;
        return CNStatus::kNotSupported;
    }
    
    int64_t GetFeatureCount(bool force) const override {
        (void)force;
        return 0;
    }
    
    void ResetReading() override {}
    std::unique_ptr<CNFeature> GetNextFeature() override { return nullptr; }
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override {
        (void)fid;
        return nullptr;
    }
    
    CNStatus GetExtent(Envelope& extent, bool force) const override {
        (void)force;
        extent = extent_;
        return CNStatus::kSuccess;
    }
    
private:
    std::string name_;
    int width_;
    int height_;
    int band_count_;
    std::vector<std::unique_ptr<CNRasterBand>> bands_;
    double geo_transform_[6] = {0, 1, 0, 0, 0, -1};
    Envelope extent_;
};

}
}

#endif
