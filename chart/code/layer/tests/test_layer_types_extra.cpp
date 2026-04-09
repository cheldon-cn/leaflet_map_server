#include "gtest/gtest.h"
#include "ogc/layer/vector_layer.h"
#include "ogc/layer/raster_layer.h"
#include "ogc/layer/raster_dataset.h"
#include "ogc/layer/gdal_adapter.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geom/factory.h"

#include <memory>
#include <vector>

using namespace ogc;

class TestVectorLayer : public CNVectorLayer {
public:
    TestVectorLayer(const std::string& name, GeomType geom_type)
        : name_(name), geom_type_(geom_type) {
        feature_defn_ = std::make_shared<CNFeatureDefn>(name.c_str());
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
        return std::unique_ptr<CNFeature>(features_[read_cursor_++]->Clone());
    }
    
    std::unique_ptr<CNFeature> GetFeature(int64_t fid) override {
        for (auto& f : features_) {
            if (f->GetFID() == fid) return std::unique_ptr<CNFeature>(f->Clone());
        }
        return nullptr;
    }
    
    CNStatus CreateFeature(CNFeature* feature) override {
        features_.push_back(std::unique_ptr<CNFeature>(feature->Clone()));
        return CNStatus::kSuccess;
    }
    
    std::string GetDataSourcePath() const override { return ""; }
    std::string GetFormatName() const override { return "TestVector"; }
    bool IsReadOnly() const override { return false; }
    
    void SetSpatialFilter(const CNGeometry* geometry) override {
        (void)geometry;
    }
    
    const CNGeometry* GetSpatialFilter() const override { return nullptr; }
    
    CNStatus SetAttributeFilter(const std::string& query) override {
        (void)query;
        return CNStatus::kNotSupported;
    }
    
    bool TestCapability(CNLayerCapability capability) const override {
        (void)capability;
        return false;
    }
    
    std::unique_ptr<CNLayer> Clone() const override {
        return nullptr;
    }
    
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

class CNVectorLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<TestVectorLayer>("test_vector", GeomType::kPoint);
        auto* field = CreateCNFieldDefn("name");
        field->SetType(CNFieldType::kString);
        layer_->GetFeatureDefn()->AddFieldDefn(field);
    }

    std::unique_ptr<TestVectorLayer> layer_;
};

TEST_F(CNVectorLayerTest, GetLayerType) {
    EXPECT_EQ(layer_->GetLayerType(), CNLayerType::kVector);
}

TEST_F(CNVectorLayerTest, GetName) {
    EXPECT_EQ(layer_->GetName(), "test_vector");
}

TEST_F(CNVectorLayerTest, GetGeomType) {
    EXPECT_EQ(layer_->GetGeomType(), GeomType::kPoint);
}

TEST_F(CNVectorLayerTest, GetGeomFieldCount) {
    EXPECT_EQ(layer_->GetGeomFieldCount(), 1);
}

TEST_F(CNVectorLayerTest, GetEncoding) {
    EXPECT_EQ(layer_->GetEncoding(), "UTF-8");
}

TEST_F(CNVectorLayerTest, CreateFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(true), 1);
}

TEST_F(CNVectorLayerTest, GetFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    layer_->CreateFeature(feature);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), 1);
}

TEST_F(CNVectorLayerTest, GetNextFeature) {
    CNFeature* f1 = new CNFeature(layer_->GetFeatureDefn());
    f1->SetFID(1);
    layer_->CreateFeature(f1);
    
    CNFeature* f2 = new CNFeature(layer_->GetFeatureDefn());
    f2->SetFID(2);
    layer_->CreateFeature(f2);
    
    layer_->ResetReading();
    auto feature = layer_->GetNextFeature();
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->GetFID(), 1);
    
    feature = layer_->GetNextFeature();
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->GetFID(), 2);
    
    feature = layer_->GetNextFeature();
    EXPECT_EQ(feature, nullptr);
}

TEST_F(CNVectorLayerTest, SetCoordinateTransform) {
    CNStatus status = layer_->SetCoordinateTransform(nullptr);
    EXPECT_EQ(status, CNStatus::kNotSupported);
}

TEST_F(CNVectorLayerTest, ClearCoordinateTransform) {
    layer_->ClearCoordinateTransform();
    SUCCEED();
}

TEST_F(CNVectorLayerTest, GetGeomFieldDefn) {
    auto* defn = layer_->GetGeomFieldDefn(0);
    EXPECT_EQ(defn, nullptr);
}

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
    
    CNStatus SetGeoTransform(const double* transform) override {
        for (int i = 0; i < 6; ++i) geo_transform_[i] = transform[i];
        return CNStatus::kSuccess;
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
    
    void SetSpatialFilter(const CNGeometry* geometry) override {
        (void)geometry;
    }
    
    const CNGeometry* GetSpatialFilter() const override { return nullptr; }
    
    CNStatus SetAttributeFilter(const std::string& query) override {
        (void)query;
        return CNStatus::kNotSupported;
    }
    
    bool TestCapability(CNLayerCapability capability) const override {
        (void)capability;
        return false;
    }
    
    std::unique_ptr<CNLayer> Clone() const override {
        return nullptr;
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

class CNRasterLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<TestRasterLayer>("test_raster", 100, 100, 3);
    }

    std::unique_ptr<TestRasterLayer> layer_;
};

TEST_F(CNRasterLayerTest, GetLayerType) {
    EXPECT_EQ(layer_->GetLayerType(), CNLayerType::kRaster);
}

TEST_F(CNRasterLayerTest, GetName) {
    EXPECT_EQ(layer_->GetName(), "test_raster");
}

TEST_F(CNRasterLayerTest, GetWidth) {
    EXPECT_EQ(layer_->GetWidth(), 100);
}

TEST_F(CNRasterLayerTest, GetHeight) {
    EXPECT_EQ(layer_->GetHeight(), 100);
}

TEST_F(CNRasterLayerTest, GetBandCount) {
    EXPECT_EQ(layer_->GetBandCount(), 3);
}

TEST_F(CNRasterLayerTest, GetPixelWidth) {
    EXPECT_DOUBLE_EQ(layer_->GetPixelWidth(), 1.0);
}

TEST_F(CNRasterLayerTest, GetPixelHeight) {
    EXPECT_DOUBLE_EQ(layer_->GetPixelHeight(), 1.0);
}

TEST_F(CNRasterLayerTest, GetDataType) {
    EXPECT_EQ(layer_->GetDataType(), CNDataType::kByte);
}

TEST_F(CNRasterLayerTest, GetBand) {
    auto* band = layer_->GetBand(0);
    ASSERT_NE(band, nullptr);
    EXPECT_EQ(band->GetWidth(), 100);
    EXPECT_EQ(band->GetHeight(), 100);
}

TEST_F(CNRasterLayerTest, GetBandInvalid) {
    auto* band = layer_->GetBand(-1);
    EXPECT_EQ(band, nullptr);
    
    band = layer_->GetBand(10);
    EXPECT_EQ(band, nullptr);
}

TEST_F(CNRasterLayerTest, GetGeoTransform) {
    double transform[6];
    layer_->GetGeoTransform(transform);
    EXPECT_DOUBLE_EQ(transform[0], 0);
    EXPECT_DOUBLE_EQ(transform[1], 1);
}

TEST_F(CNRasterLayerTest, SetGeoTransform) {
    double transform[6] = {100, 0.5, 0, 200, 0, -0.5};
    CNStatus status = layer_->SetGeoTransform(transform);
    EXPECT_EQ(status, CNStatus::kSuccess);
    
    double result[6];
    layer_->GetGeoTransform(result);
    EXPECT_DOUBLE_EQ(result[0], 100);
    EXPECT_DOUBLE_EQ(result[1], 0.5);
}

TEST_F(CNRasterLayerTest, GetSpatialRef) {
    auto* ref = layer_->GetSpatialRef();
    EXPECT_EQ(ref, nullptr);
}

class CNRasterBandTest : public ::testing::Test {
protected:
    void SetUp() override {
        band_ = std::make_unique<TestRasterBand>(100, 100, CNDataType::kByte);
    }

    std::unique_ptr<TestRasterBand> band_;
};

TEST_F(CNRasterBandTest, GetWidth) {
    EXPECT_EQ(band_->GetWidth(), 100);
}

TEST_F(CNRasterBandTest, GetHeight) {
    EXPECT_EQ(band_->GetHeight(), 100);
}

TEST_F(CNRasterBandTest, GetDataType) {
    EXPECT_EQ(band_->GetDataType(), CNDataType::kByte);
}

TEST_F(CNRasterBandTest, GetNoDataValue) {
    double value;
    int has_value = band_->GetNoDataValue(value);
    EXPECT_EQ(has_value, 1);
    EXPECT_DOUBLE_EQ(value, -9999.0);
}

TEST_F(CNRasterBandTest, GetMinimum) {
    EXPECT_DOUBLE_EQ(band_->GetMinimum(), 0.0);
}

TEST_F(CNRasterBandTest, GetMaximum) {
    EXPECT_DOUBLE_EQ(band_->GetMaximum(), 255.0);
}

TEST_F(CNRasterBandTest, GetOffset) {
    EXPECT_DOUBLE_EQ(band_->GetOffset(), 0.0);
}

TEST_F(CNRasterBandTest, GetScale) {
    EXPECT_DOUBLE_EQ(band_->GetScale(), 1.0);
}

TEST_F(CNRasterBandTest, GetDescription) {
    EXPECT_EQ(band_->GetDescription(), "TestBand");
}

TEST_F(CNRasterBandTest, ReadRasterNotSupported) {
    std::vector<uint8_t> buffer(100 * 100);
    CNStatus status = band_->ReadRaster(0, 0, 100, 100, buffer.data(), CNDataType::kByte);
    EXPECT_EQ(status, CNStatus::kNotSupported);
}

TEST_F(CNRasterBandTest, WriteRasterNotSupported) {
    std::vector<uint8_t> buffer(100 * 100, 0);
    CNStatus status = band_->WriteRaster(0, 0, 100, 100, buffer.data(), CNDataType::kByte);
    EXPECT_EQ(status, CNStatus::kNotSupported);
}

class CNDataTypeTest : public ::testing::Test {
protected:
};

TEST_F(CNDataTypeTest, Values) {
    EXPECT_EQ(static_cast<int>(CNDataType::kByte), 0);
    EXPECT_EQ(static_cast<int>(CNDataType::kUInt16), 1);
    EXPECT_EQ(static_cast<int>(CNDataType::kInt16), 2);
    EXPECT_EQ(static_cast<int>(CNDataType::kUInt32), 3);
    EXPECT_EQ(static_cast<int>(CNDataType::kInt32), 4);
    EXPECT_EQ(static_cast<int>(CNDataType::kFloat32), 5);
    EXPECT_EQ(static_cast<int>(CNDataType::kFloat64), 6);
    EXPECT_EQ(static_cast<int>(CNDataType::kComplexInt16), 7);
    EXPECT_EQ(static_cast<int>(CNDataType::kComplexInt32), 8);
    EXPECT_EQ(static_cast<int>(CNDataType::kComplexFloat32), 9);
    EXPECT_EQ(static_cast<int>(CNDataType::kComplexFloat64), 10);
}

class CNGDALAdapterTest : public ::testing::Test {
protected:
};

TEST_F(CNGDALAdapterTest, IsGDALAvailable) {
    bool available = CNGDALAdapter::IsGDALAvailable();
    EXPECT_FALSE(available);
}

TEST_F(CNGDALAdapterTest, WrapLayerNull) {
    auto layer = CNGDALAdapter::WrapLayer(nullptr);
    EXPECT_EQ(layer, nullptr);
}

TEST_F(CNGDALAdapterTest, UnwrapLayerNull) {
    void* result = CNGDALAdapter::UnwrapLayer(nullptr);
    EXPECT_EQ(result, nullptr);
}

TEST_F(CNGDALAdapterTest, WrapDatasetNull) {
    auto ds = CNGDALAdapter::WrapDataset(nullptr);
    EXPECT_EQ(ds, nullptr);
}

TEST_F(CNGDALAdapterTest, UnwrapDatasetNull) {
    void* result = CNGDALAdapter::UnwrapDataset(nullptr);
    EXPECT_EQ(result, nullptr);
}
