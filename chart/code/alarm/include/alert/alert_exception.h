#ifndef ALERT_SYSTEM_ALERT_EXCEPTION_H
#define ALERT_SYSTEM_ALERT_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace alert {

class AlertException : public std::runtime_error {
public:
    explicit AlertException(const std::string& message)
        : std::runtime_error(message), m_errorCode(0) {}
    
    AlertException(int errorCode, const std::string& message)
        : std::runtime_error(message), m_errorCode(errorCode) {}
    
    virtual ~AlertException() = default;
    
    int GetErrorCode() const { return m_errorCode; }
    
private:
    int m_errorCode;
};

class InvalidParameterException : public AlertException {
public:
    explicit InvalidParameterException(const std::string& message)
        : AlertException(1001, message) {}
};

class DataNotFoundException : public AlertException {
public:
    explicit DataNotFoundException(const std::string& message)
        : AlertException(1002, message) {}
};

class EngineException : public AlertException {
public:
    explicit EngineException(const std::string& message)
        : AlertException(2001, message) {}
};

class ConfigurationException : public AlertException {
public:
    explicit ConfigurationException(const std::string& message)
        : AlertException(3001, message) {}
};

class ConnectionException : public AlertException {
public:
    explicit ConnectionException(const std::string& message)
        : AlertException(4001, message) {}
};

class TimeoutException : public AlertException {
public:
    explicit TimeoutException(const std::string& message)
        : AlertException(4002, message) {}
};

class PushException : public AlertException {
public:
    explicit PushException(const std::string& message)
        : AlertException(5001, message) {}
};

class FileNotFoundException : public AlertException {
public:
    explicit FileNotFoundException(const std::string& message)
        : AlertException(6001, message) {}
};

class FileWriteException : public AlertException {
public:
    explicit FileWriteException(const std::string& message)
        : AlertException(6002, message) {}
};

}

#endif
