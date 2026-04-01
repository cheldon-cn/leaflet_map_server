#include <gtest/gtest.h>
#include "ogc/draw/draw_scope_guard.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/simple2d_engine.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/geometry.h"

using namespace ogc::draw;

class DrawScopeGuardTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(100, 100);
        context = DrawContext::Create(device).release();
    }

    void TearDown() override {
        delete context;
        delete device;
    }

    RasterImageDevice* device = nullptr;
    DrawContext* context = nullptr;
};

TEST_F(DrawScopeGuardTest, Construction) {
    DrawScopeGuard guard(context);
    EXPECT_TRUE(guard.Ok());
}

TEST_F(DrawScopeGuardTest, ConstructionWithNull) {
    DrawScopeGuard guard(nullptr);
    EXPECT_TRUE(guard.Ok());
}

TEST_F(DrawScopeGuardTest, DrawPointWithNullContext) {
    DrawScopeGuard guard(nullptr);
    auto result = guard.DrawPoint(50, 50);
    EXPECT_FALSE(guard.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawScopeGuardTest, DrawLineWithNullContext) {
    DrawScopeGuard guard(nullptr);
    auto result = guard.DrawLine(10, 10, 90, 90);
    EXPECT_FALSE(guard.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawScopeGuardTest, DrawRectWithNullContext) {
    DrawScopeGuard guard(nullptr);
    auto result = guard.DrawRect(10, 10, 80, 80);
    EXPECT_FALSE(guard.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawScopeGuardTest, DrawCircleWithNullContext) {
    DrawScopeGuard guard(nullptr);
    auto result = guard.DrawCircle(50, 50, 25);
    EXPECT_FALSE(guard.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawScopeGuardTest, DrawTextWithNullContext) {
    DrawScopeGuard guard(nullptr);
    auto result = guard.DrawText(10, 50, "Test");
    EXPECT_FALSE(guard.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawScopeGuardTest, Commit) {
    {
        DrawScopeGuard guard(context);
        guard.Commit();
        EXPECT_TRUE(guard.Ok());
    }
}

TEST_F(DrawScopeGuardTest, RollbackOnDestruction) {
    {
        DrawScopeGuard guard(context);
        guard.DrawPoint(50, 50);
    }
}

TEST_F(DrawScopeGuardTest, RollbackCallback) {
    bool callbackCalled = false;
    {
        DrawScopeGuard guard(context);
        guard.SetRollbackCallback([&callbackCalled]() {
            callbackCalled = true;
        });
        guard.DrawPoint(50, 50);
    }
    EXPECT_TRUE(callbackCalled);
}

TEST_F(DrawScopeGuardTest, NoRollbackCallbackOnCommit) {
    bool callbackCalled = false;
    {
        DrawScopeGuard guard(context);
        guard.SetRollbackCallback([&callbackCalled]() {
            callbackCalled = true;
        });
        guard.DrawPoint(50, 50);
        guard.Commit();
    }
    EXPECT_FALSE(callbackCalled);
}

TEST_F(DrawScopeGuardTest, MoveConstructor) {
    DrawScopeGuard guard1(context);
    
    DrawScopeGuard guard2(std::move(guard1));
    EXPECT_TRUE(guard2.Ok());
}

TEST_F(DrawScopeGuardTest, MoveAssignment) {
    DrawScopeGuard guard1(context);
    
    DrawScopeGuard guard2(nullptr);
    guard2 = std::move(guard1);
    EXPECT_TRUE(guard2.Ok());
}

TEST_F(DrawScopeGuardTest, GetLastError) {
    DrawScopeGuard guard(nullptr);
    guard.DrawPoint(50, 50);
    auto error = guard.GetLastError();
    EXPECT_NE(error, DrawResult::kSuccess);
}

TEST_F(DrawScopeGuardTest, DrawGeometryWithNullContext) {
    DrawScopeGuard guard(nullptr);
    auto result = guard.DrawGeometry(nullptr);
    EXPECT_FALSE(guard.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

class DrawSessionTest : public ::testing::Test {
protected:
    void SetUp() override {
        device = new RasterImageDevice(100, 100);
        context = DrawContext::Create(device).release();
    }

    void TearDown() override {
        delete context;
        delete device;
    }

    RasterImageDevice* device = nullptr;
    DrawContext* context = nullptr;
};

TEST_F(DrawSessionTest, Construction) {
    DrawSession session(context);
    EXPECT_FALSE(session.IsActive());
}

TEST_F(DrawSessionTest, ConstructionWithNull) {
    DrawSession session(nullptr);
    EXPECT_FALSE(session.IsActive());
}

TEST_F(DrawSessionTest, BeginWithNullContext) {
    DrawSession session(nullptr);
    auto result = session.Begin();
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, DrawWithoutBegin) {
    DrawSession session(context);
    auto result = session.DrawPoint(50, 50);
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, CancelWithoutBegin) {
    DrawSession session(context);
    session.Cancel();
    EXPECT_FALSE(session.IsActive());
    EXPECT_FALSE(session.Ok());
}

TEST_F(DrawSessionTest, GetResult) {
    DrawSession session(context);
    auto result = session.GetResult();
    EXPECT_EQ(result, DrawResult::kSuccess);
}

TEST_F(DrawSessionTest, GetContext) {
    DrawSession session(context);
    EXPECT_EQ(session.GetContext(), context);
}

TEST_F(DrawSessionTest, GetContextNull) {
    DrawSession session(nullptr);
    EXPECT_EQ(session.GetContext(), nullptr);
}

TEST_F(DrawSessionTest, MoveConstructor) {
    DrawSession session1(context);
    session1.Begin();
    
    DrawSession session2(std::move(session1));
    EXPECT_EQ(session2.GetContext(), context);
}

TEST_F(DrawSessionTest, MoveAssignment) {
    DrawSession session1(context);
    session1.Begin();
    
    DrawSession session2(nullptr);
    session2 = std::move(session1);
    EXPECT_EQ(session2.GetContext(), context);
}

TEST_F(DrawSessionTest, DoubleBegin) {
    DrawSession session(context);
    auto firstResult = session.Begin();
    if (firstResult == DrawResult::kSuccess) {
        auto result = session.Begin();
        EXPECT_FALSE(session.Ok());
        EXPECT_EQ(result, DrawResult::kInvalidState);
    } else {
        EXPECT_NE(firstResult, DrawResult::kSuccess);
    }
}

TEST_F(DrawSessionTest, DrawLineWithoutBegin) {
    DrawSession session(context);
    auto result = session.DrawLine(10, 10, 90, 90);
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, DrawRectWithoutBegin) {
    DrawSession session(context);
    auto result = session.DrawRect(10, 10, 80, 80);
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, DrawCircleWithoutBegin) {
    DrawSession session(context);
    auto result = session.DrawCircle(50, 50, 25);
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, DrawTextWithoutBegin) {
    DrawSession session(context);
    auto result = session.DrawText(10, 50, "Test");
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, DrawPolygonWithoutBegin) {
    DrawSession session(context);
    double x[] = {10, 50, 90, 50};
    double y[] = {50, 10, 50, 90};
    auto result = session.DrawPolygon(x, y, 4);
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, DrawGeometryWithoutBegin) {
    DrawSession session(context);
    auto result = session.DrawGeometry(nullptr);
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}

TEST_F(DrawSessionTest, DrawImageWithoutBegin) {
    DrawSession session(context);
    Image img;
    auto result = session.DrawImage(10, 10, img);
    EXPECT_FALSE(session.Ok());
    EXPECT_EQ(result, DrawResult::kInvalidState);
}
