#pragma once

#include <vector>
#include <string>
#include <openssl/aes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class AESKey
 * @brief Manages AES encryption keys with secure initialization.
 *
 * Stores a 256-bit (32-byte) key for AES-256 encryption.
 * The key is zero-initialized and populated from a string.
 */
class AESKey {
public:
    /**
     * @brief Constructs an AESKey from a string.
     * @param key_str The encryption key as a string.
     *
     * The key is zero-padded or truncated to fit 32 bytes.
     */
    AESKey(const std::string& key_str);
    unsigned char key[32];  ///< The 256-bit encryption key
};

/**
 * @class AESCrypt
 * @brief Handles AES-256-CBC encryption and decryption operations.
 *
 * Uses OpenSSL's EVP API for cryptographic operations with PKCS7 padding.
 * Each instance manages its own initialization vector (IV) for secure encryption.
 */
class AESCrypt {
public:
    /**
     * @brief Constructs an AESCrypt instance with the given key.
     * @param key The AESKey to use for encryption/decryption.
     *
     * Initializes the IV to zero. Use setIV() to set a random IV before encryption.
     */
    AESCrypt(const AESKey& key);

    /**
     * @brief Sets the encryption key.
     * @param key The AESKey to use.
     */
    void setKey(const AESKey& key);

    /**
     * @brief Sets the initialization vector (IV).
     * @param iv Pointer to a 16-byte IV array.
     *
     * The IV must be unique for each encryption operation with the same key.
     */
    void setIV(unsigned char* iv);

    /**
     * @brief Gets the current initialization vector.
     * @return Pointer to the 16-byte IV array.
     */
    unsigned char* getIV();

    /**
     * @brief Encrypts data using AES-256-CBC.
     * @param data The plaintext data to encrypt.
     * @return std::vector<unsigned char> The encrypted ciphertext.
     * @throws std::runtime_error If encryption fails.
     */
    std::vector<unsigned char> enc(const std::vector<unsigned char>& data);

    /**
     * @brief Decrypts data using AES-256-CBC.
     * @param data The ciphertext data to decrypt.
     * @return std::vector<unsigned char> The decrypted plaintext.
     * @throws std::runtime_error If decryption fails.
     */
    std::vector<unsigned char> dec(const std::vector<unsigned char>& data);

private:
    AESKey key;                    ///< The encryption key
    unsigned char iv[AES_BLOCK_SIZE];  ///< The initialization vector (16 bytes)
};

/**
 * @brief Encrypts a file using AES-256-CBC.
 * @param filename Path to the file to encrypt.
 * @param key The encryption key.
 * @return std::string Path to the encrypted file (original filename + ".enc").
 * @throws std::runtime_error If file operations or encryption fail.
 *
 * Reads the entire file into memory, encrypts it, and writes to a new file
 * with the ".enc" extension appended.
 */
std::string encrypt(const std::string& filename, const AESKey& key);

/**
 * @brief Decrypts a file using AES-256-CBC.
 * @param filename Path to the encrypted file.
 * @param key The decryption key.
 * @throws std::runtime_error If file operations or decryption fail.
 *
 * Reads the encrypted file, decrypts it, and writes to a new file
 * with the ".enc" extension removed.
 */
void decrypt(const std::string& filename, const AESKey& key);

#ifdef __cplusplus
}
#endif 