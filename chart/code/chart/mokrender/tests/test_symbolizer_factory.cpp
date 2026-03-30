#include <gtest/gtest.h>
#include "ogc/mokrender/symbolizer_factory.h"
#include "ogc/mokrender/common.h"

using namespace ogc::mokrender;

class SymbolizerFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory = new SymbolizerFactory();
    }
    
    void TearDown() override {
        delete factory;
    }
    
    SymbolizerFactory* factory;
};

TEST_F(SymbolizerFactoryTest, Initialize) {
    MokRenderResult result = factory->Initialize();
    EXPECT_TRUE(result.IsSuccess());
}

TEST_F(SymbolizerFactoryTest, CreatePointSymbolizer) {
    factory->Initialize();
    void* symbolizer = factory->CreatePointSymbolizer();
    EXPECT_NE(symbolizer, nullptr);
}

TEST_F(SymbolizerFactoryTest, CreateLineSymbolizer) {
    factory->Initialize();
    void* symbolizer = factory->CreateLineSymbolizer();
    EXPECT_NE(symbolizer, nullptr);
}

TEST_F(SymbolizerFactoryTest, CreatePolygonSymbolizer) {
    factory->Initialize();
    void* symbolizer = factory->CreatePolygonSymbolizer();
    EXPECT_NE(symbolizer, nullptr);
}

TEST_F(SymbolizerFactoryTest, CreateTextSymbolizer) {
    factory->Initialize();
    void* symbolizer = factory->CreateTextSymbolizer();
    EXPECT_NE(symbolizer, nullptr);
}

TEST_F(SymbolizerFactoryTest, CreateRasterSymbolizer) {
    factory->Initialize();
    void* symbolizer = factory->CreateRasterSymbolizer();
    EXPECT_NE(symbolizer, nullptr);
}
