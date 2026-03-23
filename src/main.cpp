/**
 * @file main.cpp
 * @brief Main entry point for the EclipseLock file encryption application.
 *
 * EclipseLock is a GTK-based GUI application for encrypting and decrypting
 * files using AES-256-CBC encryption.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <getopt.h>
#include <gtkmm.h>
#include "gui/MainWindow.h"

/**
 * @brief Application entry point.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit code (0 for success).
 *
 * Initializes the GTK application framework and OpenSSL libraries,
 * then displays the main window for file encryption/decryption.
 */
int main(int argc, char* argv[])
{
    auto app = Gtk::Application::create(argc, argv, "com.eclipselock.app");

    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    MainWindow window;

    return app->run(window);
}
