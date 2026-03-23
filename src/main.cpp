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
    // Create GTK application with unique application ID
    auto app = Gtk::Application::create(argc, argv, "com.eclipselock.app");

    // Initialize OpenSSL libraries
    // Loads all available encryption algorithms
    OpenSSL_add_all_algorithms();
    // Loads human-readable error strings for OpenSSL error codes
    ERR_load_crypto_strings();

    // Create and display the main application window
    MainWindow window;

    // Run the GTK main loop
    return app->run(window);
}