#include "jwt_auth.h"
#include "../utils/logger.h"
#include <algorithm>
#include <random>
#include <sstream>
#include <iomanip>
#include <ctime>

#include <jwt-cpp/jwt.h>
#ifdef JWT_SERVER_SUPPORT
#endif
namespace cycle {
namespace auth {

JWTAuth::JWTAuth(const JWTConfig& config)
    : config_(config) {
    
    LOG_INFO("JWT authentication initialized");
    LoadDefaultUsers();
}

std::string JWTAuth::GenerateToken(const User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (!user.is_active) {
        LOG_WARN("Attempt to generate token for inactive user: " + user.username);
        return "";
    }
    
    auto now = std::chrono::system_clock::now();
    auto expiry = now + std::chrono::hours(config_.token_expiry_hours);
    
    try {
        auto token = jwt::create()
            .set_issuer(config_.issuer)
            .set_audience(config_.audience)
            .set_issued_at(now)
            .set_expires_at(expiry)
            .sign(jwt::algorithm::hs256{config_.secret_key});
        
        LOG_DEBUG("Token generated for user: " + user.username);
        return token;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to generate token: " + std::string(e.what()));
        return "";
    }
}

bool JWTAuth::ValidateToken(const std::string& token) {
    if (token.empty()) {
        return false;
    }
    
    if (IsTokenRevoked(token)) {
        LOG_WARN("Attempt to use revoked token");
        return false;
    }
    
    try {
        auto decoded = jwt::decode(token);
        auto verifier = jwt::verify()
            .allow_algorithm(jwt::algorithm::hs256{config_.secret_key})
            .with_issuer(config_.issuer)
            .with_audience(config_.audience);
        
        verifier.verify(decoded);
        
        return true;
        
    } catch (const std::exception& e) {
        LOG_WARN("Token verification failed: " + std::string(e.what()));
        return false;
    }
}

TokenPayload JWTAuth::DecodeToken(const std::string& token) {
    TokenPayload payload;
    
    if (!ValidateToken(token)) {
        return payload;
    }
    
    try {
        auto decoded = jwt::decode(token);
        
        payload.username = decoded.get_subject();
        payload.role = "user";
        
        auto exp = decoded.get_expires_at();
        payload.expires_at = exp;
        
        auto iat = decoded.get_issued_at();
        payload.issued_at = iat;
        
    } catch (const std::exception& e) {
        LOG_ERROR("Failed to decode token: " + std::string(e.what()));
    }
    
    return payload;
}

bool JWTAuth::VerifyPermission(const std::string& token, const std::string& permission) {
    auto payload = DecodeToken(token);
    
    if (payload.IsExpired()) {
        return false;
    }
    
    return payload.HasPermission(permission);
}

std::string JWTAuth::RefreshToken(const std::string& token) {
    auto payload = DecodeToken(token);
    
    if (payload.username.empty()) {
        return "";
    }
    
    User user;
    user.username = payload.username;
    user.role = payload.role;
    user.permissions = payload.permissions;
    user.is_active = true;
    
    return GenerateToken(user);
}

void JWTAuth::RevokeToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto expiry = std::chrono::system_clock::now() + std::chrono::hours(24);
    revoked_tokens_[token] = expiry;
    
    LOG_INFO("Token revoked");
}

bool JWTAuth::IsTokenRevoked(const std::string& token) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = revoked_tokens_.find(token);
    if (it == revoked_tokens_.end()) {
        return false;
    }
    
    if (it->second < std::chrono::system_clock::now()) {
        return false;
    }
    
    return true;
}

void JWTAuth::AddUser(const User& user) {
    std::lock_guard<std::mutex> lock(mutex_);
    users_[user.username] = user;
    
    LOG_INFO("User added: " + user.username);
}

bool JWTAuth::AuthenticateUser(const std::string& username, const std::string& password) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = users_.find(username);
    if (it == users_.end()) {
        LOG_WARN("User not found: " + username);
        return false;
    }
    
    if (!it->second.is_active) {
        LOG_WARN("User is inactive: " + username);
        return false;
    }
    
    return true;
}

User JWTAuth::GetUser(const std::string& username) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = users_.find(username);
    if (it != users_.end()) {
        return it->second;
    }
    
    return User();
}

void JWTAuth::SetConfig(const JWTConfig& config) {
    std::lock_guard<std::mutex> lock(mutex_);
    config_ = config;
}

JWTConfig JWTAuth::GetConfig() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return config_;
}

std::string JWTAuth::GenerateSecret() const {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(33, 126);
    
    std::string secret;
    for (int i = 0; i < 32; ++i) {
        secret += static_cast<char>(dis(gen));
    }
    
    return secret;
}

void JWTAuth::LoadDefaultUsers() {
    User admin;
    admin.username = "admin";
    admin.role = "admin";
    admin.permissions = {"read", "write", "delete"};
    admin.is_active = true;
    
    AddUser(admin);
    
    User user;
    user.username = "user";
    user.role = "user";
    user.permissions = {"read"};
    user.is_active = true;
    
    AddUser(user);
    
    LOG_INFO("Default users loaded");
}

// AuthMiddleware实现
AuthMiddleware::AuthMiddleware(std::shared_ptr<JWTAuth> auth)
    : auth_(auth) {
    
    // 默认设置公共路径
    AddPublicPath("/health");
    AddPublicPath("/metrics");
    AddPublicPath("/1.0.0/WMTSCapabilities.xml");
    AddPublicPath("/auth/login");
    AddPublicPath("/auth/logout");
    AddPublicPath("/auth/refresh");
    AddPublicPath("/auth/user");
    
    // 设置需要认证的路径
    AddProtectedPath("/tile");
    AddProtectedPath("/generate");
}

bool AuthMiddleware::ProcessRequest(const std::string& token, const std::string& path, const std::string& method) {
    // 检查是否是公共路径
    if (!RequireAuthentication(path)) {
        return true;
    }
    
    // 需要认证但没有提供token
    if (token.empty()) {
        LOG_WARN("Authentication required for path: " + path);
        return false;
    }
    
    // 验证token
    if (!auth_->ValidateToken(token)) {
        LOG_WARN("Invalid token for path: " + path);
        return false;
    }
    
    // 检查是否需要特定权限
    std::string required_permission = GetRequiredPermission(path, method);
    if (!required_permission.empty()) {
        if (!auth_->VerifyPermission(token, required_permission)) {
            LOG_WARN("Permission denied for path: " + path + ", permission: " + required_permission);
            return false;
        }
    }
    
    return true;
}

bool AuthMiddleware::RequireAuthentication(const std::string& path) const {
    for (const auto& rule : path_rules_) {
        if (rule.require_auth) {
            if (path.find(rule.path) != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

bool AuthMiddleware::RequirePermission(const std::string& path, const std::string& method) const {
    for (const auto& rule : path_rules_) {
        if (rule.require_auth && !rule.permission.empty()) {
            if (path.find(rule.path) != std::string::npos) {
                return true;
            }
        }
    }
    return false;
}

void AuthMiddleware::AddPublicPath(const std::string& path) {
    path_rules_.emplace_back(path, "", "", false);
    LOG_DEBUG("Public path added: " + path);
}

void AuthMiddleware::AddProtectedPath(const std::string& path, const std::string& permission) {
    path_rules_.emplace_back(path, "", permission, true);
    LOG_DEBUG("Protected path added: " + path + ", permission: " + permission);
}

std::string AuthMiddleware::GetRequiredPermission(const std::string& path, const std::string& method) const {
    for (const auto& rule : path_rules_) {
        if (rule.require_auth && !rule.permission.empty()) {
            if (path.find(rule.path) != std::string::npos) {
                return rule.permission;
            }
        }
    }
    return "";
}

} // namespace auth
} // namespace cycle
