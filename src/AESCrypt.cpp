#include "AESCrypt.h"
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <stdexcept>
#include <cstring>
#include <fstream>

/**
 * AESKey Constructor
 *
 * Initializes the key array to zero, then copies the input string.
 * Uses memset for zero-initialization and strncpy for safe copying.
 * If key_str is shorter than 32 bytes, remaining bytes stay zero.
 * If longer, the key is truncated to 32 bytes.
 */
AESKey::AESKey(const std::string& key_str) {
    // Zero-initialize the entire key array for security
    memset(this->key, 0, sizeof(this->key));
    // Copy the key string, truncating if necessary
    strncpy((char*)this->key, key_str.c_str(), sizeof(this->key));
}

/**
 * AESCrypt Constructor
 *
 * Initializes the IV to zero. The IV should be set to a random value
 * before encryption using setIV() for secure operation.
 */
AESCrypt::AESCrypt(const AESKey& key) : key(key) {
    // Zero-initialize the IV
    memset(iv, 0, AES_BLOCK_SIZE);
}

/**
 * Sets the initialization vector for encryption/decryption.
 * Copies 16 bytes (AES_BLOCK_SIZE) from the provided IV.
 */
void AESCrypt::setIV(unsigned char* iv) {
    memcpy(this->iv, iv, AES_BLOCK_SIZE);
}

/**
 * Returns a pointer to the current IV.
 * Used to retrieve the randomly generated IV for storage with encrypted data.
 */
unsigned char* AESCrypt::getIV() {
    return iv;
}

/**
 * Encrypts data using AES-256-CBC mode.
 *
 * Uses OpenSSL's EVP API for high-level cryptographic operations.
 * The output buffer is pre-allocated with extra space for padding.
 *
 * @param data The plaintext data to encrypt
 * @return Encrypted ciphertext as a byte vector
 * @throws std::runtime_error on encryption failure
 */
std::vector<unsigned char> AESCrypt::enc(const std::vector<unsigned char>& data) {
    // Create a new cipher context for encryption
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    // Allocate ciphertext buffer with extra space for PKCS7 padding
    std::vector<unsigned char> ciphertext(data.size() + AES_BLOCK_SIZE);
    int len;

    // Initialize encryption operation with AES-256-CBC
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption initialization failed");
    }

    // Encrypt the data
    if (!EVP_EncryptUpdate(ctx, ciphertext.data(), &len, data.data(), data.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption update failed");
    }

    int ciphertext_len = len;

    // Finalize encryption, handling PKCS7 padding
    if (!EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Encryption finalization failed");
    }

    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);
    EVP_CIPHER_CTX_free(ctx);
    return ciphertext;
}

/**
 * Decrypts data using AES-256-CBC mode.
 *
 * Uses OpenSSL's EVP API for high-level cryptographic operations.
 * The output buffer is pre-allocated to match input size.
 *
 * @param data The ciphertext data to decrypt
 * @return Decrypted plaintext as a byte vector
 * @throws std::runtime_error on decryption failure
 */
std::vector<unsigned char> AESCrypt::dec(const std::vector<unsigned char>& data) {
    // Create a new cipher context for decryption
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    // Allocate plaintext buffer
    std::vector<unsigned char> plaintext(data.size());
    int len;

    // Initialize decryption operation with AES-256-CBC
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.key, iv)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption initialization failed");
    }

    // Decrypt the data
    if (!EVP_DecryptUpdate(ctx, plaintext.data(), &len, data.data(), data.size())) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption update failed");
    }

    int plaintext_len = len;

    // Finalize decryption, removing PKCS7 padding
    if (!EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        throw std::runtime_error("Decryption finalization failed");
    }

    plaintext_len += len;
    plaintext.resize(plaintext_len);
    EVP_CIPHER_CTX_free(ctx);
    return plaintext;
}

/**
 * Encrypts a file and saves it with a ".enc" extension.
 *
 * Reads the entire file into memory, encrypts it using AES-256-CBC,
 * and writes the encrypted data to a new file.
 *
 * @param filename Path to the input file
 * @param key The encryption key
 * @return Path to the encrypted file
 * @throws std::runtime_error on file I/O or encryption errors
 */
std::string encrypt(const std::string& filename, const AESKey& key) {
    // Open input file in binary mode
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Failed to open file for encryption");
    }

    // Read entire file into memory
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    // Encrypt the data
    AESCrypt aesCrypt(key);
    std::vector<unsigned char> encryptedData = aesCrypt.enc(data);

    // Write encrypted data to output file with .enc extension
    std::ofstream outputFile(filename + ".enc", std::ios::binary);
    outputFile.write(reinterpret_cast<const char*>(encryptedData.data()), encryptedData.size());
    outputFile.close();

    return filename + ".enc"; // Return the name of the encrypted file
}


/**
 * Decrypts a file and saves it with the ".enc" extension removed.
 *
 * Reads the encrypted file into memory, decrypts it using AES-256-CBC,
 * and writes the decrypted data to a new file.
 *
 * @param filename Path to the encrypted file (should end with .enc)
 * @param key The decryption key
 * @throws std::runtime_error on file I/O or decryption errors
 */
void decrypt(const std::string& filename, const AESKey& key) {
    // Open input file in binary mode
    std::ifstream inputFile(filename, std::ios::binary);
    if (!inputFile) {
        throw std::runtime_error("Failed to open file for decryption");
    }

    // Read entire file into memory
    std::vector<unsigned char> data((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
    inputFile.close();

    // Decrypt the data
    AESCrypt aesCrypt(key);
    std::vector<unsigned char> decryptedData = aesCrypt.dec(data);

    // Generate output filename by removing the last extension
    std::string outputFilename = filename.substr(0, filename.find_last_of('.'));
    // Write decrypted data to output file
    std::ofstream outputFile(outputFilename, std::ios::binary);
    outputFile.write(reinterpret_cast<const char*>(decryptedData.data()), decryptedData.size());
    outputFile.close();
}