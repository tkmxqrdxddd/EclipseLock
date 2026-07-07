#include "AESCrypt.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/crypto.h>
#include <stdexcept>
#include <cstring>
#include <fstream>

AESKey::AESKey(const std::string& password, const std::array<unsigned char, SALT_SIZE>& salt) {
    if (PKCS5_PBKDF2_HMAC(
            password.c_str(),
            password.size(),
            salt.data(),
            salt.size(),
            PBKDF2_ITERATIONS,
            EVP_sha256(),
            KEY_SIZE,
            key
        ) != 1) {
        throw std::runtime_error("PBKDF2 key derivation failed");
    }
}

AESKey::~AESKey() {
    OPENSSL_cleanse(key, KEY_SIZE);
}

const unsigned char* AESKey::getKey() const {
    return key;
}

AESCrypt::AESCrypt(std::shared_ptr<const AESKey> key) : m_key(std::move(key)) {
}

namespace {
    struct CipherCtxDeleter {
        void operator()(EVP_CIPHER_CTX* ctx) const noexcept {
            EVP_CIPHER_CTX_free(ctx);
        }
    };
    using CipherCtxPtr = std::unique_ptr<EVP_CIPHER_CTX, CipherCtxDeleter>;
}

std::vector<unsigned char> AESCrypt::enc(const std::vector<unsigned char>& data, unsigned char* iv) {
    CipherCtxPtr ctx(EVP_CIPHER_CTX_new());
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    if (!EVP_EncryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, m_key->getKey(), iv)) {
        throw std::runtime_error("Encryption initialization failed");
    }

    std::vector<unsigned char> ciphertext(data.size() + EVP_CIPHER_CTX_block_size(ctx.get()));
    int len;

    if (!EVP_EncryptUpdate(ctx.get(), ciphertext.data(), &len, data.data(), data.size())) {
        throw std::runtime_error("Encryption update failed");
    }

    int ciphertext_len = len;

    if (!EVP_EncryptFinal_ex(ctx.get(), ciphertext.data() + len, &len)) {
        throw std::runtime_error("Encryption finalization failed");
    }

    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);
    return ciphertext;
}

std::vector<unsigned char> AESCrypt::dec(const std::vector<unsigned char>& data, unsigned char* iv) {
    CipherCtxPtr ctx(EVP_CIPHER_CTX_new());
    if (!ctx) {
        throw std::runtime_error("Failed to create cipher context");
    }

    std::vector<unsigned char> plaintext(data.size());
    int len;

    if (!EVP_DecryptInit_ex(ctx.get(), EVP_aes_256_cbc(), NULL, m_key->getKey(), iv)) {
        throw std::runtime_error("Decryption initialization failed");
    }

    if (!EVP_DecryptUpdate(ctx.get(), plaintext.data(), &len, data.data(), data.size())) {
        throw std::runtime_error("Decryption update failed");
    }

    int plaintext_len = len;

    if (!EVP_DecryptFinal_ex(ctx.get(), plaintext.data() + len, &len)) {
        throw std::runtime_error("Decryption finalization failed");
    }

    plaintext_len += len;
    plaintext.resize(plaintext_len);
    return plaintext;
}

std::array<unsigned char, HMAC_SIZE> AESCrypt::computeHMAC(const unsigned char* key,
                                                           const std::vector<unsigned char>& data) {
    std::array<unsigned char, HMAC_SIZE> hmac_result;
    unsigned int hmac_len = 0;

    if (HMAC(EVP_sha256(),
             key, KEY_SIZE,
             data.data(), data.size(),
             hmac_result.data(),
             &hmac_len) == nullptr) {
        throw std::runtime_error("HMAC computation failed");
    }

    return hmac_result;
}

bool AESCrypt::verifyHMAC(const unsigned char* key,
                          const std::vector<unsigned char>& data,
                          const std::array<unsigned char, HMAC_SIZE>& expected_tag) {
    auto computed_tag = computeHMAC(key, data);
    return CRYPTO_memcmp(computed_tag.data(), expected_tag.data(), HMAC_SIZE) == 0;
}

static std::vector<unsigned char> readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    return std::vector<unsigned char>(
        (std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>()
    );
}

static void writeFile(const std::string& path, const std::vector<unsigned char>& data) {
    std::ofstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to create file: " + path);
    }
    if (!file.write(reinterpret_cast<const char*>(data.data()), data.size())) {
        throw std::runtime_error("Failed to write file: " + path);
    }
}

std::string encrypt(const std::string& filename, const std::string& password) {
    std::array<unsigned char, SALT_SIZE> salt;
    if (RAND_bytes(salt.data(), salt.size()) != 1) {
        throw std::runtime_error("Failed to generate random salt");
    }

    auto aesKey = std::make_shared<const AESKey>(password, salt);

    std::array<unsigned char, IV_SIZE> iv;
    if (RAND_bytes(iv.data(), iv.size()) != 1) {
        throw std::runtime_error("Failed to generate random IV");
    }

    std::vector<unsigned char> data = readFile(filename);

    AESCrypt aesCrypt(aesKey);
    std::vector<unsigned char> ciphertext = aesCrypt.enc(data, iv.data());

    auto hmac = AESCrypt::computeHMAC(aesKey->getKey(), ciphertext);

    std::string outputPath = filename + ".enc";

    std::ofstream outputFile(outputPath, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Failed to create encrypted file: " + outputPath);
    }

    if (!outputFile.write(reinterpret_cast<const char*>(salt.data()), salt.size()) ||
        !outputFile.write(reinterpret_cast<const char*>(iv.data()), iv.size()) ||
        !outputFile.write(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size()) ||
        !outputFile.write(reinterpret_cast<const char*>(hmac.data()), hmac.size())) {
        outputFile.close();
        std::remove(outputPath.c_str());
        throw std::runtime_error("Failed to write encrypted data to: " + outputPath);
    }

    outputFile.close();
    return outputPath;
}

void decrypt(const std::string& filename, const std::string& password) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Failed to open file for decryption: " + filename);
    }

    inputFile.seekg(0, std::ios::end);
    size_t fileSize = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    if (fileSize < (SALT_SIZE + IV_SIZE + HMAC_SIZE)) {
        throw std::runtime_error("Invalid encrypted file format: file too small");
    }

    std::array<unsigned char, SALT_SIZE> salt;
    if (!inputFile.read(reinterpret_cast<char*>(salt.data()), salt.size())) {
        throw std::runtime_error("Failed to read salt from encrypted file");
    }

    std::array<unsigned char, IV_SIZE> iv;
    if (!inputFile.read(reinterpret_cast<char*>(iv.data()), iv.size())) {
        throw std::runtime_error("Failed to read IV from encrypted file");
    }

    size_t cryptoDataSize = fileSize - SALT_SIZE - IV_SIZE - HMAC_SIZE;
    std::vector<unsigned char> ciphertext(cryptoDataSize);
    if (!inputFile.read(reinterpret_cast<char*>(ciphertext.data()), cryptoDataSize)) {
        throw std::runtime_error("Failed to read ciphertext from encrypted file");
    }

    std::array<unsigned char, HMAC_SIZE> storedHmac;
    if (!inputFile.read(reinterpret_cast<char*>(storedHmac.data()), storedHmac.size())) {
        throw std::runtime_error("Failed to read HMAC from encrypted file");
    }
    inputFile.close();

    auto aesKey = std::make_shared<const AESKey>(password, salt);

    if (!AESCrypt::verifyHMAC(aesKey->getKey(), ciphertext, storedHmac)) {
        throw std::runtime_error("HMAC verification failed: file may be tampered or corrupted");
    }

    AESCrypt aesCrypt(aesKey);
    std::vector<unsigned char> plaintext = aesCrypt.dec(ciphertext, iv.data());

    std::string outputFilename;
    size_t encPos = filename.rfind(".enc");
    if (encPos != std::string::npos) {
        outputFilename = filename.substr(0, encPos);
    } else {
        outputFilename = filename + ".decrypted";
    }

    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Failed to create decrypted file: " + outputFilename);
    }

    if (!outputFile.write(reinterpret_cast<const char*>(plaintext.data()), plaintext.size())) {
        outputFile.close();
        std::remove(outputFilename.c_str());
        throw std::runtime_error("Failed to write decrypted data to: " + outputFilename);
    }

    outputFile.close();
}
