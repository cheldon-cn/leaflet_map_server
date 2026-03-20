#pragma once

#include "common.h"
#include <string>
#include <memory>
#include <stdexcept>

namespace ogc {
namespace db {

class OGC_DB_API Result {
public:
    Result() : m_code(DbResult::kSuccess), m_message(), m_context() {}
    
    explicit Result(DbResult code) 
        : m_code(code), m_message(GetDbResultString(code)), m_context() {}
    
    Result(DbResult code, const std::string& message)
        : m_code(code), m_message(message), m_context() {}
    
    Result(DbResult code, const std::string& message, const std::string& context)
        : m_code(code), m_message(message), m_context(context) {}
    
    bool IsSuccess() const noexcept { 
        return m_code == DbResult::kSuccess; 
    }
    
    bool IsError() const noexcept { 
        return m_code != DbResult::kSuccess; 
    }
    
    DbResult GetCode() const noexcept { 
        return m_code; 
    }
    
    const std::string& GetMessage() const noexcept { 
        return m_message; 
    }
    
    const std::string& GetContext() const noexcept { 
        return m_context; 
    }
    
    void SetContext(const std::string& context) {
        m_context = context;
    }
    
    std::string ToString() const {
        if (m_context.empty()) {
            return m_message;
        }
        return m_message + " [" + m_context + "]";
    }
    
    static Result Success() {
        return Result(DbResult::kSuccess);
    }
    
    static Result Error(DbResult code) {
        return Result(code);
    }
    
    static Result Error(DbResult code, const std::string& message) {
        return Result(code, message);
    }
    
    static Result Error(DbResult code, const std::string& message, const std::string& context) {
        return Result(code, message, context);
    }
    
    bool operator==(const Result& other) const noexcept {
        return m_code == other.m_code;
    }
    
    bool operator!=(const Result& other) const noexcept {
        return m_code != other.m_code;
    }
    
    explicit operator bool() const noexcept {
        return IsSuccess();
    }

private:
    DbResult m_code;
    std::string m_message;
    std::string m_context;
};

class OGC_DB_API DbException : public std::runtime_error {
public:
    explicit DbException(DbResult code, const std::string& message = "")
        : std::runtime_error(message.empty() ? GetDbResultString(code) : message)
        , m_code(code)
        , m_context()
    {}
    
    DbException(DbResult code, const std::string& message, const std::string& context)
        : std::runtime_error(message)
        , m_code(code)
        , m_context(context)
    {}
    
    DbResult GetErrorCode() const noexcept { return m_code; }
    const std::string& GetContext() const noexcept { return m_context; }

private:
    DbResult m_code;
    std::string m_context;
};

#define DB_THROW(code, msg) \
    throw ogc::db::DbException(code, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + msg)

#define DB_THROW_IF(condition, code, msg) \
    if (condition) { DB_THROW(code, msg); }

}
}
