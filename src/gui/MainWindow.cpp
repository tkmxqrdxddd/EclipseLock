#include "MainWindow.h"
#include <gtkmm/aboutdialog.h>
#include <iostream>

/**
 * MainWindow Constructor
 *
 * Sets up the complete GUI interface including:
 * - Window properties and size
 * - File chooser for selecting files to encrypt/decrypt
 * - Password entry field for the encryption key
 * - Action buttons (Encrypt, Decrypt, About)
 * - Progress bar and status label
 * - Signal connections for all interactive elements
 */
MainWindow::MainWindow()
{
    // Set window title and default dimensions
    set_title("EclipseLock - File Encryption Tool");
    set_default_size(600, 300);
    set_border_width(10);

    // Configure file chooser to open files
    m_file_chooser.set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
    // Connect file selection change event
    m_file_chooser.signal_file_set().connect(
        sigc::mem_fun(*this, &MainWindow::on_file_changed));

    // Configure key entry as a password field (hide characters)
    m_key_entry.set_visibility(false);
    m_key_entry.set_placeholder_text("Enter encryption key...");

    // Configure buttons - initially disabled until valid inputs provided
    m_encrypt_button.set_sensitive(false);
    m_decrypt_button.set_sensitive(false);
    // Connect button click events to handlers
    m_encrypt_button.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_encrypt_button_clicked));
    m_decrypt_button.signal_signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_decrypt_button_clicked));
    m_about_button.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_about_clicked));

    // Configure progress bar to show text and start at 0%
    m_progress_bar.set_fraction(0.0);
    m_progress_bar.set_show_text(true);

    // Configure status label with italic markup
    m_status_label.set_markup("<i>Ready</i>");
    m_status_label.set_justify(Gtk::JUSTIFY_CENTER);

    // Build the interface - assemble file selection row
    m_file_box.append(m_file_label);
    m_file_box.append(m_file_chooser);
    m_file_box.set_spacing(10);

    // Assemble key input row
    m_key_box.append(m_key_label);
    m_key_box.append(m_key_entry);
    m_key_box.set_spacing(10);

    // Assemble button row
    m_button_box.append(m_encrypt_button);
    m_button_box.append(m_decrypt_button);
    m_button_box.append(m_about_button);
    m_button_box.set_spacing(10);
    m_button_box.set_halign(Gtk::ALIGN_CENTER);

    // Assemble main vertical layout
    m_main_box.append(m_file_box);
    m_main_box.append(m_key_box);
    m_main_box.append(m_button_box);
    m_main_box.append(m_progress_bar);
    m_main_box.append(m_status_label);
    m_main_box.set_spacing(10);

    // Set the main box as the window's child widget
    set_child(m_main_box);

    // Connect key release event to validate inputs dynamically
    // Enable/disable buttons based on whether both file and key are provided
    m_key_entry.signal_changed().connect([this]() {
        if (!m_file_chooser.get_filename().empty() && !m_key_entry.get_text().empty()) {
            m_encrypt_button.set_sensitive(true);
            m_decrypt_button.set_sensitive(true);
        } else {
            m_encrypt_button.set_sensitive(false);
            m_decrypt_button.set_sensitive(false);
        }
    });
}

/**
 * MainWindow Destructor
 */
MainWindow::~MainWindow()
{
}

/**
 * Handles encryption button click.
 *
 * Validates inputs, performs file encryption using the AESCrypt library,
 * updates the progress bar, and displays success/error messages.
 */
void MainWindow::on_encrypt_button_clicked()
{
    if (!validate_inputs()) return;

    try {
        Glib::ustring filename = m_file_chooser.get_filename();
        Glib::ustring key = m_key_entry.get_text();

        update_status("Encrypting...");
        m_progress_bar.set_fraction(0.1);

        // Use existing encryption code from AESCrypt
        AESKey aesKey(key.raw());
        std::string encrypted_file = encrypt(filename.raw(), aesKey);

        m_progress_bar.set_fraction(1.0);
        update_status("Encryption complete! Encrypted file: " + Glib::ustring(encrypted_file));

        // Reset progress bar after a short delay
        Glib::timeout_add_seconds(2, [this]() {
            m_progress_bar.set_fraction(0.0);
            return false; // Remove the timeout
        });

    }
    catch (const std::exception& e) {
        // Display error dialog on encryption failure
        Gtk::MessageDialog dialog(*this, "Encryption Error", false, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text(e.what());
        dialog.run();
        update_status("Encryption failed!");
    }
}

/**
 * Handles decryption button click.
 *
 * Validates inputs, performs file decryption using the AESCrypt library,
 * updates the progress bar, and displays success/error messages.
 */
void MainWindow::on_decrypt_button_clicked()
{
    if (!validate_inputs()) return;

    try {
        Glib::ustring filename = m_file_chooser.get_filename();
        Glib::ustring key = m_key_entry.get_text();

        update_status("Decrypting...");
        m_progress_bar.set_fraction(0.1);

        // Use existing decryption code from AESCrypt
        AESKey aesKey(key.raw());
        decrypt(filename.raw(), aesKey);

        m_progress_bar.set_fraction(1.0);
        update_status("Decryption complete!");

        // Reset progress bar after a short delay
        Glib::timeout_add_seconds(2, [this]() {
            m_progress_bar.set_fraction(0.0);
            return false; // Remove the timeout
        });

    }
    catch (const std::exception& e) {
        // Display error dialog on decryption failure
        Gtk::MessageDialog dialog(*this, "Decryption Error", false, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text(e.what());
        dialog.run();
        update_status("Decryption failed!");
    }
}

/**
 * Handles about button click.
 *
 * Displays an about dialog with application information including
 * version, copyright, license, and author details.
 */
void MainWindow::on_about_clicked()
{
    Gtk::AboutDialog dialog;
    dialog.set_transient_for(*this);
    dialog.set_program_name("EclipseLock");
    dialog.set_version("2.0.0");
    dialog.set_copyright("© 2025 EclipseLock Team");
    dialog.set_comments("A modern file encryption tool with GTK interface");
    dialog.set_license("MIT License");
    dialog.set_website("https://github.com/tkmxqrdxddd/eclipselock");
    dialog.set_website_label("View on GitHub");

    std::vector<Glib::ustring> authors = {"tkmxqrdxddd"};
    dialog.set_authors(authors);

    dialog.run();
}

/**
 * Handles file selection changes.
 *
 * Updates the status label with the selected filename and enables
 * action buttons if a key is also provided.
 */
void MainWindow::on_file_changed()
{
    Glib::ustring filename = m_file_chooser.get_filename();
    if (!filename.empty()) {
        update_status("Selected file: " + Glib::ustring(filename));

        // Enable buttons if key is also provided
        if (!m_key_entry.get_text().empty()) {
            m_encrypt_button.set_sensitive(true);
            m_decrypt_button.set_sensitive(true);
        }
    } else {
        update_status("Ready");
        m_encrypt_button.set_sensitive(false);
        m_decrypt_button.set_sensitive(false);
    }
}

/**
 * Updates the status label with a formatted message.
 *
 * @param message The status message to display.
 */
void MainWindow::update_status(const Glib::ustring& message)
{
    m_status_label.set_markup("<i>" + message + "</i>");
}

/**
 * Validates user inputs before encryption/decryption operations.
 *
 * @return true if both file and key are provided, false otherwise.
 *
 * Displays warning dialogs for missing inputs to guide the user.
 */
bool MainWindow::validate_inputs()
{
    if (m_key_entry.get_text().empty()) {
        Gtk::MessageDialog dialog(*this, "Input Error", false, Gtk::MESSAGE_WARNING);
        dialog.set_secondary_text("Please enter an encryption key.");
        dialog.run();
        return false;
    }

    if (m_file_chooser.get_filename().empty()) {
        Gtk::MessageDialog dialog(*this, "Input Error", false, Gtk::MESSAGE_WARNING);
        dialog.set_secondary_text("Please select a file.");
        dialog.run();
        return false;
    }

    return true;
}
  