#include "AESCrypt.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <stdexcept>
#include <cstring>
#include <fstream>
#include <algorithm>

AESKey::AESKey(const std::string& password) {
    // Derive a 256-bit key from the password using SHA-256
    std::array<unsigned char, SHA256_DIGEST_LENGTH> hash;
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), 
           password.size(), hash.data());
    
    // Copy the hash to the key array
    std::copy(hash.begin(), hash.begin() + key_.size(), key_.begin());
}

AESCrypt::AESCrypt(const AESKey& key) : key_(key) {
    generateIV();
}

void AESCrypt::setKey(const AESKey& key) {
    key_ = key;
}

void AESCrypt::generateIV() {
    if (RAND_bytes(iv_.data(), static_cast<int>(iv_.size())) != 1) {
        throw std::runtime_error("Failed to generate random IV");
    }
}

void AESCrypt::setIV(const unsigned char* iv) {
    std::copy(iv, iv + AES_BLOCK_SIZE, iv_.begin());
}

const unsigned char* AESCrypt::getIV() const {
    return iv_.data();
}

std::vector<unsigned char> AESCrypt::encrypt(const std::vector<unsigned char>& data) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create encryption context");
    }

    std::vector<unsigned char> ciphertext(data.size() + AES_BLOCK_SIZE);
    int len = 0;
    int ciphertext_len = 0;

    try {
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.getKey().data(), iv_.data()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Encryption initialization failed");
        }

        if (EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), static_cast<int>(data.size())) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Encryption update failed");
        }
        ciphertext_len = len;

        if (EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Encryption finalization failed");
        }
        ciphertext_len += len;

        ciphertext.resize(ciphertext_len);
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

std::vector<unsigned char> AESCrypt::decrypt(const std::vector<unsigned char>& data) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create decryption context");
    }

    std::vector<unsigned char> plaintext(data.size());
    int len = 0;
    int plaintext_len = 0;

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key_.getKey().data(), iv_.data()) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption initialization failed");
    }

    if (EVP_DecryptUpdate(ctx, plaintext.data(), &len, data.data(), static_cast<int>(data.size())) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption update failed");
    }
    plaintext_len = len;

    if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) != 1) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption failed - incorrect key or corrupted file");
    }
    plaintext_len += len;

    plaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

std::string encryptFile(const std::string& filename, const AESKey& key) {
    // Open input file
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Failed to open file for encryption: " + filename);
    }

    // Read file contents
    std::vector<unsigned char> data(
        (std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>()
    );
    inputFile.close();

    // Encrypt data
    AESCrypt aesCrypt(key);
    std::vector<unsigned char> encryptedData = aesCrypt.encrypt(data);

    // Write IV + encrypted data to output file
    std::string outputFilename = filename + ".enc";
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Failed to create encrypted file: " + outputFilename);
    }

    // Write IV first (so it can be read during decryption)
    outputFile.write(reinterpret_cast<const char*>(aesCrypt.getIV()), AES_BLOCK_SIZE);
    outputFile.write(reinterpret_cast<const char*>(encryptedData.data()), 
                     static_cast<std::streamsize>(encryptedData.size()));
    outputFile.close();

    return outputFilename;
}

void decryptFile(const std::string& filename, const AESKey& key) {
    // Open encrypted file
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Failed to open file for decryption: " + filename);
    }

    // Read IV from the beginning of the file
    std::array<unsigned char, AES_BLOCK_SIZE> iv;
    inputFile.read(reinterpret_cast<char*>(iv.data()), AES_BLOCK_SIZE);
    if (!inputFile) {
        throw std::runtime_error("Invalid encrypted file format: missing IV");
    }

    // Read encrypted data
    std::vector<unsigned char> data(
        (std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>()
    );
    inputFile.close();

    // Decrypt data
    AESCrypt aesCrypt(key);
    aesCrypt.setIV(iv.data());
    std::vector<unsigned char> decryptedData = aesCrypt.decrypt(data);

    // Determine output filename (remove .enc extension)
    std::string outputFilename;
    const std::string encExtension = ".enc";
    if (filename.size() >= encExtension.size() &&
        filename.compare(filename.size() - encExtension.size(), encExtension.size(), encExtension) == 0) {
        outputFilename = filename.substr(0, filename.size() - encExtension.size());
    } else {
        outputFilename = filename + ".decrypted";
    }

    // Write decrypted data
    std::ofstream outputFile(outputFilename, std::ios::binary);
    if (!outputFile) {
        throw std::runtime_error("Failed to create decrypted file: " + outputFilename);
    }
    outputFile.write(reinterpret_cast<const char*>(decryptedData.data()), 
                     static_cast<std::streamsize>(decryptedData.size()));
    outputFile.close();
}
