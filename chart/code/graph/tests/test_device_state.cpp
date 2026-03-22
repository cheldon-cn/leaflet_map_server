#include <gtest/gtest.h>
#include "ogc/draw/device_state.h"

using namespace ogc::draw;

class DeviceStateTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
    
    void TearDown() override {
    }
};

TEST_F(DeviceStateTest, EnumValues) {
    EXPECT_EQ(static_cast<int>(DeviceState::kCreated), 0);
    EXPECT_EQ(static_cast<int>(DeviceState::kInitialized), 1);
    EXPECT_EQ(static_cast<int>(DeviceState::kActive), 2);
    EXPECT_EQ(static_cast<int>(DeviceState::kPaused), 3);
    EXPECT_EQ(static_cast<int>(DeviceState::kError), 4);
    EXPECT_EQ(static_cast<int>(DeviceState::kClosed), 5);
}

TEST_F(DeviceStateTest, StateOrder) {
    EXPECT_LT(static_cast<int>(DeviceState::kCreated), static_cast<int>(DeviceState::kInitialized));
    EXPECT_LT(static_cast<int>(DeviceState::kInitialized), static_cast<int>(DeviceState::kActive));
    EXPECT_LT(static_cast<int>(DeviceState::kActive), static_cast<int>(DeviceState::kPaused));
    EXPECT_LT(static_cast<int>(DeviceState::kPaused), static_cast<int>(DeviceState::kError));
    EXPECT_LT(static_cast<int>(DeviceState::kError), static_cast<int>(DeviceState::kClosed));
}

TEST_F(DeviceStateTest, AllStatesDistinct) {
    EXPECT_NE(static_cast<int>(DeviceState::kCreated), static_cast<int>(DeviceState::kInitialized));
    EXPECT_NE(static_cast<int>(DeviceState::kInitialized), static_cast<int>(DeviceState::kActive));
    EXPECT_NE(static_cast<int>(DeviceState::kActive), static_cast<int>(DeviceState::kPaused));
    EXPECT_NE(static_cast<int>(DeviceState::kPaused), static_cast<int>(DeviceState::kError));
    EXPECT_NE(static_cast<int>(DeviceState::kError), static_cast<int>(DeviceState::kClosed));
}
