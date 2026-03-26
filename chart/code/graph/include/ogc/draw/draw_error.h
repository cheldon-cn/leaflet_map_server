#ifndef OGC_DRAW_DRAW_ERROR_H
#define OGC_DRAW_DRAW_ERROR_H

#include "ogc/draw/export.h"
#include "ogc/draw/draw_result.h"
#include <stdexcept>
#include <string>

namespace ogc {
namespace draw {

class OGC_GRAPH_API DrawError {
public:
    DrawError();
    explicit DrawError(DrawResult result);
    DrawError(DrawResult result, const std::string& message);
    DrawError(DrawResult result, const std::string& message, const std::string& context);
    
    DrawResult GetResult() const;
    const std::string& GetMessage() const;
    const std::string& GetContext() const;
    int GetCode() const;
    
    void SetResult(DrawResult result);
    void SetMessage(const std::string& message);
    void SetContext(const std::string& context);
    
    bool IsSuccess() const;
    bool IsError() const;
    
    std::string ToString() const;
    
    void Reset();
    
    static DrawError Success();
    static DrawError Failed(const std::string& message);
    static DrawError InvalidParams(const std::string& message);
    static DrawError DeviceNotReady(const std::string& device_name);
    static DrawError EngineNotReady(const std::string& engine_name);
    static DrawError OutOfMemory(const std::string& context);
    static DrawError FileNotFound(const std::string& file_path);
    static DrawError UnsupportedFormat(const std::string& format);

private:
    DrawResult m_result;
    std::string m_message;
    std::string m_context;
};

class OGC_GRAPH_API DrawException : public std::runtime_error {
public:
    explicit DrawException(DrawResult result);
    DrawException(DrawResult result, const std::string& message);
    DrawException(const DrawError& error);
    
    DrawResult GetResult() const;
    const DrawError& GetError() const;
    
private:
    DrawError m_error;
};

}  
}  

#endif  
