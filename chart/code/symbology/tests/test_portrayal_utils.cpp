#include <gtest/gtest.h>
#include "ogc/portrayal/utils/string_utils.h"
#include "ogc/portrayal/utils/file_utils.h"
#include "ogc/portrayal/utils/string_pool.h"

using namespace ogc::portrayal::utils;

class StringUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(StringUtilsTest, Trim) {
    EXPECT_EQ(StringUtils::Trim("  hello  "), "hello");
    EXPECT_EQ(StringUtils::Trim("\t\nhello\r\n"), "hello");
    EXPECT_EQ(StringUtils::Trim("hello"), "hello");
    EXPECT_EQ(StringUtils::Trim("  "), "");
    EXPECT_EQ(StringUtils::Trim(""), "");
}

TEST_F(StringUtilsTest, ToLower) {
    EXPECT_EQ(StringUtils::ToLower("HELLO"), "hello");
    EXPECT_EQ(StringUtils::ToLower("Hello World"), "hello world");
    EXPECT_EQ(StringUtils::ToLower("hello"), "hello");
    EXPECT_EQ(StringUtils::ToLower(""), "");
}

TEST_F(StringUtilsTest, ToUpper) {
    EXPECT_EQ(StringUtils::ToUpper("hello"), "HELLO");
    EXPECT_EQ(StringUtils::ToUpper("Hello World"), "HELLO WORLD");
    EXPECT_EQ(StringUtils::ToUpper("HELLO"), "HELLO");
    EXPECT_EQ(StringUtils::ToUpper(""), "");
}

TEST_F(StringUtilsTest, Split) {
    auto parts = StringUtils::Split("a,b,c", ',');
    EXPECT_EQ(parts.size(), 3u);
    EXPECT_EQ(parts[0], "a");
    EXPECT_EQ(parts[1], "b");
    EXPECT_EQ(parts[2], "c");
}

TEST_F(StringUtilsTest, SplitSingleElement) {
    auto parts = StringUtils::Split("hello", ',');
    EXPECT_EQ(parts.size(), 1u);
    EXPECT_EQ(parts[0], "hello");
}

TEST_F(StringUtilsTest, SplitEmptyString) {
    auto parts = StringUtils::Split("", ',');
    EXPECT_EQ(parts.size(), 1u);
    EXPECT_EQ(parts[0], "");
}

TEST_F(StringUtilsTest, Join) {
    std::vector<std::string> parts = {"a", "b", "c"};
    EXPECT_EQ(StringUtils::Join(parts, ","), "a,b,c");
}

TEST_F(StringUtilsTest, JoinEmpty) {
    std::vector<std::string> parts;
    EXPECT_EQ(StringUtils::Join(parts, ","), "");
}

TEST_F(StringUtilsTest, StartsWith) {
    EXPECT_TRUE(StringUtils::StartsWith("hello world", "hello"));
    EXPECT_FALSE(StringUtils::StartsWith("hello world", "world"));
    EXPECT_FALSE(StringUtils::StartsWith("hi", "hello"));
}

TEST_F(StringUtilsTest, EndsWith) {
    EXPECT_TRUE(StringUtils::EndsWith("hello world", "world"));
    EXPECT_FALSE(StringUtils::EndsWith("hello world", "hello"));
    EXPECT_FALSE(StringUtils::EndsWith("hi", "hello"));
}

TEST_F(StringUtilsTest, ConvertEncodingSame) {
    std::string input = "hello";
    EXPECT_EQ(StringUtils::ConvertEncoding(input, "UTF-8", "UTF-8"), input);
}

class FileUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(FileUtilsTest, GetDirectory) {
    EXPECT_EQ(FileUtils::GetDirectory("/path/to/file.txt"), "/path/to");
    EXPECT_EQ(FileUtils::GetDirectory("C:\\path\\to\\file.txt"), "C:\\path\\to");
    EXPECT_EQ(FileUtils::GetDirectory("file.txt"), "");
}

TEST_F(FileUtilsTest, GetFileName) {
    EXPECT_EQ(FileUtils::GetFileName("/path/to/file.txt"), "file.txt");
    EXPECT_EQ(FileUtils::GetFileName("file.txt"), "file.txt");
}

TEST_F(FileUtilsTest, GetFileExtension) {
    EXPECT_EQ(FileUtils::GetFileExtension("/path/to/file.txt"), "txt");
    EXPECT_EQ(FileUtils::GetFileExtension("file.svg"), "svg");
    EXPECT_EQ(FileUtils::GetFileExtension("noext"), "");
    EXPECT_EQ(FileUtils::GetFileExtension(".hidden"), "");
}

TEST_F(FileUtilsTest, CombinePath) {
    EXPECT_EQ(FileUtils::CombinePath("/path/to", "file.txt"), "/path/to/file.txt");
    EXPECT_EQ(FileUtils::CombinePath("/path/to/", "file.txt"), "/path/to/file.txt");
    EXPECT_EQ(FileUtils::CombinePath("", "file.txt"), "file.txt");
    EXPECT_EQ(FileUtils::CombinePath("/path/to", ""), "/path/to");
}

TEST_F(FileUtilsTest, ExistsNonExistent) {
    EXPECT_FALSE(FileUtils::Exists("/nonexistent/path/file.txt"));
}

TEST_F(FileUtilsTest, IsFileNonExistent) {
    EXPECT_FALSE(FileUtils::IsFile("/nonexistent/path/file.txt"));
}

TEST_F(FileUtilsTest, IsDirectoryNonExistent) {
    EXPECT_FALSE(FileUtils::IsDirectory("/nonexistent/path"));
}

class StringPoolTest : public ::testing::Test {
protected:
    void SetUp() override {}
    void TearDown() override {}
};

TEST_F(StringPoolTest, InternBasic) {
    StringPool pool;
    const std::string& s1 = pool.Intern("hello");
    const std::string& s2 = pool.Intern("hello");
    EXPECT_EQ(s1, s2);
    EXPECT_EQ(&s1, &s2);
}

TEST_F(StringPoolTest, InternDifferentStrings) {
    StringPool pool;
    const std::string& s1 = pool.Intern("hello");
    const std::string& s2 = pool.Intern("world");
    EXPECT_NE(s1, s2);
    EXPECT_NE(&s1, &s2);
}

TEST_F(StringPoolTest, Size) {
    StringPool pool;
    EXPECT_EQ(pool.Size(), 0u);
    pool.Intern("hello");
    EXPECT_EQ(pool.Size(), 1u);
    pool.Intern("hello");
    EXPECT_EQ(pool.Size(), 1u);
    pool.Intern("world");
    EXPECT_EQ(pool.Size(), 2u);
}

TEST_F(StringPoolTest, Clear) {
    StringPool pool;
    pool.Intern("hello");
    pool.Intern("world");
    EXPECT_EQ(pool.Size(), 2u);
    pool.Clear();
    EXPECT_EQ(pool.Size(), 0u);
}

TEST_F(StringPoolTest, GetMemoryUsage) {
    StringPool pool;
    pool.Intern("hello");
    size_t usage = pool.GetMemoryUsage();
    EXPECT_GT(usage, 0u);
}
