#include "ogc/mokrender/raster_device_output.h"
#include "ogc/draw/raster_image_device.h"
#include "ogc/draw/draw_context.h"
#include "ogc/draw/draw_style.h"
#include "ogc/draw/font.h"
#include "ogc/envelope.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <tuple>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#undef DrawText
#endif

namespace ogc {
namespace mokrender {

#ifdef _WIN32
static std::wstring StringToWString(const std::string& str) {
    if (str.empty()) return std::wstring();
    int size = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    std::wstring wstr(size - 1, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], size);
    return wstr;
}
#endif

static const uint8_t FONT_8X8[128][8] = {
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00},
    {0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00},
    {0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00},
    {0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00},
    {0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00},
    {0x06,0x0C,0x18,0x00,0x00,0x00,0x00,0x00},
    {0x0C,0x18,0x30,0x30,0x30,0x18,0x0C,0x00},
    {0x30,0x18,0x0C,0x0C,0x0C,0x18,0x30,0x00},
    {0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00},
    {0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x30},
    {0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x18,0x18,0x00},
    {0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00},
    {0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00},
    {0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00},
    {0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00},
    {0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00},
    {0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00},
    {0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00},
    {0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00},
    {0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00},
    {0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00},
    {0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00},
    {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x00,0x00},
    {0x00,0x0C,0x0C,0x00,0x0C,0x0C,0x18,0x00},
    {0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00},
    {0x00,0x00,0x3F,0x00,0x3F,0x00,0x00,0x00},
    {0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00},
    {0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00},
    {0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00},
    {0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00},
    {0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00},
    {0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00},
    {0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00},
    {0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00},
    {0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00},
    {0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00},
    {0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00},
    {0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},
    {0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00},
    {0x67,0x36,0x1E,0x0C,0x1E,0x36,0x67,0x00},
    {0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00},
    {0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00},
    {0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00},
    {0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00},
    {0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00},
    {0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00},
    {0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00},
    {0x1E,0x33,0x07,0x1E,0x38,0x33,0x1E,0x00},
    {0x3F,0x36,0x0C,0x0C,0x0C,0x0C,0x0C,0x00},
    {0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00},
    {0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00},
    {0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00},
    {0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00},
    {0x33,0x33,0x33,0x1E,0x0C,0x0C,0x0C,0x00},
    {0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00},
    {0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00},
    {0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00},
    {0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00},
    {0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF},
    {0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00},
    {0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00},
    {0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00},
    {0x38,0x30,0x30,0x3E,0x33,0x33,0x6E,0x00},
    {0x00,0x00,0x1E,0x33,0x3F,0x03,0x1E,0x00},
    {0x1C,0x36,0x06,0x0F,0x06,0x06,0x0F,0x00},
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F},
    {0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00}, // 'g' - lowercase g
    {0x0C,0x0C,0x00,0x0E,0x0C,0x0C,0x1E,0x00},
    {0x18,0x18,0x00,0x1C,0x18,0x18,0x18,0x3C},
    {0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00},
    {0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00},
    {0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00},
    {0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00},
    {0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00},
    {0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F},
    {0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78},
    {0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00},
    {0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00},
    {0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00},
    {0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00},
    {0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00},
    {0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00},
    {0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00},
    {0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F},
    {0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00},
    {0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00},
    {0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00},
    {0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00},
    {0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00}
};

static void DrawRectOnImage(uint8_t* data, int width, int height, int channels,
                            int x, int y, int w, int h,
                            uint8_t r, uint8_t g, uint8_t b, bool fill = true) {
    for (int py = y; py < y + h && py < height; ++py) {
        for (int px = x; px < x + w && px < width; ++px) {
            if (px >= 0 && py >= 0) {
                bool isBorder = (py == y || py == y + h - 1 || px == x || px == x + w - 1);
                if (fill || isBorder) {
                    size_t idx = static_cast<size_t>(py * width * channels + px * channels);
                    data[idx + 0] = r;
                    data[idx + 1] = g;
                    data[idx + 2] = b;
                    if (channels >= 4) {
                        data[idx + 3] = 255;
                    }
                }
            }
        }
    }
}

static void DrawTextOnImage(uint8_t* data, int width, int height, int channels,
                            int x, int y, const std::string& text,
                            uint8_t r, uint8_t g, uint8_t b, bool withBackground = true) {
    int textLen = static_cast<int>(text.size());
    int textWidth = textLen * 8 + 4;
    int textHeight = 10;
    
    if (withBackground) {
        DrawRectOnImage(data, width, height, channels, x - 2, y - 1, textWidth, textHeight, 255, 255, 255, true);
        DrawRectOnImage(data, width, height, channels, x - 2, y - 1, textWidth, textHeight, 0, 0, 0, false);
    }
    
    int startX = x;
    int startY = y;
    
    for (size_t i = 0; i < text.size(); ++i) {
        unsigned char c = static_cast<unsigned char>(text[i]);
        if (c >= 128) c = '?';
        
        if (c == ' ') {
            startX += 6;
            continue;
        }
        
        const uint8_t* glyph = FONT_8X8[c];
        
        for (int row = 0; row < 8; ++row) {
            uint8_t rowData = glyph[row];
            for (int col = 0; col < 8; ++col) {
                if (rowData & (0x01 << col)) {
                    int px = startX + col;
                    int py = startY + row;
                    
                    if (px >= 0 && px < width && py >= 0 && py < height) {
                        size_t idx = static_cast<size_t>(py * width * channels + px * channels);
                        data[idx + 0] = r;
                        data[idx + 1] = g;
                        data[idx + 2] = b;
                        if (channels >= 4) {
                            data[idx + 3] = 255;
                        }
                    }
                }
            }
        }
        
        startX += 8;
    }
}

#ifdef _WIN32
static bool DrawTextWithGDI(uint8_t* data, int width, int height, int channels,
                            int x, int y, const std::string& text,
                            uint8_t r, uint8_t g, uint8_t b,
                            const std::string& fontName = "Arial", int fontSize = 12) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok) {
        return false;
    }
    
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = CreateCompatibleDC(NULL);
    void* pvBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (!hBitmap || !pvBits) {
        DeleteDC(hdc);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return false;
    }
    
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);
    
    uint8_t* bits = static_cast<uint8_t*>(pvBits);
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            int srcIdx = (py * width + px) * channels;
            int dstIdx = (py * width + px) * 4;
            
            bits[dstIdx + 0] = data[srcIdx + 2];
            bits[dstIdx + 1] = data[srcIdx + 1];
            bits[dstIdx + 2] = data[srcIdx + 0];
            bits[dstIdx + 3] = (channels >= 4) ? data[srcIdx + 3] : 255;
        }
    }
    
    {
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        
        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(255, 255, 255, 255));
        Gdiplus::Pen borderPen(Gdiplus::Color(255, 0, 0, 0), 1.0f);
        
        Gdiplus::FontFamily fontFamily(StringToWString(fontName).c_str());
        Gdiplus::Font font(&fontFamily, static_cast<Gdiplus::REAL>(fontSize), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentNear);
        format.SetLineAlignment(Gdiplus::StringAlignmentNear);
        
        Gdiplus::RectF layoutRect(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y), 0, 0);
        Gdiplus::RectF textRect;
        graphics.MeasureString(StringToWString(text).c_str(), -1, &font, layoutRect, &format, &textRect);
        
        textRect.X -= 2;
        textRect.Y -= 1;
        textRect.Width += 6;
        textRect.Height += 2;
        
        graphics.FillRectangle(&backgroundBrush, textRect);
        graphics.DrawRectangle(&borderPen, textRect);
        
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, r, g, b));
        graphics.DrawString(StringToWString(text).c_str(), -1, &font, 
                           Gdiplus::PointF(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y)),
                           &format, &textBrush);
    }
    
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            int srcIdx = (py * width + px) * 4;
            int dstIdx = (py * width + px) * channels;
            
            data[dstIdx + 0] = bits[srcIdx + 2];
            data[dstIdx + 1] = bits[srcIdx + 1];
            data[dstIdx + 2] = bits[srcIdx + 0];
            if (channels >= 4) {
                data[dstIdx + 3] = bits[srcIdx + 3];
            }
        }
    }
    
    SelectObject(hdc, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdc);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    
    return true;
}

static bool DrawMultipleTextsWithGDI(uint8_t* data, int width, int height, int channels,
                                    const std::vector<std::tuple<int, int, std::string>>& texts,
                                    uint8_t r, uint8_t g, uint8_t b,
                                    const std::string& fontName = "Arial", int fontSize = 12) {
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) != Gdiplus::Ok) {
        return false;
    }
    
    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    
    HDC hdc = CreateCompatibleDC(NULL);
    void* pvBits = NULL;
    HBITMAP hBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pvBits, NULL, 0);
    if (!hBitmap || !pvBits) {
        DeleteDC(hdc);
        Gdiplus::GdiplusShutdown(gdiplusToken);
        return false;
    }
    
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdc, hBitmap);
    
    uint8_t* bits = static_cast<uint8_t*>(pvBits);
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            int srcIdx = (py * width + px) * channels;
            int dstIdx = (py * width + px) * 4;
            
            bits[dstIdx + 0] = data[srcIdx + 2];
            bits[dstIdx + 1] = data[srcIdx + 1];
            bits[dstIdx + 2] = data[srcIdx + 0];
            bits[dstIdx + 3] = (channels >= 4) ? data[srcIdx + 3] : 255;
        }
    }
    
    {
        Gdiplus::Graphics graphics(hdc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAlias);
        
        Gdiplus::SolidBrush backgroundBrush(Gdiplus::Color(255, 255, 255, 255));
        Gdiplus::Pen borderPen(Gdiplus::Color(255, 0, 0, 0), 1.0f);
        
        Gdiplus::FontFamily fontFamily(StringToWString(fontName).c_str());
        Gdiplus::Font font(&fontFamily, static_cast<Gdiplus::REAL>(fontSize), Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
        
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentNear);
        format.SetLineAlignment(Gdiplus::StringAlignmentNear);
        
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, r, g, b));
        
        for (const auto& item : texts) {
            int x = std::get<0>(item);
            int y = std::get<1>(item);
            const std::string& text = std::get<2>(item);
            
            Gdiplus::RectF layoutRect(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y), 0, 0);
            Gdiplus::RectF textRect;
            graphics.MeasureString(StringToWString(text).c_str(), -1, &font, layoutRect, &format, &textRect);
            
            textRect.X -= 2;
            textRect.Y -= 1;
            textRect.Width += 6;
            textRect.Height += 2;
            
            graphics.FillRectangle(&backgroundBrush, textRect);
            graphics.DrawRectangle(&borderPen, textRect);
            
            graphics.DrawString(StringToWString(text).c_str(), -1, &font, 
                               Gdiplus::PointF(static_cast<Gdiplus::REAL>(x), static_cast<Gdiplus::REAL>(y)),
                               &format, &textBrush);
        }
    }
    
    for (int py = 0; py < height; ++py) {
        for (int px = 0; px < width; ++px) {
            int srcIdx = (py * width + px) * 4;
            int dstIdx = (py * width + px) * channels;
            
            data[dstIdx + 0] = bits[srcIdx + 2];
            data[dstIdx + 1] = bits[srcIdx + 1];
            data[dstIdx + 2] = bits[srcIdx + 0];
            if (channels >= 4) {
                data[dstIdx + 3] = bits[srcIdx + 3];
            }
        }
    }
    
    SelectObject(hdc, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdc);
    Gdiplus::GdiplusShutdown(gdiplusToken);
    
    return true;
}
#endif

class RasterDeviceOutput::Impl {
public:
    int width, height;
    double dpi;
    bool initialized;
    std::shared_ptr<draw::RasterImageDevice> device;
    
    Impl() : width(800), height(600), dpi(96.0), initialized(false) {}
};

RasterDeviceOutput::RasterDeviceOutput() : m_impl(new Impl()) {
}

RasterDeviceOutput::~RasterDeviceOutput() {
}

MokRenderResult RasterDeviceOutput::Initialize(const RenderConfig& config) {
    m_impl->width = config.outputWidth;
    m_impl->height = config.outputHeight;
    m_impl->dpi = config.dpi;
    
    m_impl->device = draw::RasterImageDevice::Create(config.outputWidth, config.outputHeight, 4);
    if (!m_impl->device) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to create raster device");
    }
    
    draw::DrawResult result = m_impl->device->Initialize();
    if (result != draw::DrawResult::kSuccess) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to initialize raster device");
    }
    
    m_impl->initialized = true;
    return MokRenderResult();
}

static bool SaveAsBMP(const std::string& filepath, int width, int height, const uint8_t* data, int channels) {
    int rowSize = width * 3;
    int padding = (4 - (rowSize % 4)) % 4;
    int paddedRowSize = rowSize + padding;
    int imageSize = paddedRowSize * height;
    
    int fileSize = 54 + imageSize;
    
    std::vector<uint8_t> header(54, 0);
    
    header[0] = 'B';
    header[1] = 'M';
    
    header[2] = fileSize & 0xFF;
    header[3] = (fileSize >> 8) & 0xFF;
    header[4] = (fileSize >> 16) & 0xFF;
    header[5] = (fileSize >> 24) & 0xFF;
    
    header[10] = 54;
    
    header[14] = 40;
    
    header[18] = width & 0xFF;
    header[19] = (width >> 8) & 0xFF;
    header[20] = (width >> 16) & 0xFF;
    header[21] = (width >> 24) & 0xFF;
    
    header[22] = height & 0xFF;
    header[23] = (height >> 8) & 0xFF;
    header[24] = (height >> 16) & 0xFF;
    header[25] = (height >> 24) & 0xFF;
    
    header[26] = 1;
    header[28] = 24;
    
    std::ofstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    file.write(reinterpret_cast<const char*>(header.data()), 54);
    
    std::vector<uint8_t> row(paddedRowSize, 0);
    
    for (int y = height - 1; y >= 0; --y) {
        for (int x = 0; x < width; ++x) {
            int srcIdx = (y * width + x) * channels;
            int dstIdx = x * 3;
            
            if (channels >= 3) {
                row[dstIdx + 0] = data[srcIdx + 2];
                row[dstIdx + 1] = data[srcIdx + 1];
                row[dstIdx + 2] = data[srcIdx + 0];
            } else {
                row[dstIdx + 0] = data[srcIdx];
                row[dstIdx + 1] = data[srcIdx];
                row[dstIdx + 2] = data[srcIdx];
            }
        }
        file.write(reinterpret_cast<const char*>(row.data()), paddedRowSize);
    }
    
    file.close();
    return true;
}

MokRenderResult RasterDeviceOutput::Render(const std::string& outputPath) {
    {
        std::ofstream logFile(outputPath, std::ios::app);
        logFile << "Render function called" << std::endl;
        logFile.close();
    }
    
    if (!m_impl->initialized) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Not initialized");
    }
    
    draw::Color white = draw::Color::White();
    m_impl->device->Clear(white);
    
    draw::DrawParams params;
    params.extent = Envelope(0, 0, 1000, 1000);
    params.pixel_width = m_impl->width;
    params.pixel_height = m_impl->height;
    params.dpi = m_impl->dpi;
    
    draw::DrawResult drawResult = m_impl->device->BeginDraw(params);
    if (drawResult != draw::DrawResult::kSuccess) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to begin drawing");
    }
    
    draw::DrawStyle redStyle;
    redStyle.stroke.color = draw::Color(255, 0, 0, 255).GetRGBA();
    redStyle.stroke.width = 2.0;
    redStyle.stroke.visible = true;
    
    draw::DrawStyle blueStyle;
    blueStyle.fill.color = draw::Color(0, 0, 255, 100).GetRGBA();
    blueStyle.fill.visible = true;
    blueStyle.stroke.color = draw::Color(0, 0, 255, 255).GetRGBA();
    blueStyle.stroke.width = 1.0;
    blueStyle.stroke.visible = true;
    
    draw::DrawStyle greenStyle;
    greenStyle.stroke.color = draw::Color(0, 255, 0, 255).GetRGBA();
    greenStyle.stroke.width = 3.0;
    greenStyle.stroke.visible = true;
    
    draw::DrawStyle yellowFillStyle;
    yellowFillStyle.fill.color = draw::Color(255, 255, 0, 180).GetRGBA();
    yellowFillStyle.fill.visible = true;
    yellowFillStyle.stroke.color = draw::Color(255, 165, 0, 255).GetRGBA();
    yellowFillStyle.stroke.width = 2.0;
    yellowFillStyle.stroke.visible = true;
    
    draw::DrawStyle purpleFillStyle;
    purpleFillStyle.fill.color = draw::Color(128, 0, 128, 150).GetRGBA();
    purpleFillStyle.fill.visible = true;
    purpleFillStyle.stroke.color = draw::Color(75, 0, 130, 255).GetRGBA();
    purpleFillStyle.stroke.width = 2.0;
    purpleFillStyle.stroke.visible = true;
    
    draw::DrawStyle cyanStyle;
    cyanStyle.stroke.color = draw::Color(0, 255, 255, 255).GetRGBA();
    cyanStyle.stroke.width = 2.0;
    cyanStyle.stroke.visible = true;
    cyanStyle.fill.color = draw::Color(0, 255, 255, 100).GetRGBA();
    cyanStyle.fill.visible = true;
    
    draw::DrawStyle orangeStyle;
    orangeStyle.stroke.color = draw::Color(255, 128, 0, 255).GetRGBA();
    orangeStyle.stroke.width = 3.0;
    orangeStyle.stroke.visible = true;
    
    draw::DrawStyle magentaStyle;
    magentaStyle.fill.color = draw::Color(255, 0, 255, 120).GetRGBA();
    magentaStyle.fill.visible = true;
    magentaStyle.stroke.color = draw::Color(255, 0, 255, 255).GetRGBA();
    magentaStyle.stroke.width = 2.0;
    magentaStyle.stroke.visible = true;
    
    for (int i = 0; i < 10; ++i) {
        double x = 50 + i * 75;
        double y = 50 + (i % 3) * 30;
        m_impl->device->DrawCircle(x, y, 15, redStyle);
    }
    
    for (int i = 0; i < 5; ++i) {
        double cx = 100 + i * 140;
        double cy = 150;
        double rx = 30 + i * 5;
        double ry = 20 + i * 3;
        m_impl->device->DrawEllipse(cx, cy, rx, ry, yellowFillStyle);
    }
    
    for (int i = 0; i < 5; ++i) {
        double x = 80 + i * 140;
        m_impl->device->DrawLine(x, 220, x + 80, 320, greenStyle);
    }
    
    for (int i = 0; i < 3; ++i) {
        double x = 120 + i * 220;
        double y = 380 + i * 40;
        m_impl->device->DrawRect(x, y, 120, 70, blueStyle);
    }
    
    for (int i = 0; i < 4; ++i) {
        double cx = 150 + i * 180;
        double cy = 520;
        m_impl->device->DrawEllipse(cx, cy, 50, 30, purpleFillStyle);
    }
    
    for (int i = 0; i < 3; ++i) {
        double baseX = 450;
        double baseY = 100 + i * 100;
        
        for (int j = 0; j < 4; ++j) {
            double x = baseX + j * 25;
            double y = baseY + (j % 2) * 15;
            m_impl->device->DrawCircle(x, y, 8, cyanStyle);
        }
    }
    
    for (int i = 0; i < 3; ++i) {
        double baseX = 550;
        double baseY = 100 + i * 100;
        
        m_impl->device->DrawLine(baseX, baseY, baseX + 30, baseY + 20, orangeStyle);
        m_impl->device->DrawLine(baseX + 40, baseY + 10, baseX + 70, baseY + 30, orangeStyle);
    }
    
    for (int i = 0; i < 3; ++i) {
        double baseX = 650;
        double baseY = 100 + i * 100;
        
        m_impl->device->DrawRect(baseX, baseY, 40, 30, magentaStyle);
        m_impl->device->DrawRect(baseX + 50, baseY + 10, 35, 25, magentaStyle);
    }
    
    draw::Font font("Arial", 14);
    draw::Color textColor = draw::Color(0, 0, 0, 255);
    
    const char* annotations[] = {
        "Point 1", "Point 2", "Point 3", "Point 4", "Point 5",
        "Line A", "Line B", "Line C",
        "Polygon 1", "Polygon 2",
        "Ellipse Zone", "Circle Area",
        "Annotation Demo", "Mock Data Render"
    };
    
    double textPositions[][2] = {
        {30, 30}, {105, 30}, {180, 30}, {255, 30}, {330, 30},
        {100, 200}, {240, 200}, {380, 200},
        {150, 360}, {370, 400},
        {100, 500}, {280, 500},
        {350, 550}, {500, 570}
    };
    
    for (int i = 0; i < 14; ++i) {
        m_impl->device->DrawText(textPositions[i][0], textPositions[i][1], 
                                 annotations[i], font, textColor);
    }
    
    drawResult = m_impl->device->EndDraw();
    if (drawResult != draw::DrawResult::kSuccess) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to end drawing");
    }
    
    uint8_t* data = m_impl->device->GetData();
    int width = m_impl->device->GetWidth();
    int height = m_impl->device->GetHeight();
    int channels = m_impl->device->GetChannels();
    
    if (!data || width <= 0 || height <= 0) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Invalid image data");
    }
    
    const char* textLabels[] = {
        "P1", "P2", "P3", "P4", "P5",
        "Line1", "Line2", "Line3",
        "Rect1", "Rect2",
        "Ellipse1", "Ellipse2",
        "[MultiPoint:4pts]", "[MultiLine:2lines]", "[MultiPolygon:2polys]",
        "=== LEGEND ===",
        "Red=Point, Green=Line, Blue=Polygon",
        "Cyan=MultiPt, Orange=MultiLn, Magenta=MultiPoly"
    };
    
    int textX[] = {30, 105, 180, 255, 330, 100, 240, 380, 150, 370, 100, 280, 420, 520, 620, 10, 10, 10};
    int textY[] = {80, 80, 80, 80, 80, 340, 340, 340, 460, 460, 560, 560, 60, 60, 60, 580, 595, 610};
    
    for (int i = 0; i < 18; ++i) {
    //    DrawTextOnImage(data, width, height, channels, textX[i], textY[i], textLabels[i], 0, 0, 0, true);
    }
    
    
#ifdef _WIN32
   
    
    std::vector<std::tuple<int, int, std::string>> gdiTexts;
    gdiTexts.push_back(std::make_tuple(10, 10, "GDI Text Demo - Arial 14pt"));
    gdiTexts.push_back(std::make_tuple(10, 35, "Points: Red circles"));
    gdiTexts.push_back(std::make_tuple(10, 55, "Lines: Green lines"));
    gdiTexts.push_back(std::make_tuple(200, 10, "Polygons: Blue rectangles"));
    gdiTexts.push_back(std::make_tuple(200, 30, "MultiPoint: Cyan circles"));
    gdiTexts.push_back(std::make_tuple(400, 10, "MultiLine: Orange lines"));
    gdiTexts.push_back(std::make_tuple(400, 30, "MultiPoly: Magenta rects"));
    
    bool gdiResult = DrawMultipleTextsWithGDI(data, width, height, channels, gdiTexts, 0, 0, 0, "Arial", 14);
    
#else
#endif
    
    std::string bmpPath = outputPath;
    if (bmpPath.size() >= 4 && bmpPath.substr(bmpPath.size() - 4) == ".png") {
        bmpPath = bmpPath.substr(0, bmpPath.size() - 4) + ".bmp";
    }
    
    bool saved = SaveAsBMP(bmpPath, width, height, data, channels);
    if (!saved) {
        return MokRenderResult(MokRenderErrorCode::RENDER_ERROR,
                              "Failed to save output file: " + bmpPath);
    }
    
    return MokRenderResult();
}

}
}
