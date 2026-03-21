#include "gtest/gtest.h"
#include "ogc/feature/datetime.h"
#include <ctime>

using namespace ogc;

TEST(CNDateTime, DefaultConstructor) {
    CNDateTime dt;
    EXPECT_EQ(dt.year, 0);
    EXPECT_EQ(dt.month, 0);
    EXPECT_EQ(dt.day, 0);
    EXPECT_EQ(dt.hour, 0);
    EXPECT_EQ(dt.minute, 0);
    EXPECT_EQ(dt.second, 0);
}

TEST(CNDateTime, ParamConstructor) {
    CNDateTime dt(2024, 3, 15, 10, 30, 45);
    EXPECT_EQ(dt.year, 2024);
    EXPECT_EQ(dt.month, 3);
    EXPECT_EQ(dt.day, 15);
    EXPECT_EQ(dt.hour, 10);
    EXPECT_EQ(dt.minute, 30);
    EXPECT_EQ(dt.second, 45);
}

TEST(CNDateTime, Timestamp) {
    CNDateTime dt(2024, 3, 15, 0, 0, 0);
    int64_t ts = dt.ToTimestamp();
    EXPECT_GT(ts, 0);
}

TEST(CNDateTime, Comparison) {
    CNDateTime dt1(2024, 3, 15, 10, 30, 0);
    CNDateTime dt2(2024, 3, 15, 10, 30, 0);
    CNDateTime dt3(2024, 3, 15, 11, 0, 0);
    
    EXPECT_TRUE(dt1 == dt2);
    EXPECT_FALSE(dt1 == dt3);
    EXPECT_TRUE(dt1 < dt3);
    EXPECT_FALSE(dt3 < dt1);
}
