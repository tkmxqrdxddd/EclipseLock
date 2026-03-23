#pragma once
#include <gtkmm.h>
#include "../AESCrypt.h"

/**
 * @class MainWindow
 * @brief Main application window for the EclipseLock file encryption tool.
 *
 * Provides a GTK-based GUI for selecting files, entering encryption keys,
 * and performing encryption/decryption operations.
 */
class MainWindow : public Gtk::Window {
public:
    /**
     * @brief Constructs the main application window.
     *
     * Initializes all GUI components, sets up signal connections,
     * and configures the window layout.
     */
    MainWindow();
    virtual ~MainWindow();

protected:
    /**
     * @brief Handles the encrypt button click event.
     *
     * Validates inputs, reads the selected file, encrypts it using
     * the provided key, and updates the status display.
     */
    void on_encrypt_button_clicked();

    /**
     * @brief Handles the decrypt button click event.
     *
     * Validates inputs, reads the encrypted file, decrypts it using
     * the provided key, and updates the status display.
     */
    void on_decrypt_button_clicked();

    /**
     * @brief Handles file selection changes.
     *
     * Updates the status label and enables/disables action buttons
     * based on whether a valid file is selected.
     */
    void on_file_changed();

    /**
     * @brief Handles the about button click event.
     *
     * Displays an about dialog with application information.
     */
    void on_about_clicked();

    /**
     * @brief Updates the status label with a message.
     * @param message The status message to display.
     */
    void update_status(const Glib::ustring& message);

    /**
     * @brief Validates user inputs before encryption/decryption.
     * @return true if inputs are valid, false otherwise.
     *
     * Checks that both a file is selected and a key is provided.
     * Displays warning dialogs for missing inputs.
     */
    bool validate_inputs();

private:
    // Main layout container
    Gtk::Box m_main_box{Gtk::Orientation::VERTICAL};  ///< Vertical box for main layout

    // File selection section
    Gtk::Box m_file_box{Gtk::Orientation::HORIZONTAL};  ///< Horizontal box for file selection row
    Gtk::Label m_file_label{"File:"};  ///< Label for file selector
    Gtk::FileChooserButton m_file_chooser{"Select a file"};  ///< File picker widget

    // Key input section
    Gtk::Box m_key_box{Gtk::Orientation::HORIZONTAL};  ///< Horizontal box for key input row
    Gtk::Label m_key_label{"Encryption Key:"};  ///< Label for key entry
    Gtk::Entry m_key_entry;  ///< Password entry field for encryption key

    // Action buttons section
    Gtk::Box m_button_box{Gtk::Orientation::HORIZONTAL};  ///< Horizontal box for action buttons
    Gtk::Button m_encrypt_button{"Encrypt"};  ///< Button to trigger encryption
    Gtk::Button m_decrypt_button{"Decrypt"};  ///< Button to trigger decryption
    Gtk::Button m_about_button{"About"};  ///< Button to show about dialog

    // Progress and status section
    Gtk::ProgressBar m_progress_bar;  ///< Progress indicator for operations
    Gtk::Label m_status_label{"Ready"};  ///< Status message display
};