#ifndef OGC_ALERT_FEEDBACK_SERVICE_H
#define OGC_ALERT_FEEDBACK_SERVICE_H

#include "types.h"
#include "export.h"
#include <string>
#include <vector>
#include <memory>

namespace ogc {
namespace alert {

enum class FeedbackType {
    POSITIVE,
    NEGATIVE,
    NEUTRAL,
    SUGGESTION,
    BUG_REPORT
};

enum class FeedbackStatus {
    PENDING,
    REVIEWED,
    RESOLVED,
    DISMISSED
};

struct AlertFeedback {
    std::string feedback_id;
    std::string alert_id;
    std::string user_id;
    FeedbackType type;
    FeedbackStatus status;
    std::string comment;
    int rating;
    std::string suggestion;
    DateTime created_at;
    DateTime updated_at;
    std::string reviewer;
    std::string resolution_note;
    
    AlertFeedback()
        : type(FeedbackType::NEUTRAL)
        , status(FeedbackStatus::PENDING)
        , rating(0) {}
};

struct FeedbackStats {
    int total_feedbacks;
    int positive_count;
    int negative_count;
    int neutral_count;
    int suggestion_count;
    int bug_report_count;
    double avg_rating;
    int pending_count;
    int resolved_count;
};

struct FeedbackFilter {
    std::string user_id;
    std::string alert_id;
    FeedbackType type;
    FeedbackStatus status;
    DateTime start_time;
    DateTime end_time;
    int min_rating;
    int max_rating;
    
    FeedbackFilter()
        : type(FeedbackType::NEUTRAL)
        , status(FeedbackStatus::PENDING)
        , min_rating(0)
        , max_rating(5) {}
};

class OGC_ALERT_API IFeedbackService {
public:
    virtual ~IFeedbackService() = default;
    
    virtual bool SubmitFeedback(const AlertFeedback& feedback) = 0;
    virtual bool UpdateFeedback(const std::string& feedback_id, const AlertFeedback& feedback) = 0;
    virtual bool DeleteFeedback(const std::string& feedback_id) = 0;
    virtual AlertFeedback GetFeedback(const std::string& feedback_id) = 0;
    virtual std::vector<AlertFeedback> GetFeedbacksByAlert(const std::string& alert_id) = 0;
    virtual std::vector<AlertFeedback> GetFeedbacksByUser(const std::string& user_id) = 0;
    virtual std::vector<AlertFeedback> QueryFeedbacks(const FeedbackFilter& filter) = 0;
    
    virtual bool ReviewFeedback(const std::string& feedback_id, const std::string& reviewer, const std::string& note) = 0;
    virtual bool ResolveFeedback(const std::string& feedback_id, const std::string& resolution_note) = 0;
    virtual bool DismissFeedback(const std::string& feedback_id, const std::string& reason) = 0;
    
    virtual FeedbackStats GetStats() = 0;
    virtual FeedbackStats GetStatsByAlertType(AlertType alert_type) = 0;
    virtual FeedbackStats GetStatsByDateRange(const DateTime& start, const DateTime& end) = 0;
    
    virtual std::vector<AlertFeedback> GetPendingFeedbacks() = 0;
    virtual std::vector<AlertFeedback> GetHighPriorityFeedbacks() = 0;
    
    static std::unique_ptr<IFeedbackService> Create();
};

class OGC_ALERT_API FeedbackService : public IFeedbackService {
public:
    FeedbackService();
    ~FeedbackService();
    
    bool SubmitFeedback(const AlertFeedback& feedback) override;
    bool UpdateFeedback(const std::string& feedback_id, const AlertFeedback& feedback) override;
    bool DeleteFeedback(const std::string& feedback_id) override;
    AlertFeedback GetFeedback(const std::string& feedback_id) override;
    std::vector<AlertFeedback> GetFeedbacksByAlert(const std::string& alert_id) override;
    std::vector<AlertFeedback> GetFeedbacksByUser(const std::string& user_id) override;
    std::vector<AlertFeedback> QueryFeedbacks(const FeedbackFilter& filter) override;
    
    bool ReviewFeedback(const std::string& feedback_id, const std::string& reviewer, const std::string& note) override;
    bool ResolveFeedback(const std::string& feedback_id, const std::string& resolution_note) override;
    bool DismissFeedback(const std::string& feedback_id, const std::string& reason) override;
    
    FeedbackStats GetStats() override;
    FeedbackStats GetStatsByAlertType(AlertType alert_type) override;
    FeedbackStats GetStatsByDateRange(const DateTime& start, const DateTime& end) override;
    
    std::vector<AlertFeedback> GetPendingFeedbacks() override;
    std::vector<AlertFeedback> GetHighPriorityFeedbacks() override;
    
private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
}

#endif
