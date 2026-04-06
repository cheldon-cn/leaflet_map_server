#include "alert/alert_engine_factory.h"
#include "alert/alert_exception.h"
#include "alert/checker_adapter.h"
#include "ogc/alert/alert_checker.h"
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

void AlertEngineFactory::RegisterEngineEx(AlertType type, EngineCreatorEx creator) {
    m_creatorsEx[type] = creator;
}

IAlertEngine* AlertEngineFactory::CreateEngine(AlertType type) {
    auto itEx = m_creatorsEx.find(type);
    if (itEx != m_creatorsEx.end()) {
        return itEx->second().release();
    }
    
    auto it = m_creators.find(type);
    if (it != m_creators.end()) {
        return it->second();
    }
    
    auto engine = alarm::CheckerRegistry::Instance().CreateEngine(type);
    if (engine) {
        return engine.release();
    }
    
    throw EngineException("Engine not registered for type: " + AlertTypeToString(type));
}

std::unique_ptr<IAlertEngine> AlertEngineFactory::CreateEngineEx(AlertType type) {
    auto itEx = m_creatorsEx.find(type);
    if (itEx != m_creatorsEx.end()) {
        return itEx->second();
    }
    
    auto engine = alarm::CheckerRegistry::Instance().CreateEngine(type);
    if (engine) {
        return engine;
    }
    
    auto it = m_creators.find(type);
    if (it != m_creators.end()) {
        return std::unique_ptr<IAlertEngine>(it->second());
    }
    
    throw EngineException("Engine not registered for type: " + AlertTypeToString(type));
}

bool AlertEngineFactory::IsEngineRegistered(AlertType type) const {
    if (m_creatorsEx.find(type) != m_creatorsEx.end()) {
        return true;
    }
    if (m_creators.find(type) != m_creators.end()) {
        return true;
    }
    return alarm::CheckerRegistry::Instance().GetChecker(type) != nullptr;
}

std::vector<AlertType> AlertEngineFactory::GetRegisteredTypes() const {
    std::vector<AlertType> types;
    for (const auto& pair : m_creators) {
        types.push_back(pair.first);
    }
    for (const auto& pair : m_creatorsEx) {
        if (std::find(types.begin(), types.end(), pair.first) == types.end()) {
            types.push_back(pair.first);
        }
    }
    return types;
}

void AlertEngineFactory::RegisterDefaultEngines() {
}

void AlertEngineFactory::RegisterAlertCheckers() {
    auto depth_checker = ogc::alert::IAlertChecker::Create(ogc::alert::AlertType::kDepth);
    if (depth_checker) {
        alarm::CheckerRegistry::Instance().RegisterChecker(
            AlertType::kDepthAlert, 
            std::shared_ptr<ogc::alert::IAlertChecker>(depth_checker.release()));
    }
    
    auto collision_checker = ogc::alert::IAlertChecker::Create(ogc::alert::AlertType::kCollision);
    if (collision_checker) {
        alarm::CheckerRegistry::Instance().RegisterChecker(
            AlertType::kCollisionAlert, 
            std::shared_ptr<ogc::alert::IAlertChecker>(collision_checker.release()));
    }
    
    auto weather_checker = ogc::alert::IAlertChecker::Create(ogc::alert::AlertType::kWeather);
    if (weather_checker) {
        alarm::CheckerRegistry::Instance().RegisterChecker(
            AlertType::kWeatherAlert, 
            std::shared_ptr<ogc::alert::IAlertChecker>(weather_checker.release()));
    }
    
    auto channel_checker = ogc::alert::IAlertChecker::Create(ogc::alert::AlertType::kChannel);
    if (channel_checker) {
        alarm::CheckerRegistry::Instance().RegisterChecker(
            AlertType::kChannelAlert, 
            std::shared_ptr<ogc::alert::IAlertChecker>(channel_checker.release()));
    }
}

}
