#include "ogc/draw/draw_error.h"
#include <sstream>

namespace ogc {
namespace draw {

DrawError::DrawError()
    : m_result(DrawResult::kSuccess)
    , m_message("")
    , m_context("") {
}

DrawError::DrawError(DrawResult result)
    : m_result(result)
    , m_message(GetResultDescription(result))
    , m_context("") {
}

DrawError::DrawError(DrawResult result, const std::string& message)
    : m_result(result)
    , m_message(message)
    , m_context("") {
}

DrawError::DrawError(DrawResult result, const std::string& message, const std::string& context)
    : m_result(result)
    , m_message(message)
    , m_context(context) {
}

DrawResult DrawError::GetResult() const {
    return m_result;
}

const std::string& DrawError::GetMessage() const {
    return m_message;
}

const std::string& DrawError::GetContext() const {
    return m_context;
}

int DrawError::GetCode() const {
    return static_cast<int>(m_result);
}

void DrawError::SetResult(DrawResult result) {
    m_result = result;
}

void DrawError::SetMessage(const std::string& message) {
    m_message = message;
}

void DrawError::SetContext(const std::string& context) {
    m_context = context;
}

bool DrawError::IsSuccess() const {
    return m_result == DrawResult::kSuccess;
}

bool DrawError::IsError() const {
    return m_result != DrawResult::kSuccess;
}

std::string DrawError::ToString() const {
    std::ostringstream oss;
    oss << "[" << GetResultString(m_result) << "] " << m_message;
    if (!m_context.empty()) {
        oss << " (context: " << m_context << ")";
    }
    return oss.str();
}

void DrawError::Reset() {
    m_result = DrawResult::kSuccess;
    m_message = "";
    m_context = "";
}

DrawError DrawError::Success() {
    return DrawError(DrawResult::kSuccess, "Operation completed successfully");
}

DrawError DrawError::Failed(const std::string& message) {
    return DrawError(DrawResult::kFailed, message);
}

DrawError DrawError::InvalidParams(const std::string& message) {
    return DrawError(DrawResult::kInvalidParams, message);
}

DrawError DrawError::DeviceNotReady(const std::string& device_name) {
    return DrawError(DrawResult::kDeviceNotReady, 
                     "Device is not ready", device_name);
}

DrawError DrawError::EngineNotReady(const std::string& engine_name) {
    return DrawError(DrawResult::kEngineNotReady,
                     "Engine is not ready", engine_name);
}

DrawError DrawError::OutOfMemory(const std::string& context) {
    return DrawError(DrawResult::kOutOfMemory,
                     "Out of memory", context);
}

DrawError DrawError::FileNotFound(const std::string& file_path) {
    return DrawError(DrawResult::kFileNotFound,
                     "File not found", file_path);
}

DrawError DrawError::UnsupportedFormat(const std::string& format) {
    return DrawError(DrawResult::kUnsupportedFormat,
                     "Unsupported format", format);
}

DrawException::DrawException(DrawResult result)
    : std::runtime_error(GetResultDescription(result))
    , m_error(result) {
}

DrawException::DrawException(DrawResult result, const std::string& message)
    : std::runtime_error(message)
    , m_error(result, message) {
}

DrawException::DrawException(const DrawError& error)
    : std::runtime_error(error.ToString())
    , m_error(error) {
}

DrawResult DrawException::GetResult() const {
    return m_error.GetResult();
}

const DrawError& DrawException::GetError() const {
    return m_error;
}

}  
}  
