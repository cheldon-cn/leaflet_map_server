#ifndef OGC_DRAW_SVG_DEVICE_H
#define OGC_DRAW_SVG_DEVICE_H

#include "ogc/draw/draw_device.h"
#include "ogc/draw/export.h"
#include <string>
#include <sstream>
#include <map>

namespace ogc {
namespace draw {

class OGC_DRAW_API SvgDevice : public DrawDevice {
public:
    SvgDevice(const std::string& filePath, double width, double height);
    ~SvgDevice() override;

    DeviceType GetType() const override { return DeviceType::kSvg; }
    std::string GetName() const override { return "SvgDevice"; }
    int GetWidth() const override { return static_cast<int>(m_width); }
    int GetHeight() const override { return static_cast<int>(m_height); }
    double GetDpi() const override { return 96.0; }
    void SetDpi(double dpi) override { }
    int GetColorDepth() const override { return 32; }

    DrawResult Initialize() override;
    DrawResult Finalize() override;
    DeviceState GetState() const override { return m_state; }
    bool IsReady() const override { return m_initialized; }

    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    void SetPreferredEngineType(EngineType type) override { }
    EngineType GetPreferredEngineType() const override { return EngineType::kVector; }

    DeviceCapabilities QueryCapabilities() const override;
    bool IsFeatureAvailable(const std::string& featureName) const override;

    bool IsDeviceLost() const override { return false; }
    DrawResult RecoverDevice() override { return DrawResult::kSuccess; }

    bool IsInitialized() const { return m_initialized; }

    void SetTitle(const std::string& title);
    void SetDescription(const std::string& desc);

    bool Save();
    std::string GetFilePath() const { return m_filePath; }
    std::string GetSvgContent() const { return m_content.str(); }

    void AppendContent(const std::string& content);
    void BeginGroup(const std::string& id = "");
    void EndGroup();

private:
    void WriteHeader();
    void WriteFooter();

    std::string m_filePath;
    double m_width;
    double m_height;
    bool m_initialized;
    int m_groupDepth;
    DeviceState m_state;

    std::stringstream m_content;
    std::string m_title;
    std::string m_description;

    static std::map<std::string, std::pair<double, double>> s_pageSizes;
};

} // namespace draw
} // namespace ogc

#endif // OGC_DRAW_SVG_DEVICE_H
