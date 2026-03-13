#ifndef CYCLE_ENCODER_IENCODER_H
#define CYCLE_ENCODER_IENCODER_H

#include <vector>
#include <cstdint>
#include <string>
#include <memory>

namespace cycle {
namespace encoder {

enum class ImageFormat {
    PNG8,
    PNG32,
    WEBP
};

struct EncodeOptions {
    ImageFormat format = ImageFormat::PNG32;
    
    int  png_compression = 6;
    int  quality = 80;
    int  webp_quality = 80;
    bool webp_lossless = false;
    
    int dpi_x = 96;
    int dpi_y = 96;
    
    std::string software = "Cycle Map Server";
    std::string copyright = "";
    
    bool IsValid() const {
        return png_compression >= 1 && png_compression <= 9 &&
               webp_quality >= 0 && webp_quality <= 100 &&
               dpi_x > 0 && dpi_y > 0;
    }
};

struct RawImage {
    int width;
    int height;
    int channels;
    std::vector<uint8_t> data;
    
    size_t GetDataSize() const {
        return static_cast<size_t>(width * height * channels);
    }
    
    bool IsValid() const {
        return width > 0 && height > 0 && 
               (channels == 3 || channels == 4) &&
               data.size() == GetDataSize();
    }
    
    static RawImage Create(int w, int h, int ch) {
        RawImage img;
        img.width = w;
        img.height = h;
        img.channels = ch;
        img.data.resize(w * h * ch);
        return img;
    }
};

using ImageData = RawImage;

struct EncodedImage {
    std::vector<uint8_t> data;
    std::string mime_type;
    ImageFormat format;
    size_t size;
    
    bool IsValid() const {
        return !data.empty() && !mime_type.empty();
    }
    
    size_t GetDataSize() const {
        return data.size();
    }
};

class IEncoder {
public:
    virtual ~IEncoder() = default;
    
    virtual EncodedImage Encode(const RawImage& image,
                                const EncodeOptions& options) = 0;
    
    virtual ImageFormat GetSupportedFormat() const = 0;
    virtual std::string GetMimeType() const = 0;
    virtual std::string GetFormatName() const = 0;
    virtual bool Initialize() = 0;
    virtual bool IsInitialized() const = 0;
    virtual std::string GetName() const = 0;
    virtual bool SupportsFormat(ImageFormat format) const = 0;
};

inline std::string ImageFormatToString(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG8:   return "png8";
        case ImageFormat::PNG32:  return "png32";
        case ImageFormat::WEBP:   return "webp";
        default:                  return "unknown";
    }
}

inline ImageFormat StringToImageFormat(const std::string& str) {
    if (str == "png8")   return ImageFormat::PNG8;
    if (str == "png32")  return ImageFormat::PNG32;
    if (str == "webp")   return ImageFormat::WEBP;
    return ImageFormat::PNG32;
}

inline std::string GetMimeType(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG8:
        case ImageFormat::PNG32:  return "image/png";
        case ImageFormat::WEBP:   return "image/webp";
        default:                  return "application/octet-stream";
    }
}

} // namespace encoder
} // namespace cycle

#endif // CYCLE_ENCODER_IENCODER_H
