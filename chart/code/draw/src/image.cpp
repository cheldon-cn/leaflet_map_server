#include "ogc/draw/image.h"
#include <cstring>
#include <memory>

namespace ogc {
namespace draw {

struct Image::Impl {
    int width;
    int height;
    int channels;
    std::vector<uint8_t> data;
    
    Impl() : width(0), height(0), channels(0) {}
    Impl(int w, int h, int c) : width(w), height(h), channels(c) {
        data.resize(static_cast<size_t>(w) * h * c);
    }
};

Image::Image()
    : impl_(new Impl()) {
}

Image::Image(int width, int height, int channels)
    : impl_(new Impl(width, height, channels)) {
}

Image::Image(int width, int height, int channels, const uint8_t* data)
    : impl_(new Impl(width, height, channels)) {
    if (data) {
        std::memcpy(impl_->data.data(), data, impl_->data.size());
    }
}

Image::~Image() = default;

Image::Image(const Image& other)
    : impl_(new Impl(other.impl_->width, other.impl_->height, other.impl_->channels)) {
    if (!other.impl_->data.empty()) {
        std::memcpy(impl_->data.data(), other.impl_->data.data(), other.impl_->data.size());
    }
}

Image& Image::operator=(const Image& other) {
    if (this != &other) {
        impl_->width = other.impl_->width;
        impl_->height = other.impl_->height;
        impl_->channels = other.impl_->channels;
        impl_->data = other.impl_->data;
    }
    return *this;
}

Image::Image(Image&& other) noexcept
    : impl_(std::move(other.impl_)) {
    other.impl_.reset(new Impl());
}

Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) {
        impl_ = std::move(other.impl_);
        other.impl_.reset(new Impl());
    }
    return *this;
}

int Image::GetWidth() const { return impl_->width; }
int Image::GetHeight() const { return impl_->height; }
int Image::GetChannels() const { return impl_->channels; }

uint8_t* Image::GetData() { return impl_->data.empty() ? nullptr : impl_->data.data(); }
const uint8_t* Image::GetData() const { return impl_->data.empty() ? nullptr : impl_->data.data(); }
size_t Image::GetDataSize() const { return impl_->data.size(); }

bool Image::IsValid() const { return impl_->width > 0 && impl_->height > 0 && impl_->channels > 0 && !impl_->data.empty(); }
bool Image::IsEmpty() const { return impl_->data.empty(); }

ImageFormat Image::GetFormat() const {
    switch (impl_->channels) {
        case 1: return ImageFormat::kGrayscale;
        case 2: return ImageFormat::kGrayscaleAlpha;
        case 3: return ImageFormat::kRGB;
        case 4: return ImageFormat::kRGBA;
        default: return ImageFormat::kUnknown;
    }
}

void Image::Resize(int width, int height, int channels) {
    impl_->width = width;
    impl_->height = height;
    impl_->channels = channels;
    impl_->data.resize(static_cast<size_t>(width) * height * channels);
}

void Image::Clear() {
    impl_->width = 0;
    impl_->height = 0;
    impl_->channels = 0;
    impl_->data.clear();
}

bool Image::LoadFromFile(const std::string& path) {
    (void)path;
    return false;
}

bool Image::SaveToFile(const std::string& path) const {
    (void)path;
    return false;
}

Image Image::Clone() const {
    return Image(impl_->width, impl_->height, impl_->channels, impl_->data.empty() ? nullptr : impl_->data.data());
}

Color Image::GetPixel(int x, int y) const {
    if (x < 0 || x >= impl_->width || y < 0 || y >= impl_->height || impl_->data.empty()) {
        return Color::Transparent();
    }
    
    size_t index = static_cast<size_t>((y * impl_->width + x) * impl_->channels);
    
    if (impl_->channels >= 3) {
        uint8_t r = impl_->data[index];
        uint8_t g = impl_->data[index + 1];
        uint8_t b = impl_->data[index + 2];
        uint8_t a = impl_->channels >= 4 ? impl_->data[index + 3] : 255;
        return Color(r, g, b, a);
    } else if (impl_->channels == 1) {
        uint8_t v = impl_->data[index];
        return Color(v, v, v, 255);
    }
    
    return Color::Transparent();
}

void Image::SetPixel(int x, int y, const Color& color) {
    if (x < 0 || x >= impl_->width || y < 0 || y >= impl_->height || impl_->data.empty()) {
        return;
    }
    
    size_t index = static_cast<size_t>((y * impl_->width + x) * impl_->channels);
    
    if (impl_->channels >= 3) {
        impl_->data[index] = color.GetRed();
        impl_->data[index + 1] = color.GetGreen();
        impl_->data[index + 2] = color.GetBlue();
        if (impl_->channels >= 4) {
            impl_->data[index + 3] = color.GetAlpha();
        }
    } else if (impl_->channels == 1) {
        impl_->data[index] = color.GetRed();
    }
}

Image Image::Scaled(int newWidth, int newHeight) const {
    (void)newWidth;
    (void)newHeight;
    return Clone();
}

Image Image::Cropped(int x, int y, int width, int height) const {
    if (x < 0 || y < 0 || width <= 0 || height <= 0) {
        return Image();
    }
    if (x + width > impl_->width || y + height > impl_->height) {
        return Image();
    }
    
    Image result(width, height, impl_->channels);
    for (int row = 0; row < height; ++row) {
        size_t srcIndex = static_cast<size_t>(((y + row) * impl_->width + x) * impl_->channels);
        size_t dstIndex = static_cast<size_t>(row * width * impl_->channels);
        std::memcpy(result.impl_->data.data() + dstIndex, impl_->data.data() + srcIndex, 
                    static_cast<size_t>(width) * impl_->channels);
    }
    return result;
}

Image Image::FromData(int width, int height, int channels, const uint8_t* data) {
    return Image(width, height, channels, data);
}

Image Image::Solid(int width, int height, const Color& color) {
    Image img(width, height, 4);
    for (int i = 0; i < width * height; ++i) {
        img.impl_->data[i * 4] = color.GetRed();
        img.impl_->data[i * 4 + 1] = color.GetGreen();
        img.impl_->data[i * 4 + 2] = color.GetBlue();
        img.impl_->data[i * 4 + 3] = color.GetAlpha();
    }
    return img;
}

}  
}  
