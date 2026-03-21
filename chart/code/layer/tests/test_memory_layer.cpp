#include "gtest/gtest.h"
#include "ogc/layer/memory_layer.h"
#include "ogc/layer/geometry_compat.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/field_defn.h"
#include "ogc/feature/field_value.h"
#include "ogc/geometry.h"
#include "ogc/factory.h"

using namespace ogc;

class CNMemoryLayerTest : public ::testing::Test {
protected:
    void SetUp() override {
        layer_ = std::make_unique<CNMemoryLayer>("test_layer", GeomType::kPoint);
        
        auto* field = CreateCNFieldDefn("name");
        field->SetType(CNFieldType::kString);
        field->SetWidth(255);
        layer_->CreateField(field);
    }

    std::unique_ptr<CNMemoryLayer> layer_;
};

TEST_F(CNMemoryLayerTest, GetName) {
    EXPECT_EQ(layer_->GetName(), "test_layer");
}

TEST_F(CNMemoryLayerTest, GetLayerType) {
    EXPECT_EQ(layer_->GetLayerType(), CNLayerType::kMemory);
}

TEST_F(CNMemoryLayerTest, GetGeomType) {
    EXPECT_EQ(layer_->GetGeomType(), GeomType::kPoint);
}

TEST_F(CNMemoryLayerTest, CreateFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue("Test Point"));
    
    auto point = GeometryFactory::GetInstance().CreatePoint(1.0, 2.0);
    feature->SetGeometry(std::move(point));
    
    CNStatus status = layer_->CreateFeature(feature);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
}

TEST_F(CNMemoryLayerTest, GetFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    feature->SetField(0, CNFieldValue("Test Point"));
    
    auto point = GeometryFactory::GetInstance().CreatePoint(1.0, 2.0);
    feature->SetGeometry(std::move(point));
    
    layer_->CreateFeature(feature);
    
    auto retrieved = layer_->GetFeature(1);
    ASSERT_NE(retrieved, nullptr);
    EXPECT_EQ(retrieved->GetFID(), 1);
}

TEST_F(CNMemoryLayerTest, DeleteFeature) {
    CNFeature* feature = new CNFeature(layer_->GetFeatureDefn());
    feature->SetFID(1);
    layer_->CreateFeature(feature);
    
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
    
    CNStatus status = layer_->DeleteFeature(1);
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(layer_->GetFeatureCount(), 0);
}

TEST_F(CNMemoryLayerTest, GetExtent) {
    CNFeature* f1 = new CNFeature(layer_->GetFeatureDefn());
    f1->SetFID(1);
    f1->SetGeometry(GeometryFactory::GetInstance().CreatePoint(0.0, 0.0));
    layer_->CreateFeature(f1);
    
    CNFeature* f2 = new CNFeature(layer_->GetFeatureDefn());
    f2->SetFID(2);
    f2->SetGeometry(GeometryFactory::GetInstance().CreatePoint(10.0, 10.0));
    layer_->CreateFeature(f2);
    
    Envelope extent;
    CNStatus status = layer_->GetExtent(extent);
    
    EXPECT_EQ(status, CNStatus::kSuccess);
    EXPECT_EQ(extent.GetMinX(), 0.0);
    EXPECT_EQ(extent.GetMaxX(), 10.0);
    EXPECT_EQ(extent.GetMinY(), 0.0);
    EXPECT_EQ(extent.GetMaxY(), 10.0);
}

TEST_F(CNMemoryLayerTest, ResetReading) {
    for (int i = 1; i <= 3; ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(i);
        layer_->CreateFeature(f);
    }
    
    layer_->GetNextFeature();
    layer_->GetNextFeature();
    layer_->ResetReading();
    
    auto f1 = layer_->GetNextFeature();
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f1->GetFID(), 1);
}

TEST_F(CNMemoryLayerTest, Transaction) {
    CNFeature* f1 = new CNFeature(layer_->GetFeatureDefn());
    f1->SetFID(1);
    layer_->CreateFeature(f1);
    
    layer_->StartTransaction();
    
    CNFeature* f2 = new CNFeature(layer_->GetFeatureDefn());
    f2->SetFID(2);
    layer_->CreateFeature(f2);
    
    EXPECT_EQ(layer_->GetFeatureCount(), 2);
    
    layer_->RollbackTransaction();
    
    EXPECT_EQ(layer_->GetFeatureCount(), 1);
}

TEST_F(CNMemoryLayerTest, Clear) {
    for (int i = 1; i <= 5; ++i) {
        CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
        f->SetFID(i);
        layer_->CreateFeature(f);
    }
    
    EXPECT_EQ(layer_->GetFeatureCount(), 5);
    
    layer_->Clear();
    
    EXPECT_EQ(layer_->GetFeatureCount(), 0);
}

TEST_F(CNMemoryLayerTest, SetSpatialFilter) {
    CNFeature* f1 = new CNFeature(layer_->GetFeatureDefn());
    f1->SetFID(1);
    f1->SetGeometry(GeometryFactory::GetInstance().CreatePoint(5.0, 5.0));
    layer_->CreateFeature(f1);
    
    CNFeature* f2 = new CNFeature(layer_->GetFeatureDefn());
    f2->SetFID(2);
    f2->SetGeometry(GeometryFactory::GetInstance().CreatePoint(50.0, 50.0));
    layer_->CreateFeature(f2);
    
    layer_->SetSpatialFilterRect(0.0, 0.0, 10.0, 10.0);
    
    int count = 0;
    layer_->ResetReading();
    while (auto f = layer_->GetNextFeature()) {
        ++count;
    }
    
    EXPECT_EQ(count, 1);
}

TEST_F(CNMemoryLayerTest, Clone) {
    CNFeature* f = new CNFeature(layer_->GetFeatureDefn());
    f->SetFID(1);
    f->SetField(0, CNFieldValue("Test"));
    layer_->CreateFeature(f);
    
    auto cloned = layer_->Clone();
    ASSERT_NE(cloned, nullptr);
    EXPECT_EQ(cloned->GetName(), layer_->GetName());
    EXPECT_EQ(cloned->GetFeatureCount(), layer_->GetFeatureCount());
}
