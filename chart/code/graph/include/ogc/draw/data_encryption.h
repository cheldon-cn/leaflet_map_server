#ifndef OGC_DRAW_DATA_ENCRYPTION_H
#define OGC_DRAW_DATA_ENCRYPTION_H

#include "ogc/draw/export.h"
#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <chrono>

namespace ogc {
namespace draw {

struct EncryptionKey {
    std::vector<uint8_t> key;
    std::vector<uint8_t> iv;
    std::string keyId;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point expiresAt;
    
    EncryptionKey() = default;
    
    bool IsValid() const {
        return !key.empty() && key.size() == 32;
    }
    
    bool IsExpired() const {
        return std::chrono::system_clock::now() > expiresAt;
    }
};

struct Coordinate {
    double longitude;
    double latitude;
    
    Coordinate() : longitude(0.0), latitude(0.0) {}
    Coordinate(double lon, double lat) : longitude(lon), latitude(lat) {}
};

enum class EncryptionAlgorithm {
    kAES256_CBC = 0,
    kAES256_GCM = 1,
    kAES256_CTR = 2
};

enum class CoordinateTransformType {
    kWGS84 = 0,
    kGCJ02 = 1,
    kBD09 = 2
};

class OGC_GRAPH_API DataEncryption {
public:
    static std::unique_ptr<DataEncryption> Create();
    
    virtual ~DataEncryption();
    
    virtual bool Initialize(const std::string& keyStoragePath) = 0;
    virtual void Shutdown() = 0;
    
    virtual EncryptionKey GenerateKey() = 0;
    virtual EncryptionKey GenerateKey(const std::string& keyId) = 0;
    
    virtual bool SetActiveKey(const EncryptionKey& key) = 0;
    virtual EncryptionKey GetActiveKey() const = 0;
    
    virtual bool StoreKey(const EncryptionKey& key, const std::string& password) = 0;
    virtual EncryptionKey LoadKey(const std::string& keyId, const std::string& password) = 0;
    virtual bool DeleteKey(const std::string& keyId) = 0;
    virtual bool HasKey(const std::string& keyId) const = 0;
    
    virtual std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data) = 0;
    virtual std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data,
                                         const EncryptionKey& key) = 0;
    virtual std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data,
                                         const EncryptionKey& key,
                                         EncryptionAlgorithm algorithm) = 0;
    
    virtual std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData) = 0;
    virtual std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData,
                                         const EncryptionKey& key) = 0;
    virtual std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData,
                                         const EncryptionKey& key,
                                         EncryptionAlgorithm algorithm) = 0;
    
    virtual bool EncryptFile(const std::string& inputPath,
                            const std::string& outputPath) = 0;
    virtual bool DecryptFile(const std::string& inputPath,
                            const std::string& outputPath) = 0;
    
    virtual std::string EncryptString(const std::string& text) = 0;
    virtual std::string DecryptString(const std::string& encryptedText) = 0;
    
    virtual void SetAlgorithm(EncryptionAlgorithm algorithm) = 0;
    virtual EncryptionAlgorithm GetAlgorithm() const = 0;
    
    virtual Coordinate WGS84ToGCJ02(const Coordinate& coord) = 0;
    virtual Coordinate GCJ02ToWGS84(const Coordinate& coord) = 0;
    virtual Coordinate WGS84ToBD09(const Coordinate& coord) = 0;
    virtual Coordinate BD09ToWGS84(const Coordinate& coord) = 0;
    virtual Coordinate GCJ02ToBD09(const Coordinate& coord) = 0;
    virtual Coordinate BD09ToGCJ02(const Coordinate& coord) = 0;
    
    virtual Coordinate TransformCoordinate(const Coordinate& coord,
                                          CoordinateTransformType fromType,
                                          CoordinateTransformType toType) = 0;
    
    virtual std::vector<Coordinate> TransformCoordinates(
        const std::vector<Coordinate>& coords,
        CoordinateTransformType fromType,
        CoordinateTransformType toType) = 0;
    
    virtual void SetCoordinateTransformEnabled(bool enabled) = 0;
    virtual bool IsCoordinateTransformEnabled() const = 0;
    
    virtual void SetDefaultTransformType(CoordinateTransformType type) = 0;
    virtual CoordinateTransformType GetDefaultTransformType() const = 0;
    
    virtual std::vector<uint8_t> Hash(const std::vector<uint8_t>& data) = 0;
    virtual std::string HashString(const std::string& text) = 0;
    
    virtual bool VerifyHash(const std::vector<uint8_t>& data,
                           const std::vector<uint8_t>& hash) = 0;
    virtual bool VerifyHashString(const std::string& text,
                                  const std::string& hash) = 0;
    
    virtual std::vector<uint8_t> DeriveKey(const std::string& password,
                                          const std::vector<uint8_t>& salt,
                                          int iterations = 10000) = 0;
    
    virtual std::vector<uint8_t> GenerateSalt() = 0;
    
    virtual bool IsInitialized() const = 0;
    
    virtual std::string GetKeyStoragePath() const = 0;
    
protected:
    DataEncryption() = default;
};

}  
}  

#endif
