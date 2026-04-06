#ifndef OGC_DRAW_TEST_MOCKS_H
#define OGC_DRAW_TEST_MOCKS_H

#include "ogc/draw/draw_engine.h"
#include "ogc/draw/draw_device.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/draw_engine_plugin.h"
#include "ogc/draw/draw_device_plugin.h"
#include <gmock/gmock.h>

namespace ogc {
namespace draw {
namespace testing {

class MockDrawDevice : public DrawDevice {
public:
    MOCK_METHOD(DeviceType, GetType, (), (const, override));
    MOCK_METHOD(std::string, GetName, (), (const, override));
    MOCK_METHOD(int, GetWidth, (), (const, override));
    MOCK_METHOD(int, GetHeight, (), (const, override));
    MOCK_METHOD(double, GetDpi, (), (const, override));
    MOCK_METHOD(void, SetDpi, (double), (override));
    MOCK_METHOD(int, GetColorDepth, (), (const, override));
    MOCK_METHOD(DrawResult, Initialize, (), (override));
    MOCK_METHOD(DrawResult, Finalize, (), (override));
    MOCK_METHOD(DeviceState, GetState, (), (const, override));
    MOCK_METHOD(bool, IsReady, (), (const, override));
    MOCK_METHOD(std::unique_ptr<DrawEngine>, CreateEngine, (), (override));
    MOCK_METHOD(std::vector<EngineType>, GetSupportedEngineTypes, (), (const, override));
    MOCK_METHOD(void, SetPreferredEngineType, (EngineType), (override));
    MOCK_METHOD(EngineType, GetPreferredEngineType, (), (const, override));
    MOCK_METHOD(DeviceCapabilities, QueryCapabilities, (), (const, override));
    MOCK_METHOD(bool, IsFeatureAvailable, (const std::string&), (const, override));
    MOCK_METHOD(bool, IsDeviceLost, (), (const, override));
    MOCK_METHOD(DrawResult, RecoverDevice, (), (override));
};

class MockDrawEngine : public DrawEngine {
public:
    MOCK_METHOD(std::string, GetName, (), (const, override));
    MOCK_METHOD(EngineType, GetType, (), (const, override));
    MOCK_METHOD(DrawDevice*, GetDevice, (), (const, override));
    MOCK_METHOD(bool, IsActive, (), (const, override));
    MOCK_METHOD(DrawResult, Begin, (), (override));
    MOCK_METHOD(void, End, (), (override));
    
    MOCK_METHOD(DrawResult, DrawPoints, (const double*, const double*, int, const DrawStyle&), (override));
    MOCK_METHOD(DrawResult, DrawLines, (const double*, const double*, const double*, const double*, int, const DrawStyle&), (override));
    MOCK_METHOD(DrawResult, DrawLineString, (const double*, const double*, int, const DrawStyle&), (override));
    MOCK_METHOD(DrawResult, DrawPolygon, (const double*, const double*, int, const DrawStyle&, bool), (override));
    MOCK_METHOD(DrawResult, DrawRect, (double, double, double, double, const DrawStyle&, bool), (override));
    MOCK_METHOD(DrawResult, DrawCircle, (double, double, double, const DrawStyle&, bool), (override));
    MOCK_METHOD(DrawResult, DrawEllipse, (double, double, double, double, const DrawStyle&, bool), (override));
    MOCK_METHOD(DrawResult, DrawArc, (double, double, double, double, double, double, const DrawStyle&), (override));
    MOCK_METHOD(DrawResult, DrawText, (double, double, const std::string&, const Font&, const Color&), (override));
    MOCK_METHOD(DrawResult, DrawImage, (double, double, const Image&, double, double), (override));
    MOCK_METHOD(DrawResult, DrawImageRect, (double, double, double, double, const Image&), (override));
    MOCK_METHOD(DrawResult, DrawGeometry, (const Geometry*, const DrawStyle&), (override));
    
    MOCK_METHOD(void, SetTransform, (const TransformMatrix&), (override));
    MOCK_METHOD(TransformMatrix, GetTransform, (), (const, override));
    MOCK_METHOD(void, ResetTransform, (), (override));
    MOCK_METHOD(void, SetClipRect, (double, double, double, double), (override));
    MOCK_METHOD(void, SetClipRegion, (const Region&), (override));
    MOCK_METHOD(Region, GetClipRegion, (), (const, override));
    MOCK_METHOD(void, ResetClip, (), (override));
    
    MOCK_METHOD(void, Save, (StateFlags), (override));
    MOCK_METHOD(void, Restore, (), (override));
    
    MOCK_METHOD(void, SetOpacity, (double), (override));
    MOCK_METHOD(double, GetOpacity, (), (const, override));
    MOCK_METHOD(void, SetTolerance, (double), (override));
    MOCK_METHOD(double, GetTolerance, (), (const, override));
    MOCK_METHOD(void, SetAntialiasingEnabled, (bool), (override));
    MOCK_METHOD(bool, IsAntialiasingEnabled, (), (const, override));
    
    MOCK_METHOD(void, Clear, (const Color&), (override));
    MOCK_METHOD(void, Flush, (), (override));
    
    MOCK_METHOD(TextMetrics, MeasureText, (const std::string&, const Font&), (override));
    MOCK_METHOD(bool, SupportsFeature, (const std::string&), (const, override));
};

class MockDrawEnginePlugin : public DrawEnginePlugin {
public:
    MOCK_METHOD(std::string, GetName, (), (const, override));
    MOCK_METHOD(std::string, GetVersion, (), (const, override));
    MOCK_METHOD(std::string, GetDescription, (), (const, override));
    MOCK_METHOD(EngineType, GetEngineType, (), (const, override));
    MOCK_METHOD(std::unique_ptr<DrawEngine>, CreateEngine, (DrawDevice*), (override));
    MOCK_METHOD(bool, IsAvailable, (), (const, override));
    MOCK_METHOD(std::vector<std::string>, GetSupportedDeviceTypes, (), (const, override));
};

class MockDrawDevicePlugin : public DrawDevicePlugin {
public:
    MOCK_METHOD(std::string, GetName, (), (const, override));
    MOCK_METHOD(std::string, GetVersion, (), (const, override));
    MOCK_METHOD(std::string, GetDescription, (), (const, override));
    MOCK_METHOD(DeviceType, GetSupportedDeviceType, (), (const, override));
    MOCK_METHOD(std::unique_ptr<DrawDevice>, CreateDevice, (int, int), (override));
    MOCK_METHOD(std::unique_ptr<DrawDevice>, CreateDevice, (void*, int, int), (override));
    MOCK_METHOD(bool, IsAvailable, (), (const, override));
};

}
}
}

#endif
