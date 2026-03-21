#include "gtest/gtest.h"
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_defn.h"
#include "ogc/feature/field_defn.h"

using namespace ogc;

TEST(CNFeatureCollection, DefaultConstructor) {
    CNFeatureCollection collection;
    EXPECT_EQ(collection.GetFeatureCount(), 0);
    EXPECT_EQ(collection.GetFeatureDefinition(), nullptr);
}

TEST(CNFeatureCollection, SetFeatureDefinition) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn);
    EXPECT_EQ(collection.GetFeatureDefinition(), defn);
    defn->ReleaseReference();
}

TEST(CNFeatureCollection, AddFeature) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn);
    
    CNFeature* feature = new CNFeature(defn);
    feature->SetFID(1);
    collection.AddFeature(feature);
    
    EXPECT_EQ(collection.GetFeatureCount(), 1);
    EXPECT_EQ(collection.GetFeature(0)->GetFID(), 1);
    
    defn->ReleaseReference();
}

TEST(CNFeatureCollection, GetFeature) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn);
    
    CNFeature* f1 = new CNFeature(defn);
    f1->SetFID(1);
    collection.AddFeature(f1);
    
    CNFeature* f2 = new CNFeature(defn);
    f2->SetFID(2);
    collection.AddFeature(f2);
    
    EXPECT_EQ(collection.GetFeature(0)->GetFID(), 1);
    EXPECT_EQ(collection.GetFeature(1)->GetFID(), 2);
    EXPECT_EQ(collection.GetFeature(2), nullptr);
    
    defn->ReleaseReference();
}

TEST(CNFeatureCollection, Clear) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn);
    
    for (int i = 0; i < 5; ++i) {
        CNFeature* f = new CNFeature(defn);
        f->SetFID(i);
        collection.AddFeature(f);
    }
    
    EXPECT_EQ(collection.GetFeatureCount(), 5);
    collection.Clear();
    EXPECT_EQ(collection.GetFeatureCount(), 0);
    
    defn->ReleaseReference();
}

TEST(CNFeatureCollection, Clone) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeatureCollection collection;
    collection.SetFeatureDefinition(defn);
    
    CNFeature* f = new CNFeature(defn);
    f->SetFID(100);
    collection.AddFeature(f);
    
    CNFeatureCollection* cloned = collection.Clone();
    EXPECT_EQ(cloned->GetFeatureCount(), 1);
    EXPECT_EQ(cloned->GetFeature(0)->GetFID(), 100);
    
    delete cloned;
    defn->ReleaseReference();
}

TEST(CNFeatureCollection, Swap) {
    CNFeatureDefn* defn = CNFeatureDefn::Create("test");
    CNFeatureCollection col1;
    CNFeatureCollection col2;
    
    CNFeature* f1 = new CNFeature(defn);
    f1->SetFID(1);
    col1.AddFeature(f1);
    
    CNFeature* f2 = new CNFeature(defn);
    f2->SetFID(2);
    col2.AddFeature(f2);
    
    EXPECT_EQ(col1.GetFeatureCount(), 1);
    EXPECT_EQ(col2.GetFeatureCount(), 1);
    
    col1.Swap(col2);
    
    EXPECT_EQ(col1.GetFeatureCount(), 1);
    EXPECT_EQ(col1.GetFeature(0)->GetFID(), 2);
    EXPECT_EQ(col2.GetFeatureCount(), 1);
    EXPECT_EQ(col2.GetFeature(0)->GetFID(), 1);
    
    defn->ReleaseReference();
}
