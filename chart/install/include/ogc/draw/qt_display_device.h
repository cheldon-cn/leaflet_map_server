#ifndef OGC_DRAW_QT_DISPLAY_DEVICE_H
#define OGC_DRAW_QT_DISPLAY_DEVICE_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_device.h"

#ifdef DRAW_HAS_QT

#include <QPaintDevice>
#include <QImage>
#include <QPixmap>
#include <QWidget>
#include <memory>

namespace ogc {
namespace draw {

class OGC_DRAW_API QtDisplayDevice : public DrawDevice {
public:
    explicit QtDisplayDevice(QPaintDevice* device);
    explicit QtDisplayDevice(QWidget* widget);
    explicit QtDisplayDevice(const QSize& size, QImage::Format format = QImage::Format_ARGB32);
    QtDisplayDevice(int width, int height, QImage::Format format = QImage::Format_ARGB32);
    ~QtDisplayDevice() override;
    
    DeviceType GetType() const override { return DeviceType::kDisplay; }
    int GetWidth() const override;
    int GetHeight() const override;
    double GetDpi() const override;
    
    std::unique_ptr<DrawEngine> CreateEngine() override;
    std::vector<EngineType> GetSupportedEngineTypes() const override;
    EngineType GetPreferredEngineType() const override { return EngineType::kSimple2D; }
    
    QPaintDevice* GetPaintDevice() const { return m_paintDevice; }
    QImage* GetImage() { return m_image.get(); }
    QPixmap* GetPixmap() { return m_pixmap.get(); }
    QWidget* GetWidget() { return m_widget; }
    
    bool SaveToFile(const std::string& path, const char* format = nullptr, int quality = -1);
    
    void SetDpi(double dpi);
    bool IsValid() const;
    QSize GetSize() const;
    
    static QtDisplayDevice* FromImage(QImage* image);
    static QtDisplayDevice* FromPixmap(QPixmap* pixmap);
    static QtDisplayDevice* FromWidget(QWidget* widget);

private:
    QPaintDevice* m_paintDevice = nullptr;
    std::unique_ptr<QImage> m_image;
    std::unique_ptr<QPixmap> m_pixmap;
    QWidget* m_widget = nullptr;
    bool m_ownsDevice = false;
    double m_dpi = 96.0;
};

}  
}  

#endif

#endif
