#include "gtest/gtest.h"
#include "ogc/feature/feature_iterator.h"
#include "ogc/feature/feature.h"
#include "ogc/feature/feature_collection.h"
#include "ogc/feature/feature_defn.h"

using namespace ogc;

class FeatureIteratorTest : public ::testing::Test {
protected:
    void SetUp() override {
        defn_ = CNFeatureDefn::Create("test");
        collection_ = new CNFeatureCollection();
        collection_->SetFeatureDefinition(defn_);
        
        for (int i = 0; i < 5; ++i) {
            CNFeature* f = new CNFeature(defn_);
            f->SetFID(i * 10);
            collection_->AddFeature(f);
        }
    }
    
    void TearDown() override {
        delete collection_;
        if (defn_) {
            defn_->ReleaseReference();
        }
    }
    
    CNFeatureDefn* defn_ = nullptr;
    CNFeatureCollection* collection_ = nullptr;
};

TEST_F(FeatureIteratorTest, Constructor) {
    CNFeatureIterator iterator(collection_);
    EXPECT_EQ(iterator.GetPosition(), 0);
    EXPECT_EQ(iterator.GetTotalCount(), 5);
}

TEST_F(FeatureIteratorTest, HasNext) {
    CNFeatureIterator iterator(collection_);
    
    EXPECT_TRUE(iterator.HasNext());
    
    for (int i = 0; i < 5; ++i) {
        iterator.Next();
    }
    
    EXPECT_FALSE(iterator.HasNext());
}

TEST_F(FeatureIteratorTest, Next) {
    CNFeatureIterator iterator(collection_);
    
    CNFeature* f0 = iterator.Next();
    ASSERT_NE(f0, nullptr);
    EXPECT_EQ(f0->GetFID(), 0);
    
    CNFeature* f1 = iterator.Next();
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f1->GetFID(), 10);
    
    CNFeature* f2 = iterator.Next();
    ASSERT_NE(f2, nullptr);
    EXPECT_EQ(f2->GetFID(), 20);
}

TEST_F(FeatureIteratorTest, Reset) {
    CNFeatureIterator iterator(collection_);
    
    iterator.Next();
    iterator.Next();
    EXPECT_EQ(iterator.GetPosition(), 2);
    
    iterator.Reset();
    EXPECT_EQ(iterator.GetPosition(), 0);
    EXPECT_TRUE(iterator.HasNext());
}

TEST_F(FeatureIteratorTest, OperatorDereference) {
    CNFeatureIterator iterator(collection_);
    
    CNFeature* f = *iterator;
    ASSERT_NE(f, nullptr);
    EXPECT_EQ(f->GetFID(), 0);
}

TEST_F(FeatureIteratorTest, OperatorArrow) {
    CNFeatureIterator iterator(collection_);
    
    EXPECT_EQ(iterator->GetFID(), 0);
}

TEST_F(FeatureIteratorTest, OperatorPreIncrement) {
    CNFeatureIterator iterator(collection_);
    
    ++iterator;
    EXPECT_EQ(iterator.GetPosition(), 1);
    EXPECT_EQ(iterator->GetFID(), 10);
    
    ++iterator;
    EXPECT_EQ(iterator.GetPosition(), 2);
    EXPECT_EQ(iterator->GetFID(), 20);
}

TEST_F(FeatureIteratorTest, OperatorPostIncrement) {
    CNFeatureIterator iterator(collection_);
    
    CNFeatureIterator old = iterator++;
    EXPECT_EQ(old.GetPosition(), 0);
    EXPECT_EQ(iterator.GetPosition(), 1);
}

TEST_F(FeatureIteratorTest, CopyConstructor) {
    CNFeatureIterator iterator1(collection_);
    iterator1.Next();
    iterator1.Next();
    
    CNFeatureIterator iterator2(iterator1);
    EXPECT_EQ(iterator2.GetPosition(), 2);
    EXPECT_EQ(iterator2.GetTotalCount(), 5);
}

TEST_F(FeatureIteratorTest, AssignmentOperator) {
    CNFeatureIterator iterator1(collection_);
    iterator1.Next();
    
    CNFeatureIterator iterator2(collection_);
    iterator2 = iterator1;
    
    EXPECT_EQ(iterator2.GetPosition(), 1);
}

TEST_F(FeatureIteratorTest, EmptyCollection) {
    CNFeatureCollection emptyCollection;
    CNFeatureIterator iterator(&emptyCollection);
    
    EXPECT_FALSE(iterator.HasNext());
    EXPECT_EQ(iterator.GetTotalCount(), 0);
    EXPECT_EQ(iterator.Next(), nullptr);
}

TEST_F(FeatureIteratorTest, SingleFeature) {
    CNFeatureCollection singleCollection;
    singleCollection.SetFeatureDefinition(defn_);
    
    CNFeature* f = new CNFeature(defn_);
    f->SetFID(999);
    singleCollection.AddFeature(f);
    
    CNFeatureIterator iterator(&singleCollection);
    
    EXPECT_TRUE(iterator.HasNext());
    CNFeature* feature = iterator.Next();
    ASSERT_NE(feature, nullptr);
    EXPECT_EQ(feature->GetFID(), 999);
    EXPECT_FALSE(iterator.HasNext());
}

TEST_F(FeatureIteratorTest, IterateAll) {
    CNFeatureIterator iterator(collection_);
    
    int count = 0;
    while (iterator.HasNext()) {
        CNFeature* f = iterator.Next();
        ASSERT_NE(f, nullptr);
        count++;
    }
    
    EXPECT_EQ(count, 5);
}

TEST_F(FeatureIteratorTest, GetPosition) {
    CNFeatureIterator iterator(collection_);
    
    EXPECT_EQ(iterator.GetPosition(), 0);
    iterator.Next();
    EXPECT_EQ(iterator.GetPosition(), 1);
    iterator.Next();
    EXPECT_EQ(iterator.GetPosition(), 2);
}

TEST_F(FeatureIteratorTest, GetTotalCount) {
    CNFeatureIterator iterator(collection_);
    EXPECT_EQ(iterator.GetTotalCount(), 5);
}

TEST_F(FeatureIteratorTest, NullCollection) {
    CNFeatureIterator iterator(nullptr);
    EXPECT_EQ(iterator.GetTotalCount(), 0);
    EXPECT_FALSE(iterator.HasNext());
}

TEST_F(FeatureIteratorTest, MultipleIterations) {
    for (int iteration = 0; iteration < 3; ++iteration) {
        CNFeatureIterator iterator(collection_);
        int count = 0;
        while (iterator.HasNext()) {
            iterator.Next();
            count++;
        }
        EXPECT_EQ(count, 5);
    }
}

TEST_F(FeatureIteratorTest, ResetAndReiterate) {
    CNFeatureIterator iterator(collection_);
    
    while (iterator.HasNext()) {
        iterator.Next();
    }
    EXPECT_FALSE(iterator.HasNext());
    
    iterator.Reset();
    EXPECT_TRUE(iterator.HasNext());
    
    int count = 0;
    while (iterator.HasNext()) {
        iterator.Next();
        count++;
    }
    EXPECT_EQ(count, 5);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
