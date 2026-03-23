#pragma once

#include <vector>
#include <string>
#include <array>
#include <openssl/aes.h>

constexpr size_t KEY_SIZE = 32;
constexpr size_t IV_SIZE = 16;
constexpr size_t SALT_SIZE = 16;
constexpr size_t HMAC_SIZE = 32;
constexpr int PBKDF2_ITERATIONS = 100000;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class AESKey
 * @brief Manages AES encryption keys with secure PBKDF2 key derivation.
 *
 * Stores a 256-bit (32-byte) key for AES-256 encryption.
 * Uses PBKDF2-HMAC-SHA256 to derive a secure key from a password and salt.
 */
class AESKey {
public:
    /**
     * @brief Constructs an AESKey using PBKDF2 key derivation.
     * @param password The password string.
     * @param salt The salt array for key derivation.
     *
     * Uses PBKDF2-HMAC-SHA256 with 100,000 iterations to derive
     * a cryptographically secure 256-bit key from the password.
     */
    AESKey(const std::string& password, const std::array<unsigned char, SALT_SIZE>& salt);

    /**
     * @brief Gets the derived key.
     * @return Pointer to the 32-byte derived key.
     */
    const unsigned char* getKey() const;

private:
    unsigned char key[KEY_SIZE];
};

/**
 * @class AESCrypt
 * @brief Handles AES-256-CBC encryption and decryption operations with authentication.
 *
 * Uses OpenSSL's EVP API for cryptographic operations with PKCS7 padding.
 * Provides HMAC-SHA256 authentication to detect tampering.
 * Each encryption operation uses a unique random IV.
 */
class AESCrypt {
public:
    /**
     * @brief Constructs an AESCrypt instance with the given derived key.
     * @param key The AESKey to use for encryption/decryption.
     */
    AESCrypt(const AESKey& key);

    /**
     * @brief Encrypts data using AES-256-CBC with HMAC-SHA256 authentication.
     * @param data The plaintext data to encrypt.
     * @param iv The initialization vector to use (must be unique per encryption).
     * @return std::vector<unsigned char> The encrypted ciphertext (without HMAC).
     * @throws std::runtime_error If encryption fails.
     *
     * @note The IV should be generated randomly for each encryption operation.
     * @note The HMAC should be computed separately and stored with the ciphertext.
     */
    std::vector<unsigned char> enc(const std::vector<unsigned char>& data, unsigned char* iv);

    /**
     * @brief Decrypts data using AES-256-CBC.
     * @param data The ciphertext data to decrypt.
     * @param iv The initialization vector used during encryption.
     * @return std::vector<unsigned char> The decrypted plaintext.
     * @throws std::runtime_error If decryption fails.
     */
    std::vector<unsigned char> dec(const std::vector<unsigned char>& data, unsigned char* iv);

    /**
     * @brief Computes HMAC-SHA256 of data for authentication.
     * @param key The AES key (also used for HMAC).
     * @param data The data to authenticate.
     * @return std::array<unsigned char, HMAC_SIZE> The HMAC tag.
     */
    static std::array<unsigned char, HMAC_SIZE> computeHMAC(const unsigned char* key,
                                                            const std::vector<unsigned char>& data);

    /**
     * @brief Verifies HMAC-SHA256 tag for data integrity.
     * @param key The AES key (also used for HMAC).
     * @param data The data to verify.
     * @param expected_tag The expected HMAC tag.
     * @return true if HMAC matches, false otherwise.
     */
    static bool verifyHMAC(const unsigned char* key,
                          const std::vector<unsigned char>& data,
                          const std::array<unsigned char, HMAC_SIZE>& expected_tag);

private:
    const unsigned char* key_ptr;
};

/**
 * @brief Encrypts a file using AES-256-CBC with PBKDF2 key derivation and HMAC authentication.
 * @param filename Path to the file to encrypt.
 * @param password The password for encryption.
 * @return std::string Path to the encrypted file (original filename + ".enc").
 * @throws std::runtime_error If file operations, key derivation, or encryption fail.
 *
 * File format: [16-byte salt][16-byte IV][ciphertext][32-byte HMAC]
 *
 * Uses PBKDF2-HMAC-SHA256 for key derivation with random salt.
 * Generates a random IV for each encryption.
 * Appends HMAC-SHA256 for integrity verification.
 */
std::string encrypt(const std::string& filename, const std::string& password);

/**
 * @brief Decrypts a file using AES-256-CBC with PBKDF2 key derivation and HMAC verification.
 * @param filename Path to the encrypted file.
 * @param password The password for decryption.
 * @throws std::runtime_error If file operations, key derivation, decryption, or HMAC verification fail.
 *
 * Reads salt and IV from the encrypted file.
 * Derives key using PBKDF2 with the stored salt.
 * Verifies HMAC before decryption to detect tampering.
 * Writes decrypted data to a new file with the ".enc" extension removed.
 */
void decrypt(const std::string& filename, const std::string& password);

#ifdef __cplusplus
}
#endif
