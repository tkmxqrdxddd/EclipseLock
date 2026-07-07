#include <iostream>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <gtkmm.h>
#include "gui/MainWindow.h"
#include "AESCrypt.h"

static void print_usage(const char* prog) {
    std::cout << "EclipseLock - File Encryption Tool\n"
              << "Usage:\n"
              << "  " << prog << "                            Launch GUI\n"
              << "  " << prog << " encrypt <file> <password>  Encrypt a file\n"
              << "  " << prog << " decrypt <file> <password>  Decrypt a file\n"
              << "  " << prog << " help                       Show this help\n";
}

static void init_openssl() {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        std::string cmd = argv[1];
        if (cmd == "encrypt" && argc == 4) {
            init_openssl();
            try {
                std::string result = encrypt(argv[2], argv[3]);
                std::cout << "Encryption complete: " << result << std::endl;
                return 0;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
            }
        }
        if (cmd == "decrypt" && argc == 4) {
            init_openssl();
            try {
                decrypt(argv[2], argv[3]);
                std::cout << "Decryption complete." << std::endl;
                return 0;
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << std::endl;
                return 1;
            }
        }
        if (cmd == "help" || cmd == "--help") {
            print_usage(argv[0]);
            return 0;
        }
        std::cerr << "Unknown command: " << cmd << "\n";
        print_usage(argv[0]);
        return 1;
    }

    init_openssl();

    auto app = Gtk::Application::create(argc, argv, "com.eclipselock.app");
    MainWindow window;
    return app->run(window);
}
