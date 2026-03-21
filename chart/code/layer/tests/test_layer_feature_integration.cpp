#include "gtest/gtest.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/wkb_wkt_converter.h"
#include "ogc/feature/geojson_converter.h"
#include "ogc/geometry.h"
#include "ogc/factory.h"

using namespace ogc;

class LayerFeatureIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("integration_test", GeomType::kPoint);
        
        auto* defn = layer_->GetFeatureDefn();
        auto* name_field = CreateCNFieldDefn("name");
        name_field->SetType(CNFieldType::kString);
        name_field->SetWidth(255);
        defn->AddFieldDefn(name_field);
        
        auto* id_field = CreateCNFieldDefn("id");
        id_field->SetType(CNFieldType::kInteger);
        defn->AddFieldDefn(id_field);
    }
    
    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(LayerFeatureIntegrationTest, FeatureDefnSharing) {
    auto* layer_defn = layer_->GetFeatureDefn();
    ASSERT_NE(layer_defn, nullptr);
    EXPECT_EQ(layer_defn->GetFieldCount(), 2);
    
    CNFeature feature(layer_defn);
    auto* feature_defn = feature.GetFeatureDefn();
    
    EXPECT_EQ(feature_defn, layer_defn);
}

TEST_F(LayerFeatureIntegrationTest, FeatureCreationAndRetrieval) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(100);
    feature->SetField(0, CNFieldValue("TestCity"));
    feature->SetField(1, CNFieldValue(42));
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(116.4, 39.9));
    
    CNStatus status = layer_->CreateFeature(feature);
    ASSERT_EQ(status, CNStatus::kSuccess);
    
    auto retrieved = layer_->GetFeature(100);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), 100);
    EXPECT_EQ(retrieved->GetField(0).GetString(), "TestCity");
    EXPECT_EQ(retrieved->GetField(1).GetInteger(), 42);
    
    auto geom = retrieved->GetGeometry();
    ASSERT_NE(geom, nullptr);
    EXPECT_EQ(geom->GetGeometryType(), GeomType::kPoint);
}

TEST_F(LayerFeatureIntegrationTest, BatchFeatureOperations) {
    std::vector<CNFeature*> features;
    
    for (int i = 0; i < 10; ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(i + 1);
        f->SetField(0, CNFieldValue("Point_" + std::to_string(i)));
        f->SetField(1, CNFieldValue(i));
        f->SetGeometry(GeometryFactory::GetInstance().CreatePoint(static_cast<double>(i), static_cast<double>(i * 2)));
        features.push_back(f);
    }
    
    int64_t created = layer_->CreateFeatureBatch(features);
    EXPECT_EQ(created, 10);
    EXPECT_EQ(layer_->GetFeatureCount(), 10);
}

TEST_F(LayerFeatureIntegrationTest, FeatureIteration) {
    for (int i = 1; i <= 5; ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(i);
        f->SetField(0, CNFieldValue("Feature_" + std::to_string(i)));
        layer_->CreateFeature(f);
    }
    
    layer_->ResetReading();
    int count = 0;
    while (auto f = layer_->GetNextFeature()) {
        ++count;
        EXPECT_GT(f->GetFID(), 0);
    }
    EXPECT_EQ(count, 5);
}

TEST_F(LayerFeatureIntegrationTest, WKTConversion) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue("WKT Test"));
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(10.0, 20.0));
    
    layer_->CreateFeature(feature);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    
    CNWkbWktConverter converter;
    std::string wkt = converter.ToWKT(retrieved.get());
    
    EXPECT_NE(wkt.find("POINT"), std::string::npos);
}

TEST_F(LayerFeatureIntegrationTest, GeoJSONConversion) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue("JSON Test"));
    feature->SetGeometry(GeometryFactory::GetInstance().CreatePoint(10.0, 20.0));
    
    layer_->CreateFeature(feature);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    
    CNGeoJSONConverter converter;
    std::string json = converter.ToGeoJSON(retrieved.get());
    
    EXPECT_NE(json.find("type"), std::string::npos);
    EXPECT_NE(json.find("coordinates"), std::string::npos);
}

TEST_F(LayerFeatureIntegrationTest, FeatureCollectionIntegration) {
    auto collection = std::make_unique<CNFeatureCollection>();
    collection->SetFeatureDefinition(layer_->GetFeatureDefn());
    
    for (int i = 1; i <= 3; ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(i);
        f->SetField(0, CNFieldValue("Collection_" + std::to_string(i)));
        collection->AddFeature(f);
    }
    
    EXPECT_EQ(collection->GetFeatureCount(), 3);
    
    auto* feature = collection->GetFeature(0);
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->GetField(0).GetString(), "Collection_1");
}

TEST_F(LayerFeatureIntegrationTest, LayerExtentCalculation) {
    std::vector<std::pair<double, double>> coords = {
        {0.0, 0.0}, {10.0, 0.0}, {10.0, 10.0}, {0.0, 10.0}
    };
    
    for (size_t i = 0; i < coords.size(); ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(static_cast<int64_t>(i + 1));
        f->SetField(0, CNFieldValue("Point_" + std::to_string(i)));
        f->SetGeometry(GeometryFactory::GetInstance().CreatePoint(coords[i].first, coords[i].second));
        layer_->CreateFeature(f);
    }
    
    Envelope extent;
    CNStatus status = layer_->GetExtent(extent);
    
    ASSERT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(extent.GetMinX(), 0.0);
    EXPECT_EQ(extent.GetMaxX(), 10.0);
    EXPECT_EQ(extent.GetMinY(), 0.0);
    EXPECT_EQ(extent.GetMaxY(), 10.0);
}

TEST_F(LayerFeatureIntegrationTest, SpatialFiltering) {
    std::vector<std::pair<double, double>> points = {
        {5.0, 5.0}, {15.0, 15.0}, {25.0, 25.0}
    };
    
    for (size_t i = 0; i < points.size(); ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(static_cast<int64_t>(i + 1));
        f->SetField(0, CNFieldValue("Point_" + std::to_string(i)));
        f->SetGeometry(GeometryFactory::GetInstance().CreatePoint(points[i].first, points[i].second));
        layer_->CreateFeature(f);
    }
    
    layer_->SetSpatialFilterRect(0.0, 0.0, 20.0, 20.0);
    
    int filtered_count = 0;
    layer_->ResetReading();
    while (auto f = layer_->GetNextFeature()) {
        ++filtered_count;
    }
    
    EXPECT_EQ(filtered_count, 2);
}

TEST_F(LayerFeatureIntegrationTest, TransactionIsolation) {
    CNFeature* f1 = new CNFeature(layer_->GetFeatureDefn());
    f1->SetFID(1);
    f1->SetField(0, CNFieldValue("BeforeCommit"));
    layer_->CreateFeature(f1);
    
    layer_->StartTransaction();
    
    CNFeature* f2 = new CNFeature(layer_->GetFeatureDefn());
    f2->SetFID(2);
    f2->SetField(0, CNFieldValue("InTransaction"));
    layer_->CreateFeature(f2);
    
    EXPECT_EQ(layer_->GetFeatureCount(), 2);
    
    layer_->CommitTransaction();
    
    EXPECT_EQ(layer_->GetFeatureCount(), 2);
    
    auto feature = layer_->GetFeature(2);
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->GetField(0).GetString(), "InTransaction");
}

TEST_F(LayerFeatureIntegrationTest, TransactionRollback) {
    CNFeature* f1 = new CNFeature(layer_->GetFeatureDefn());
    f1->SetFID(1);
    f1->SetField(0, CNFieldValue("Original"));
    layer_->CreateFeature(f1);
    
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
    
    layer_->StartTransaction();
    
    CNFeature* f2 = new CNFeature(layer_->GetFeatureDefn());
    f2->SetFID(2);
    f2->SetField(0, CNFieldValue("ToBeRolledBack"));
    layer_->CreateFeature(f2);
    
    EXPECT_EQ(layer_->GetFeatureCount(), 2);
    
    layer_->RollbackTransaction();
    
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
    
    auto feature = layer_->GetFeature(2);
    EXPECT_EQ(feature, nullptr);
}

TEST_F(LayerFeatureIntegrationTest, ClonePreservesFeatures) {
    for (int i = 1; i <= 3; ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(i);
        f->SetField(0, CNFieldValue("CloneTest_" + std::to_string(i)));
        f->SetGeometry(GeometryFactory::GetInstance().CreatePoint(static_cast<double>(i), static_cast<double>(i)));
        layer_->CreateFeature(f);
    }
    
    auto cloned = layer_->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetFeatureCount(), 3);
    
    auto f1 = cloned->GetFeature(1);
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f1->GetField(0).GetString(), "CloneTest_1");
}
