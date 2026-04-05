#ifndef ALERT_REPOSITORY_H
#define ALERT_REPOSITORY_H

#include "alert/alert.h"
#include "alert/alert_data_access.h"
#include <memory>
#include <vector>
#include <string>

namespace alert {

class IAlertRepository {
public:
    virtual ~IAlertRepository() {}
    
    virtual bool Create(const Alert& alert) = 0;
    virtual bool Update(const Alert& alert) = 0;
    virtual bool Remove(const std::string& alertId) = 0;
    virtual Alert GetById(const std::string& alertId) = 0;
    
    virtual std::vector<Alert> FindAll(int limit = 100, int offset = 0) = 0;
    virtual std::vector<Alert> FindByShipId(const std::string& shipId) = 0;
    virtual std::vector<Alert> FindByType(AlertType type) = 0;
    virtual std::vector<Alert> FindByLevel(AlertLevel level) = 0;
    virtual std::vector<Alert> FindByStatus(AlertStatus status) = 0;
    virtual std::vector<Alert> FindActive() = 0;
    
    virtual bool Acknowledge(const std::string& alertId, const std::string& operatorId) = 0;
    virtual bool Expire(const std::string& alertId) = 0;
    virtual bool Dismiss(const std::string& alertId, const std::string& reason) = 0;
    
    virtual AlertStatistics GetStatistics(const std::string& startTime, const std::string& endTime) = 0;
    virtual int GetActiveCount() = 0;
};

class AlertRepository : public IAlertRepository {
public:
    AlertRepository();
    AlertRepository(std::shared_ptr<IAlertDataAccess> dataAccess);
    virtual ~AlertRepository();
    
    bool Create(const Alert& alert) override;
    bool Update(const Alert& alert) override;
    bool Remove(const std::string& alertId) override;
    Alert GetById(const std::string& alertId) override;
    
    std::vector<Alert> FindAll(int limit = 100, int offset = 0) override;
    std::vector<Alert> FindByShipId(const std::string& shipId) override;
    std::vector<Alert> FindByType(AlertType type) override;
    std::vector<Alert> FindByLevel(AlertLevel level) override;
    std::vector<Alert> FindByStatus(AlertStatus status) override;
    std::vector<Alert> FindActive() override;
    
    bool Acknowledge(const std::string& alertId, const std::string& operatorId) override;
    bool Expire(const std::string& alertId) override;
    bool Dismiss(const std::string& alertId, const std::string& reason) override;
    
    AlertStatistics GetStatistics(const std::string& startTime, const std::string& endTime) override;
    int GetActiveCount() override;
    
    void SetDataAccess(std::shared_ptr<IAlertDataAccess> dataAccess);
    
private:
    std::shared_ptr<IAlertDataAccess> m_dataAccess;
    bool m_ownsDataAccess;
};

}

#endif
