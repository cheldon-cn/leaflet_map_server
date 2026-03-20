#pragma once

/**
 * @file exception.h
 * @brief 几何库异常类定义
 */

#include "common.h"
#include <stdexcept>
#include <string>
#include <exception>

namespace ogc {

/**
 * @brief 几何操作异常类
 * 
 * 用于报告几何操作中的错误，包含错误码和上下文信息
 */
class OGC_API GeometryException : public std::runtime_error {
public:
    explicit GeometryException(GeomResult code, const std::string& message = "")
        : std::runtime_error(message.empty() ? GetResultString(code) : message)
        , m_code(code)
        , m_context("")
    {}
    
    GeometryException(GeomResult code, const std::string& message, const std::string& context)
        : std::runtime_error(message)
        , m_code(code)
        , m_context(context)
    {}
    
    GeomResult GetErrorCode() const noexcept { return m_code; }
    const std::string& GetContext() const noexcept { return m_context; }
    
    std::exception_ptr GetInnerException() const noexcept { return m_innerException; }
    void SetInnerException(std::exception_ptr ex) { m_innerException = ex; }

private:
    GeomResult m_code;
    std::string m_context;
    std::exception_ptr m_innerException;
};

}

#define GLM_THROW(code, msg) \
    throw ogc::GeometryException(code, std::string(__FILE__) + ":" + std::to_string(__LINE__) + " " + msg)

#define GLM_THROW_IF(condition, code, msg) \
    if (condition) { GLM_THROW(code, msg); }
