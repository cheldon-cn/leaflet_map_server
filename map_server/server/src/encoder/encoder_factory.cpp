#include "encoder_factory.h"
#include "png_encoder.h"
#ifdef ENABLE_WEBP
#include "webp_encoder.h"
#endif
#include "../utils/logger.h"

namespace cycle {
namespace encoder {

std::shared_ptr<IEncoder> EncoderFactory::Create(ImageFormat format) {
    switch (format) {
        case ImageFormat::PNG8:
        case ImageFormat::PNG32:
            return std::make_shared<PngEncoder>();
        
#ifdef ENABLE_WEBP
        case ImageFormat::WEBP:
            return std::make_shared<WebpEncoder>();
#endif
        
        default:
            LOG_ERROR("Unsupported image format");
            return nullptr;
    }
}

std::shared_ptr<IEncoder> EncoderFactory::Create(const std::string& formatStr) {
    return Create(StringToImageFormat(formatStr));
}

std::vector<ImageFormat> EncoderFactory::GetSupportedFormats() {
    std::vector<ImageFormat> formats = {
        ImageFormat::PNG8,
        ImageFormat::PNG32,
        ImageFormat::WEBP
    };
    
#ifdef ENABLE_WEBP
    formats.push_back(ImageFormat::WEBP);
#endif
    
    return formats;
}

ImageFormat EncoderFactory::GetDefaultFormat()
{
    return ImageFormat(ImageFormat::PNG8);
}

} // namespace encoder
} // namespace cycle
