#ifndef OGC_ALERT_CHECKER_H
#define OGC_ALERT_CHECKER_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

struct CheckContext;

class OGC_ALERT_API IAlertChecker {
public:
    virtual ~IAlertChecker() = default;
    
    virtual std::string GetCheckerId() const = 0;
    virtual AlertType GetAlertType() const = 0;
    virtual int GetPriority() const = 0;
    virtual bool IsEnabled() const = 0;
    virtual void SetEnabled(bool enabled) = 0;
    
    virtual std::vector<AlertPtr> Check(const CheckContext& context) = 0;
    
    virtual void SetThreshold(const AlertThreshold& threshold) = 0;
    virtual AlertThreshold GetThreshold() const = 0;
    
    static std::unique_ptr<IAlertChecker> Create(AlertType type);
};

using IAlertCheckerPtr = std::shared_ptr<IAlertChecker>;

}
}

#endif
