#include "AESCrypt.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <stdexcept>
#include <cstring>
#include <fstream>

AESKey::AESKey(const std::string& key_str) {
    memset(this->key, 0, sizeof(this->key));
    strncpy((char*)this->key, key_str.c_str(), sizeof(this->key));
}

AESCrypt::AESCrypt(const AESKey& key) : key(key) {
    memset(iv, 0, AES_BLOCK_SIZE);
}

void AESCrypt::setIV(unsigned char* iv) {
    memcpy(this->iv, iv, AES_BLOCK_SIZE);
}

unsigned char* AESCrypt::getIV() {
    return iv;
}

std::vector<unsigned char> AESCrypt::enc(const std::vector<unsigned char>& data) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> ciphertext(data.size() + AES_BLOCK_SIZE);
    int len;

    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption initialization failed");
    }

    if (!EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption update failed");
    }

    int ciphertext_len = len;

    if (!EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption finalization failed");
    }

    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

std::vector<unsigned char> AESCrypt::dec(const std::vector<unsigned char>& data) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> plaintext(data.size());
    int len;

    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption initialization failed");
    }

    if (!EVP_DecryptUpdate(ctx, plaintext.data(), &len, data.data(), data.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption update failed");
    }

    int plaintext_len = len;

    if (!EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption finalization failed");
    }

    plaintext_len += len;
    plaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

// Function implementations for encrypt and decrypt file operations
std::string encrypt(const std::string& filename, const AESKey& key) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Failed to open file for encryption");
    }

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    AESCrypt aesCrypt(key);
    std::vector<unsigned char> encryptedData = aesCrypt.enc(data);

    std::ofstream outputFile(filename + ".enc", std::ios::binary);
    outputFile.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
    outputFile.close();

    return filename + ".enc"; // Return the name of the encrypted file
}


void decrypt(const std::string& filename, const AESKey& key) {
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Failed to open file for decryption");
    }

    std::vector<unsigned char> data((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    AESCrypt aesCrypt(key);
    std::vector<unsigned char> decryptedData = aesCrypt.dec(data);

    std::string outputFilename = filename.substr(0, filename.find_last_of('.'));
    std::ofstream outputFile(outputFilename, std::ios::binary);
    outputFile.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
    outputFile.close();
}