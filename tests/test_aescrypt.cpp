#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "../src/AESCrypt.h"
#include <openssl/rand.h>
#include <openssl/aes.h>
#include <fstream>

TEST_CASE("AESKey derivation", "[crypto][key]") {
    std::array<unsigned char, SALT_SIZE> salt{};
    REQUIRE(RAND_bytes(salt.data(), salt.size()) == 1);

    SECTION("Derives key from password") {
        AESKey key("test_password", salt);
        REQUIRE(key.getKey() != nullptr);
    }

    SECTION("Different passwords produce different keys") {
        AESKey key1("password1", salt);
        AESKey key2("password2", salt);
        REQUIRE(std::memcmp(key1.getKey(), key2.getKey(), KEY_SIZE) != 0);
    }

    SECTION("Different salts produce different keys") {
        std::array<unsigned char, SALT_SIZE> salt2{};
        salt2[0] = 42;
        AESKey key1("password", salt);
        AESKey key2("password", salt2);
        REQUIRE(std::memcmp(key1.getKey(), key2.getKey(), KEY_SIZE) != 0);
    }

    SECTION("Empty password is accepted") {
        REQUIRE_NOTHROW(AESKey("", salt));
    }
}

TEST_CASE("AESCrypt encrypt/decrypt roundtrip", "[crypto][aes]") {
    std::array<unsigned char, SALT_SIZE> salt{};
    std::array<unsigned char, IV_SIZE> iv{};
    REQUIRE(RAND_bytes(salt.data(), salt.size()) == 1);
    REQUIRE(RAND_bytes(iv.data(), iv.size()) == 1);

    auto key = std::make_shared<const AESKey>("test_password", salt);
    AESCrypt crypt(key);

    SECTION("Encrypts and decrypts small data") {
        std::vector<unsigned char> plaintext = {0x48, 0x65, 0x6C, 0x6C, 0x6F};
        auto ciphertext = crypt.enc(plaintext, iv.data());

        std::array<unsigned char, IV_SIZE> iv2;
        std::memcpy(iv2.data(), iv.data(), IV_SIZE);
        auto decrypted = crypt.dec(ciphertext, iv2.data());

        REQUIRE(plaintext == decrypted);
    }

    SECTION("Encrypts and decrypts empty data") {
        std::vector<unsigned char> plaintext;
        auto ciphertext = crypt.enc(plaintext, iv.data());

        std::array<unsigned char, IV_SIZE> iv2;
        std::memcpy(iv2.data(), iv.data(), IV_SIZE);
        auto decrypted = crypt.dec(ciphertext, iv2.data());

        REQUIRE(plaintext == decrypted);
    }

    SECTION("Encrypts and decrypts data with exact block size") {
        std::vector<unsigned char> plaintext(AES_BLOCK_SIZE, 0xAB);
        auto ciphertext = crypt.enc(plaintext, iv.data());

        std::array<unsigned char, IV_SIZE> iv2;
        std::memcpy(iv2.data(), iv.data(), IV_SIZE);
        auto decrypted = crypt.dec(ciphertext, iv2.data());

        REQUIRE(plaintext == decrypted);
    }
}

TEST_CASE("HMAC computation and verification", "[crypto][hmac]") {
    std::array<unsigned char, SALT_SIZE> salt{};
    REQUIRE(RAND_bytes(salt.data(), salt.size()) == 1);

    AESKey key("test_password", salt);
    std::vector<unsigned char> data = {0x01, 0x02, 0x03, 0x04};

    SECTION("Computes and verifies HMAC") {
        auto tag = AESCrypt::computeHMAC(key.getKey(), data);
        REQUIRE(AESCrypt::verifyHMAC(key.getKey(), data, tag));
    }

    SECTION("Detects tampered data") {
        auto tag = AESCrypt::computeHMAC(key.getKey(), data);
        std::vector<unsigned char> tampered = data;
        tampered[0] ^= 0xFF;
        REQUIRE_FALSE(AESCrypt::verifyHMAC(key.getKey(), tampered, tag));
    }

    SECTION("Detects wrong key") {
        std::array<unsigned char, SALT_SIZE> salt2{};
        salt2[0] = 99;
        AESKey wrong_key("wrong_password", salt2);
        auto tag = AESCrypt::computeHMAC(key.getKey(), data);
        REQUIRE_FALSE(AESCrypt::verifyHMAC(wrong_key.getKey(), data, tag));
    }

    SECTION("Detects corrupted tag") {
        auto tag = AESCrypt::computeHMAC(key.getKey(), data);
        tag[0] ^= 0xFF;
        REQUIRE_FALSE(AESCrypt::verifyHMAC(key.getKey(), data, tag));
    }
}

TEST_CASE("File encrypt/decrypt roundtrip", "[crypto][file]") {
    auto tmpfile = std::tmpnam(nullptr);
    auto encfile = std::string(tmpfile) + ".enc";
    std::string password = "test_password";

    std::vector<unsigned char> original_data = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE};

    {
        std::ofstream out(tmpfile, std::ios::binary);
        out.write(reinterpret_cast<const char*>(original_data.data()), original_data.size());
    }

    SECTION("Encrypts and decrypts a file") {
        std::string result = encrypt(tmpfile, password);
        REQUIRE(result == encfile);

        decrypt(encfile, password);

        std::ifstream in(tmpfile, std::ios::binary);
        std::vector<unsigned char> decrypted(
            (std::istreambuf_iterator<char>(in)),
            std::istreambuf_iterator<char>()
        );
        REQUIRE(original_data == decrypted);
    }

    SECTION("Decrypt with wrong password fails") {
        encrypt(tmpfile, password);
        REQUIRE_THROWS_AS(decrypt(encfile, "wrong_password"), std::runtime_error);
    }

    std::remove(tmpfile);
    std::remove(encfile.c_str());
}

TEST_CASE("Malformed encrypted file detection", "[crypto][error]") {
    auto tmpfile = std::tmpnam(nullptr);

    SECTION("File too small") {
        {
            std::ofstream out(tmpfile, std::ios::binary);
            out << "too short";
        }
        REQUIRE_THROWS_AS(decrypt(tmpfile, "password"), std::runtime_error);
        std::remove(tmpfile);
    }

    SECTION("Corrupted HMAC") {
        auto encfile = std::string(tmpfile) + ".enc";
        {
            std::ofstream out(tmpfile, std::ios::binary);
            out.write("dummy", 5);
        }
        encrypt(tmpfile, "password");

        std::fstream f(encfile, std::ios::binary | std::ios::in | std::ios::out);
        f.seekp(-1, std::ios::end);
        f.put(0xFF);

        REQUIRE_THROWS_AS(decrypt(encfile, "password"), std::runtime_error);

        std::remove(tmpfile);
        std::remove(encfile.c_str());
    }
}
