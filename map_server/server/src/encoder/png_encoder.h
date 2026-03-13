#ifndef CYCLE_ENCODER_PNG_ENCODER_H
#define CYCLE_ENCODER_PNG_ENCODER_H

#include "iencoder.h"
#include <png.h>

namespace cycle {
namespace encoder {

class PngEncoder : public IEncoder {
public:
    PngEncoder();
    ~PngEncoder() override;
    
    EncodedImage Encode(const RawImage& image,
                       const EncodeOptions& options) override;
    
    ImageFormat GetSupportedFormat() const override { return ImageFormat::PNG32; }
    std::string GetMimeType() const override { return "image/png"; }
    std::string GetFormatName() const override { return "PNG"; }

    bool Initialize() override { return true; }
    bool IsInitialized() const override { return true; }
    std::string GetName() const override { return "PNG"; }

    bool SupportsFormat(ImageFormat format) const override {
        return format == ImageFormat::PNG8 || format == ImageFormat::PNG32;
    }
    
private:
    bool EncodePng8(const RawImage& image, 
                    const EncodeOptions& options,
                    std::vector<uint8_t>& output);
    
    bool EncodePng32(const RawImage& image,
                     const EncodeOptions& options,
                     std::vector<uint8_t>& output);
    
    void WritePhysicalChunk(png_structp png_ptr,
                           png_infop info_ptr,
                           int dpiX,
                           int dpiY);
    
    static void PngWriteCallback(png_structp png_ptr,
                                 png_bytep data,
                                 png_size_t length);
    
    static void PngFlushCallback(png_structp png_ptr);
};

} // namespace encoder
} // namespace cycle

#endif // CYCLE_ENCODER_PNG_ENCODER_H
