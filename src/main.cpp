#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <getopt.h>

// Define a class for the AES key
class AESKey {
public:
    AESKey(const std::string& key_str) {
        memset(this->key, 0, 32); // Initialize the key array with zeros
        strncpy((char*)this->key, key_str.c_str(), 32); // Copy the key string into the key array
    }

    unsigned char key[32];
};

// Class for AES encryption and decryption
class AESCrypt {
public:
    AESCrypt(const AESKey& key) : key(key) {
        memset(iv, 0, AES_BLOCK_SIZE);
    }

    void setKey(const AESKey& key) {
        this->key = key;
    }

    void setIV(unsigned char* iv) {
        memcpy(this->iv, iv, AES_BLOCK_SIZE);
    }

    unsigned char* getIV() {
        return iv;
    }

    std::vector<unsigned char> enc(const std::vector<unsigned char>& data) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        int len;
        std::vector<unsigned char> encrypted_data(data.size() + EVP_MAX_BLOCK_LENGTH);
        
        if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.key, iv) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error initializing encryption context");
        }
        
        if (EVP_EncryptUpdate(ctx, encrypted_data.data(), &len, data.data(), data.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error encrypting data");
        }
        
        int final_len;
        if (EVP_EncryptFinal_ex(ctx, encrypted_data.data() + len, &final_len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error finalizing encryption");
        }
        
        EVP_CIPHER_CTX_free(ctx);
        encrypted_data.resize(len + final_len);
        return encrypted_data;
    }

    std::vector<unsigned char> dec(const std::vector<unsigned char>& data) {
        EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
        int len;
        std::vector<unsigned char> decrypted_data(data.size() + EVP_MAX_BLOCK_LENGTH);
        
        if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key.key, iv) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error initializing decryption context");
        }
        
        if (EVP_DecryptUpdate(ctx, decrypted_data.data(), &len, data.data(), data.size()) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error decrypting data");
        }
        
        int final_len;
        if (EVP_DecryptFinal_ex(ctx, decrypted_data.data() + len, &final_len) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::runtime_error("Error finalizing decryption");
        }
        
        EVP_CIPHER_CTX_free(ctx);
        decrypted_data.resize(len + final_len);
        return decrypted_data;
    }

private:
    AESKey key;
    unsigned char iv[AES_BLOCK_SIZE];
};

// Define a class for file handling
class FileHandler {
public:
    FileHandler(const std::string& filename, const std::string& mode) {
        input.open(filename, std::ios::binary);
        if (!input.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            exit(1);
        }
    }

    std::ifstream& getInputStream() { return input; }

    std::vector<unsigned char> readData(size_t size) {
        std::vector<unsigned char> data(size);
        input.read((char*)data.data(), size);
        return data;
    }

    unsigned char* getIV(size_t size) {
        unsigned char* iv = new unsigned char[size];
        input.read((char*)iv, size);
        return iv;
    }

private:
    std::ifstream input;
};

// Function to encrypt a file
std::string encrypt(const std::string& filename, const AESKey& key) {
    FileHandler file_handler(filename, "read");
    auto& input = file_handler.getInputStream();
    
    // Get file size
    input.seekg(0, std::ios::end);
    size_t fileSize = input.tellg();
    input.seekg(0, std::ios::beg);
    
    std::vector<unsigned char> buffer = file_handler.readData(fileSize);
    
    // Add PKCS7 padding
    size_t padding = AES_BLOCK_SIZE - (buffer.size() % AES_BLOCK_SIZE);
    buffer.insert(buffer.end(), padding, padding);
    
    AESCrypt aes_crypt(key);
    
    // Generate random IV
    unsigned char iv[AES_BLOCK_SIZE];
    if (RAND_bytes(iv, AES_BLOCK_SIZE) != 1) {
        std::cerr << "Error generating IV" << std::endl;
        exit(1);
    }
    aes_crypt.setIV(iv);
    
    std::vector<unsigned char> encrypted_data = aes_crypt.enc(buffer);

    std::string encrypted_filename = filename + ".enc";
    std::ofstream encrypted_file(encrypted_filename, std::ios::binary);
    if (!encrypted_file.is_open()) {
        std::cerr << "Error opening file: " << encrypted_filename << std::endl;
        exit(1);
    }
    
    // Write IV first
    encrypted_file.write((char*)iv, AES_BLOCK_SIZE);
    // Then write encrypted data
    encrypted_file.write((char*)encrypted_data.data(), encrypted_data.size());
    encrypted_file.close();

    return encrypted_filename;
}

// Function to decrypt a file
void decrypt(const std::string& filename, const AESKey& key) {
    FileHandler file_handler(filename, "read");
    
    // Read IV first
    unsigned char* iv = file_handler.getIV(AES_BLOCK_SIZE);
    
    // Get file size minus IV
    auto& input = file_handler.getInputStream();
    input.seekg(0, std::ios::end);
    size_t fileSize = input.tellg() - AES_BLOCK_SIZE;
    input.seekg(AES_BLOCK_SIZE, std::ios::beg);
    
    // Read encrypted data
    std::vector<unsigned char> buffer = file_handler.readData(fileSize);
    
    // Setup decryption
    AESCrypt aes_crypt(key);
    aes_crypt.setIV(iv);
    
    // Decrypt using the class method
    std::vector<unsigned char> decrypted_data_vec = aes_crypt.dec(buffer);
    
    // Get original filename by removing both .enc extension
    std::string decrypted_filename = filename;
    size_t enc_pos = decrypted_filename.find(".enc");
    if (enc_pos != std::string::npos) {
        decrypted_filename = decrypted_filename.substr(0, enc_pos);
    }
    
    std::ofstream decrypted_file(decrypted_filename, std::ios::binary);
    if (!decrypted_file.is_open()) {
        std::cerr << "Error opening file: " << decrypted_filename << std::endl;
        exit(1);
    }
    
    // Remove PKCS7 padding
    unsigned char padding_value = decrypted_data_vec.back();
    if (padding_value <= AES_BLOCK_SIZE) {
        decrypted_data_vec.resize(decrypted_data_vec.size() - padding_value);
    }
    
    decrypted_file.write((char*)decrypted_data_vec.data(), decrypted_data_vec.size());
    decrypted_file.close();
    
    delete[] iv;
}

// Function to display usage
void usage() {
    std::cout << "Usage: " << std::endl;
    std::cout << "  -e, --encrypt <file>  Encrypt the file" << std::endl;
    std::cout << "  -d, --decrypt <file>  Decrypt the file" << std::endl;
    std::cout << "  -k, --key <key>      Set the encryption key" << std::endl;
    std::cout << "  -h, --help          Display this help message" << std::endl;
}

int main(int argc, char** argv) {
    AESKey key("your_secret_key_here");
    std::string filename;
    bool encrypt_mode = false;
    bool decrypt_mode = false;
    std::string encryption_key;

    static struct option long_options[] = {
        {"encrypt", required_argument, 0, 'e'},
        {"decrypt", required_argument, 0, 'd'},
        {"key", required_argument, 0, 'k'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "e:d:k:h", long_options, NULL)) != -1) {
        switch(opt) {
            case 'e':
                encrypt_mode = true;
                filename = optarg;
                break;
            case 'd':
                decrypt_mode = true;
                filename = optarg;
                break;
            case 'k':
                encryption_key = optarg;
                break;
            case 'h':
                usage();
                return 0;
            default:
                usage();
                return 1;
        }
    }

    if (encrypt_mode) {
        AESKey new_key(encryption_key);
        encrypt(filename, new_key);
    } else if (decrypt_mode) {
        AESKey new_key(encryption_key);
        decrypt(filename, new_key);
    } else {
        usage();
        return 1;
    }

    return 0;
}