#ifndef ERROR_CODES_H
#define ERROR_CODES_H

#include <string>

namespace chart {
namespace parser {

enum class ErrorCode {
    Success = 0,
    
    // File errors (1000-1099)
    ErrFileNotFound = 1000,
    ErrFileOpenFailed = 1001,
    ErrFileReadFailed = 1002,
    ErrFileFormatInvalid = 1003,
    ErrFileCorrupted = 1004,
    
    // GDAL errors (1100-1199)
    ErrGDALInitFailed = 1100,
    ErrGDALDriverNotFound = 1101,
    ErrGDALDatasetOpenFailed = 1102,
    ErrGDALLayerNotFound = 1103,
    ErrGDALFeatureReadFailed = 1104,
    
    // Parse errors (1200-1299)
    ErrParseGeometryFailed = 1200,
    ErrParseAttributeFailed = 1201,
    ErrParseFeatureFailed = 1202,
    ErrParseCoordinateFailed = 1203,
    ErrParseMetadataFailed = 1204,
    
    // Validation errors (1300-1399)
    ErrValidationFailed = 1300,
    ErrInvalidGeometry = 1301,
    ErrInvalidAttribute = 1302,
    ErrInvalidCoordinate = 1303,
    ErrInvalidFeatureType = 1304,
    
    // Memory errors (1400-1499)
    ErrMemoryAllocation = 1400,
    ErrMemoryOverflow = 1401,
    
    // Configuration errors (1500-1599)
    ErrInvalidConfig = 1500,
    ErrUnsupportedFormat = 1501,
    
    // Unknown error
    ErrUnknown = 9999
};

inline std::string ErrorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::Success: return "Success";
        
        // File errors
        case ErrorCode::ErrFileNotFound: return "File not found";
        case ErrorCode::ErrFileOpenFailed: return "Failed to open file";
        case ErrorCode::ErrFileReadFailed: return "Failed to read file";
        case ErrorCode::ErrFileFormatInvalid: return "Invalid file format";
        case ErrorCode::ErrFileCorrupted: return "File is corrupted";
        
        // GDAL errors
        case ErrorCode::ErrGDALInitFailed: return "GDAL initialization failed";
        case ErrorCode::ErrGDALDriverNotFound: return "GDAL driver not found";
        case ErrorCode::ErrGDALDatasetOpenFailed: return "Failed to open GDAL dataset";
        case ErrorCode::ErrGDALLayerNotFound: return "GDAL layer not found";
        case ErrorCode::ErrGDALFeatureReadFailed: return "Failed to read GDAL feature";
        
        // Parse errors
        case ErrorCode::ErrParseGeometryFailed: return "Failed to parse geometry";
        case ErrorCode::ErrParseAttributeFailed: return "Failed to parse attribute";
        case ErrorCode::ErrParseFeatureFailed: return "Failed to parse feature";
        case ErrorCode::ErrParseCoordinateFailed: return "Failed to parse coordinate";
        case ErrorCode::ErrParseMetadataFailed: return "Failed to parse metadata";
        
        // Validation errors
        case ErrorCode::ErrValidationFailed: return "Validation failed";
        case ErrorCode::ErrInvalidGeometry: return "Invalid geometry";
        case ErrorCode::ErrInvalidAttribute: return "Invalid attribute";
        case ErrorCode::ErrInvalidCoordinate: return "Invalid coordinate";
        case ErrorCode::ErrInvalidFeatureType: return "Invalid feature type";
        
        // Memory errors
        case ErrorCode::ErrMemoryAllocation: return "Memory allocation failed";
        case ErrorCode::ErrMemoryOverflow: return "Memory overflow";
        
        // Configuration errors
        case ErrorCode::ErrInvalidConfig: return "Invalid configuration";
        case ErrorCode::ErrUnsupportedFormat: return "Unsupported format";
        
        default: return "Unknown error";
    }
}

} // namespace parser
} // namespace chart

#endif // ERROR_CODES_H
