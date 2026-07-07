#pragma once

#include <memory>
#include <vector>
#include <string>
#include <array>

constexpr size_t KEY_SIZE = 32;
constexpr size_t IV_SIZE = 16;
constexpr size_t SALT_SIZE = 16;
constexpr size_t HMAC_SIZE = 32;
constexpr int PBKDF2_ITERATIONS = 100000;

class AESKey {
public:
    AESKey(const std::string& password, const std::array<unsigned char, SALT_SIZE>& salt);
    ~AESKey();

    AESKey(const AESKey&) = delete;
    AESKey& operator=(const AESKey&) = delete;
    AESKey(AESKey&&) = delete;
    AESKey& operator=(AESKey&&) = delete;

    const unsigned char* getKey() const;

private:
    unsigned char key[KEY_SIZE];
};

class AESCrypt {
public:
    explicit AESCrypt(std::shared_ptr<const AESKey> key);

    std::vector<unsigned char> enc(const std::vector<unsigned char>& data, unsigned char* iv);
    std::vector<unsigned char> dec(const std::vector<unsigned char>& data, unsigned char* iv);

    static std::array<unsigned char, HMAC_SIZE> computeHMAC(const unsigned char* key,
                                                            const std::vector<unsigned char>& data);

    static bool verifyHMAC(const unsigned char* key,
                          const std::vector<unsigned char>& data,
                          const std::array<unsigned char, HMAC_SIZE>& expected_tag);

private:
    std::shared_ptr<const AESKey> m_key;
};

std::string encrypt(const std::string& filename, const std::string& password);
void decrypt(const std::string& filename, const std::string& password);
