#ifndef ALERT_SYSTEM_ALERT_ENGINE_FACTORY_H
#define ALERT_SYSTEM_ALERT_ENGINE_FACTORY_H

#include "i_alert_engine.h"
#include "alert_types.h"
#include <map>
#include <vector>
#include <functional>
#include <memory>

namespace alert {

class AlertEngineFactory {
public:
    typedef std::function<IAlertEngine*()> EngineCreator;
    typedef std::function<std::unique_ptr<IAlertEngine>()> EngineCreatorEx;
    
    static AlertEngineFactory& GetInstance();
    
    void RegisterEngine(AlertType type, EngineCreator creator);
    void RegisterEngineEx(AlertType type, EngineCreatorEx creator);
    
    IAlertEngine* CreateEngine(AlertType type);
    std::unique_ptr<IAlertEngine> CreateEngineEx(AlertType type);
    
    bool IsEngineRegistered(AlertType type) const;
    
    std::vector<AlertType> GetRegisteredTypes() const;
    
    void RegisterDefaultEngines();
    void RegisterAlertCheckers();
    
private:
    AlertEngineFactory();
    ~AlertEngineFactory();
    
    AlertEngineFactory(const AlertEngineFactory&);
    AlertEngineFactory& operator=(const AlertEngineFactory&);
    
    std::map<AlertType, EngineCreator> m_creators;
    std::map<AlertType, EngineCreatorEx> m_creatorsEx;
};

template<typename T>
class EngineRegistrar {
public:
    explicit EngineRegistrar(AlertType type) {
        AlertEngineFactory::GetInstance().RegisterEngine(type, []() -> IAlertEngine* {
            return new T();
        });
    }
};

template<typename T>
class EngineRegistrarEx {
public:
    explicit EngineRegistrarEx(AlertType type) {
        AlertEngineFactory::GetInstance().RegisterEngineEx(type, []() -> std::unique_ptr<IAlertEngine> {
            return std::unique_ptr<IAlertEngine>(new T());
        });
    }
};

}

#endif
