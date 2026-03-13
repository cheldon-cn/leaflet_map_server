#include "png_encoder.h"
#include "../utils/logger.h"
#include <cstring>

namespace cycle {
namespace encoder {

struct PngWriteData {
    std::vector<uint8_t>* buffer;
    size_t offset;
};

PngEncoder::PngEncoder() {
}

PngEncoder::~PngEncoder() {
}

EncodedImage PngEncoder::Encode(const RawImage& image,
                                const EncodeOptions& options) {
    EncodedImage result;
    
    if (!image.IsValid()) {
        LOG_ERROR("Invalid image data for PNG encoding");
        return result;
    }
    
    if (!options.IsValid()) {
        LOG_ERROR("Invalid encoding options");
        return result;
    }
    
    std::vector<uint8_t> output;
    bool success = false;
    
    if (options.format == ImageFormat::PNG8) {
        success = EncodePng8(image, options, output);
    } else {
        success = EncodePng32(image, options, output);
    }
    
    if (success) {
        result.data = std::move(output);
        result.mime_type = GetMimeType();
        result.format = options.format;
        result.size = result.data.size();
    }
    
    return result;
}

bool PngEncoder::EncodePng8(const RawImage& image,
                            const EncodeOptions& options,
                            std::vector<uint8_t>& output) {
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                    nullptr, nullptr, nullptr);
    if (!png_ptr) {
        LOG_ERROR("Failed to create PNG write struct");
        return false;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        LOG_ERROR("Failed to create PNG info struct");
        return false;
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        LOG_ERROR("PNG encoding error");
        return false;
    }
    
    PngWriteData writeData;
    writeData.buffer = &output;
    writeData.offset = 0;
    
    png_set_write_fn(png_ptr, &writeData, PngWriteCallback, PngFlushCallback);
    
    png_set_IHDR(png_ptr, info_ptr,
                 image.width, image.height,
                 8, PNG_COLOR_TYPE_PALETTE,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    std::vector<png_color> palette(256);
    std::vector<png_byte> trans(256);
    
    for (size_t i = 0; i < 256; ++i) {
        palette[i].red = static_cast<png_byte>(i);
        palette[i].green = static_cast<png_byte>(i);
        palette[i].blue = static_cast<png_byte>(i);
        trans[i] = 255;
    }
    
    png_set_PLTE(png_ptr, info_ptr, palette.data(), 256);
    png_set_tRNS(png_ptr, info_ptr, trans.data(), 256, nullptr);
    
    WritePhysicalChunk(png_ptr, info_ptr, options.dpi_x, options.dpi_y);
    
    png_set_compression_level(png_ptr, options.png_compression);
    
    png_write_info(png_ptr, info_ptr);
    
    std::vector<png_bytep> row_pointers(image.height);
    std::vector<png_byte> indexed_data(image.width * image.height);
    
    for (int y = 0; y < image.height; ++y) {
        row_pointers[y] = &indexed_data[y * image.width];
        
        for (int x = 0; x < image.width; ++x) {
            size_t src_idx = (y * image.width + x) * image.channels;
            
            uint8_t r = image.data[src_idx];
            uint8_t g = image.data[src_idx + 1];
            uint8_t b = image.data[src_idx + 2];
            
            indexed_data[y * image.width + x] = static_cast<uint8_t>(
                (r + g + b) / 3);
        }
    }
    
    png_write_image(png_ptr, row_pointers.data());
    png_write_end(png_ptr, nullptr);
    
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    return true;
}

bool PngEncoder::EncodePng32(const RawImage& image,
                             const EncodeOptions& options,
                             std::vector<uint8_t>& output) {
    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                    nullptr, nullptr, nullptr);
    if (!png_ptr) {
        LOG_ERROR("Failed to create PNG write struct");
        return false;
    }
    
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_write_struct(&png_ptr, nullptr);
        LOG_ERROR("Failed to create PNG info struct");
        return false;
    }
    
    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        LOG_ERROR("PNG encoding error");
        return false;
    }
    
    PngWriteData writeData;
    writeData.buffer = &output;
    writeData.offset = 0;
    
    png_set_write_fn(png_ptr, &writeData, PngWriteCallback, PngFlushCallback);
    
    int colorType = (image.channels == 4) ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB;
    
    png_set_IHDR(png_ptr, info_ptr,
                 image.width, image.height,
                 8, colorType,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    
    WritePhysicalChunk(png_ptr, info_ptr, options.dpi_x, options.dpi_y);
    
    png_set_compression_level(png_ptr, options.png_compression);
    
    png_write_info(png_ptr, info_ptr);
    
    std::vector<png_bytep> row_pointers(image.height);
    for (int y = 0; y < image.height; ++y) {
        row_pointers[y] = const_cast<png_bytep>(
            &image.data[y * image.width * image.channels]);
    }
    
    png_write_image(png_ptr, row_pointers.data());
    png_write_end(png_ptr, nullptr);
    
    png_destroy_write_struct(&png_ptr, &info_ptr);
    
    return true;
}

void PngEncoder::WritePhysicalChunk(png_structp png_ptr,
                                    png_infop info_ptr,
                                    int dpiX,
                                    int dpiY) {
    png_uint_32 res_x = static_cast<png_uint_32>(dpiX * 39.37);
    png_uint_32 res_y = static_cast<png_uint_32>(dpiY * 39.37);
    
    png_set_pHYs(png_ptr, info_ptr, res_x, res_y, PNG_RESOLUTION_METER);
}

void PngEncoder::PngWriteCallback(png_structp png_ptr,
                                  png_bytep data,
                                  png_size_t length) {
    PngWriteData* writeData = static_cast<PngWriteData*>(png_get_io_ptr(png_ptr));
    
    size_t oldSize = writeData->buffer->size();
    writeData->buffer->resize(oldSize + length);
    
    std::memcpy(writeData->buffer->data() + oldSize, data, length);
}

void PngEncoder::PngFlushCallback(png_structp png_ptr) {
}

} // namespace encoder
} // namespace cycle
