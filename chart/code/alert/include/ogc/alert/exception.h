#ifndef OGC_ALERT_EXCEPTION_H
#define OGC_ALERT_EXCEPTION_H

#include "export.h"
#include <string>
#include <stdexcept>
#include <sstream>

namespace ogc {
namespace alert {

class OGC_ALERT_API AlertException : public std::runtime_error {
public:
    explicit AlertException(const std::string& message)
        : std::runtime_error(message)
        , m_code(0)
        , m_context("") {
    }
    
    AlertException(int code, const std::string& message)
        : std::runtime_error(message)
        , m_code(code)
        , m_context("") {
    }
    
    AlertException(int code, const std::string& message, const std::string& context)
        : std::runtime_error(message)
        , m_code(code)
        , m_context(context) {
    }
    
    virtual ~AlertException() = default;
    
    int GetCode() const { return m_code; }
    const std::string& GetContext() const { return m_context; }
    
    virtual std::string GetFullMessage() const {
        std::ostringstream oss;
        oss << "[Code:" << m_code << "] " << what();
        if (!m_context.empty()) {
            oss << " (Context: " << m_context << ")";
        }
        return oss.str();
    }

protected:
    int m_code;
    std::string m_context;
};

class OGC_ALERT_API ConfigurationException : public AlertException {
public:
    explicit ConfigurationException(const std::string& message)
        : AlertException(1001, message) {}
    
    ConfigurationException(const std::string& message, const std::string& context)
        : AlertException(1001, message, context) {}
};

class OGC_ALERT_API DataSourceException : public AlertException {
public:
    explicit DataSourceException(const std::string& message)
        : AlertException(2001, message) {}
    
    DataSourceException(const std::string& message, const std::string& context)
        : AlertException(2001, message, context) {}
};

class OGC_ALERT_API ConnectionException : public DataSourceException {
public:
    explicit ConnectionException(const std::string& message)
        : DataSourceException(message) {
        m_code = 2002;
    }
    
    ConnectionException(const std::string& message, const std::string& context)
        : DataSourceException(message, context) {
        m_code = 2002;
    }
};

class OGC_ALERT_API QueryException : public DataSourceException {
public:
    explicit QueryException(const std::string& message)
        : DataSourceException(message) {
        m_code = 2003;
    }
    
    QueryException(const std::string& message, const std::string& context)
        : DataSourceException(message, context) {
        m_code = 2003;
    }
};

class OGC_ALERT_API AlertCheckerException : public AlertException {
public:
    explicit AlertCheckerException(const std::string& message)
        : AlertException(3001, message) {}
    
    AlertCheckerException(const std::string& message, const std::string& context)
        : AlertException(3001, message, context) {}
};

class OGC_ALERT_API AlgorithmException : public AlertCheckerException {
public:
    explicit AlgorithmException(const std::string& message)
        : AlertCheckerException(message) {
        m_code = 3002;
    }
    
    AlgorithmException(const std::string& message, const std::string& context)
        : AlertCheckerException(message, context) {
        m_code = 3002;
    }
};

class OGC_ALERT_API ValidationException : public AlertCheckerException {
public:
    explicit ValidationException(const std::string& message)
        : AlertCheckerException(message) {
        m_code = 3003;
    }
    
    ValidationException(const std::string& message, const std::string& context)
        : AlertCheckerException(message, context) {
        m_code = 3003;
    }
};

class OGC_ALERT_API ProcessorException : public AlertException {
public:
    explicit ProcessorException(const std::string& message)
        : AlertException(4001, message) {}
    
    ProcessorException(const std::string& message, const std::string& context)
        : AlertException(4001, message, context) {}
};

class OGC_ALERT_API DeduplicationException : public ProcessorException {
public:
    explicit DeduplicationException(const std::string& message)
        : ProcessorException(message) {
        m_code = 4002;
    }
    
    DeduplicationException(const std::string& message, const std::string& context)
        : ProcessorException(message, context) {
        m_code = 4002;
    }
};

class OGC_ALERT_API PushException : public AlertException {
public:
    explicit PushException(const std::string& message)
        : AlertException(5001, message) {}
    
    PushException(const std::string& message, const std::string& context)
        : AlertException(5001, message, context) {}
};

class OGC_ALERT_API ChannelException : public PushException {
public:
    explicit ChannelException(const std::string& message)
        : PushException(message) {
        m_code = 5002;
    }
    
    ChannelException(const std::string& message, const std::string& context)
        : PushException(message, context) {
        m_code = 5002;
    }
};

class OGC_ALERT_API TimeoutException : public AlertException {
public:
    explicit TimeoutException(const std::string& message)
        : AlertException(6001, message) {}
    
    TimeoutException(const std::string& message, const std::string& context)
        : AlertException(6001, message, context) {}
};

class OGC_ALERT_API CacheException : public AlertException {
public:
    explicit CacheException(const std::string& message)
        : AlertException(7001, message) {}
    
    CacheException(const std::string& message, const std::string& context)
        : AlertException(7001, message, context) {}
};

class OGC_ALERT_API SchedulerException : public AlertException {
public:
    explicit SchedulerException(const std::string& message)
        : AlertException(8001, message) {}
    
    SchedulerException(const std::string& message, const std::string& context)
        : AlertException(8001, message, context) {}
};

class OGC_ALERT_API EngineException : public AlertException {
public:
    explicit EngineException(const std::string& message)
        : AlertException(9001, message) {}
    
    EngineException(const std::string& message, const std::string& context)
        : AlertException(9001, message, context) {}
};

class OGC_ALERT_API EngineNotInitializedException : public EngineException {
public:
    explicit EngineNotInitializedException()
        : EngineException("AlertEngine not initialized") {
        m_code = 9002;
    }
};

class OGC_ALERT_API EngineAlreadyRunningException : public EngineException {
public:
    explicit EngineAlreadyRunningException()
        : EngineException("AlertEngine already running") {
        m_code = 9003;
    }
};

inline const char* GetExceptionCategory(int code) {
    if (code >= 1000 && code < 2000) return "Configuration";
    if (code >= 2000 && code < 3000) return "DataSource";
    if (code >= 3000 && code < 4000) return "AlertChecker";
    if (code >= 4000 && code < 5000) return "Processor";
    if (code >= 5000 && code < 6000) return "Push";
    if (code >= 6000 && code < 7000) return "Timeout";
    if (code >= 7000 && code < 8000) return "Cache";
    if (code >= 8000 && code < 9000) return "Scheduler";
    if (code >= 9000 && code < 10000) return "Engine";
    return "Unknown";
}

}
}

#endif
