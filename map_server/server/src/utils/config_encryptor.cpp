#include "config_encryptor.h"
#include "logger.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <nlohmann/json.hpp>
#ifdef OPENSSL_SUPPORT
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#endif

namespace cycle {
namespace utils {

ConfigEncryptor::ConfigEncryptor() 
    : initialized_(false) {
    
#ifdef OPENSSL_SUPPORT
    // 初始化OpenSSL（如果尚未初始化）
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
#endif
}

ConfigEncryptor::~ConfigEncryptor() {
    SecureClean();
}

bool ConfigEncryptor::Initialize(const std::string& master_key_path) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (initialized_) {
        LOG_WARN("ConfigEncryptor already initialized");
        return true;
    }
    
    // 尝试从文件加载主密钥
    if (!master_key_path.empty()) {
        if (!LoadMasterKey(master_key_path)) {
            LOG_ERROR("Failed to load master key from: " + master_key_path);
            return false;
        }
    } else {
        // 从环境变量获取密钥

        const char* env_key = std::getenv("CYCLE_CONFIG_KEY");
        if (env_key) {
            if (!GenerateKeyFromPassword(env_key)) {
                LOG_ERROR("Failed to generate key from environment");
                return false;
            }
        } else {
            LOG_WARN("No master key provided, using development mode");
            // 开发模式下使用固定密钥（生产环境不应使用）
            std::string dev_key = "development-key-change-in-production";
            if (!GenerateKeyFromPassword(dev_key)) {
                LOG_ERROR("Failed to generate development key");
                return false;
            }
        }
    }
    
    initialized_ = true;
    LOG_INFO("ConfigEncryptor initialized successfully");
    return true;
}

bool ConfigEncryptor::IsInitialized() const {
    return initialized_;
}

std::string ConfigEncryptor::EncryptConfig(const std::string& plaintext) {
    if (!initialized_) {
        LOG_ERROR("ConfigEncryptor not initialized");
        return "";
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        std::vector<unsigned char> plaintext_vec(plaintext.begin(), plaintext.end());
        std::vector<unsigned char> ciphertext;
        
        if (!EncryptAES(plaintext_vec, ciphertext)) {
            LOG_ERROR("Encryption failed");
            return "";
        }
        
        // 转换为Base64编码的字符串
        std::string encoded;
        encoded.resize(ciphertext.size() * 2);
        
        for (size_t i = 0; i < ciphertext.size(); ++i) {
            char buf[3];
            snprintf(buf, sizeof(buf), "%02x", ciphertext[i]);
            encoded[i * 2] = buf[0];
            encoded[i * 2 + 1] = buf[1];
        }
        
        return encoded;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Encryption error: " + std::string(e.what()));
        return "";
    }
}

std::string ConfigEncryptor::DecryptConfig(const std::string& ciphertext) {
    if (!initialized_) {
        LOG_ERROR("ConfigEncryptor not initialized");
        return "";
    }
    
    if (ciphertext.empty()) {
        return "";
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    try {
        // 从十六进制字符串转换
        if (ciphertext.length() % 2 != 0) {
            LOG_ERROR("Invalid ciphertext format");
            return "";
        }
        
        std::vector<unsigned char> ciphertext_vec;
        ciphertext_vec.resize(ciphertext.length() / 2);
        
        for (size_t i = 0; i < ciphertext.length(); i += 2) {
            std::string byte_str = ciphertext.substr(i, 2);
            ciphertext_vec[i / 2] = static_cast<unsigned char>(std::stoul(byte_str, nullptr, 16));
        }
        
        std::vector<unsigned char> plaintext;
        if (!DecryptAES(ciphertext_vec, plaintext)) {
            LOG_ERROR("Decryption failed");
            return "";
        }
        
        return std::string(plaintext.begin(), plaintext.end());
        
    } catch (const std::exception& e) {
        LOG_ERROR("Decryption error: " + std::string(e.what()));
        return "";
    }
}

bool ConfigEncryptor::EncryptConfigFile(const std::string& input_file, const std::string& output_file) {
    if (!initialized_) {
        LOG_ERROR("ConfigEncryptor not initialized");
        return false;
    }
    
    try {
        // 读取原始配置文件
        std::ifstream in_file(input_file);
        if (!in_file.is_open()) {
            LOG_ERROR("Failed to open input file: " + input_file);
            return false;
        }
        
        std::stringstream buffer;
        buffer << in_file.rdbuf();
        std::string plaintext = buffer.str();
        in_file.close();
        
        // 加密内容
        std::string encrypted = EncryptConfig(plaintext);
        if (encrypted.empty()) {
            LOG_ERROR("Failed to encrypt file content");
            return false;
        }
        
        // 写入加密文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            LOG_ERROR("Failed to create output file: " + output_file);
            return false;
        }
        
        out_file << encrypted;
        out_file.close();
        
        LOG_INFO("Config file encrypted: " + input_file + " -> " + output_file);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("File encryption error: " + std::string(e.what()));
        return false;
    }
}

bool ConfigEncryptor::DecryptConfigFile(const std::string& input_file, const std::string& output_file) {
    if (!initialized_) {
        LOG_ERROR("ConfigEncryptor not initialized");
        return false;
    }
    
    try {
        // 读取加密文件
        std::ifstream in_file(input_file);
        if (!in_file.is_open()) {
            LOG_ERROR("Failed to open input file: " + input_file);
            return false;
        }
        
        std::stringstream buffer;
        buffer << in_file.rdbuf();
        std::string ciphertext = buffer.str();
        in_file.close();
        
        // 解密内容
        std::string decrypted = DecryptConfig(ciphertext);
        if (decrypted.empty()) {
            LOG_ERROR("Failed to decrypt file content");
            return false;
        }
        
        // 写入解密文件
        std::ofstream out_file(output_file);
        if (!out_file.is_open()) {
            LOG_ERROR("Failed to create output file: " + output_file);
            return false;
        }
        
        out_file << decrypted;
        out_file.close();
        
        LOG_INFO("Config file decrypted: " + input_file + " -> " + output_file);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("File decryption error: " + std::string(e.what()));
        return false;
    }
}

std::string ConfigEncryptor::EncryptSensitiveField(const std::string& field_name, const std::string& value) {
    if (value.empty()) {
        return "";
    }
    
    std::string encrypted = EncryptConfig(value);
    if (!encrypted.empty()) {
        LOG_DEBUG("Encrypted sensitive field: " + field_name);
    }
    
    return encrypted;
}

std::string ConfigEncryptor::DecryptSensitiveField(const std::string& field_name, const std::string& encrypted_value) {
    if (encrypted_value.empty()) {
        return "";
    }
    
    std::string decrypted = DecryptConfig(encrypted_value);
    if (!decrypted.empty()) {
        LOG_DEBUG("Decrypted sensitive field: " + field_name);
    }
    
    return decrypted;
}

bool ConfigEncryptor::GenerateMasterKey(const std::string& output_path) {
#ifdef OPENSSL_SUPPORT
    std::vector<unsigned char> key(KEY_LENGTH);
    std::vector<unsigned char> iv(IV_LENGTH);
    std::vector<unsigned char> salt(SALT_LENGTH);
    
    // 生成随机盐值

    if (RAND_bytes(salt.data(), SALT_LENGTH) != 1) {
        HandleOpenSSLError("Generate salt");
        return false;
    }
    
    // 生成随机密钥和IV

    if (RAND_bytes(key.data(), KEY_LENGTH) != 1) {
        HandleOpenSSLError("Generate key");
        return false;
    }
    
    if (RAND_bytes(iv.data(), IV_LENGTH) != 1) {
        HandleOpenSSLError("Generate IV");
        return false;
    }
    
    // 保存密钥文件

    try {
        std::ofstream key_file(output_path, std::ios::binary);
        if (!key_file.is_open()) {
            LOG_ERROR("Failed to create key file: " + output_path);
            return false;
        }
        
        // 写入盐值、密钥和IV

        key_file.write(reinterpret_cast<const char*>(salt.data()), salt.size());
        key_file.write(reinterpret_cast<const char*>(key.data()), key.size());
        key_file.write(reinterpret_cast<const char*>(iv.data()), iv.size());
        key_file.close();
        
        LOG_INFO("Master key generated: " + output_path);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Key generation error: " + std::string(e.what()));
        return false;
    }
#else
    LOG_ERROR("OpenSSL support not enabled");
    return false;
#endif
}

bool ConfigEncryptor::LoadMasterKey(const std::string& key_path) {
    try {
        std::ifstream key_file(key_path, std::ios::binary);
        if (!key_file.is_open()) {
            LOG_ERROR("Failed to open key file: " + key_path);
            return false;
        }
        
        // 读取盐值、密钥和IV
        std::vector<unsigned char> salt(SALT_LENGTH);
        std::vector<unsigned char> key(KEY_LENGTH);
        std::vector<unsigned char> iv(IV_LENGTH);
        
        key_file.read(reinterpret_cast<char*>(salt.data()), salt.size());
        key_file.read(reinterpret_cast<char*>(key.data()), key.size());
        key_file.read(reinterpret_cast<char*>(iv.data()), iv.size());
        key_file.close();
        
        if (!key_file.good()) {
            LOG_ERROR("Invalid key file format: " + key_path);
            return false;
        }
        
        // 设置密钥和IV
        master_key_ = key;
        encryption_iv_ = iv;
        
        LOG_INFO("Master key loaded: " + key_path);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Key loading error: " + std::string(e.what()));
        return false;
    }
}

bool ConfigEncryptor::RotateMasterKey(const std::string& new_key_path) {
    if (!initialized_) {
        LOG_ERROR("ConfigEncryptor not initialized");
        return false;
    }
    
    // 生成新密钥
    if (!GenerateMasterKey(new_key_path)) {
        LOG_ERROR("Failed to generate new master key");
        return false;
    }
    
    LOG_INFO("Master key rotated: " + new_key_path);
    return true;
}

void ConfigEncryptor::SecureClean() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    SecureErase(master_key_);
    SecureErase(encryption_iv_);
    
    initialized_ = false;
    
    LOG_DEBUG("ConfigEncryptor securely cleaned");
}

std::string ConfigEncryptor::GetEncryptionStatus() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::stringstream status;
    status << "ConfigEncryptor Status:" << std::endl;
    status << "- Initialized: " << (initialized_ ? "Yes" : "No") << std::endl;
    status << "- Key Loaded: " << (!master_key_.empty() ? "Yes" : "No") << std::endl;
    status << "- Key Length: " << master_key_.size() << " bytes" << std::endl;
    
    return status.str();
}

bool ConfigEncryptor::GenerateKeyFromPassword(const std::string& password) {
#ifdef OPENSSL_SUPPORT
    std::vector<unsigned char> salt(SALT_LENGTH);
    
    // 生成随机盐值
    if (RAND_bytes(salt.data(), SALT_LENGTH) != 1) {
        HandleOpenSSLError("Generate salt");
        return false;
    }
    
    return DeriveKey(password, master_key_, encryption_iv_);
#else
    LOG_ERROR("OpenSSL support not enabled");
    return false;
#endif
}

bool ConfigEncryptor::DeriveKey(const std::string& password, std::vector<unsigned char>& key, std::vector<unsigned char>& iv) {
#ifdef OPENSSL_SUPPORT
    key.resize(KEY_LENGTH);
    iv.resize(IV_LENGTH);
    
    std::vector<unsigned char> salt(SALT_LENGTH);
    if (RAND_bytes(salt.data(), SALT_LENGTH) != 1) {
        HandleOpenSSLError("Generate salt");
        return false;
    }
    
    // 使用PBKDF2派生密钥
    if (PKCS5_PBKDF2_HMAC(password.c_str(), password.length(),
                         salt.data(), salt.size(),
                         ITERATIONS, EVP_sha256(),
                         KEY_LENGTH + IV_LENGTH, key.data()) != 1) {
        HandleOpenSSLError("PBKDF2 key derivation");
        return false;
    }
    
    // 分离密钥和IV
    std::copy(key.begin(), key.begin() + KEY_LENGTH, key.begin());
    std::copy(key.begin() + KEY_LENGTH, key.end(), iv.begin());
    
    return true;
#else
    LOG_ERROR("OpenSSL support not enabled");
    return false;
#endif
}

bool ConfigEncryptor::EncryptAES(const std::vector<unsigned char>& plaintext, std::vector<unsigned char>& ciphertext) {
#ifdef OPENSSL_SUPPORT
    if (master_key_.empty() || encryption_iv_.empty()) {
        LOG_ERROR("Encryption key not available");
        return false;
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        HandleOpenSSLError("Create cipher context");
        return false;
    }
    
    // 初始化加密操作
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                          master_key_.data(), encryption_iv_.data()) != 1) {
        HandleOpenSSLError("Encrypt init");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 分配输出缓冲区
    ciphertext.resize(plaintext.size() + EVP_CIPHER_CTX_block_size(ctx));
    
    int len;
    int ciphertext_len = 0;
    
    // 提供要加密的消息
    if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, 
                         plaintext.data(), plaintext.size()) != 1) {
        HandleOpenSSLError("Encrypt update");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len = len;
    
    // 完成加密
    if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
        HandleOpenSSLError("Encrypt final");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len += len;
    
    // 调整输出大小
    ciphertext.resize(ciphertext_len);
    
    EVP_CIPHER_CTX_free(ctx);
    return true;
#else
    LOG_ERROR("OpenSSL support not enabled");
    return false;
#endif
}

bool ConfigEncryptor::DecryptAES(const std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& plaintext) {
#ifdef OPENSSL_SUPPORT
    if (master_key_.empty() || encryption_iv_.empty()) {
        LOG_ERROR("Decryption key not available");
        return false;
    }
    
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        HandleOpenSSLError("Create cipher context");
        return false;
    }
    
    // 初始化解密操作
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, 
                          master_key_.data(), encryption_iv_.data()) != 1) {
        HandleOpenSSLError("Decrypt init");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    
    // 分配输出缓冲区
    plaintext.resize(ciphertext.size() + EVP_CIPHER_CTX_block_size(ctx));
    
    int len;
    int plaintext_len = 0;
    
    // 提供要解密的消息
    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, 
                         ciphertext.data(), ciphertext.size()) != 1) {
        HandleOpenSSLError("Decrypt update");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len = len;
    
    // 完成解密
    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        HandleOpenSSLError("Decrypt final");
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len += len;
    
    // 调整输出大小
    plaintext.resize(plaintext_len);
    
    EVP_CIPHER_CTX_free(ctx);
    return true;
#else
    LOG_ERROR("OpenSSL support not enabled");
    return false;
#endif
}

void ConfigEncryptor::HandleOpenSSLError(const std::string& operation) {
#ifdef OPENSSL_SUPPORT
    char error_buf[256];
    ERR_error_string_n(ERR_get_error(), error_buf, sizeof(error_buf));
    LOG_ERROR("OpenSSL error in " + operation + ": " + std::string(error_buf));
#endif
}

void ConfigEncryptor::SecureErase(std::vector<unsigned char>& data) {
#ifdef OPENSSL_SUPPORT
    if (!data.empty()) {
        // 使用随机数据覆盖
        RAND_bytes(data.data(), data.size());
        // 清零
        std::fill(data.begin(), data.end(), 0);
        data.clear();
        data.shrink_to_fit();
    }
#endif
}

// SecureConfigManager 实现
SecureConfigManager& SecureConfigManager::Instance() {
    static SecureConfigManager instance;
    return instance;
}

bool SecureConfigManager::Initialize(const std::string& config_path, const std::string& key_path) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    
    config_file_path_ = config_path;
    encryptor_ = std::make_unique<ConfigEncryptor>();
    
    if (!encryptor_->Initialize(key_path)) {
        LOG_ERROR("Failed to initialize config encryptor");
        return false;
    }
    
    secure_mode_ = true;
    
    // 加载配置
    if (!LoadSecureConfig(config_path)) {
        LOG_WARN("Failed to load secure config, running in fallback mode");
        secure_mode_ = false;
    }
    
    LOG_INFO("SecureConfigManager initialized");
    return true;
}

std::string SecureConfigManager::GetSecureConfigValue(const std::string& key) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    
    auto it = config_cache_.find(key);
    if (it != config_cache_.end()) {
        // 如果是敏感字段且已加密，需要解密
        auto sensitive_fields = GetSensitiveFields();
        if (std::find(sensitive_fields.begin(), sensitive_fields.end(), key) != sensitive_fields.end()) {
            if (secure_mode_ && encryptor_) {
                return encryptor_->DecryptSensitiveField(key, it->second);
            }
        }
        return it->second;
    }
    
    return "";
}

bool SecureConfigManager::SetSecureConfigValue(const std::string& key, const std::string& value, bool encrypt) {
    std::lock_guard<std::mutex> lock(config_mutex_);
    
    std::string stored_value = value;
    
    if (encrypt && secure_mode_ && encryptor_) {
        auto sensitive_fields = GetSensitiveFields();
        if (std::find(sensitive_fields.begin(), sensitive_fields.end(), key) != sensitive_fields.end()) {
            stored_value = encryptor_->EncryptSensitiveField(key, value);
            if (stored_value.empty()) {
                LOG_ERROR("Failed to encrypt value for key: " + key);
                return false;
            }
        }
    }
    
    config_cache_[key] = stored_value;
    return true;
}

bool SecureConfigManager::LoadSecureConfig(const std::string& config_path) {
    try {
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            LOG_ERROR("Failed to open config file: " + config_path);
            return false;
        }
        
        nlohmann::json config_json;
        config_file >> config_json;
        config_file.close();
        
        // 解析配置并存储
        for (auto& item : config_json.items()) {
			auto& key = item.key();
			auto& value = item.value();
            if (value.is_string()) {
                config_cache_[key] = value.get<std::string>();
            }
        }
        
        LOG_INFO("Secure config loaded: " + config_path);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Config loading error: " + std::string(e.what()));
        return false;
    }
}

bool SecureConfigManager::SaveSecureConfig(const std::string& config_path) {
    try {
        nlohmann::json config_json;
        
		for (const auto&item : config_cache_) {
				auto& key = item.first;
				auto& value = item.second;
            config_json[key] = value;
        }
        
        std::ofstream config_file(config_path);
        if (!config_file.is_open()) {
            LOG_ERROR("Failed to create config file: " + config_path);
            return false;
        }
        
        config_file << config_json.dump(4);
        config_file.close();
        
        LOG_INFO("Secure config saved: " + config_path);
        return true;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Config saving error: " + std::string(e.what()));
        return false;
    }
}

bool SecureConfigManager::LoadFromEnvironment() {
    // 从环境变量加载敏感配置
    const char* db_password = std::getenv("CYCLE_DB_PASSWORD");
    if (db_password) {
        SetSecureConfigValue("database.password", db_password, true);
    }
    
    const char* jwt_secret = std::getenv("CYCLE_JWT_SECRET");
    if (jwt_secret) {
        SetSecureConfigValue("security.jwt_secret", jwt_secret, true);
    }
    
    LOG_INFO("Environment variables loaded");
    return true;
}

bool SecureConfigManager::ValidateConfigSecurity() {
    if (!secure_mode_) {
        LOG_WARN("Running in insecure mode");
        return false;
    }
    
    // 检查敏感字段是否已加密
    auto sensitive_fields = GetSensitiveFields();
    bool all_secure = true;
    
    for (const auto& field : sensitive_fields) {
        auto it = config_cache_.find(field);
        if (it != config_cache_.end()) {
            // 检查是否为加密格式（十六进制字符串）
            std::string value = it->second;
            if (value.length() % 2 == 0 && 
                std::all_of(value.begin(), value.end(), ::isxdigit)) {
                LOG_DEBUG("Field is encrypted: " + field);
            } else {
                LOG_WARN("Field is not encrypted: " + field);
                all_secure = false;
            }
        }
    }
    
    return all_secure;
}

const std::vector<std::string>& SecureConfigManager::GetSensitiveFields() {
    static const std::vector<std::string> sensitive_fields = {
        "database.password",
        "security.jwt_secret", 
        "security.ssl_key_password",
        "api.keys",
        "monitoring.credentials",
        "cache.redis_password"
    };
    
    return sensitive_fields;
}

} // namespace utils
} // namespace cycle
