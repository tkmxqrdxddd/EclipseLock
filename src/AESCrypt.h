#pragma once

#include <vector>
#include <string>
#include <array>
#include <cstdint>
#include <openssl/aes.h>

/**
 * @brief AES-256 encryption key class
 * 
 * Derives a 256-bit key from a password string using SHA-256 hashing
 */
class AESKey {
public:
    explicit AESKey(const std::string& password);
    
    const std::array<unsigned char, 32>& getKey() const { return key_; }

private:
    std::array<unsigned char, 32> key_;
};

/**
 * @brief AES-256-CBC encryption/decryption class
 * 
 * Handles encryption and decryption of data using AES-256 in CBC mode.
 * Stores and manages the initialization vector (IV) for cryptographic operations.
 */
class AESCrypt {
public:
    explicit AESCrypt(const AESKey& key);
    
    /**
     * @brief Set a new encryption key
     */
    void setKey(const AESKey& key);
    
    /**
     * @brief Generate a random IV
     */
    void generateIV();
    
    /**
     * @brief Set the IV from external data
     * @param iv Pointer to AES_BLOCK_SIZE bytes
     */
    void setIV(const unsigned char* iv);
    
    /**
     * @brief Get the current IV
     * @return Pointer to the IV data
     */
    const unsigned char* getIV() const;
    
    /**
     * @brief Encrypt data
     * @param data Plain data to encrypt
     * @return Encrypted ciphertext
     */
    std::vector<unsigned char> encrypt(const std::vector<unsigned char>& data);
    
    /**
     * @brief Decrypt data
     * @param data Encrypted ciphertext to decrypt
     * @return Decrypted plain data
     */
    std::vector<unsigned char> decrypt(const std::vector<unsigned char>& data);

private:
    AESKey key_;
    std::array<unsigned char, AES_BLOCK_SIZE> iv_;
};

/**
 * @brief Encrypt a file using AES-256-CBC
 * 
 * Reads the input file, encrypts its contents, and writes to a .enc file.
 * The IV is prepended to the encrypted file for later decryption.
 * 
 * @param filename Path to the file to encrypt
 * @param key Encryption key
 * @return Path to the encrypted file
 * @throws std::runtime_error on file or encryption errors
 */
std::string encryptFile(const std::string& filename, const AESKey& key);

/**
 * @brief Decrypt a file using AES-256-CBC
 * 
 * Reads the encrypted file (including IV from the beginning),
 * decrypts its contents, and writes to the original filename.
 * 
 * @param filename Path to the .enc file to decrypt
 * @param key Decryption key
 * @throws std::runtime_error on file or decryption errors
 */
void decryptFile(const std::string& filename, const AESKey& key);
