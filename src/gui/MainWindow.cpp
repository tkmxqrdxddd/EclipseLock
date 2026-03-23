#include "MainWindow.h"
#include <gtkmm/aboutdialog.h>
#include <iostream>

MainWindow::MainWindow()
{
    set_title("EclipseLock - File Encryption Tool");
    set_default_size(600, 300);
    set_border_width(10);

    m_file_chooser.set_action(Gtk::FILE_CHOOSER_ACTION_OPEN);
    m_file_chooser.signal_file_set().connect(
        sigc::mem_fun(*this, &MainWindow::on_file_changed));

    m_key_entry.set_visibility(false);
    m_key_entry.set_placeholder_text("Enter encryption key...");

    m_encrypt_button.set_sensitive(false);
    m_decrypt_button.set_sensitive(false);
    m_encrypt_button.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_encrypt_button_clicked));
    m_decrypt_button.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_decrypt_button_clicked));
    m_about_button.signal_clicked().connect(
        sigc::mem_fun(*this, &MainWindow::on_about_clicked));

    m_progress_bar.set_fraction(0.0);
    m_progress_bar.set_show_text(true);

    m_status_label.set_markup("<i>Ready</i>");
    m_status_label.set_justify(Gtk::JUSTIFY_CENTER);

    m_file_box.pack_start(m_file_label, false, false);
    m_file_box.pack_start(m_file_chooser, true, true);
    m_file_box.set_spacing(10);

    m_key_box.pack_start(m_key_label, false, false);
    m_key_box.pack_start(m_key_entry, true, true);
    m_key_box.set_spacing(10);

    m_button_box.pack_start(m_encrypt_button, false, false);
    m_button_box.pack_start(m_decrypt_button, false, false);
    m_button_box.pack_start(m_about_button, false, false);
    m_button_box.set_spacing(10);
    m_button_box.set_halign(Gtk::ALIGN_CENTER);

    m_main_box.pack_start(m_file_box, false, false);
    m_main_box.pack_start(m_key_box, false, false);
    m_main_box.pack_start(m_button_box, false, false);
    m_main_box.pack_start(m_progress_bar, false, false);
    m_main_box.pack_start(m_status_label, false, false);
    m_main_box.set_spacing(10);

    add(m_main_box);

    show_all_children();

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

MainWindow::~MainWindow()
{
}

void MainWindow::on_encrypt_button_clicked()
{
    if (!validate_inputs()) return;

    try {
        Glib::ustring filename = m_file_chooser.get_filename();
        Glib::ustring password = m_key_entry.get_text();

        update_status("Encrypting...");
        m_progress_bar.set_fraction(0.1);

        std::string encrypted_file = encrypt(filename.raw(), password.raw());

        m_progress_bar.set_fraction(1.0);
        update_status("Encryption complete! Encrypted file: " + Glib::ustring(encrypted_file));

        Glib::signal_timeout().connect_seconds([this]() {
            m_progress_bar.set_fraction(0.0);
            return false;
        }, 2);

    }
    catch (const std::exception& e) {
        Gtk::MessageDialog dialog(*this, "Encryption Error", false, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text(e.what());
        dialog.run();
        update_status("Encryption failed!");
    }
}

void MainWindow::on_decrypt_button_clicked()
{
    if (!validate_inputs()) return;

    try {
        Glib::ustring filename = m_file_chooser.get_filename();
        Glib::ustring password = m_key_entry.get_text();

        update_status("Decrypting...");
        m_progress_bar.set_fraction(0.1);

        decrypt(filename.raw(), password.raw());

        m_progress_bar.set_fraction(1.0);
        update_status("Decryption complete!");

        Glib::signal_timeout().connect_seconds([this]() {
            m_progress_bar.set_fraction(0.0);
            return false;
        }, 2);

    }
    catch (const std::exception& e) {
        Gtk::MessageDialog dialog(*this, "Decryption Error", false, Gtk::MESSAGE_ERROR);
        dialog.set_secondary_text(e.what());
        dialog.run();
        update_status("Decryption failed!");
    }
}

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

void MainWindow::on_file_changed()
{
    Glib::ustring filename = m_file_chooser.get_filename();
    if (!filename.empty()) {
        update_status("Selected file: " + Glib::ustring(filename));

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

void MainWindow::update_status(const Glib::ustring& message)
{
    m_status_label.set_markup("<i>" + message + "</i>");
}

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
