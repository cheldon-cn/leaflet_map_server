#include <gtest/gtest.h>
#include <gmock/gmock.h>

#ifdef __ANDROID__

#include "android_adapter/android_engine.h"
#include "android_adapter/android_touch_handler.h"
#include "android_adapter/android_lifecycle.h"
#include "android_adapter/android_permission.h"
#include "android_adapter/android_platform.h"

using namespace ogc::android;
using namespace testing;

class AndroidEngineTest : public Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(AndroidEngineTest, GetInstance_ReturnsNonNull) {
    auto* instance = AndroidEngine::GetInstance();
    EXPECT_NE(instance, nullptr);
}

TEST_F(AndroidEngineTest, IsInitialized_InitiallyFalse) {
    auto* instance = AndroidEngine::GetInstance();
    EXPECT_FALSE(instance->IsInitialized());
}

class AndroidTouchHandlerTest : public Test {
protected:
    void SetUp() override {
        handler = AndroidTouchHandler::GetInstance();
    }
    
    AndroidTouchHandler* handler;
};

TEST_F(AndroidTouchHandlerTest, GetInstance_ReturnsNonNull) {
    EXPECT_NE(handler, nullptr);
}

TEST_F(AndroidTouchHandlerTest, InitialGestureState_IsDefault) {
    const auto& state = handler->GetGestureState();
    EXPECT_FALSE(state.isPinching);
    EXPECT_FALSE(state.isPanning);
    EXPECT_FALSE(state.isRotating);
    EXPECT_FLOAT_EQ(state.scale, 1.0f);
}

TEST_F(AndroidTouchHandlerTest, ResetGestureState_ResetsAllValues) {
    handler->ResetGestureState();
    const auto& state = handler->GetGestureState();
    EXPECT_FALSE(state.isPinching);
    EXPECT_FALSE(state.isPanning);
    EXPECT_FALSE(state.isRotating);
}

class AndroidLifecycleTest : public Test {
protected:
    void SetUp() override {
        lifecycle = AndroidLifecycle::GetInstance();
    }
    
    AndroidLifecycle* lifecycle;
};

TEST_F(AndroidLifecycleTest, GetInstance_ReturnsNonNull) {
    EXPECT_NE(lifecycle, nullptr);
}

TEST_F(AndroidLifecycleTest, InitialState_IsUnknown) {
    EXPECT_EQ(lifecycle->GetState(), LifecycleState::kUnknown);
}

TEST_F(AndroidLifecycleTest, IsRunning_InitiallyFalse) {
    EXPECT_FALSE(lifecycle->IsRunning());
}

class AndroidPermissionTest : public Test {
protected:
    void SetUp() override {
        permission = AndroidPermission::GetInstance();
    }
    
    AndroidPermission* permission;
};

TEST_F(AndroidPermissionTest, GetInstance_ReturnsNonNull) {
    EXPECT_NE(permission, nullptr);
}

TEST_F(AndroidPermissionTest, GetPermissionName_Location) {
    std::string name = permission->GetPermissionName(PermissionType::kLocation);
    EXPECT_EQ(name, "android.permission.ACCESS_FINE_LOCATION");
}

TEST_F(AndroidPermissionTest, GetPermissionName_Storage) {
    std::string name = permission->GetPermissionName(PermissionType::kStorage);
    EXPECT_EQ(name, "android.permission.READ_EXTERNAL_STORAGE");
}

class AndroidPlatformTest : public Test {
protected:
    void SetUp() override {
        platform = AndroidPlatform::GetInstance();
    }
    
    AndroidPlatform* platform;
};

TEST_F(AndroidPlatformTest, GetInstance_ReturnsNonNull) {
    EXPECT_NE(platform, nullptr);
}

TEST_F(AndroidPlatformTest, GetDeviceInfo_ReturnsValidStruct) {
    const auto& info = platform->GetDeviceInfo();
    EXPECT_FALSE(info.manufacturer.empty() && info.model.empty());
}

#else

TEST(AndroidAdapterSkippedTest, NotAndroidPlatform) {
    GTEST_SKIP() << "Android adapter tests are skipped on non-Android platforms";
}

#endif

int main(int argc, char** argv) {
    InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
