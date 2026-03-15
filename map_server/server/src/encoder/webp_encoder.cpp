#include "webp_encoder.h"
#include "../utils/logger.h"
#include <algorithm>
#include <cstring>

#ifdef HAVE_WEBP
namespace cycle {
namespace encoder {

WebPEncoder::WebPEncoder()
    : quality_(80)
    , compressionLevel_(6)
    , initialized_(false) {
    LOG_INFO("WebP encoder created");
}

WebPEncoder::~WebPEncoder() {
    LOG_INFO("WebP encoder destroyed");
}

bool WebPEncoder::Initialize() {
    if (initialized_) {
        LOG_WARN("WebP encoder is already initialized");
        return true;
    }
    
    int version = WebPGetEncoderVersion();
    LOG_INFO("WebP encoder version: " + std::to_string(version));
    
    initialized_ = true;
    LOG_INFO("WebP encoder initialized successfully");
    
    return true;
}

bool WebPEncoder::IsInitialized() const {
    return initialized_;
}

EncodedImage WebPEncoder::Encode(const RawImage& image, const EncodeOptions& options) {
    EncodedImage result;
    
    if (!initialized_) {
        lastError_ = "WebP encoder not initialized";
        LOG_ERROR(lastError_);
        return result;
    }
    
    if (image.width <= 0 || image.height <= 0 || image.data.empty()) {
        lastError_ = "Invalid image data";
        LOG_ERROR(lastError_);
        return result;
    }
    
    size_t pixelCount = static_cast<size_t>(image.width * image.height);
    uint8_t* rgbaData = new uint8_t[pixelCount * 4];
    
    if (image.channels == 4) {
        std::memcpy(rgbaData, image.data.data(), pixelCount * 4);
    } else if (image.channels == 3) {
        for (size_t i = 0; i < pixelCount; ++i) {
            rgbaData[i * 4] = image.data[i * 3];
            rgbaData[i * 4 + 1] = image.data[i * 3 + 1];
            rgbaData[i * 4 + 2] = image.data[i * 3 + 2];
            rgbaData[i * 4 + 3] = 255;
        }
    }
    
    uint8_t* output = nullptr;
    size_t outputSize = 0;

    if (options.webp_lossless) {
        outputSize = WebPEncodeLosslessRGBA(rgbaData, image.width, image.height,
            image.width * 4, &output);
    }
    else {
        outputSize = WebPEncodeRGBA(rgbaData, image.width, image.height,
            image.width * 4, options.webp_quality, &output);
    }

    if (outputSize == 0 || output == nullptr) {
        LOG_ERROR("WebP encoding failed");
        return {};
    }

    // 将数据复制到 std::vector
    std::vector<uint8_t> outputresult(output, output + outputSize);

    // 释放 WebP 分配的内存
    WebPFree(output);

    delete[] rgbaData;
    
    if (outputSize == 0) {
        lastError_ = "Failed to encode WebP";
        LOG_ERROR(lastError_);
        return result;
    }
    
    result.data = std::move(outputresult);
    result.format = ImageFormat::WEBP;
    result.mime_type = "image/webp";
    result.size = result.data.size();
    
    return result;
}

bool WebPEncoder::SupportsFormat(ImageFormat format) const {
    return format == ImageFormat::WEBP;
}

std::string WebPEncoder::GetName() const {
    return "WebP Encoder";
}

ImageFormat WebPEncoder::GetSupportedFormat() const {
    return ImageFormat::WEBP;
}

std::string WebPEncoder::GetMimeType() const {
    return "image/webp";
}

std::string WebPEncoder::GetFormatName() const {
    return "WebP";
}

void WebPEncoder::SetQuality(int quality) {
    quality_ = std::max(0, std::min(100, quality));
}

int WebPEncoder::GetQuality() const {
    return quality_;
}

void WebPEncoder::SetCompressionLevel(int level) {
    compressionLevel_ = std::max(0, std::min(9, level));
}

int WebPEncoder::GetCompressionLevel() const {
    return compressionLevel_;
}

std::string WebPEncoder::GetLastError() const {
    return lastError_;
}

std::vector<uint8_t> WebPEncoder::EncodeLossless(const RawImage& image) {
    EncodedImage result = Encode(image, EncodeOptions());
    return result.data;
}

std::vector<uint8_t> WebPEncoder::EncodeLossy(const RawImage& image, int quality) {
    EncodeOptions options;
    options.webp_quality = quality;
    EncodedImage result = Encode(image, options);
    return result.data;
}

bool WebPEncoder::ValidateImageData(const RawImage& image) {
    return image.width > 0 && image.height > 0 && !image.data.empty();
}

bool WebPEncoder::ConvertToRGB(const RawImage& image, std::vector<uint8_t>& rgbData) {
    if (image.channels == 3) {
        rgbData = image.data;
        return true;
    }
    return false;
}

bool WebPEncoder::ConvertToRGBA(const RawImage& image, std::vector<uint8_t>& rgbaData) {
    if (image.channels == 4) {
        rgbaData = image.data;
        return true;
    }
    return false;
}

} // namespace encoder
} // namespace cycle
#endif