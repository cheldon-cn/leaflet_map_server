#ifndef CYCLE_ENCODER_ENCODER_FACTORY_H
#define CYCLE_ENCODER_ENCODER_FACTORY_H

#include "iencoder.h"
#include <memory>
#include <string>
#include <vector>

namespace cycle {
namespace encoder {

class EncoderFactory {
public:
    static std::shared_ptr<IEncoder> Create(ImageFormat format);
    static std::shared_ptr<IEncoder> Create(const std::string& formatStr);
    
    static std::vector<ImageFormat> GetSupportedFormats();
    static ImageFormat GetDefaultFormat();
};

} // namespace encoder
} // namespace cycle

#endif // CYCLE_ENCODER_ENCODER_FACTORY_H
