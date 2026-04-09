#ifndef DATA_CONVERTER_H
#define DATA_CONVERTER_H

#include "parse_result.h"
#include <memory>
#include <functional>

namespace chart {
namespace parser {

class IDataConverter {
public:
    virtual ~IDataConverter() = default;
    
    virtual bool ConvertGeometry(void* sourceGeometry, Geometry& outGeometry) = 0;
    virtual bool ConvertAttributes(void* sourceFeature, AttributeMap& outAttributes) = 0;
    
    virtual std::string GetName() const = 0;
    virtual std::string GetSourceType() const = 0;
};

class DataConverterFactory {
public:
    using CreatorFunc = std::function<std::unique_ptr<IDataConverter>()>;
    
    static DataConverterFactory& Instance();
    
    void RegisterConverter(const std::string& type, CreatorFunc creator);
    void UnregisterConverter(const std::string& type);
    
    std::unique_ptr<IDataConverter> CreateConverter(const std::string& type);
    
    std::vector<std::string> GetRegisteredTypes() const;
    bool HasConverter(const std::string& type) const;
    
private:
    DataConverterFactory() = default;
    ~DataConverterFactory() = default;
    
    DataConverterFactory(const DataConverterFactory&) = delete;
    DataConverterFactory& operator=(const DataConverterFactory&) = delete;
    
    std::map<std::string, CreatorFunc> m_creators;
};

template<typename T>
class ConverterRegistrar {
public:
    explicit ConverterRegistrar(const std::string& type) {
        DataConverterFactory::Instance().RegisterConverter(type, 
            []() -> std::unique_ptr<IDataConverter> {
                return std::make_unique<T>();
            });
    }
};

#define REGISTER_DATA_CONVERTER(Type, ClassName) \
    static chart::parser::ConverterRegistrar<ClassName> g_##ClassName##_registrar(Type)

} // namespace parser
} // namespace chart

#endif // DATA_CONVERTER_H
