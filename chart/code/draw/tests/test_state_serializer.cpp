#include <gtest/gtest.h>
#include "ogc/draw/state_serializer.h"
#include "ogc/draw/draw_engine.h"
#include "ogc/draw/transform_matrix.h"
#include "ogc/geom/geometry.h"

using namespace ogc::draw;

class MockDrawEngine : public DrawEngine {
public:
    MockDrawEngine() 
        : m_transform(TransformMatrix::Identity())
        , m_opacity(1.0)
        , m_tolerance(0.0)
        , m_antialiasing(true)
        , m_active(false) {}

    std::string GetName() const override { return "MockEngine"; }
    EngineType GetType() const override { return EngineType::kSimple2D; }
    DrawDevice* GetDevice() const override { return nullptr; }

    DrawResult Begin() override { m_active = true; return DrawResult::kSuccess; }
    void End() override { m_active = false; }
    bool IsActive() const override { return m_active; }

    DrawResult DrawPoints(const double*, const double*, int, const DrawStyle&) override { return DrawResult::kSuccess; }
    DrawResult DrawLines(const double*, const double*, const double*, const double*, int, const DrawStyle&) override { return DrawResult::kSuccess; }
    DrawResult DrawLineString(const double*, const double*, int, const DrawStyle&) override { return DrawResult::kSuccess; }
    DrawResult DrawPolygon(const double*, const double*, int, const DrawStyle&, bool) override { return DrawResult::kSuccess; }
    DrawResult DrawRect(double, double, double, double, const DrawStyle&, bool) override { return DrawResult::kSuccess; }
    DrawResult DrawCircle(double, double, double, const DrawStyle&, bool) override { return DrawResult::kSuccess; }
    DrawResult DrawEllipse(double, double, double, double, const DrawStyle&, bool) override { return DrawResult::kSuccess; }
    DrawResult DrawArc(double, double, double, double, double, double, const DrawStyle&) override { return DrawResult::kSuccess; }
    DrawResult DrawText(double, double, const std::string&, const Font&, const Color&) override { return DrawResult::kSuccess; }
    DrawResult DrawImage(double, double, const Image&, double, double) override { return DrawResult::kSuccess; }
    DrawResult DrawImageRect(double, double, double, double, const Image&) override { return DrawResult::kSuccess; }
    DrawResult DrawGeometry(const ogc::Geometry*, const DrawStyle&) override { return DrawResult::kSuccess; }

    void SetTransform(const TransformMatrix& matrix) override { m_transform = matrix; }
    TransformMatrix GetTransform() const override { return m_transform; }
    void ResetTransform() override { m_transform = TransformMatrix::Identity(); }

    void SetClipRect(double x, double y, double w, double h) override {
        m_clipRegion = Region(Rect(x, y, w, h));
    }
    void SetClipRegion(const Region& region) override { m_clipRegion = region; }
    void ResetClip() override { m_clipRegion.Clear(); }
    Region GetClipRegion() const override { return m_clipRegion; }

    void Save(StateFlags) override {}
    void Restore() override {}

    void SetOpacity(double opacity) override { m_opacity = opacity; }
    double GetOpacity() const override { return m_opacity; }

    void SetTolerance(double tolerance) override { m_tolerance = tolerance; }
    double GetTolerance() const override { return m_tolerance; }

    void SetAntialiasingEnabled(bool enabled) override { m_antialiasing = enabled; }
    bool IsAntialiasingEnabled() const override { return m_antialiasing; }

    TextMetrics MeasureText(const std::string&, const Font&) override { return TextMetrics(); }

    void Clear(const Color&) override {}
    void Flush() override {}

private:
    TransformMatrix m_transform;
    Region m_clipRegion;
    double m_opacity;
    double m_tolerance;
    bool m_antialiasing;
    bool m_active;
};

TEST(StateSerializerTest, CaptureNullEngine) {
    EngineState state = StateSerializer::Capture(nullptr);
    
    EXPECT_TRUE(state.transform.IsIdentity());
    EXPECT_TRUE(state.clipRegion.IsEmpty());
    EXPECT_DOUBLE_EQ(state.opacity, 1.0);
    EXPECT_TRUE(state.antialiasing);
}

TEST(StateSerializerTest, CaptureEngineState) {
    MockDrawEngine engine;
    engine.Begin();
    
    engine.SetTransform(TransformMatrix::Translate(10.0, 20.0));
    engine.SetClipRect(0.0, 0.0, 100.0, 100.0);
    engine.SetOpacity(0.5);
    engine.SetTolerance(0.01);
    engine.SetAntialiasingEnabled(false);
    
    EngineState state = StateSerializer::Capture(&engine);
    
    EXPECT_FALSE(state.transform.IsIdentity());
    EXPECT_FALSE(state.clipRegion.IsEmpty());
    EXPECT_DOUBLE_EQ(state.opacity, 0.5);
    EXPECT_DOUBLE_EQ(state.tolerance, 0.01);
    EXPECT_FALSE(state.antialiasing);
}

TEST(StateSerializerTest, RestoreNullEngine) {
    EngineState state;
    state.opacity = 0.5;
    
    StateSerializer::Restore(nullptr, state);
}

TEST(StateSerializerTest, RestoreEngineState) {
    MockDrawEngine engine;
    engine.Begin();
    
    EngineState state;
    state.transform = TransformMatrix::Translate(50.0, 100.0);
    state.clipRegion = Region(Rect(10.0, 10.0, 200.0, 200.0));
    state.opacity = 0.75;
    state.tolerance = 0.005;
    state.antialiasing = false;
    state.savedFlags = static_cast<StateFlags>(StateFlag::kAll);
    
    StateSerializer::Restore(&engine, state);
    
    TransformMatrix restoredTransform = engine.GetTransform();
    EXPECT_DOUBLE_EQ(restoredTransform.m[0][2], 50.0);
    EXPECT_DOUBLE_EQ(restoredTransform.m[1][2], 100.0);
    
    EXPECT_DOUBLE_EQ(engine.GetOpacity(), 0.75);
    EXPECT_DOUBLE_EQ(engine.GetTolerance(), 0.005);
    EXPECT_FALSE(engine.IsAntialiasingEnabled());
}

TEST(StateSerializerTest, RestorePartialState) {
    MockDrawEngine engine;
    engine.Begin();
    
    engine.SetTransform(TransformMatrix::Translate(10.0, 20.0));
    engine.SetOpacity(1.0);
    
    EngineState state;
    state.transform = TransformMatrix::Translate(50.0, 100.0);
    state.opacity = 0.5;
    state.savedFlags = static_cast<StateFlags>(StateFlag::kTransform);
    
    StateSerializer::Restore(&engine, state);
    
    TransformMatrix restoredTransform = engine.GetTransform();
    EXPECT_DOUBLE_EQ(restoredTransform.m[0][2], 50.0);
    
    EXPECT_DOUBLE_EQ(engine.GetOpacity(), 1.0);
}

TEST(StateSerializerTest, MergeStates) {
    EngineState target;
    target.transform = TransformMatrix::Translate(10.0, 20.0);
    target.opacity = 0.5;
    target.tolerance = 0.01;
    
    EngineState source;
    source.transform = TransformMatrix::Translate(100.0, 200.0);
    source.opacity = 0.8;
    source.tolerance = 0.02;
    
    StateSerializer::Merge(target, source, static_cast<StateFlags>(StateFlag::kTransform));
    
    EXPECT_DOUBLE_EQ(target.transform.m[0][2], 100.0);
    EXPECT_DOUBLE_EQ(target.transform.m[1][2], 200.0);
    EXPECT_DOUBLE_EQ(target.opacity, 0.5);
    EXPECT_DOUBLE_EQ(target.tolerance, 0.01);
}

TEST(StateSerializerTest, MergeAllStates) {
    EngineState target;
    target.transform = TransformMatrix::Identity();
    target.opacity = 1.0;
    
    EngineState source;
    source.transform = TransformMatrix::Scale(2.0, 2.0);
    source.opacity = 0.5;
    source.tolerance = 0.1;
    source.antialiasing = false;
    
    StateSerializer::Merge(target, source, static_cast<StateFlags>(StateFlag::kAll));
    
    EXPECT_DOUBLE_EQ(target.transform.m[0][0], 2.0);
    EXPECT_DOUBLE_EQ(target.opacity, 0.5);
    EXPECT_DOUBLE_EQ(target.tolerance, 0.1);
    EXPECT_FALSE(target.antialiasing);
}

TEST(StateSerializerTest, EngineStateDefaultValues) {
    EngineState state;
    
    EXPECT_TRUE(state.transform.IsIdentity());
    EXPECT_TRUE(state.clipRegion.IsEmpty());
    EXPECT_DOUBLE_EQ(state.opacity, 1.0);
    EXPECT_DOUBLE_EQ(state.tolerance, 0.0);
    EXPECT_TRUE(state.antialiasing);
}

TEST(StateSerializerTest, CaptureRestoreRoundtrip) {
    MockDrawEngine engine;
    engine.Begin();
    
    engine.SetTransform(TransformMatrix::Rotate(0.5));
    engine.SetClipRect(50.0, 50.0, 300.0, 200.0);
    engine.SetOpacity(0.33);
    engine.SetTolerance(0.001);
    engine.SetAntialiasingEnabled(true);
    
    EngineState captured = StateSerializer::Capture(&engine);
    
    MockDrawEngine engine2;
    engine2.Begin();
    StateSerializer::Restore(&engine2, captured);
    
    TransformMatrix t1 = engine.GetTransform();
    TransformMatrix t2 = engine2.GetTransform();
    
    EXPECT_DOUBLE_EQ(t1.m[0][0], t2.m[0][0]);
    EXPECT_DOUBLE_EQ(t1.m[0][1], t2.m[0][1]);
    EXPECT_DOUBLE_EQ(t1.m[1][0], t2.m[1][0]);
    EXPECT_DOUBLE_EQ(t1.m[1][1], t2.m[1][1]);
    
    EXPECT_DOUBLE_EQ(engine2.GetOpacity(), engine.GetOpacity());
    EXPECT_DOUBLE_EQ(engine2.GetTolerance(), engine.GetTolerance());
    EXPECT_EQ(engine2.IsAntialiasingEnabled(), engine.IsAntialiasingEnabled());
}
