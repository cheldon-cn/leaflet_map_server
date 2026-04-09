#include "gtest/gtest.h"
#include "ogc/layer/datasource.h"
#include "ogc/layer/driver.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/layer/layer_observer.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/geom/factory.h"

using namespace ogc;

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

class TestDriver : public CNDriver {
public:
    const char* GetName() const override { return "TestDriver"; }
    const char* GetDescription() const override { return "Test Driver for Unit Tests"; }
    std::vector<std::string> GetExtensions() const override { return {".test" }; }
    std::vector<std::string> GetMimeTypes() const override { return { "application/x-test" }; }
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

class CNDataSourceTest : public ::testing::Test {
protected:
    void SetUp() override {
        ds_ = std::make_unique<TestDataSource>("test_ds", "/path/to/test.test");
    }
    
    std::unique_ptr<TestDataSource> ds_;
};

TEST_F(CNDataSourceTest, GetName) {
    EXPECT_EQ(ds_->GetName(), "test_ds");
}

TEST_F(CNDataSourceTest, GetPath) {
    EXPECT_EQ(ds_->GetPath(), "/path/to/test.test");
}

TEST_F(CNDataSourceTest, IsReadOnly) {
    EXPECT_FALSE(ds_->IsReadOnly());
}

TEST_F(CNDataSourceTest, GetDriverName) {
    EXPECT_STREQ(ds_->GetDriverName(), "TestDriver");
}

TEST_F(CNDataSourceTest, GetLayerCountEmpty) {
    EXPECT_EQ(ds_->GetLayerCount(), 0);
}

TEST_F(CNDataSourceTest, CreateLayer) {
    CNLayer* layer = ds_->CreateLayer("new_layer", GeomType::kPoint, nullptr);
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ(layer->GetName(), "new_layer");
    EXPECT_EQ(ds_->GetLayerCount(), 1);
    EXPECT_EQ(ds_->GetLayer(0), layer);
}

TEST_F(CNDataSourceTest, GetLayerByIndex) {
    ds_->CreateLayer("layer1", GeomType::kPoint, nullptr);
    CNLayer* layer = ds_->GetLayer(0);
    ASSERT_NE(layer, nullptr);
}

TEST_F(CNDataSourceTest, GetLayerByName) {
    ds_->CreateLayer("test_layer", GeomType::kPoint, nullptr);
    CNLayer* layer = ds_->GetLayerByName("test_layer");
    ASSERT_NE(layer, nullptr);
    EXPECT_EQ(layer->GetName(), "test_layer");
    EXPECT_EQ(ds_->GetLayerByName("nonexistent"), nullptr);
}

TEST_F(CNDataSourceTest, DeleteLayer) {
    ds_->CreateLayer("to_delete", GeomType::kPoint, nullptr);
    CNStatus status = ds_->DeleteLayer("to_delete");
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(ds_->GetLayerCount(), 0);
}

TEST_F(CNDataSourceTest, DeleteLayerNotFound) {
    CNStatus status = ds_->DeleteLayer("nonexistent");
    EXPECT_EQ(status, CNStatus::kFeatureNotFound);
}

TEST_F(CNDataSourceTest, GetLayerInvalidIndex) {
    CNLayer* layer = ds_->GetLayer(-1);
    EXPECT_EQ(layer, nullptr);
    
    layer = ds_->GetLayer(100);
    EXPECT_EQ(layer, nullptr);
}

TEST_F(CNDataSourceTest, SetReadOnly) {
    ds_->SetReadOnly(true);
    EXPECT_TRUE(ds_->IsReadOnly());
}

TEST_F(CNDataSourceTest, StaticOpen) {
    auto ds = CNDataSource::Open("/path/to/file.test", false);
    EXPECT_EQ(ds, nullptr);
}

TEST_F(CNDataSourceTest, StaticCreate) {
    auto ds = CNDataSource::Create("/path/to/new.test", "TestDriver");
    EXPECT_EQ(ds, nullptr);
}

class CNDriverTest : public ::testing::Test {
protected:
    void SetUp() override {
        driver_ = std::make_unique<TestDriver>();
    }
    
    std::unique_ptr<TestDriver> driver_;
};

TEST_F(CNDriverTest, GetName) {
    EXPECT_STREQ(driver_->GetName(), "TestDriver");
}

TEST_F(CNDriverTest, GetDescription) {
    EXPECT_STREQ(driver_->GetDescription(), "Test Driver for Unit Tests");
}

TEST_F(CNDriverTest, GetExtensions) {
    auto exts = driver_->GetExtensions();
    ASSERT_EQ(exts.size(), 1);
    EXPECT_EQ(exts[0], ".test");
}

TEST_F(CNDriverTest, GetMimeTypes) {
    auto mimes = driver_->GetMimeTypes();
    ASSERT_EQ(mimes.size(), 1);
    EXPECT_EQ(mimes[0], "application/x-test");
}

TEST_F(CNDriverTest, CanOpen) {
    EXPECT_TRUE(driver_->CanOpen("file.test"));
    EXPECT_FALSE(driver_->CanOpen("file.txt"));
}

TEST_F(CNDriverTest, CanCreate) {
    EXPECT_TRUE(driver_->CanCreate("new.test"));
    EXPECT_FALSE(driver_->CanCreate("new.txt"));
}

TEST_F(CNDriverTest, SupportsUpdate) {
    EXPECT_TRUE(driver_->SupportsUpdate());
}

TEST_F(CNDriverTest, SupportsMultipleLayers) {
    EXPECT_TRUE(driver_->SupportsMultipleLayers());
}

TEST_F(CNDriverTest, SupportsTransactions) {
    EXPECT_FALSE(driver_->SupportsTransactions());
}

TEST_F(CNDriverTest, Open) {
    auto ds = driver_->Open("file.test", false);
    ASSERT_NE(ds, nullptr);
    EXPECT_EQ(ds->GetName(), "test_ds");
}

TEST_F(CNDriverTest, OpenInvalid) {
    auto ds = driver_->Open("file.txt", false);
    EXPECT_EQ(ds, nullptr);
}

TEST_F(CNDriverTest, Create) {
    auto ds = driver_->Create("new.test", {});
    ASSERT_NE(ds, nullptr);
    EXPECT_EQ(ds->GetName(), "test_ds");
}

TEST_F(CNDriverTest, CreateInvalid) {
    auto ds = driver_->Create("new.txt", {});
    EXPECT_EQ(ds, nullptr);
}

class CNObservableLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        observable_ = std::make_unique<TestObservableLayer>("observable_test", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("name");
        field->SetType(CNFieldType::kString);
        observable_->GetFeatureDefn()->AddFieldDefn(field);
        
        observer1_ = std::make_unique<TestObserver>();
        observer2_ = std::make_unique<TestObserver>();
    }
    
    std::unique_ptr<TestObservableLayer> observable_;
    std::unique_ptr<TestObserver> observer1_;
    std::unique_ptr<TestObserver> observer2_;
};

TEST_F(CNObservableLayerTest, AddObserver) {
    observable_->AddObserver(observer1_.get());
    SUCCEED();
}

TEST_F(CNObservableLayerTest, RemoveObserver) {
    observable_->AddObserver(observer1_.get());
    observable_->RemoveObserver(observer1_.get());
    SUCCEED();
}

TEST_F(CNObservableLayerTest, ClearObservers) {
    observable_->AddObserver(observer1_.get());
    observable_->AddObserver(observer2_.get());
    observable_->ClearObservers();
    SUCCEED();
}

TEST_F(CNObservableLayerTest, NotifyObservers) {
    observable_->AddObserver(observer1_.get());
    observable_->AddObserver(observer2_.get());
    
    CNFeature* feature = new CNFeature(observable_->GetFeatureDefn());
    feature->SetFID(1);
    
    CNStatus status = observable_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    EXPECT_EQ(observer1_->GetEventCount(), 1);
    EXPECT_EQ(observer1_->GetLastEvent(), CNLayerEventType::kFeatureAdded);
    EXPECT_EQ(observer2_->GetEventCount(), 1);
    EXPECT_EQ(observer2_->GetLastEvent(), CNLayerEventType::kFeatureAdded);
}

TEST_F(CNObservableLayerTest, NotifyChangingAllow) {
    observable_->AddObserver(observer1_.get());
    observer1_->SetAllowChange(true);
    
    CNFeature* feature = new CNFeature(observable_->GetFeatureDefn());
    feature->SetFID(1);
    
    CNStatus status = observable_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(observer1_->GetEventCount(), 1);
}

TEST_F(CNObservableLayerTest, NotifyChangingDeny) {
    observable_->AddObserver(observer1_.get());
    observer1_->SetAllowChange(false);
    
    CNFeature* feature = new CNFeature(observable_->GetFeatureDefn());
    feature->SetFID(1);
    
    CNStatus status = observable_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kError);
    EXPECT_EQ(observer1_->GetEventCount(), 0);
}
