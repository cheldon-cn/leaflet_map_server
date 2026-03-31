#include <gtest/gtest.h>
#include "chart_parser/data_converter.h"
#include "chart_parser/ogr_data_converter.h"

using namespace chart::parser;

class MockDataConverter : public IDataConverter {
public:
    std::string GetName() const override { return "MockConverter"; }
    std::string GetSourceType() const override { return "Mock"; }
    
    bool ConvertGeometry(void* sourceGeometry, Geometry& outGeometry) override {
        return sourceGeometry != nullptr;
    }
    
    bool ConvertAttributes(void* sourceFeature, AttributeMap& outAttributes) override {
        return sourceFeature != nullptr;
    }
};

class DataConverterFactoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        factory_ = &DataConverterFactory::Instance();
    }
    
    void TearDown() override {
        auto types = factory_->GetRegisteredTypes();
        for (const auto& type : types) {
            factory_->UnregisterConverter(type);
        }
    }
    
    DataConverterFactory* factory_;
};

TEST_F(DataConverterFactoryTest, Instance_ReturnsSingleton) {
    auto& instance1 = DataConverterFactory::Instance();
    auto& instance2 = DataConverterFactory::Instance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(DataConverterFactoryTest, RegisterConverter_AddsConverter) {
    factory_->RegisterConverter("Mock", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    EXPECT_TRUE(factory_->HasConverter("Mock"));
}

TEST_F(DataConverterFactoryTest, RegisterConverter_SameTypeTwice_Replaces) {
    factory_->RegisterConverter("Mock", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    factory_->RegisterConverter("Mock", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    EXPECT_TRUE(factory_->HasConverter("Mock"));
}

TEST_F(DataConverterFactoryTest, UnregisterConverter_RemovesConverter) {
    factory_->RegisterConverter("Mock", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    factory_->UnregisterConverter("Mock");
    
    EXPECT_FALSE(factory_->HasConverter("Mock"));
}

TEST_F(DataConverterFactoryTest, UnregisterConverter_NonExistent_NoException) {
    factory_->UnregisterConverter("NonExistent");
    SUCCEED();
}

TEST_F(DataConverterFactoryTest, CreateConverter_ReturnsValidConverter) {
    factory_->RegisterConverter("Mock", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    auto converter = factory_->CreateConverter("Mock");
    EXPECT_NE(converter, nullptr);
    EXPECT_EQ(converter->GetName(), "MockConverter");
}

TEST_F(DataConverterFactoryTest, CreateConverter_NonExistent_ReturnsNull) {
    auto converter = factory_->CreateConverter("NonExistent");
    EXPECT_EQ(converter, nullptr);
}

TEST_F(DataConverterFactoryTest, HasConverter_Registered_ReturnsTrue) {
    factory_->RegisterConverter("Mock", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    EXPECT_TRUE(factory_->HasConverter("Mock"));
}

TEST_F(DataConverterFactoryTest, HasConverter_NotRegistered_ReturnsFalse) {
    EXPECT_FALSE(factory_->HasConverter("NonExistent"));
}

TEST_F(DataConverterFactoryTest, GetRegisteredTypes_ReturnsAllTypes) {
    factory_->RegisterConverter("Type1", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    factory_->RegisterConverter("Type2", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    auto types = factory_->GetRegisteredTypes();
    EXPECT_EQ(types.size(), 2u);
}

TEST_F(DataConverterFactoryTest, GetRegisteredTypes_Empty_ReturnsEmpty) {
    auto types = factory_->GetRegisteredTypes();
    EXPECT_TRUE(types.empty());
}

TEST_F(DataConverterFactoryTest, MultipleConverters_WorkIndependently) {
    factory_->RegisterConverter("Converter1", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    factory_->RegisterConverter("Converter2", []() -> std::unique_ptr<IDataConverter> {
        return std::make_unique<MockDataConverter>();
    });
    
    auto conv1 = factory_->CreateConverter("Converter1");
    auto conv2 = factory_->CreateConverter("Converter2");
    
    EXPECT_NE(conv1, nullptr);
    EXPECT_NE(conv2, nullptr);
}

class IDataConverterTest : public ::testing::Test {
};

TEST_F(IDataConverterTest, MockConverter_ConvertGeometry_ValidInput) {
    MockDataConverter converter;
    Geometry geo;
    int dummy = 42;
    
    bool result = converter.ConvertGeometry(&dummy, geo);
    EXPECT_TRUE(result);
}

TEST_F(IDataConverterTest, MockConverter_ConvertGeometry_NullInput) {
    MockDataConverter converter;
    Geometry geo;
    
    bool result = converter.ConvertGeometry(nullptr, geo);
    EXPECT_FALSE(result);
}

TEST_F(IDataConverterTest, MockConverter_ConvertAttributes_ValidInput) {
    MockDataConverter converter;
    AttributeMap attrs;
    int dummy = 42;
    
    bool result = converter.ConvertAttributes(&dummy, attrs);
    EXPECT_TRUE(result);
}

TEST_F(IDataConverterTest, MockConverter_ConvertAttributes_NullInput) {
    MockDataConverter converter;
    AttributeMap attrs;
    
    bool result = converter.ConvertAttributes(nullptr, attrs);
    EXPECT_FALSE(result);
}

TEST_F(IDataConverterTest, MockConverter_GetName_ReturnsCorrectName) {
    MockDataConverter converter;
    EXPECT_EQ(converter.GetName(), "MockConverter");
}

TEST_F(IDataConverterTest, MockConverter_GetSourceType_ReturnsCorrectType) {
    MockDataConverter converter;
    EXPECT_EQ(converter.GetSourceType(), "Mock");
}

class ConverterRegistrarTest : public ::testing::Test {
protected:
    void TearDown() override {
        DataConverterFactory::Instance().UnregisterConverter("TestConverter");
    }
};

TEST_F(ConverterRegistrarTest, Registrar_RegistersConverter) {
    static ConverterRegistrar<MockDataConverter> registrar("TestConverter");
    
    EXPECT_TRUE(DataConverterFactory::Instance().HasConverter("TestConverter"));
}
