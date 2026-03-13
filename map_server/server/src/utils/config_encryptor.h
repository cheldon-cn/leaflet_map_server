#ifndef CYCLE_UTILS_CONFIG_ENCRYPTOR_H
#define CYCLE_UTILS_CONFIG_ENCRYPTOR_H

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <map>
#ifdef OPENSSL_SUPPORT
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif


namespace cycle {
namespace utils {

class ConfigEncryptor {
public:
    ConfigEncryptor();
    ~ConfigEncryptor();
    
    bool Initialize(const std::string& master_key_path = "");
    bool IsInitialized() const;
    
    // 加密配置数据
    std::string EncryptConfig(const std::string& plaintext);
    std::string DecryptConfig(const std::string& ciphertext);
    
    // 文件加密/解密
    bool EncryptConfigFile(const std::string& input_file, const std::string& output_file);
    bool DecryptConfigFile(const std::string& input_file, const std::string& output_file);
    
    // 敏感字段处理
    std::string EncryptSensitiveField(const std::string& field_name, const std::string& value);
    std::string DecryptSensitiveField(const std::string& field_name, const std::string& encrypted_value);
    
    // 密钥管理
    bool GenerateMasterKey(const std::string& output_path);
    bool LoadMasterKey(const std::string& key_path);
    bool RotateMasterKey(const std::string& new_key_path);
    
    // 安全清理
    void SecureClean();
    
    // 状态检查
    std::string GetEncryptionStatus() const;
    
private:
    bool GenerateKeyFromPassword(const std::string& password);
    bool DeriveKey(const std::string& password, std::vector<unsigned char>& key, std::vector<unsigned char>& iv);
    bool EncryptAES(const std::vector<unsigned char>& plaintext, std::vector<unsigned char>& ciphertext);
    bool DecryptAES(const std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& plaintext);
    
    void HandleOpenSSLError(const std::string& operation);
    void SecureErase(std::vector<unsigned char>& data);
    
    std::vector<unsigned char> master_key_;
    std::vector<unsigned char> encryption_iv_;
    bool initialized_;
    
    static const int KEY_LENGTH = 32; // 256-bit key
    static const int IV_LENGTH = 16;  // 128-bit IV
    static const int SALT_LENGTH = 16;
    static const int ITERATIONS = 10000;
    
    mutable std::mutex mutex_;
};

class SecureConfigManager {
public:
    static SecureConfigManager& Instance();
    
    bool Initialize(const std::string& config_path, const std::string& key_path = "");
    
    // 安全配置访问
    std::string GetSecureConfigValue(const std::string& key);
    bool SetSecureConfigValue(const std::string& key, const std::string& value, bool encrypt = true);
    
    // 批量操作
    bool LoadSecureConfig(const std::string& config_path);
    bool SaveSecureConfig(const std::string& config_path);
    
    // 环境变量支持
    bool LoadFromEnvironment();
    
    // 配置验证
    bool ValidateConfigSecurity();
    
    // 敏感字段列表
    static const std::vector<std::string>& GetSensitiveFields();

    SecureConfigManager() = default;
private:
    
    std::unique_ptr<ConfigEncryptor> encryptor_;
    std::map<std::string, std::string> config_cache_;
    std::string config_file_path_;
    bool secure_mode_;
    
    mutable std::mutex config_mutex_;
};

} // namespace utils
} // namespace cycle

#endif // CYCLE_UTILS_CONFIG_ENCRYPTOR_H