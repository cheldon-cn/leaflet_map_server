#include <gtest/gtest.h>
#include "ogc/draw/draw_version.h"

using namespace ogc::draw;

class VersionTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

TEST_F(VersionTest, CurrentVersion) {
    Version v = Version::Current();
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 0);
    EXPECT_EQ(v.patch, 0);
}

TEST_F(VersionTest, FromString) {
    Version v1 = Version::FromString("1.2.3");
    EXPECT_EQ(v1.major, 1);
    EXPECT_EQ(v1.minor, 2);
    EXPECT_EQ(v1.patch, 3);
    
    Version v2 = Version::FromString("2.0");
    EXPECT_EQ(v2.major, 2);
    EXPECT_EQ(v2.minor, 0);
    EXPECT_EQ(v2.patch, 0);
    
    Version v3 = Version::FromString("3");
    EXPECT_EQ(v3.major, 3);
    EXPECT_EQ(v3.minor, 0);
    EXPECT_EQ(v3.patch, 0);
}

TEST_F(VersionTest, ToString) {
    Version v{1, 2, 3};
    EXPECT_EQ(v.ToString(), "1.2.3");
    
    Version v2{2, 0, 0};
    EXPECT_EQ(v2.ToString(), "2.0.0");
}

TEST_F(VersionTest, Equality) {
    Version v1{1, 2, 3};
    Version v2{1, 2, 3};
    Version v3{1, 2, 4};
    
    EXPECT_TRUE(v1 == v2);
    EXPECT_FALSE(v1 == v3);
    EXPECT_TRUE(v1 != v3);
    EXPECT_FALSE(v1 != v2);
}

TEST_F(VersionTest, Comparison) {
    Version v1{1, 0, 0};
    Version v2{1, 0, 1};
    Version v3{1, 1, 0};
    Version v4{2, 0, 0};
    
    EXPECT_TRUE(v1 < v2);
    EXPECT_TRUE(v2 < v3);
    EXPECT_TRUE(v3 < v4);
    
    EXPECT_TRUE(v1 <= v1);
    EXPECT_TRUE(v1 <= v2);
    EXPECT_FALSE(v2 <= v1);
    
    EXPECT_TRUE(v4 > v3);
    EXPECT_TRUE(v3 > v2);
    
    EXPECT_TRUE(v1 >= v1);
    EXPECT_TRUE(v2 >= v1);
    EXPECT_FALSE(v1 >= v2);
}

TEST_F(VersionTest, Compatibility) {
    Version v1{1, 0, 0};
    Version v2{1, 0, 1};
    Version v3{1, 1, 0};
    Version v4{2, 0, 0};
    
    EXPECT_TRUE(v1.IsCompatible(v1));
    EXPECT_TRUE(v2.IsCompatible(v1));
    EXPECT_TRUE(v3.IsCompatible(v1));
    EXPECT_FALSE(v4.IsCompatible(v1));
    
    EXPECT_TRUE(v1.IsCompatible(v2));
    EXPECT_FALSE(v1.IsCompatible(v3));
}

class VersionManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        auto& vm = VersionManager::Instance();
        vm.RegisterDeprecation("OldFeature1", Version{0, 9, 0}, Version{1, 1, 0});
        vm.RegisterDeprecation("OldFeature2", Version{1, 0, 0}, Version{2, 0, 0});
    }
};

TEST_F(VersionManagerTest, Singleton) {
    auto& vm1 = VersionManager::Instance();
    auto& vm2 = VersionManager::Instance();
    EXPECT_EQ(&vm1, &vm2);
}

TEST_F(VersionManagerTest, GetLibraryVersion) {
    auto& vm = VersionManager::Instance();
    Version v = vm.GetLibraryVersion();
    EXPECT_EQ(v.major, 1);
    EXPECT_EQ(v.minor, 0);
    EXPECT_EQ(v.patch, 0);
}

TEST_F(VersionManagerTest, IsCompatible) {
    auto& vm = VersionManager::Instance();
    
    EXPECT_TRUE(vm.IsCompatible(Version{1, 0, 0}));
    EXPECT_TRUE(vm.IsCompatible(Version{1, 0, 5}));
    EXPECT_FALSE(vm.IsCompatible(Version{2, 0, 0}));
    EXPECT_FALSE(vm.IsCompatible(Version{1, 1, 0}));
}

TEST_F(VersionManagerTest, Deprecation) {
    auto& vm = VersionManager::Instance();
    
    EXPECT_TRUE(vm.IsDeprecated("OldFeature1"));
    EXPECT_TRUE(vm.IsDeprecated("OldFeature2"));
    EXPECT_FALSE(vm.IsDeprecated("NonExistentFeature"));
}

TEST_F(VersionManagerTest, Removed) {
    auto& vm = VersionManager::Instance();
    
    EXPECT_FALSE(vm.IsRemoved("OldFeature1"));
    EXPECT_FALSE(vm.IsRemoved("OldFeature2"));
}

TEST_F(VersionManagerTest, DeprecationMessage) {
    auto& vm = VersionManager::Instance();
    
    std::string msg = vm.GetDeprecationMessage("OldFeature1");
    EXPECT_FALSE(msg.empty());
    EXPECT_TRUE(msg.find("OldFeature1") != std::string::npos);
}

TEST_F(VersionManagerTest, WarningCallback) {
    auto& vm = VersionManager::Instance();
    
    std::string warningMsg;
    vm.SetWarningCallback([&warningMsg](const std::string& msg) {
        warningMsg = msg;
    });
    
    vm.CheckCompatibility(Version{2, 0, 0}, "TestPlugin");
    EXPECT_FALSE(warningMsg.empty());
    EXPECT_TRUE(warningMsg.find("TestPlugin") != std::string::npos);
}

TEST_F(VersionManagerTest, CheckCompatibilityCompatible) {
    auto& vm = VersionManager::Instance();
    
    std::string warningMsg;
    vm.SetWarningCallback([&warningMsg](const std::string& msg) {
        warningMsg = msg;
    });
    
    vm.CheckCompatibility(Version{1, 0, 0}, "CompatiblePlugin");
    EXPECT_TRUE(warningMsg.empty());
}
