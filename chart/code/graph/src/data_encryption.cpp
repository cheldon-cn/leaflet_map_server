#include "ogc/draw/data_encryption.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <random>
#include <cstring>
#include <map>

namespace ogc {
namespace draw {

static const double PI = 3.14159265358979323846;
static const double A = 6378245.0;
static const double EE = 0.00669342162296594323;
static const double X_PI = PI * 3000.0 / 180.0;

class DataEncryptionImpl : public DataEncryption {
public:
    DataEncryptionImpl()
        : m_initialized(false)
        , m_algorithm(EncryptionAlgorithm::kAES256_CBC)
        , m_coordTransformEnabled(false)
        , m_defaultTransformType(CoordinateTransformType::kWGS84)
    {
    }
    
    ~DataEncryptionImpl() override {
        Shutdown();
    }
    
    bool Initialize(const std::string& keyStoragePath) override {
        if (m_initialized) {
            return true;
        }
        
        m_keyStoragePath = keyStoragePath;
        m_activeKey = GenerateKey();
        
        m_initialized = true;
        return true;
    }
    
    void Shutdown() override {
        m_initialized = false;
        m_activeKey = EncryptionKey();
        m_storedKeys.clear();
    }
    
    EncryptionKey GenerateKey() override {
        std::string keyId = GenerateKeyId();
        return GenerateKey(keyId);
    }
    
    EncryptionKey GenerateKey(const std::string& keyId) override {
        EncryptionKey key;
        key.keyId = keyId;
        key.key.resize(32);
        key.iv.resize(16);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, 255);
        
        for (size_t i = 0; i < key.key.size(); ++i) {
            key.key[i] = static_cast<uint8_t>(dis(gen));
        }
        
        for (size_t i = 0; i < key.iv.size(); ++i) {
            key.iv[i] = static_cast<uint8_t>(dis(gen));
        }
        
        key.createdAt = std::chrono::system_clock::now();
        key.expiresAt = key.createdAt + std::chrono::hours(24 * 365);
        
        return key;
    }
    
    bool SetActiveKey(const EncryptionKey& key) override {
        if (!key.IsValid()) {
            return false;
        }
        
        m_activeKey = key;
        return true;
    }
    
    EncryptionKey GetActiveKey() const override {
        return m_activeKey;
    }
    
    bool StoreKey(const EncryptionKey& key, const std::string& password) override {
        if (!key.IsValid()) {
            return false;
        }
        
        m_storedKeys[key.keyId] = key;
        return true;
    }
    
    EncryptionKey LoadKey(const std::string& keyId, const std::string& password) override {
        auto it = m_storedKeys.find(keyId);
        if (it != m_storedKeys.end()) {
            return it->second;
        }
        return EncryptionKey();
    }
    
    bool DeleteKey(const std::string& keyId) override {
        auto it = m_storedKeys.find(keyId);
        if (it != m_storedKeys.end()) {
            m_storedKeys.erase(it);
            return true;
        }
        return false;
    }
    
    bool HasKey(const std::string& keyId) const override {
        return m_storedKeys.find(keyId) != m_storedKeys.end();
    }
    
    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data) override {
        return Encrypt(data, m_activeKey, m_algorithm);
    }
    
    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data,
                                  const EncryptionKey& key) override {
        return Encrypt(data, key, m_algorithm);
    }
    
    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& data,
                                  const EncryptionKey& key,
                                  EncryptionAlgorithm algorithm) override {
        if (!m_initialized || !key.IsValid() || data.empty()) {
            return std::vector<uint8_t>();
        }
        
        std::vector<uint8_t> result;
        
        size_t blockSize = 16;
        size_t paddingSize = blockSize - (data.size() % blockSize);
        
        result.reserve(data.size() + paddingSize + 16);
        
        for (size_t i = 0; i < 16; ++i) {
            result.push_back(key.iv[i % key.iv.size()]);
        }
        
        for (size_t i = 0; i < data.size(); ++i) {
            uint8_t dataByte = data[i];
            uint8_t keyByte = key.key[i % key.key.size()];
            uint8_t ivByte = key.iv[i % key.iv.size()];
            
            uint8_t encrypted = dataByte ^ keyByte ^ ivByte;
            result.push_back(encrypted);
        }
        
        for (size_t i = 0; i < paddingSize; ++i) {
            uint8_t paddingByte = static_cast<uint8_t>(paddingSize);
            uint8_t keyByte = key.key[(data.size() + i) % key.key.size()];
            uint8_t ivByte = key.iv[(data.size() + i) % key.iv.size()];
            
            uint8_t encrypted = paddingByte ^ keyByte ^ ivByte;
            result.push_back(encrypted);
        }
        
        return result;
    }
    
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData) override {
        return Decrypt(encryptedData, m_activeKey, m_algorithm);
    }
    
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData,
                                  const EncryptionKey& key) override {
        return Decrypt(encryptedData, key, m_algorithm);
    }
    
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& encryptedData,
                                  const EncryptionKey& key,
                                  EncryptionAlgorithm algorithm) override {
        if (!m_initialized || !key.IsValid() || encryptedData.size() < 32) {
            return std::vector<uint8_t>();
        }
        
        std::vector<uint8_t> result;
        
        size_t dataSize = encryptedData.size() - 16;
        result.reserve(dataSize);
        
        for (size_t i = 16; i < encryptedData.size(); ++i) {
            uint8_t encryptedByte = encryptedData[i];
            uint8_t keyByte = key.key[(i - 16) % key.key.size()];
            uint8_t ivByte = key.iv[(i - 16) % key.iv.size()];
            
            uint8_t decrypted = encryptedByte ^ keyByte ^ ivByte;
            result.push_back(decrypted);
        }
        
        if (!result.empty()) {
            uint8_t padding = result.back();
            if (padding > 0 && padding <= 16 && result.size() >= static_cast<size_t>(padding)) {
                bool validPadding = true;
                for (size_t i = result.size() - padding; i < result.size(); ++i) {
                    if (result[i] != padding) {
                        validPadding = false;
                        break;
                    }
                }
                if (validPadding) {
                    result.resize(result.size() - padding);
                }
            }
        }
        
        return result;
    }
    
    bool EncryptFile(const std::string& inputPath,
                     const std::string& outputPath) override {
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile) {
            return false;
        }
        
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(inputFile)),
                                  std::istreambuf_iterator<char>());
        inputFile.close();
        
        std::vector<uint8_t> encrypted = Encrypt(data);
        if (encrypted.empty() && !data.empty()) {
            return false;
        }
        
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile) {
            return false;
        }
        
        outputFile.write(reinterpret_cast<const char*>(encrypted.data()), encrypted.size());
        return true;
    }
    
    bool DecryptFile(const std::string& inputPath,
                     const std::string& outputPath) override {
        std::ifstream inputFile(inputPath, std::ios::binary);
        if (!inputFile) {
            return false;
        }
        
        std::vector<uint8_t> data((std::istreambuf_iterator<char>(inputFile)),
                                  std::istreambuf_iterator<char>());
        inputFile.close();
        
        std::vector<uint8_t> decrypted = Decrypt(data);
        if (decrypted.empty() && !data.empty()) {
            return false;
        }
        
        std::ofstream outputFile(outputPath, std::ios::binary);
        if (!outputFile) {
            return false;
        }
        
        outputFile.write(reinterpret_cast<const char*>(decrypted.data()), decrypted.size());
        return true;
    }
    
    std::string EncryptString(const std::string& text) override {
        std::vector<uint8_t> data(text.begin(), text.end());
        std::vector<uint8_t> encrypted = Encrypt(data);
        return Base64Encode(encrypted);
    }
    
    std::string DecryptString(const std::string& encryptedText) override {
        std::vector<uint8_t> data = Base64Decode(encryptedText);
        std::vector<uint8_t> decrypted = Decrypt(data);
        return std::string(decrypted.begin(), decrypted.end());
    }
    
    void SetAlgorithm(EncryptionAlgorithm algorithm) override {
        m_algorithm = algorithm;
    }
    
    EncryptionAlgorithm GetAlgorithm() const override {
        return m_algorithm;
    }
    
    Coordinate WGS84ToGCJ02(const Coordinate& coord) override {
        return TransformWGS84ToGCJ02(coord);
    }
    
    Coordinate GCJ02ToWGS84(const Coordinate& coord) override {
        return TransformGCJ02ToWGS84(coord);
    }
    
    Coordinate WGS84ToBD09(const Coordinate& coord) override {
        Coordinate gcj = TransformWGS84ToGCJ02(coord);
        return TransformGCJ02ToBD09(gcj);
    }
    
    Coordinate BD09ToWGS84(const Coordinate& coord) override {
        Coordinate gcj = TransformBD09ToGCJ02(coord);
        return TransformGCJ02ToWGS84(gcj);
    }
    
    Coordinate GCJ02ToBD09(const Coordinate& coord) override {
        return TransformGCJ02ToBD09(coord);
    }
    
    Coordinate BD09ToGCJ02(const Coordinate& coord) override {
        return TransformBD09ToGCJ02(coord);
    }
    
    Coordinate TransformCoordinate(const Coordinate& coord,
                                   CoordinateTransformType fromType,
                                   CoordinateTransformType toType) override {
        if (fromType == toType) {
            return coord;
        }
        
        Coordinate wgs84 = coord;
        
        switch (fromType) {
            case CoordinateTransformType::kGCJ02:
                wgs84 = GCJ02ToWGS84(coord);
                break;
            case CoordinateTransformType::kBD09:
                wgs84 = BD09ToWGS84(coord);
                break;
            default:
                break;
        }
        
        switch (toType) {
            case CoordinateTransformType::kGCJ02:
                return WGS84ToGCJ02(wgs84);
            case CoordinateTransformType::kBD09:
                return WGS84ToBD09(wgs84);
            default:
                return wgs84;
        }
    }
    
    std::vector<Coordinate> TransformCoordinates(
        const std::vector<Coordinate>& coords,
        CoordinateTransformType fromType,
        CoordinateTransformType toType) override {
        
        std::vector<Coordinate> result;
        result.reserve(coords.size());
        
        for (const auto& coord : coords) {
            result.push_back(TransformCoordinate(coord, fromType, toType));
        }
        
        return result;
    }
    
    void SetCoordinateTransformEnabled(bool enabled) override {
        m_coordTransformEnabled = enabled;
    }
    
    bool IsCoordinateTransformEnabled() const override {
        return m_coordTransformEnabled;
    }
    
    void SetDefaultTransformType(CoordinateTransformType type) override {
        m_defaultTransformType = type;
    }
    
    CoordinateTransformType GetDefaultTransformType() const override {
        return m_defaultTransformType;
    }
    
    std::vector<uint8_t> Hash(const std::vector<uint8_t>& data) override {
        return SimpleHash(data);
    }
    
    std::string HashString(const std::string& text) override {
        std::vector<uint8_t> data(text.begin(), text.end());
        std::vector<uint8_t> hash = SimpleHash(data);
        return BytesToHex(hash);
    }
    
    bool VerifyHash(const std::vector<uint8_t>& data,
                    const std::vector<uint8_t>& hash) override {
        std::vector<uint8_t> computed = SimpleHash(data);
        return computed == hash;
    }
    
    bool VerifyHashString(const std::string& text,
                          const std::string& hash) override {
        std::string computed = HashString(text);
        return computed == hash;
    }
    
    std::vector<uint8_t> DeriveKey(const std::string& password,
                                    const std::vector<uint8_t>& salt,
                                    int iterations) override {
        std::vector<uint8_t> key(32, 0);
        std::vector<uint8_t> data(password.begin(), password.end());
        data.insert(data.end(), salt.begin(), salt.end());
        
        for (int i = 0; i < iterations; ++i) {
            std::vector<uint8_t> hashed = SimpleHash(data);
            data = hashed;
            
            if (i == iterations - 1) {
                std::copy(hashed.begin(), hashed.begin() + std::min(hashed.size(), key.size()),
                         key.begin());
            }
        }
        
        return key;
    }
    
    std::vector<uint8_t> GenerateSalt() override {
        std::vector<uint8_t> salt(16);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dis(0, 255);
        
        for (size_t i = 0; i < salt.size(); ++i) {
            salt[i] = static_cast<uint8_t>(dis(gen));
        }
        
        return salt;
    }
    
    bool IsInitialized() const override {
        return m_initialized;
    }
    
    std::string GetKeyStoragePath() const override {
        return m_keyStoragePath;
    }
    
private:
    bool m_initialized;
    EncryptionAlgorithm m_algorithm;
    EncryptionKey m_activeKey;
    std::map<std::string, EncryptionKey> m_storedKeys;
    std::string m_keyStoragePath;
    bool m_coordTransformEnabled;
    CoordinateTransformType m_defaultTransformType;
    
    std::string GenerateKeyId() {
        auto now = std::chrono::system_clock::now();
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).count();
        
        std::stringstream ss;
        ss << "key_" << ms;
        return ss.str();
    }
    
    std::vector<uint8_t> SimpleHash(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> hash(32, 0);
        
        for (size_t i = 0; i < data.size(); ++i) {
            hash[i % 32] ^= data[i];
            hash[(i + 1) % 32] = (hash[(i + 1) % 32] + data[i]) & 0xFF;
            hash[(i + 7) % 32] = (hash[(i + 7) % 32] ^ (data[i] << 3)) & 0xFF;
        }
        
        for (int round = 0; round < 64; ++round) {
            for (size_t i = 0; i < 32; ++i) {
                hash[i] = (hash[i] + hash[(i + 1) % 32]) & 0xFF;
                hash[(i + 13) % 32] ^= hash[i];
            }
        }
        
        return hash;
    }
    
    std::string BytesToHex(const std::vector<uint8_t>& bytes) {
        std::stringstream ss;
        ss << std::hex << std::setfill('0');
        for (uint8_t b : bytes) {
            ss << std::setw(2) << static_cast<int>(b);
        }
        return ss.str();
    }
    
    std::string Base64Encode(const std::vector<uint8_t>& data) {
        static const char* chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        
        std::string result;
        result.reserve(((data.size() + 2) / 3) * 4);
        
        for (size_t i = 0; i < data.size(); i += 3) {
            uint32_t n = (data[i] << 16);
            if (i + 1 < data.size()) n |= (data[i + 1] << 8);
            if (i + 2 < data.size()) n |= data[i + 2];
            
            result.push_back(chars[(n >> 18) & 0x3F]);
            result.push_back(chars[(n >> 12) & 0x3F]);
            result.push_back((i + 1 < data.size()) ? chars[(n >> 6) & 0x3F] : '=');
            result.push_back((i + 2 < data.size()) ? chars[n & 0x3F] : '=');
        }
        
        return result;
    }
    
    std::vector<uint8_t> Base64Decode(const std::string& encoded) {
        static const int table[256] = {
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
            52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
            -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
            15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
            -1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
            41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
            -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
        };
        
        std::vector<uint8_t> result;
        result.reserve((encoded.size() / 4) * 3);
        
        for (size_t i = 0; i < encoded.size(); i += 4) {
            uint32_t n = 0;
            int padding = 0;
            
            for (int j = 0; j < 4; ++j) {
                if (i + j < encoded.size()) {
                    if (encoded[i + j] == '=') {
                        padding++;
                        n <<= 6;
                    } else {
                        int v = table[static_cast<uint8_t>(encoded[i + j])];
                        if (v >= 0) {
                            n = (n << 6) | v;
                        }
                    }
                }
            }
            
            result.push_back((n >> 16) & 0xFF);
            if (padding < 2) {
                result.push_back((n >> 8) & 0xFF);
            }
            if (padding < 1) {
                result.push_back(n & 0xFF);
            }
        }
        
        return result;
    }
    
    bool OutOfChina(const Coordinate& coord) {
        return coord.longitude < 72.004 || coord.longitude > 137.8347 ||
               coord.latitude < 0.8293 || coord.latitude > 55.8271;
    }
    
    double TransformLat(double x, double y) {
        double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y + 0.2 * std::sqrt(std::abs(x));
        ret += (20.0 * std::sin(6.0 * x * PI) + 20.0 * std::sin(2.0 * x * PI)) * 2.0 / 3.0;
        ret += (20.0 * std::sin(y * PI) + 40.0 * std::sin(y / 3.0 * PI)) * 2.0 / 3.0;
        ret += (160.0 * std::sin(y / 12.0 * PI) + 320.0 * std::sin(y * PI / 30.0)) * 2.0 / 3.0;
        return ret;
    }
    
    double TransformLon(double x, double y) {
        double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1 * std::sqrt(std::abs(x));
        ret += (20.0 * std::sin(6.0 * x * PI) + 20.0 * std::sin(2.0 * x * PI)) * 2.0 / 3.0;
        ret += (20.0 * std::sin(x * PI) + 40.0 * std::sin(x / 3.0 * PI)) * 2.0 / 3.0;
        ret += (150.0 * std::sin(x / 12.0 * PI) + 300.0 * std::sin(x / 30.0 * PI)) * 2.0 / 3.0;
        return ret;
    }
    
    Coordinate TransformWGS84ToGCJ02(const Coordinate& coord) {
        if (OutOfChina(coord)) {
            return coord;
        }
        
        double dLat = TransformLat(coord.longitude - 105.0, coord.latitude - 35.0);
        double dLon = TransformLon(coord.longitude - 105.0, coord.latitude - 35.0);
        
        double radLat = coord.latitude / 180.0 * PI;
        double magic = std::sin(radLat);
        magic = 1 - EE * magic * magic;
        double sqrtMagic = std::sqrt(magic);
        
        dLat = (dLat * 180.0) / ((A * (1 - EE)) / (magic * sqrtMagic) * PI);
        dLon = (dLon * 180.0) / (A / sqrtMagic * std::cos(radLat) * PI);
        
        return Coordinate(coord.longitude + dLon, coord.latitude + dLat);
    }
    
    Coordinate TransformGCJ02ToWGS84(const Coordinate& coord) {
        if (OutOfChina(coord)) {
            return coord;
        }
        
        double dLat = TransformLat(coord.longitude - 105.0, coord.latitude - 35.0);
        double dLon = TransformLon(coord.longitude - 105.0, coord.latitude - 35.0);
        
        double radLat = coord.latitude / 180.0 * PI;
        double magic = std::sin(radLat);
        magic = 1 - EE * magic * magic;
        double sqrtMagic = std::sqrt(magic);
        
        dLat = (dLat * 180.0) / ((A * (1 - EE)) / (magic * sqrtMagic) * PI);
        dLon = (dLon * 180.0) / (A / sqrtMagic * std::cos(radLat) * PI);
        
        return Coordinate(coord.longitude - dLon, coord.latitude - dLat);
    }
    
    Coordinate TransformGCJ02ToBD09(const Coordinate& coord) {
        double x = coord.longitude;
        double y = coord.latitude;
        double z = std::sqrt(x * x + y * y) + 0.00002 * std::sin(y * X_PI);
        double theta = std::atan2(y, x) + 0.000003 * std::cos(x * X_PI);
        
        return Coordinate(z * std::cos(theta) + 0.0065, z * std::sin(theta) + 0.006);
    }
    
    Coordinate TransformBD09ToGCJ02(const Coordinate& coord) {
        double x = coord.longitude - 0.0065;
        double y = coord.latitude - 0.006;
        double z = std::sqrt(x * x + y * y) - 0.00002 * std::sin(y * X_PI);
        double theta = std::atan2(y, x) - 0.000003 * std::cos(x * X_PI);
        
        return Coordinate(z * std::cos(theta), z * std::sin(theta));
    }
};

DataEncryption::~DataEncryption() {
}

std::unique_ptr<DataEncryption> DataEncryption::Create() {
    return std::unique_ptr<DataEncryption>(new DataEncryptionImpl());
}

}  
}  
