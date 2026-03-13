#include <gtest/gtest.h>
#include "../src/auth/jwt_auth.h"
#include <thread>
#include <chrono>

class JWTAuthTest : public ::testing::Test {
protected:
    void SetUp() override {
        cycle::auth::JWTConfig config;
        config.secret_key = "test-secret-key-for-unit-tests";
        config.token_expiry_hours = 1;
        config.issuer = "test-issuer";
        config.audience = "test-audience";
        
        auth = std::make_shared<cycle::auth::JWTAuth>(config);
    }
    
    std::shared_ptr<cycle::auth::JWTAuth> auth;
};

TEST_F(JWTAuthTest, TokenGenerationAndValidation) {
    cycle::auth::User user;
    user.username = "testuser";
    user.role = "user";
    user.permissions = {"tiles:read", "maps:generate"};
    
    std::string token = auth->GenerateToken(user);
    
    EXPECT_FALSE(token.empty());
    EXPECT_TRUE(auth->ValidateToken(token));
}

TEST_F(JWTAuthTest, TokenExpiration) {
    cycle::auth::User user;
    user.username = "testuser";
    
    // 设置很短的过期时间
    auto config = auth->GetConfig();
    config.token_expiry_hours = 0; // 0小时，立即过期
    auth->SetConfig(config);
    
    std::string token = auth->GenerateToken(user);
    
    // 等待一小段时间确保令牌过期
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    EXPECT_FALSE(auth->ValidateToken(token));
}

TEST_F(JWTAuthTest, TokenPayloadDecoding) {
    cycle::auth::User user;
    user.username = "testuser";
    user.role = "admin";
    user.permissions = {"tiles:read", "tiles:write", "users:manage"};
    
    std::string token = auth->GenerateToken(user);
    
    auto payload = auth->DecodeToken(token);
    
    EXPECT_EQ(payload.username, "testuser");
    EXPECT_EQ(payload.role, "admin");
    EXPECT_EQ(payload.permissions.size(), 3);
    EXPECT_TRUE(payload.HasPermission("tiles:read"));
    EXPECT_FALSE(payload.HasPermission("nonexistent"));
}

TEST_F(JWTAuthTest, PermissionVerification) {
    cycle::auth::User user;
    user.username = "testuser";
    user.permissions = {"tiles:read", "maps:generate"};
    
    std::string token = auth->GenerateToken(user);
    
    EXPECT_TRUE(auth->VerifyPermission(token, "tiles:read"));
    EXPECT_TRUE(auth->VerifyPermission(token, "maps:generate"));
    EXPECT_FALSE(auth->VerifyPermission(token, "users:manage"));
}

TEST_F(JWTAuthTest, TokenRevocation) {
    cycle::auth::User user;
    user.username = "testuser";
    
    std::string token = auth->GenerateToken(user);
    
    EXPECT_TRUE(auth->ValidateToken(token));
    
    auth->RevokeToken(token);
    
    EXPECT_FALSE(auth->ValidateToken(token));
    EXPECT_TRUE(auth->IsTokenRevoked(token));
}

TEST_F(JWTAuthTest, TokenRefresh) {
    cycle::auth::User user;
    user.username = "testuser";
    
    std::string originalToken = auth->GenerateToken(user);
    std::string newToken = auth->RefreshToken(originalToken);
    
    EXPECT_FALSE(newToken.empty());
    EXPECT_NE(originalToken, newToken);
    EXPECT_TRUE(auth->ValidateToken(newToken));
    
    // 原始令牌应该被撤销
    EXPECT_FALSE(auth->ValidateToken(originalToken));
}

TEST_F(JWTAuthTest, UserAuthentication) {
    cycle::auth::User user;
    user.username = "testuser";
    user.is_active = true;
    
    auth->AddUser(user);
    
    EXPECT_TRUE(auth->AuthenticateUser("testuser", "password"));
    EXPECT_FALSE(auth->AuthenticateUser("nonexistent", "password"));
    
    // 测试非活跃用户
    cycle::auth::User inactiveUser;
    inactiveUser.username = "inactive";
    inactiveUser.is_active = false;
    auth->AddUser(inactiveUser);
    
    EXPECT_FALSE(auth->AuthenticateUser("inactive", "password"));
}

TEST_F(JWTAuthTest, ThreadSafety) {
    const int numThreads = 10;
    const int tokensPerThread = 5;
    
    std::atomic<int> successCount{0};
    std::atomic<int> errorCount{0};
    
    auto worker = [&](int threadId) {
        for (int i = 0; i < tokensPerThread; ++i) {
            cycle::auth::User user;
            user.username = "user_" + std::to_string(threadId) + "_" + std::to_string(i);
            user.role = "user";
            
            try {
                std::string token = auth->GenerateToken(user);
                
                if (!token.empty() && auth->ValidateToken(token)) {
                    successCount++;
                } else {
                    errorCount++;
                }
            } catch (...) {
                errorCount++;
            }
        }
    };
    
    std::vector<std::thread> threads;
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, i);
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(successCount, numThreads * tokensPerThread);
    EXPECT_EQ(errorCount, 0);
}

TEST_F(JWTAuthTest, InvalidTokenHandling) {
    // 测试无效令牌
    EXPECT_FALSE(auth->ValidateToken(""));
    EXPECT_FALSE(auth->ValidateToken("invalid.token.here"));
    EXPECT_FALSE(auth->ValidateToken("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.invalid.payload.invalid.signature"));
    
    // 解码无效令牌应该返回空payload
    auto payload = auth->DecodeToken("invalid");
    EXPECT_TRUE(payload.username.empty());
}

class AuthMiddlewareTest : public ::testing::Test {
protected:
    void SetUp() override {
        cycle::auth::JWTConfig config;
        config.secret_key = "test-secret-key";
        
        auth = std::make_shared<cycle::auth::JWTAuth>(config);
        middleware = std::make_shared<cycle::auth::AuthMiddleware>(auth);
    }
    
    std::shared_ptr<cycle::auth::JWTAuth> auth;
    std::shared_ptr<cycle::auth::AuthMiddleware> middleware;
};

TEST_F(AuthMiddlewareTest, PublicPathAccess) {
    EXPECT_FALSE(middleware->RequireAuthentication("/health"));
    EXPECT_FALSE(middleware->RequireAuthentication("/auth/login"));
    
    // 公开路径不需要令牌
    EXPECT_TRUE(middleware->ProcessRequest("", "/health", "GET"));
    EXPECT_TRUE(middleware->ProcessRequest("", "/auth/login", "POST"));
}

TEST_F(AuthMiddlewareTest, ProtectedPathAccess) {
    EXPECT_TRUE(middleware->RequireAuthentication("/tiles/0/0/0/png"));
    EXPECT_TRUE(middleware->RequireAuthentication("/map"));
    
    // 受保护路径需要有效令牌
    cycle::auth::User user;
    user.username = "testuser";
    user.permissions = {"tiles:read"};
    
    std::string token = auth->GenerateToken(user);
    
    EXPECT_TRUE(middleware->ProcessRequest(token, "/tiles/0/0/0/png", "GET"));
    EXPECT_FALSE(middleware->ProcessRequest("", "/tiles/0/0/0/png", "GET"));
}

TEST_F(AuthMiddlewareTest, PermissionBasedAccess) {
    cycle::auth::User user;
    user.username = "viewer";
    user.permissions = {"tiles:read"}; // 只有读取权限
    
    std::string token = auth->GenerateToken(user);
    
    // 应该能够访问需要tiles:read权限的路径
    EXPECT_TRUE(middleware->ProcessRequest(token, "/tiles/0/0/0/png", "GET"));
    
    // 不应该能够访问需要更高权限的路径
    EXPECT_FALSE(middleware->ProcessRequest(token, "/admin/users", "GET"));
}

TEST_F(AuthMiddlewareTest, PathPatternMatching) {
    // 测试通配符路径匹配
    middleware->AddProtectedPath("/api/*", "api:access");
    
    EXPECT_TRUE(middleware->RequireAuthentication("/api/tiles"));
    EXPECT_TRUE(middleware->RequireAuthentication("/api/maps"));
    EXPECT_TRUE(middleware->RequireAuthentication("/api/admin/users"));
    
    // 不匹配的路径
    EXPECT_TRUE(middleware->RequireAuthentication("/api")); // 默认需要认证
}

TEST_F(AuthMiddlewareTest, RequiredPermissionLookup) {
    std::string permission = middleware->GetRequiredPermission("/tiles/0/0/0/png", "GET");
    EXPECT_EQ(permission, "tiles:read");
    
    permission = middleware->GetRequiredPermission("/map", "POST");
    EXPECT_EQ(permission, "maps:generate");
    
    permission = middleware->GetRequiredPermission("/health", "GET");
    EXPECT_TRUE(permission.empty()); // 公开路径不需要权限
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    
    std::cout << "Running JWT Authentication Tests..." << std::endl;
    std::cout << "====================================" << std::endl;
    
    int result = RUN_ALL_TESTS();
    
    std::cout << "====================================" << std::endl;
    std::cout << "JWT Authentication Tests Completed" << std::endl;
    
    return result;
}