#include "ogc/alert/feedback_service.h"
#include "ogc/draw/log.h"
#include <map>
#include <mutex>
#include <algorithm>
#include <random>

using ogc::draw::LogLevel;
using ogc::draw::LogHelper;

namespace ogc {
namespace alert {

class FeedbackService::Impl {
public:
    Impl() : m_nextId(1) {}
    
    bool SubmitFeedback(const AlertFeedback& feedback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        AlertFeedback storedFeedback = feedback;
        storedFeedback.feedback_id = GenerateId();
        storedFeedback.created_at = DateTime::Now();
        storedFeedback.updated_at = storedFeedback.created_at;
        storedFeedback.status = FeedbackStatus::PENDING;
        
        m_feedbacks[storedFeedback.feedback_id] = storedFeedback;
        m_alertFeedbacks[storedFeedback.alert_id].push_back(storedFeedback.feedback_id);
        m_userFeedbacks[storedFeedback.user_id].push_back(storedFeedback.feedback_id);
        
        LOG_INFO() << "Feedback submitted: " << storedFeedback.feedback_id 
                   << " for alert " << storedFeedback.alert_id
                   << " by user " << storedFeedback.user_id;
        return true;
    }
    
    bool UpdateFeedback(const std::string& feedback_id, const AlertFeedback& feedback) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_feedbacks.find(feedback_id);
        if (it == m_feedbacks.end()) {
            LOG_WARNING() << "Feedback not found: " << feedback_id;
            return false;
        }
        
        AlertFeedback updatedFeedback = feedback;
        updatedFeedback.feedback_id = feedback_id;
        updatedFeedback.updated_at = DateTime::Now();
        m_feedbacks[feedback_id] = updatedFeedback;
        
        LOG_INFO() << "Feedback updated: " << feedback_id;
        return true;
    }
    
    bool DeleteFeedback(const std::string& feedback_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_feedbacks.find(feedback_id);
        if (it == m_feedbacks.end()) {
            return false;
        }
        
        std::string alertId = it->second.alert_id;
        std::string userId = it->second.user_id;
        
        m_feedbacks.erase(it);
        
        auto alertIt = m_alertFeedbacks.find(alertId);
        if (alertIt != m_alertFeedbacks.end()) {
            alertIt->second.erase(
                std::remove(alertIt->second.begin(), alertIt->second.end(), feedback_id),
                alertIt->second.end());
        }
        
        auto userIt = m_userFeedbacks.find(userId);
        if (userIt != m_userFeedbacks.end()) {
            userIt->second.erase(
                std::remove(userIt->second.begin(), userIt->second.end(), feedback_id),
                userIt->second.end());
        }
        
        LOG_INFO() << "Feedback deleted: " << feedback_id;
        return true;
    }
    
    AlertFeedback GetFeedback(const std::string& feedback_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_feedbacks.find(feedback_id);
        if (it != m_feedbacks.end()) {
            return it->second;
        }
        return AlertFeedback();
    }
    
    std::vector<AlertFeedback> GetFeedbacksByAlert(const std::string& alert_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertFeedback> result;
        
        auto it = m_alertFeedbacks.find(alert_id);
        if (it != m_alertFeedbacks.end()) {
            for (const auto& feedbackId : it->second) {
                auto feedbackIt = m_feedbacks.find(feedbackId);
                if (feedbackIt != m_feedbacks.end()) {
                    result.push_back(feedbackIt->second);
                }
            }
        }
        return result;
    }
    
    std::vector<AlertFeedback> GetFeedbacksByUser(const std::string& user_id) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertFeedback> result;
        
        auto it = m_userFeedbacks.find(user_id);
        if (it != m_userFeedbacks.end()) {
            for (const auto& feedbackId : it->second) {
                auto feedbackIt = m_feedbacks.find(feedbackId);
                if (feedbackIt != m_feedbacks.end()) {
                    result.push_back(feedbackIt->second);
                }
            }
        }
        return result;
    }
    
    std::vector<AlertFeedback> QueryFeedbacks(const FeedbackFilter& filter) {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertFeedback> result;
        
        for (const auto& pair : m_feedbacks) {
            const AlertFeedback& feedback = pair.second;
            
            if (!filter.user_id.empty() && feedback.user_id != filter.user_id) continue;
            if (!filter.alert_id.empty() && feedback.alert_id != filter.alert_id) continue;
            if (feedback.type != filter.type) continue;
            if (feedback.status != filter.status) continue;
            if (feedback.rating < filter.min_rating || feedback.rating > filter.max_rating) continue;
            
            result.push_back(feedback);
        }
        
        return result;
    }
    
    bool ReviewFeedback(const std::string& feedback_id, const std::string& reviewer, const std::string& note) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_feedbacks.find(feedback_id);
        if (it == m_feedbacks.end()) {
            return false;
        }
        
        it->second.status = FeedbackStatus::REVIEWED;
        it->second.reviewer = reviewer;
        it->second.resolution_note = note;
        it->second.updated_at = DateTime::Now();
        
        LOG_INFO() << "Feedback reviewed: " << feedback_id << " by " << reviewer;
        return true;
    }
    
    bool ResolveFeedback(const std::string& feedback_id, const std::string& resolution_note) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_feedbacks.find(feedback_id);
        if (it == m_feedbacks.end()) {
            return false;
        }
        
        it->second.status = FeedbackStatus::RESOLVED;
        it->second.resolution_note = resolution_note;
        it->second.updated_at = DateTime::Now();
        
        LOG_INFO() << "Feedback resolved: " << feedback_id;
        return true;
    }
    
    bool DismissFeedback(const std::string& feedback_id, const std::string& reason) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        auto it = m_feedbacks.find(feedback_id);
        if (it == m_feedbacks.end()) {
            return false;
        }
        
        it->second.status = FeedbackStatus::DISMISSED;
        it->second.resolution_note = reason;
        it->second.updated_at = DateTime::Now();
        
        LOG_INFO() << "Feedback dismissed: " << feedback_id;
        return true;
    }
    
    FeedbackStats GetStats() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        FeedbackStats stats;
        stats.total_feedbacks = static_cast<int>(m_feedbacks.size());
        stats.positive_count = 0;
        stats.negative_count = 0;
        stats.neutral_count = 0;
        stats.suggestion_count = 0;
        stats.bug_report_count = 0;
        stats.pending_count = 0;
        stats.resolved_count = 0;
        int totalRating = 0;
        int ratingCount = 0;
        
        for (const auto& pair : m_feedbacks) {
            const AlertFeedback& feedback = pair.second;
            switch (feedback.type) {
                case FeedbackType::POSITIVE: stats.positive_count++; break;
                case FeedbackType::NEGATIVE: stats.negative_count++; break;
                case FeedbackType::NEUTRAL: stats.neutral_count++; break;
                case FeedbackType::SUGGESTION: stats.suggestion_count++; break;
                case FeedbackType::BUG_REPORT: stats.bug_report_count++; break;
            }
            
            switch (feedback.status) {
                case FeedbackStatus::PENDING: stats.pending_count++; break;
                case FeedbackStatus::RESOLVED: stats.resolved_count++; break;
                default: break;
            }
            
            if (feedback.rating > 0) {
                totalRating += feedback.rating;
                ratingCount++;
            }
        }
        
        stats.avg_rating = ratingCount > 0 ? static_cast<double>(totalRating) / ratingCount : 0.0;
        return stats;
    }
    
    FeedbackStats GetStatsByAlertType(AlertType alert_type) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        FeedbackStats stats;
        stats.total_feedbacks = 0;
        stats.positive_count = 0;
        stats.negative_count = 0;
        stats.neutral_count = 0;
        stats.suggestion_count = 0;
        stats.bug_report_count = 0;
        stats.pending_count = 0;
        stats.resolved_count = 0;
        int totalRating = 0;
        int ratingCount = 0;
        
        for (const auto& pair : m_feedbacks) {
            const AlertFeedback& feedback = pair.second;
            
            if (feedback.rating > 0) {
                totalRating += feedback.rating;
                ratingCount++;
            }
        }
        
        stats.avg_rating = ratingCount > 0 ? static_cast<double>(totalRating) / ratingCount : 0.0;
        return stats;
    }
    
    FeedbackStats GetStatsByDateRange(const DateTime& start, const DateTime& end) {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        FeedbackStats stats;
        stats.total_feedbacks = 0;
        stats.positive_count = 0;
        stats.negative_count = 0;
        stats.neutral_count = 0;
        stats.suggestion_count = 0;
        stats.bug_report_count = 0;
        stats.pending_count = 0;
        stats.resolved_count = 0;
        int totalRating = 0;
        int ratingCount = 0;
        
        for (const auto& pair : m_feedbacks) {
            const AlertFeedback& feedback = pair.second;
            
            if (feedback.created_at >= start && feedback.created_at <= end) {
                stats.total_feedbacks++;
                
                switch (feedback.type) {
                    case FeedbackType::POSITIVE: stats.positive_count++; break;
                    case FeedbackType::NEGATIVE: stats.negative_count++; break;
                    case FeedbackType::NEUTRAL: stats.neutral_count++; break;
                    case FeedbackType::SUGGESTION: stats.suggestion_count++; break;
                    case FeedbackType::BUG_REPORT: stats.bug_report_count++; break;
                }
                
                switch (feedback.status) {
                    case FeedbackStatus::PENDING: stats.pending_count++; break;
                    case FeedbackStatus::RESOLVED: stats.resolved_count++; break;
                    default: break;
                }
                
                if (feedback.rating > 0) {
                    totalRating += feedback.rating;
                    ratingCount++;
                }
            }
        }
        
        stats.avg_rating = ratingCount > 0 ? static_cast<double>(totalRating) / ratingCount : 0.0;
        return stats;
    }
    
    std::vector<AlertFeedback> GetPendingFeedbacks() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertFeedback> result;
        
        for (const auto& pair : m_feedbacks) {
            if (pair.second.status == FeedbackStatus::PENDING) {
                result.push_back(pair.second);
            }
        }
        
        return result;
    }
    
    std::vector<AlertFeedback> GetHighPriorityFeedbacks() {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::vector<AlertFeedback> result;
        
        for (const auto& pair : m_feedbacks) {
            if (pair.second.type == FeedbackType::BUG_REPORT || 
                pair.second.type == FeedbackType::NEGATIVE) {
                result.push_back(pair.second);
            }
        }
        
        return result;
    }
    
private:
    std::string GenerateId() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 15);
        static const char* hex = "0123456789abcdef";
        
        std::string id = "FB_";
        id.reserve(20);
        for (int i = 0; i < 16; ++i) {
            id += hex[dis(gen)];
        }
        return id;
    }
    
    std::map<std::string, AlertFeedback> m_feedbacks;
    std::map<std::string, std::vector<std::string>> m_alertFeedbacks;
    std::map<std::string, std::vector<std::string>> m_userFeedbacks;
    int m_nextId;
    mutable std::mutex m_mutex;
};

FeedbackService::FeedbackService()
    : m_impl(std::make_unique<Impl>()) {
}

FeedbackService::~FeedbackService() {
}

bool FeedbackService::SubmitFeedback(const AlertFeedback& feedback) {
    return m_impl->SubmitFeedback(feedback);
}

bool FeedbackService::UpdateFeedback(const std::string& feedback_id, const AlertFeedback& feedback) {
    return m_impl->UpdateFeedback(feedback_id, feedback);
}

bool FeedbackService::DeleteFeedback(const std::string& feedback_id) {
    return m_impl->DeleteFeedback(feedback_id);
}

AlertFeedback FeedbackService::GetFeedback(const std::string& feedback_id) {
    return m_impl->GetFeedback(feedback_id);
}

std::vector<AlertFeedback> FeedbackService::GetFeedbacksByAlert(const std::string& alert_id) {
    return m_impl->GetFeedbacksByAlert(alert_id);
}

std::vector<AlertFeedback> FeedbackService::GetFeedbacksByUser(const std::string& user_id) {
    return m_impl->GetFeedbacksByUser(user_id);
}

std::vector<AlertFeedback> FeedbackService::QueryFeedbacks(const FeedbackFilter& filter) {
    return m_impl->QueryFeedbacks(filter);
}

bool FeedbackService::ReviewFeedback(const std::string& feedback_id, const std::string& reviewer, const std::string& note) {
    return m_impl->ReviewFeedback(feedback_id, reviewer, note);
}

bool FeedbackService::ResolveFeedback(const std::string& feedback_id, const std::string& resolution_note) {
    return m_impl->ResolveFeedback(feedback_id, resolution_note);
}

bool FeedbackService::DismissFeedback(const std::string& feedback_id, const std::string& reason) {
    return m_impl->DismissFeedback(feedback_id, reason);
}

FeedbackStats FeedbackService::GetStats() {
    return m_impl->GetStats();
}

FeedbackStats FeedbackService::GetStatsByAlertType(AlertType alert_type) {
    return m_impl->GetStatsByAlertType(alert_type);
}

FeedbackStats FeedbackService::GetStatsByDateRange(const DateTime& start, const DateTime& end) {
    return m_impl->GetStatsByDateRange(start, end);
}

std::vector<AlertFeedback> FeedbackService::GetPendingFeedbacks() {
    return m_impl->GetPendingFeedbacks();
}

std::vector<AlertFeedback> FeedbackService::GetHighPriorityFeedbacks() {
    return m_impl->GetHighPriorityFeedbacks();
}

std::unique_ptr<IFeedbackService> IFeedbackService::Create() {
    return std::unique_ptr<IFeedbackService>(new FeedbackService());
}

}
}
