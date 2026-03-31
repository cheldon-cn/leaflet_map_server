#include "chart_parser/data_converter.h"

namespace chart {
namespace parser {

DataConverterFactory& DataConverterFactory::Instance() {
    static DataConverterFactory instance;
    return instance;
}

void DataConverterFactory::RegisterConverter(const std::string& type, CreatorFunc creator) {
    m_creators[type] = creator;
}

void DataConverterFactory::UnregisterConverter(const std::string& type) {
    m_creators.erase(type);
}

std::unique_ptr<IDataConverter> DataConverterFactory::CreateConverter(const std::string& type) {
    auto it = m_creators.find(type);
    if (it != m_creators.end()) {
        return it->second();
    }
    return nullptr;
}

std::vector<std::string> DataConverterFactory::GetRegisteredTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : m_creators) {
        types.push_back(pair.first);
    }
    return types;
}

bool DataConverterFactory::HasConverter(const std::string& type) const {
    return m_creators.find(type) != m_creators.end();
}

} // namespace parser
} // namespace chart
