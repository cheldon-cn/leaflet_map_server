#include "ogc/draw/qt_display_device.h"
#include "ogc/draw/qt_engine.h"

#ifdef DRAW_HAS_QT

#include <QScreen>
#include <QApplication>
#include <QFile>
#include <QDebug>

namespace ogc {
namespace draw {

QtDisplayDevice::QtDisplayDevice(QPaintDevice* device)
    : m_paintDevice(device)
    , m_widget(nullptr)
    , m_ownsDevice(false)
{
    if (device) {
        m_dpi = device->logicalDpiX();
        if (m_dpi <= 0) {
            m_dpi = 96.0;
        }
    }
}

QtDisplayDevice::QtDisplayDevice(QWidget* widget)
    : m_paintDevice(widget)
    , m_widget(widget)
    , m_ownsDevice(false)
{
    if (widget) {
        m_dpi = widget->logicalDpiX();
        if (m_dpi <= 0) {
            m_dpi = 96.0;
        }
    }
}

QtDisplayDevice::QtDisplayDevice(const QSize& size, QImage::Format format)
    : m_image(std::make_unique<QImage>(size, format))
    , m_widget(nullptr)
    , m_ownsDevice(true)
{
    m_paintDevice = m_image.get();
    m_image->fill(Qt::transparent);
    m_dpi = 96.0;
}

QtDisplayDevice::QtDisplayDevice(int width, int height, QImage::Format format)
    : QtDisplayDevice(QSize(width, height), format)
{
}

QtDisplayDevice::~QtDisplayDevice() {
    if (m_ownsDevice) {
        m_image.reset();
        m_pixmap.reset();
    }
    m_paintDevice = nullptr;
}

int QtDisplayDevice::GetWidth() const {
    if (m_paintDevice) {
        return m_paintDevice->width();
    }
    return 0;
}

int QtDisplayDevice::GetHeight() const {
    if (m_paintDevice) {
        return m_paintDevice->height();
    }
    return 0;
}

double QtDisplayDevice::GetDpi() const {
    return m_dpi;
}

void QtDisplayDevice::SetDpi(double dpi) {
    if (dpi > 0) {
        m_dpi = dpi;
    }
}

std::unique_ptr<DrawEngine> QtDisplayDevice::CreateEngine() {
    return std::unique_ptr<DrawEngine>(new QtEngine(this));
}

std::vector<EngineType> QtDisplayDevice::GetSupportedEngineTypes() const {
    return { EngineType::kSimple2D };
}

bool QtDisplayDevice::SaveToFile(const std::string& path, const char* format, int quality) {
    if (!m_paintDevice) {
        return false;
    }
    
    QString qPath = QString::fromUtf8(path.c_str());
    
    if (m_image) {
        return m_image->save(qPath, format, quality);
    }
    
    if (m_pixmap) {
        return m_pixmap->save(qPath, format, quality);
    }
    
    if (m_widget) {
        QPixmap pixmap = m_widget->grab();
        return pixmap.save(qPath, format, quality);
    }
    
    if (m_paintDevice) {
        QPixmap pixmap(m_paintDevice->size());
        QPainter painter(&pixmap);
        painter.drawPixmap(0, 0, QPixmap::fromImage(dynamic_cast<QImage*>(m_paintDevice)));
        return pixmap.save(qPath, format, quality);
    }
    
    return false;
}

bool QtDisplayDevice::IsValid() const {
    return m_paintDevice != nullptr && 
           m_paintDevice->width() > 0 && 
           m_paintDevice->height() > 0;
}

QSize QtDisplayDevice::GetSize() const {
    if (m_paintDevice) {
        return QSize(m_paintDevice->width(), m_paintDevice->height());
    }
    return QSize();
}

QtDisplayDevice* QtDisplayDevice::FromImage(QImage* image) {
    if (!image) return nullptr;
    return new QtDisplayDevice(image);
}

QtDisplayDevice* QtDisplayDevice::FromPixmap(QPixmap* pixmap) {
    if (!pixmap) return nullptr;
    QImage image = pixmap->toImage();
    return new QtDisplayDevice(new QImage(image));
}

QtDisplayDevice* QtDisplayDevice::FromWidget(QWidget* widget) {
    if (!widget) return nullptr;
    return new QtDisplayDevice(widget);
}

}  
}  

#endif
