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
    void on_encrypt_button_clicked();
    void on_decrypt_button_clicked();
    void on_file_changed();
    void on_about_clicked();
    void update_status(const Glib::ustring& message);
    bool validate_inputs();

private:
    Gtk::Box m_main_box{Gtk::ORIENTATION_VERTICAL};

    Gtk::Box m_file_box{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Label m_file_label{"File:"};
    Gtk::FileChooserButton m_file_chooser{"Select a file"};

    Gtk::Box m_key_box{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Label m_key_label{"Encryption Key:"};
    Gtk::Entry m_key_entry;

    Gtk::Box m_button_box{Gtk::ORIENTATION_HORIZONTAL};
    Gtk::Button m_encrypt_button{"Encrypt"};
    Gtk::Button m_decrypt_button{"Decrypt"};
    Gtk::Button m_about_button{"About"};

    Gtk::ProgressBar m_progress_bar;
    Gtk::Label m_status_label;
};
