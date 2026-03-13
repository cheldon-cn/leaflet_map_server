#ifndef CYCLE_ENCODER_WEBP_ENCODER_H
#define CYCLE_ENCODER_WEBP_ENCODER_H

#include "iencoder.h"

#ifdef HAVE_WEBP
#include <webp/encode.h>
#include <webp/decode.h>
#include <vector>
#include <memory>

namespace cycle {
namespace encoder {

class WebPEncoder : public IEncoder {
public:
    WebPEncoder();
    ~WebPEncoder() override;
    
    bool Initialize() override;
    bool IsInitialized() const override;
    
    EncodedImage Encode(const RawImage& image, const EncodeOptions& options) override;
    
    bool SupportsFormat(ImageFormat format) const override;
    std::string GetName() const override;
    
    ImageFormat GetSupportedFormat() const override;
    std::string GetMimeType() const override;
    std::string GetFormatName() const override;
    
    void SetQuality(int quality);
    int GetQuality() const;
    
    void SetCompressionLevel(int level);
    int GetCompressionLevel() const;
    
    std::string GetLastError() const;
    
private:
    std::vector<uint8_t> EncodeLossless(const RawImage& image);
    std::vector<uint8_t> EncodeLossy(const RawImage& image, int quality);
    
    bool ValidateImageData(const RawImage& image);
    bool ConvertToRGB(const RawImage& image, std::vector<uint8_t>& rgbData);
    bool ConvertToRGBA(const RawImage& image, std::vector<uint8_t>& rgbaData);
    
    mutable int quality_;
    mutable int compressionLevel_;
    mutable bool initialized_;
    mutable std::string lastError_;
};

} // namespace encoder
} // namespace cycle

#endif // HAVE_WEBP

#endif // CYCLE_ENCODER_WEBP_ENCODER_H
