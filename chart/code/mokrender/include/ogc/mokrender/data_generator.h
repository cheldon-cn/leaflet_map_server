#pragma once

#include "ogc/mokrender/interfaces.h"
#include "ogc/mokrender/common.h"
#include <memory>
#include <random>

namespace ogc {
namespace mokrender {

class OGC_MOKRENDER_API DataGenerator : public IDataGenerator {
public:
    DataGenerator();
    ~DataGenerator() override;
    
    MokRenderResult Initialize(const DataGeneratorConfig& config) override;
    MokRenderResult Generate() override;
    MokRenderResult SaveToDatabase(const std::string& dbPath) override;
    int GetGeneratedCount() const override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}
