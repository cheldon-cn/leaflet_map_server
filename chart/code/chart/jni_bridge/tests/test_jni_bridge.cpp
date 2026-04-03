#include <gtest/gtest.h>
#include <gmock/gmock.h>

#ifdef __ANDROID__

#include "jni_bridge/jni_env.h"
#include "jni_bridge/jni_convert.h"
#include "jni_bridge/jni_exception.h"
#include "jni_bridge/jni_memory.h"

using namespace ogc::jni;
using namespace testing;

class JniEnvManagerTest : public Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(JniEnvManagerTest, GetInstance_ReturnsNonNull) {
    auto* instance = JniEnvManager::GetInstance();
    EXPECT_NE(instance, nullptr);
}

TEST_F(JniEnvManagerTest, IsInitialized_InitiallyFalse) {
    auto* instance = JniEnvManager::GetInstance();
    EXPECT_FALSE(instance->IsInitialized());
}

class JniConverterTest : public Test {
protected:
    void SetUp() override {
    }
};

TEST_F(JniConverterTest, ToBool_ConvertsCorrectly) {
    EXPECT_TRUE(JniConverter::ToBool(nullptr, JNI_TRUE));
    EXPECT_FALSE(JniConverter::ToBool(nullptr, JNI_FALSE));
}

TEST_F(JniConverterTest, ToJBoolean_ConvertsCorrectly) {
    EXPECT_EQ(JNI_TRUE, JniConverter::ToJBoolean(true));
    EXPECT_EQ(JNI_FALSE, JniConverter::ToJBoolean(false));
}

TEST_F(JniConverterTest, ToInt_ConvertsCorrectly) {
    EXPECT_EQ(42, JniConverter::ToInt(nullptr, 42));
    EXPECT_EQ(-1, JniConverter::ToInt(nullptr, -1));
}

TEST_F(JniConverterTest, ToJInt_ConvertsCorrectly) {
    EXPECT_EQ(42, JniConverter::ToJInt(42));
    EXPECT_EQ(-1, JniConverter::ToJInt(-1));
}

TEST_F(JniConverterTest, ToLong_ConvertsCorrectly) {
    EXPECT_EQ(123456789L, JniConverter::ToLong(nullptr, 123456789LL));
}

TEST_F(JniConverterTest, ToJLong_ConvertsCorrectly) {
    EXPECT_EQ(123456789LL, JniConverter::ToJLong(123456789L));
}

TEST_F(JniConverterTest, ToFloat_ConvertsCorrectly) {
    EXPECT_FLOAT_EQ(3.14f, JniConverter::ToFloat(nullptr, 3.14f));
}

TEST_F(JniConverterTest, ToJFloat_ConvertsCorrectly) {
    EXPECT_FLOAT_EQ(3.14f, JniConverter::ToJFloat(3.14f));
}

TEST_F(JniConverterTest, ToDouble_ConvertsCorrectly) {
    EXPECT_DOUBLE_EQ(3.14159265358979, JniConverter::ToDouble(nullptr, 3.14159265358979));
}

TEST_F(JniConverterTest, ToJDouble_ConvertsCorrectly) {
    EXPECT_DOUBLE_EQ(3.14159265358979, JniConverter::ToJDouble(3.14159265358979));
}

TEST_F(JniConverterTest, ToJLongPtr_And_FromJLongPtr) {
    int value = 42;
    void* ptr = &value;
    jlong jptr = JniConverter::ToJLongPtr(ptr);
    void* recovered = JniConverter::FromJLongPtr(jptr);
    EXPECT_EQ(ptr, recovered);
}

class JniExceptionTest : public Test {
protected:
    void* result = JniConverter::FromJLongPtr(jptr);
    EXPECT_EQ(ptr, result);
}

class JniExceptionTest : public Test {
protected:
    void SetUp() override {
    }
};

TEST_F(JniExceptionTest, Constructor_WithMessage) {
    JniException ex("Test error");
    EXPECT_STREQ("Test error", ex.what());
    EXPECT_EQ("java/lang/Exception", ex.GetClassName());
}

TEST_F(JniExceptionTest, Constructor_WithClassNameAndMessage) {
    JniException ex("java/lang/NullPointerException", "Null reference");
    EXPECT_STREQ("Null reference", ex.what());
    EXPECT_EQ("java/lang/NullPointerException", ex.GetClassName());
}

class JniMemoryManagerTest : public Test {
protected:
    void SetUp() override {
        manager = JniMemoryManager::GetInstance();
    }
    
    JniMemoryManager* manager;
};

TEST_F(JniMemoryManagerTest, GetInstance_ReturnsNonNull) {
    EXPECT_NE(manager, nullptr);
}

TEST_F(JniMemoryManagerTest, Allocate_And_Deallocate) {
    void* ptr = manager->Allocate(100);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(0u, manager->GetAllocatedCount());
    
    manager->Deallocate(ptr);
}

TEST_F(JniMemoryManagerTest, ToJLong_And_FromJLong) {
    int value = 42;
    void* ptr = &value;
    
    jlong jptr = manager->ToJLong(ptr);
    void* result = manager->FromJLong(jptr);
    
    EXPECT_EQ(ptr, result);
}

TEST_F(JniMemoryManagerTest, SetMemoryLimit) {
    manager->SetMemoryLimit(1024);
    
    void* ptr1 = manager->Allocate(512);
    EXPECT_NE(ptr1, nullptr);
    
    manager->Deallocate(ptr1);
    manager->SetMemoryLimit(0);
}

#else

TEST(JniBridgeSkippedTest, NotAndroidPlatform) {
    GTEST_SKIP() << "JNI bridge tests are skipped on non-Android platforms";
}

#endif

int main(int argc, char** argv) {
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
