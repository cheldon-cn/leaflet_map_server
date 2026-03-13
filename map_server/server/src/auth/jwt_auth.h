#ifndef CYCLE_AUTH_JWT_AUTH_H
#define CYCLE_AUTH_JWT_AUTH_H

#include <string>
#include <map>
#include <chrono>
#include <memory>
#include <mutex>
#include <vector>

namespace cycle {
namespace auth {

struct JWTConfig {
    std::string secret_key;
    int token_expiry_hours;
    std::string issuer;
    std::string audience;
    
    JWTConfig()
        : secret_key("your-super-secret-key-change-in-production")
        , token_expiry_hours(24)
        , issuer("cycle-map-server")
        , audience("map-client") {}
};

struct User {
    std::string username;
    std::string role;
    std::vector<std::string> permissions;
    std::chrono::system_clock::time_point created_at;
    bool is_active;
    
    User()
        : role("user")
        , is_active(true)
        , created_at(std::chrono::system_clock::now()) {}
    
    bool HasPermission(const std::string& permission) const {
        return std::find(permissions.begin(), permissions.end(), permission) != permissions.end();
    }
};

struct TokenPayload {
    std::string username;
    std::string role;
    std::vector<std::string> permissions;
    std::chrono::system_clock::time_point issued_at;
    std::chrono::system_clock::time_point expires_at;
    
    bool IsExpired() const {
        return std::chrono::system_clock::now() > expires_at;
    }
    
    bool HasPermission(const std::string& permission) const {
        return std::find(permissions.begin(), permissions.end(), permission) != permissions.end();
    }
};

class JWTAuth {
public:
    explicit JWTAuth(const JWTConfig& config);
    
    std::string GenerateToken(const User& user);
    bool ValidateToken(const std::string& token);
    TokenPayload DecodeToken(const std::string& token);
    
    bool VerifyPermission(const std::string& token, const std::string& permission);
    std::string RefreshToken(const std::string& token);
    
    void RevokeToken(const std::string& token);
    bool IsTokenRevoked(const std::string& token) const;
    
    void AddUser(const User& user);
    bool AuthenticateUser(const std::string& username, const std::string& password);
    User GetUser(const std::string& username) const;
    
    void SetConfig(const JWTConfig& config);
    JWTConfig GetConfig() const;
    
private:
    std::string GenerateSecret() const;
    void LoadDefaultUsers();
    
    JWTConfig config_;
    std::map<std::string, User> users_;
    std::map<std::string, std::chrono::system_clock::time_point> revoked_tokens_;
    
    mutable std::mutex mutex_;
};

class AuthMiddleware {
public:
    explicit AuthMiddleware(std::shared_ptr<JWTAuth> auth);
    
    bool ProcessRequest(const std::string& token, const std::string& path, const std::string& method);
    bool RequireAuthentication(const std::string& path) const;
    bool RequirePermission(const std::string& path, const std::string& method) const;
    
    void AddPublicPath(const std::string& path);
    void AddProtectedPath(const std::string& path, const std::string& permission = "");
    
    std::string GetRequiredPermission(const std::string& path, const std::string& method) const;
    
private:
    struct PathRule {
        std::string path;
        std::string method;
        std::string permission;
        bool require_auth;
        
        PathRule(const std::string& p, const std::string& m, const std::string& perm, bool auth)
            : path(p), method(m), permission(perm), require_auth(auth) {}
    };
    
    std::shared_ptr<JWTAuth> auth_;
    std::vector<PathRule> path_rules_;
};

} // namespace auth
} // namespace cycle

#endif // CYCLE_AUTH_JWT_AUTH_H