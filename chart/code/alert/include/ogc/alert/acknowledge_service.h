#ifndef OGC_ALERT_ACKNOWLEDGE_SERVICE_H
#define OGC_ALERT_ACKNOWLEDGE_SERVICE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

class IAlertRepository;

class OGC_ALERT_API IAcknowledgeService {
public:
    virtual ~IAcknowledgeService() = default;
    
    virtual bool AcknowledgeAlert(const std::string& alert_id, 
                                   const std::string& user_id,
                                   const std::string& action) = 0;
    virtual bool DismissAlert(const std::string& alert_id, const std::string& user_id) = 0;
    
    virtual bool SubmitFeedback(const FeedbackData& feedback) = 0;
    virtual bool SubmitFeedback(const std::string& alert_id,
                                const std::string& user_id,
                                const std::string& feedback_type,
                                const std::string& feedback_content) = 0;
    
    virtual std::vector<AcknowledgeData> GetAcknowledgeHistory(const std::string& alert_id) = 0;
    virtual std::vector<FeedbackData> GetFeedbackHistory(const std::string& alert_id) = 0;
    
    virtual bool IsAcknowledged(const std::string& alert_id, const std::string& user_id) = 0;
    
    static std::unique_ptr<IAcknowledgeService> Create();
};

class OGC_ALERT_API AcknowledgeService : public IAcknowledgeService {
public:
    AcknowledgeService();
    explicit AcknowledgeService(std::shared_ptr<IAlertRepository> repository);
    ~AcknowledgeService();
    
    bool AcknowledgeAlert(const std::string& alert_id, 
                          const std::string& user_id,
                          const std::string& action) override;
    bool DismissAlert(const std::string& alert_id, const std::string& user_id) override;
    
    bool SubmitFeedback(const FeedbackData& feedback) override;
    bool SubmitFeedback(const std::string& alert_id,
                        const std::string& user_id,
                        const std::string& feedback_type,
                        const std::string& feedback_content) override;
    
    std::vector<AcknowledgeData> GetAcknowledgeHistory(const std::string& alert_id) override;
    std::vector<FeedbackData> GetFeedbackHistory(const std::string& alert_id) override;
    
    bool IsAcknowledged(const std::string& alert_id, const std::string& user_id) override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
