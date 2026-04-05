#ifndef ALERT_SYSTEM_ALERT_ENGINE_FACTORY_H
#define ALERT_SYSTEM_ALERT_ENGINE_FACTORY_H

#include "i_alert_engine.h"
#include "alert_types.h"
#include <map>
#include <vector>
#include <functional>

namespace alert {

class AlertEngineFactory {
public:
    typedef std::function<IAlertEngine*()> EngineCreator;
    
    static AlertEngineFactory& GetInstance();
    
    void RegisterEngine(AlertType type, EngineCreator creator);
    
    IAlertEngine* CreateEngine(AlertType type);
    
    bool IsEngineRegistered(AlertType type) const;
    
    std::vector<AlertType> GetRegisteredTypes() const;
    
    void RegisterDefaultEngines();
    
private:
    AlertEngineFactory();
    ~AlertEngineFactory();
    
    AlertEngineFactory(const AlertEngineFactory&);
    AlertEngineFactory& operator=(const AlertEngineFactory&);
    
    std::map<AlertType, EngineCreator> m_creators;
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

}

#endif
