#include <gtest/gtest.h>
#include "ogc/draw/library_compatibility.h"

using namespace ogc::draw;

class LibraryVersionTest : public ::testing::Test {
protected:
    void SetUp() override {
    }
};

class LibraryCompatibilityTest : public ::testing::Test {
protected:
    void SetUp() override {
        LibraryCompatibility::ResetMinimumVersions();
        LibraryCompatibility::SetEnableLogging(false);
    }
    
    void TearDown() override {
        LibraryCompatibility::ResetMinimumVersions();
    }
};

TEST_F(LibraryVersionTest, DefaultConstruction) {
    LibraryVersion version;
    EXPECT_EQ(version.major, 0);
    EXPECT_EQ(version.minor, 0);
    EXPECT_EQ(version.patch, 0);
    EXPECT_TRUE(version.name.empty());
}

TEST_F(LibraryVersionTest, ParameterizedConstruction) {
    LibraryVersion version("TestLib", 1, 2, 3);
    EXPECT_EQ(version.name, "TestLib");
    EXPECT_EQ(version.major, 1);
    EXPECT_EQ(version.minor, 2);
    EXPECT_EQ(version.patch, 3);
}

TEST_F(LibraryVersionTest, ToString) {
    LibraryVersion version("Test", 2, 5, 8);
    EXPECT_EQ(version.ToString(), "2.5.8");
}

TEST_F(LibraryVersionTest, IsValid) {
    LibraryVersion invalid;
    EXPECT_FALSE(invalid.IsValid());
    
    LibraryVersion valid("Test", 1, 0, 0);
    EXPECT_TRUE(valid.IsValid());
}

TEST_F(LibraryVersionTest, ComparisonOperators) {
    LibraryVersion v1("Test", 1, 0, 0);
    LibraryVersion v2("Test", 1, 0, 1);
    LibraryVersion v3("Test", 1, 1, 0);
    LibraryVersion v4("Test", 2, 0, 0);
    LibraryVersion v5("Test", 1, 0, 0);
    
    EXPECT_TRUE(v1 < v2);
    EXPECT_TRUE(v2 < v3);
    EXPECT_TRUE(v3 < v4);
    EXPECT_TRUE(v1 >= v5);
    EXPECT_TRUE(v1 == v5);
    EXPECT_FALSE(v1 == v2);
    EXPECT_TRUE(v1 >= v1);
}

TEST_F(LibraryVersionTest, Parse) {
    LibraryVersion v1 = LibraryVersion::Parse("1.2.3", "Test");
    EXPECT_EQ(v1.name, "Test");
    EXPECT_EQ(v1.major, 1);
    EXPECT_EQ(v1.minor, 2);
    EXPECT_EQ(v1.patch, 3);
    
    LibraryVersion v2 = LibraryVersion::Parse("10.20", "Test2");
    EXPECT_EQ(v2.major, 10);
    EXPECT_EQ(v2.minor, 20);
    EXPECT_EQ(v2.patch, 0);
    
    LibraryVersion v3 = LibraryVersion::Parse("5", "Test3");
    EXPECT_EQ(v3.major, 5);
    EXPECT_EQ(v3.minor, 0);
    EXPECT_EQ(v3.patch, 0);
}

TEST_F(LibraryCompatibilityTest, GetQtVersion) {
    auto version = LibraryCompatibility::GetQtVersion();
    EXPECT_EQ(version.name, "Qt");
}

TEST_F(LibraryCompatibilityTest, GetCairoVersion) {
    auto version = LibraryCompatibility::GetCairoVersion();
    EXPECT_EQ(version.name, "Cairo");
}

TEST_F(LibraryCompatibilityTest, GetPodofoVersion) {
    auto version = LibraryCompatibility::GetPodofoVersion();
    EXPECT_EQ(version.name, "podofo");
}

TEST_F(LibraryCompatibilityTest, GetOpenGLVersion) {
    auto version = LibraryCompatibility::GetOpenGLVersion();
    EXPECT_EQ(version.name, "OpenGL");
}

TEST_F(LibraryCompatibilityTest, GetFreetypeVersion) {
    auto version = LibraryCompatibility::GetFreetypeVersion();
    EXPECT_EQ(version.name, "FreeType");
}

TEST_F(LibraryCompatibilityTest, GetZlibVersion) {
    auto version = LibraryCompatibility::GetZlibVersion();
    EXPECT_EQ(version.name, "zlib");
}

TEST_F(LibraryCompatibilityTest, GetLibPngVersion) {
    auto version = LibraryCompatibility::GetLibPngVersion();
    EXPECT_EQ(version.name, "libpng");
}

TEST_F(LibraryCompatibilityTest, GetLibJpegVersion) {
    auto version = LibraryCompatibility::GetLibJpegVersion();
    EXPECT_EQ(version.name, "libjpeg");
}

TEST_F(LibraryCompatibilityTest, GetLibraryVersion) {
    auto version = LibraryCompatibility::GetLibraryVersion("Qt");
    EXPECT_EQ(version.name, "Qt");
}

TEST_F(LibraryCompatibilityTest, SetMinimumVersion) {
    LibraryVersion customMin("CustomLib", 5, 0, 0);
    LibraryCompatibility::SetMinimumVersion("CustomLib", customMin);
    
    auto retrieved = LibraryCompatibility::GetMinimumVersion("CustomLib");
    EXPECT_EQ(retrieved.major, 5);
    EXPECT_EQ(retrieved.minor, 0);
    EXPECT_EQ(retrieved.patch, 0);
}

TEST_F(LibraryCompatibilityTest, GetMinimumVersionDefault) {
    auto version = LibraryCompatibility::GetMinimumVersion("Qt");
    EXPECT_TRUE(version.IsValid());
    EXPECT_EQ(version.name, "Qt");
}

TEST_F(LibraryCompatibilityTest, GetMinimumVersionUnknown) {
    auto version = LibraryCompatibility::GetMinimumVersion("UnknownLibrary");
    EXPECT_FALSE(version.IsValid());
}

TEST_F(LibraryCompatibilityTest, ResetMinimumVersions) {
    LibraryVersion custom("Qt", 99, 99, 99);
    LibraryCompatibility::SetMinimumVersion("Qt", custom);
    
    LibraryCompatibility::ResetMinimumVersions();
    
    auto version = LibraryCompatibility::GetMinimumVersion("Qt");
    EXPECT_NE(version.major, 99);
}

TEST_F(LibraryCompatibilityTest, CheckMinimumRequirements) {
    LibraryCompatibility::CheckMinimumRequirements();
}

TEST_F(LibraryCompatibilityTest, GetCompatibilityIssues) {
    auto issues = LibraryCompatibility::GetCompatibilityIssues();
}

TEST_F(LibraryCompatibilityTest, GetDetailedCompatibilityIssues) {
    auto issues = LibraryCompatibility::GetDetailedCompatibilityIssues();
    for (const auto& issue : issues) {
        EXPECT_FALSE(issue.libraryName.empty());
        EXPECT_TRUE(issue.detected.IsValid() || !issue.required.IsValid());
    }
}

TEST_F(LibraryCompatibilityTest, GetCompatibilityReport) {
    std::string report = LibraryCompatibility::GetCompatibilityReport();
    EXPECT_FALSE(report.empty());
    EXPECT_TRUE(report.find("Library Compatibility Report") != std::string::npos);
}

TEST_F(LibraryCompatibilityTest, GetDetailedCompatibilityReport) {
    std::string report = LibraryCompatibility::GetDetailedCompatibilityReport();
    EXPECT_FALSE(report.empty());
    EXPECT_TRUE(report.find("Minimum Required Versions") != std::string::npos);
    EXPECT_TRUE(report.find("System Information") != std::string::npos);
}

TEST_F(LibraryCompatibilityTest, IsQtAvailable) {
    LibraryCompatibility::IsQtAvailable();
}

TEST_F(LibraryCompatibilityTest, IsCairoAvailable) {
    LibraryCompatibility::IsCairoAvailable();
}

TEST_F(LibraryCompatibilityTest, IsPodofoAvailable) {
    LibraryCompatibility::IsPodofoAvailable();
}

TEST_F(LibraryCompatibilityTest, IsOpenGLAvailable) {
    LibraryCompatibility::IsOpenGLAvailable();
}

TEST_F(LibraryCompatibilityTest, IsFreetypeAvailable) {
    LibraryCompatibility::IsFreetypeAvailable();
}

TEST_F(LibraryCompatibilityTest, GetAvailableLibraries) {
    auto libraries = LibraryCompatibility::GetAvailableLibraries();
}

TEST_F(LibraryCompatibilityTest, GetSystemInfo) {
    std::string info = LibraryCompatibility::GetSystemInfo();
    EXPECT_FALSE(info.empty());
    EXPECT_TRUE(info.find("Platform:") != std::string::npos);
    EXPECT_TRUE(info.find("Compiler:") != std::string::npos);
    EXPECT_TRUE(info.find("Build:") != std::string::npos);
}

TEST_F(LibraryCompatibilityTest, SetEnableLogging) {
    LibraryCompatibility::SetEnableLogging(true);
    EXPECT_TRUE(LibraryCompatibility::IsLoggingEnabled());
    
    LibraryCompatibility::SetEnableLogging(false);
    EXPECT_FALSE(LibraryCompatibility::IsLoggingEnabled());
}

TEST_F(LibraryCompatibilityTest, CompatibilityIssueToString) {
    CompatibilityIssue issue;
    issue.libraryName = "TestLib";
    issue.detected = LibraryVersion("TestLib", 1, 0, 0);
    issue.required = LibraryVersion("TestLib", 2, 0, 0);
    issue.isCritical = true;
    issue.description = "Version too old";
    
    std::string str = issue.ToString();
    EXPECT_TRUE(str.find("TestLib") != std::string::npos);
    EXPECT_TRUE(str.find("1.0.0") != std::string::npos);
    EXPECT_TRUE(str.find("2.0.0") != std::string::npos);
    EXPECT_TRUE(str.find("CRITICAL") != std::string::npos);
}

TEST_F(LibraryCompatibilityTest, VersionCaching) {
    auto v1 = LibraryCompatibility::GetQtVersion();
    auto v2 = LibraryCompatibility::GetQtVersion();
    
    EXPECT_EQ(v1.major, v2.major);
    EXPECT_EQ(v1.minor, v2.minor);
    EXPECT_EQ(v1.patch, v2.patch);
}

TEST_F(LibraryCompatibilityTest, CustomLibraryCompatibility) {
    LibraryVersion customMin("CustomLib", 10, 0, 0);
    LibraryCompatibility::SetMinimumVersion("CustomLib", customMin);
    
    auto minVersion = LibraryCompatibility::GetMinimumVersion("CustomLib");
    EXPECT_EQ(minVersion.major, 10);
}

TEST_F(LibraryCompatibilityTest, MultipleLibrariesAvailable) {
    auto available = LibraryCompatibility::GetAvailableLibraries();
    EXPECT_GE(available.size(), 0u);
}

TEST_F(LibraryCompatibilityTest, ReportContainsAllSections) {
    std::string report = LibraryCompatibility::GetDetailedCompatibilityReport();
    
    EXPECT_TRUE(report.find("Library Compatibility Report") != std::string::npos);
    EXPECT_TRUE(report.find("Detected Library Versions") != std::string::npos);
    EXPECT_TRUE(report.find("Minimum Required Versions") != std::string::npos);
    EXPECT_TRUE(report.find("System Information") != std::string::npos);
}
