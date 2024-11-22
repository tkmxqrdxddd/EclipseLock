#pragma once

#include <vector>
#include <string>
#include <openssl/aes.h>

#ifdef __cplusplus
extern "C" {
#endif

class AESKey {
public:
    AESKey(const std::string& key_str);
    unsigned char key[32];
};

class AESCrypt {
public:
    AESCrypt(const AESKey& key);
    void setKey(const AESKey& key);
    void setIV(unsigned char* iv);
    unsigned char* getIV();
    std::vector<unsigned char> enc(const std::vector<unsigned char>& data);
    std::vector<unsigned char> dec(const std::vector<unsigned char>& data);

private:
    AESKey key;
    unsigned char iv[AES_BLOCK_SIZE];
};

// Function declarations
std::string encrypt(const std::string& filename, const AESKey& key);
void decrypt(const std::string& filename, const AESKey& key);

#ifdef __cplusplus
}
#endif 