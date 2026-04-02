#include <gtest/gtest.h>
#include "ogc/draw/data_encryption.h"
#include <fstream>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#undef GetFreeSpace
#else
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#endif

using namespace ogc::draw;

namespace {

void DeleteDirectoryRecursive(const std::string& path) {
#ifdef _WIN32
    WIN32_FIND_DATAA findData;
    std::string searchPath = path + "\\*";
    HANDLE hFind = FindFirstFileA(searchPath.c_str(), &findData);
    
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, ".") != 0 && 
                strcmp(findData.cFileName, "..") != 0) {
                std::string fullPath = path + "\\" + findData.cFileName;
                if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    DeleteDirectoryRecursive(fullPath);
                } else {
                    _unlink(fullPath.c_str());
                }
            }
        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
    _rmdir(path.c_str());
#else
    DIR* dir = opendir(path.c_str());
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                std::string fullPath = path + "/" + entry->d_name;
                struct stat statBuf;
                if (stat(fullPath.c_str(), &statBuf) == 0) {
                    if (S_ISDIR(statBuf.st_mode)) {
                        DeleteDirectoryRecursive(fullPath);
                    } else {
                        unlink(fullPath.c_str());
                    }
                }
            }
        }
        closedir(dir);
    }
    rmdir(path.c_str());
#endif
}

}

class DataEncryptionTest : public ::testing::Test {
protected:
    void SetUp() override {
        m_testPath = "./test_encryption";
        DeleteDirectoryRecursive(m_testPath);
        
        encryption = DataEncryption::Create();
    }
    
    void TearDown() override {
        encryption.reset();
        DeleteDirectoryRecursive(m_testPath);
    }
    
    std::unique_ptr<DataEncryption> encryption;
    std::string m_testPath;
};

TEST_F(DataEncryptionTest, CreateEncryption) {
    ASSERT_NE(encryption, nullptr);
}

TEST_F(DataEncryptionTest, Initialize) {
    EXPECT_TRUE(encryption->Initialize(m_testPath));
    EXPECT_TRUE(encryption->IsInitialized());
}

TEST_F(DataEncryptionTest, GenerateKey) {
    EncryptionKey key = encryption->GenerateKey();
    
    EXPECT_TRUE(key.IsValid());
    EXPECT_EQ(key.key.size(), 32u);
    EXPECT_EQ(key.iv.size(), 16u);
    EXPECT_FALSE(key.keyId.empty());
}

TEST_F(DataEncryptionTest, GenerateKeyWithId) {
    EncryptionKey key = encryption->GenerateKey("test_key_id");
    
    EXPECT_TRUE(key.IsValid());
    EXPECT_EQ(key.keyId, "test_key_id");
}

TEST_F(DataEncryptionTest, SetActiveKey) {
    EncryptionKey key = encryption->GenerateKey();
    
    EXPECT_TRUE(encryption->SetActiveKey(key));
    
    EncryptionKey activeKey = encryption->GetActiveKey();
    EXPECT_EQ(activeKey.keyId, key.keyId);
}

TEST_F(DataEncryptionTest, StoreAndLoadKey) {
    encryption->Initialize(m_testPath);
    
    EncryptionKey key = encryption->GenerateKey("stored_key");
    EXPECT_TRUE(encryption->StoreKey(key, "password"));
    
    EXPECT_TRUE(encryption->HasKey("stored_key"));
    
    EncryptionKey loadedKey = encryption->LoadKey("stored_key", "password");
    EXPECT_TRUE(loadedKey.IsValid());
    EXPECT_EQ(loadedKey.keyId, "stored_key");
}

TEST_F(DataEncryptionTest, DeleteKey) {
    encryption->Initialize(m_testPath);
    
    EncryptionKey key = encryption->GenerateKey("delete_key");
    encryption->StoreKey(key, "password");
    
    EXPECT_TRUE(encryption->HasKey("delete_key"));
    
    EXPECT_TRUE(encryption->DeleteKey("delete_key"));
    EXPECT_FALSE(encryption->HasKey("delete_key"));
}

TEST_F(DataEncryptionTest, EncryptDecryptData) {
    encryption->Initialize(m_testPath);
    
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    
    std::vector<uint8_t> encrypted = encryption->Encrypt(data);
    EXPECT_FALSE(encrypted.empty());
    EXPECT_NE(encrypted, data);
    
    std::vector<uint8_t> decrypted = encryption->Decrypt(encrypted);
    EXPECT_EQ(decrypted, data);
}

TEST_F(DataEncryptionTest, EncryptDecryptString) {
    encryption->Initialize(m_testPath);
    
    std::string text = "Hello, World!";
    
    std::vector<uint8_t> textData(text.begin(), text.end());
    std::vector<uint8_t> encrypted = encryption->Encrypt(textData);
    EXPECT_FALSE(encrypted.empty());
    
    std::vector<uint8_t> decrypted = encryption->Decrypt(encrypted);
    EXPECT_EQ(decrypted.size(), textData.size());
    EXPECT_EQ(decrypted, textData);
    
    std::string encryptedStr = encryption->EncryptString(text);
    EXPECT_FALSE(encryptedStr.empty());
    EXPECT_NE(encryptedStr, text);
    
    std::string decryptedStr = encryption->DecryptString(encryptedStr);
    EXPECT_EQ(decryptedStr, text);
}

TEST_F(DataEncryptionTest, EncryptDecryptEmptyData) {
    encryption->Initialize(m_testPath);
    
    std::vector<uint8_t> data;
    
    std::vector<uint8_t> encrypted = encryption->Encrypt(data);
    EXPECT_TRUE(encrypted.empty());
}

TEST_F(DataEncryptionTest, EncryptDecryptWithKey) {
    encryption->Initialize(m_testPath);
    
    EncryptionKey key = encryption->GenerateKey();
    std::vector<uint8_t> data = {10, 20, 30, 40, 50};
    
    std::vector<uint8_t> encrypted = encryption->Encrypt(data, key);
    EXPECT_FALSE(encrypted.empty());
    
    std::vector<uint8_t> decrypted = encryption->Decrypt(encrypted, key);
    EXPECT_EQ(decrypted, data);
}

TEST_F(DataEncryptionTest, SetAlgorithm) {
    encryption->SetAlgorithm(EncryptionAlgorithm::kAES256_GCM);
    EXPECT_EQ(encryption->GetAlgorithm(), EncryptionAlgorithm::kAES256_GCM);
    
    encryption->SetAlgorithm(EncryptionAlgorithm::kAES256_CTR);
    EXPECT_EQ(encryption->GetAlgorithm(), EncryptionAlgorithm::kAES256_CTR);
}

TEST_F(DataEncryptionTest, WGS84ToGCJ02) {
    Coordinate wgs84(116.397428, 39.90923);
    
    Coordinate gcj02 = encryption->WGS84ToGCJ02(wgs84);
    
    EXPECT_NE(gcj02.longitude, wgs84.longitude);
    EXPECT_NE(gcj02.latitude, wgs84.latitude);
}

TEST_F(DataEncryptionTest, GCJ02ToWGS84) {
    Coordinate gcj02(116.404, 39.915);
    
    Coordinate wgs84 = encryption->GCJ02ToWGS84(gcj02);
    
    EXPECT_NEAR(wgs84.longitude, 116.397, 0.01);
    EXPECT_NEAR(wgs84.latitude, 39.909, 0.01);
}

TEST_F(DataEncryptionTest, WGS84ToBD09) {
    Coordinate wgs84(116.397428, 39.90923);
    
    Coordinate bd09 = encryption->WGS84ToBD09(wgs84);
    
    EXPECT_NE(bd09.longitude, wgs84.longitude);
    EXPECT_NE(bd09.latitude, wgs84.latitude);
}

TEST_F(DataEncryptionTest, BD09ToWGS84) {
    Coordinate bd09(116.404, 39.915);
    
    Coordinate wgs84 = encryption->BD09ToWGS84(bd09);
    
    EXPECT_NEAR(wgs84.longitude, 116.391, 0.02);
    EXPECT_NEAR(wgs84.latitude, 39.906, 0.02);
}

TEST_F(DataEncryptionTest, GCJ02ToBD09) {
    Coordinate gcj02(116.404, 39.915);
    
    Coordinate bd09 = encryption->GCJ02ToBD09(gcj02);
    
    EXPECT_NE(bd09.longitude, gcj02.longitude);
    EXPECT_NE(bd09.latitude, gcj02.latitude);
}

TEST_F(DataEncryptionTest, BD09ToGCJ02) {
    Coordinate bd09(116.404, 39.915);
    
    Coordinate gcj02 = encryption->BD09ToGCJ02(bd09);
    
    EXPECT_NEAR(gcj02.longitude, 116.398, 0.01);
    EXPECT_NEAR(gcj02.latitude, 39.909, 0.01);
}

TEST_F(DataEncryptionTest, TransformCoordinate) {
    Coordinate wgs84(116.397428, 39.90923);
    
    Coordinate gcj02 = encryption->TransformCoordinate(
        wgs84, CoordinateTransformType::kWGS84, CoordinateTransformType::kGCJ02);
    
    EXPECT_NE(gcj02.longitude, wgs84.longitude);
    
    Coordinate back = encryption->TransformCoordinate(
        gcj02, CoordinateTransformType::kGCJ02, CoordinateTransformType::kWGS84);
    
    EXPECT_NEAR(back.longitude, wgs84.longitude, 0.001);
    EXPECT_NEAR(back.latitude, wgs84.latitude, 0.001);
}

TEST_F(DataEncryptionTest, TransformCoordinates) {
    std::vector<Coordinate> coords = {
        Coordinate(116.397, 39.909),
        Coordinate(116.398, 39.910),
        Coordinate(116.399, 39.911)
    };
    
    std::vector<Coordinate> transformed = encryption->TransformCoordinates(
        coords, CoordinateTransformType::kWGS84, CoordinateTransformType::kGCJ02);
    
    EXPECT_EQ(transformed.size(), coords.size());
    for (size_t i = 0; i < coords.size(); ++i) {
        EXPECT_NE(transformed[i].longitude, coords[i].longitude);
    }
}

TEST_F(DataEncryptionTest, CoordinateTransformEnabled) {
    encryption->SetCoordinateTransformEnabled(true);
    EXPECT_TRUE(encryption->IsCoordinateTransformEnabled());
    
    encryption->SetCoordinateTransformEnabled(false);
    EXPECT_FALSE(encryption->IsCoordinateTransformEnabled());
}

TEST_F(DataEncryptionTest, DefaultTransformType) {
    encryption->SetDefaultTransformType(CoordinateTransformType::kGCJ02);
    EXPECT_EQ(encryption->GetDefaultTransformType(), CoordinateTransformType::kGCJ02);
    
    encryption->SetDefaultTransformType(CoordinateTransformType::kBD09);
    EXPECT_EQ(encryption->GetDefaultTransformType(), CoordinateTransformType::kBD09);
}

TEST_F(DataEncryptionTest, Hash) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    std::vector<uint8_t> hash = encryption->Hash(data);
    EXPECT_EQ(hash.size(), 32u);
    
    std::vector<uint8_t> hash2 = encryption->Hash(data);
    EXPECT_EQ(hash, hash2);
}

TEST_F(DataEncryptionTest, HashString) {
    std::string text = "Hello, World!";
    
    std::string hash = encryption->HashString(text);
    EXPECT_EQ(hash.size(), 64u);
    
    std::string hash2 = encryption->HashString(text);
    EXPECT_EQ(hash, hash2);
}

TEST_F(DataEncryptionTest, VerifyHash) {
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    std::vector<uint8_t> hash = encryption->Hash(data);
    
    EXPECT_TRUE(encryption->VerifyHash(data, hash));
    
    std::vector<uint8_t> wrongData = {1, 2, 3, 4, 6};
    EXPECT_FALSE(encryption->VerifyHash(wrongData, hash));
}

TEST_F(DataEncryptionTest, VerifyHashString) {
    std::string text = "Hello, World!";
    std::string hash = encryption->HashString(text);
    
    EXPECT_TRUE(encryption->VerifyHashString(text, hash));
    
    std::string wrongText = "Hello, World?";
    EXPECT_FALSE(encryption->VerifyHashString(wrongText, hash));
}

TEST_F(DataEncryptionTest, DeriveKey) {
    std::string password = "my_password";
    std::vector<uint8_t> salt = encryption->GenerateSalt();
    
    std::vector<uint8_t> key1 = encryption->DeriveKey(password, salt, 1000);
    EXPECT_EQ(key1.size(), 32u);
    
    std::vector<uint8_t> key2 = encryption->DeriveKey(password, salt, 1000);
    EXPECT_EQ(key1, key2);
    
    std::vector<uint8_t> differentSalt = encryption->GenerateSalt();
    std::vector<uint8_t> key3 = encryption->DeriveKey(password, differentSalt, 1000);
    EXPECT_NE(key1, key3);
}

TEST_F(DataEncryptionTest, GenerateSalt) {
    std::vector<uint8_t> salt1 = encryption->GenerateSalt();
    std::vector<uint8_t> salt2 = encryption->GenerateSalt();
    
    EXPECT_EQ(salt1.size(), 16u);
    EXPECT_EQ(salt2.size(), 16u);
    EXPECT_NE(salt1, salt2);
}

TEST_F(DataEncryptionTest, KeyValidity) {
    EncryptionKey key;
    EXPECT_FALSE(key.IsValid());
    
    key.key.resize(32);
    EXPECT_TRUE(key.IsValid());
    
    key.key.resize(16);
    EXPECT_FALSE(key.IsValid());
}

TEST_F(DataEncryptionTest, KeyExpiration) {
    EncryptionKey key = encryption->GenerateKey();
    EXPECT_FALSE(key.IsExpired());
    
    key.expiresAt = std::chrono::system_clock::now() - std::chrono::hours(1);
    EXPECT_TRUE(key.IsExpired());
}

TEST_F(DataEncryptionTest, CoordinateDefaultConstructor) {
    Coordinate coord;
    EXPECT_DOUBLE_EQ(coord.longitude, 0.0);
    EXPECT_DOUBLE_EQ(coord.latitude, 0.0);
}

TEST_F(DataEncryptionTest, CoordinateParameterizedConstructor) {
    Coordinate coord(116.397, 39.909);
    EXPECT_DOUBLE_EQ(coord.longitude, 116.397);
    EXPECT_DOUBLE_EQ(coord.latitude, 39.909);
}

TEST_F(DataEncryptionTest, OutOfChina) {
    Coordinate outside(0.0, 0.0);
    Coordinate result = encryption->WGS84ToGCJ02(outside);
    
    EXPECT_DOUBLE_EQ(result.longitude, outside.longitude);
    EXPECT_DOUBLE_EQ(result.latitude, outside.latitude);
}

TEST_F(DataEncryptionTest, MultipleEncryptDecrypt) {
    encryption->Initialize(m_testPath);
    
    for (int i = 0; i < 10; ++i) {
        std::vector<uint8_t> data;
        for (int j = 0; j < 100; ++j) {
            data.push_back(static_cast<uint8_t>((i + j) % 256));
        }
        
        std::vector<uint8_t> encrypted = encryption->Encrypt(data);
        std::vector<uint8_t> decrypted = encryption->Decrypt(encrypted);
        
        EXPECT_EQ(decrypted, data);
    }
}

TEST_F(DataEncryptionTest, DifferentKeysDifferentEncryption) {
    encryption->Initialize(m_testPath);
    
    EncryptionKey key1 = encryption->GenerateKey();
    EncryptionKey key2 = encryption->GenerateKey();
    
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    
    std::vector<uint8_t> encrypted1 = encryption->Encrypt(data, key1);
    std::vector<uint8_t> encrypted2 = encryption->Encrypt(data, key2);
    
    EXPECT_NE(encrypted1, encrypted2);
}
