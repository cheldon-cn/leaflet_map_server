#include "alert/alert_engine_factory.h"
#include "alert/alert_exception.h"
#include <stdexcept>

namespace alert {

AlertEngineFactory& AlertEngineFactory::GetInstance() {
    static AlertEngineFactory instance;
    return instance;
}

AlertEngineFactory::AlertEngineFactory() {}

AlertEngineFactory::~AlertEngineFactory() {}

void AlertEngineFactory::RegisterEngine(AlertType type, EngineCreator creator) {
    m_creators[type] = creator;
}

IAlertEngine* AlertEngineFactory::CreateEngine(AlertType type) {
    auto it = m_creators.find(type);
    if (it != m_creators.end()) {
        return it->second();
    }
    throw EngineException("Engine not registered for type: " + AlertTypeToString(type));
}

bool AlertEngineFactory::IsEngineRegistered(AlertType type) const {
    return m_creators.find(type) != m_creators.end();
}

std::vector<AlertType> AlertEngineFactory::GetRegisteredTypes() const {
    std::vector<AlertType> types;
    for (const auto& pair : m_creators) {
        types.push_back(pair.first);
    }
    return types;
}

void AlertEngineFactory::RegisterDefaultEngines() {
}

}
