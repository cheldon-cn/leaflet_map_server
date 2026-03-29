#include <ogc/draw/display_device.h>
#include <ogc/draw/simple2d_engine.h>
#include <cstring>

#ifdef _WIN32
#include <windows.h>
#endif

namespace ogc {
namespace draw {

DisplayDevice::DisplayDevice(void* nativeWindow)
    : RasterImageDevice(1, 1, PixelFormat::kRGBA8888)
    , m_nativeWindow(nativeWindow)
{
}

DisplayDevice::~DisplayDevice()
{
    Finalize();
}

DrawResult DisplayDevice::Initialize()
{
#ifdef _WIN32
    if (m_nativeWindow) {
        HWND hwnd = static_cast<HWND>(m_nativeWindow);
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        
        if (width > 0 && height > 0) {
            SetDimensions(width, height);
        }
    }
#endif
    
    return RasterImageDevice::Initialize();
}

DrawResult DisplayDevice::Finalize()
{
    m_dirtyRects.clear();
    return RasterImageDevice::Finalize();
}

std::unique_ptr<DrawEngine> DisplayDevice::CreateEngine()
{
    return std::unique_ptr<DrawEngine>(new Simple2DEngine(this));
}

std::vector<EngineType> DisplayDevice::GetSupportedEngineTypes() const
{
    return { EngineType::kSimple2D };
}

void DisplayDevice::Present()
{
    uint8_t* pixelData = GetPixelData();
    int width = GetWidth();
    int height = GetHeight();
    int stride = GetStride();
    
    if (!pixelData || width <= 0 || height <= 0) {
        return;
    }
    
#ifdef _WIN32
    if (m_nativeWindow) {
        HWND hwnd = static_cast<HWND>(m_nativeWindow);
        HDC hdc = GetDC(hwnd);
        if (hdc) {
            HDC memDC = CreateCompatibleDC(hdc);
            BITMAPINFO bmi = {0};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = width;
            bmi.bmiHeader.biHeight = -height;
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 32;
            bmi.bmiHeader.biCompression = BI_RGB;
            
            void* bits = nullptr;
            HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &bits, nullptr, 0);
            if (hBitmap && bits) {
                memcpy(bits, pixelData, stride * height);
                HBITMAP oldBitmap = static_cast<HBITMAP>(SelectObject(memDC, hBitmap));
                BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);
                SelectObject(memDC, oldBitmap);
                DeleteObject(hBitmap);
            }
            DeleteDC(memDC);
            ReleaseDC(hwnd, hdc);
        }
    }
#endif
    
    ClearDirtyRects();
}

void DisplayDevice::SetVSync(bool enabled)
{
    m_vsync = enabled;
}

void DisplayDevice::InvalidateRect(const Rect& rect)
{
    int width = GetWidth();
    int height = GetHeight();
    
    Rect clampedRect;
    clampedRect.x = rect.x < 0.0 ? 0.0 : rect.x;
    clampedRect.y = rect.y < 0.0 ? 0.0 : rect.y;
    clampedRect.w = (clampedRect.x + rect.w > width) ? (width - clampedRect.x) : rect.w;
    clampedRect.h = (clampedRect.y + rect.h > height) ? (height - clampedRect.y) : rect.h;
    
    if (clampedRect.w > 0 && clampedRect.h > 0) {
        m_dirtyRects.push_back(clampedRect);
    }
}

void DisplayDevice::InvalidateAll()
{
    m_dirtyRects.clear();
    m_dirtyRects.push_back(Rect(0, 0, static_cast<double>(GetWidth()), static_cast<double>(GetHeight())));
}

std::vector<Rect> DisplayDevice::GetDirtyRects() const
{
    return m_dirtyRects;
}

void DisplayDevice::ClearDirtyRects()
{
    m_dirtyRects.clear();
}

void DisplayDevice::Resize(int width, int height)
{
    if (width <= 0 || height <= 0) {
        return;
    }
    
    if (GetWidth() != width || GetHeight() != height) {
        Finalize();
        SetDimensions(width, height);
        Initialize();
    }
}

void DisplayDevice::UpdateWindowSize()
{
#ifdef _WIN32
    if (m_nativeWindow) {
        HWND hwnd = static_cast<HWND>(m_nativeWindow);
        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        
        if (width > 0 && height > 0) {
            Resize(width, height);
        }
    }
#endif
}

}
}
