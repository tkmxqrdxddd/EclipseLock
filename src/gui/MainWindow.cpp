#include "MainWindow.h"
#include <wx/filename.h>
#include <functional>

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_BUTTON(wxID_ANY, MainWindow::OnEncrypt)
    EVT_BUTTON(wxID_ANY, MainWindow::OnDecrypt)
    EVT_FILEPICKER_CHANGED(wxID_ANY, MainWindow::OnFileChanged)
    EVT_ENTER_WINDOW(MainWindow::OnButtonEnter)
    EVT_LEAVE_WINDOW(MainWindow::OnButtonLeave)
wxEND_EVENT_TABLE()

// Color scheme - Modern dark theme
static const wxColour COLOR_BG_PRIMARY(30, 30, 46);
static const wxColour COLOR_BG_SECONDARY(45, 45, 65);
static const wxColour COLOR_PRIMARY(78, 127, 213);
static const wxColour COLOR_PRIMARY_HOVER(98, 147, 233);
static const wxColour COLOR_SUCCESS(46, 178, 98);
static const wxColour COLOR_SUCCESS_HOVER(66, 198, 118);
static const wxColour COLOR_TEXT(230, 230, 230);
static const wxColour COLOR_TEXT_MUTED(150, 150, 160);

MainWindow::MainWindow()
    : wxFrame(nullptr, wxID_ANY, "EclipseLock - Secure File Encryption",
              wxDefaultPosition, wxSize(550, 520))
{
    // Set window properties
    SetBackgroundColour(COLOR_BG_PRIMARY);
    
    // Create main panel
    mainPanel_ = new wxPanel(this);
    mainPanel_->SetBackgroundColour(COLOR_BG_PRIMARY);
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create header section
    mainSizer->AddSpacer(30);
    
    // Title
    titleLabel_ = new wxStaticText(mainPanel_, wxID_ANY, "EclipseLock");
    titleLabel_->SetFont(wxFont(28, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    titleLabel_->SetForegroundColour(COLOR_TEXT);
    mainSizer->Add(titleLabel_, 0, wxALIGN_CENTER_HORIZONTAL);
    
    // Subtitle
    subtitleLabel_ = new wxStaticText(mainPanel_, wxID_ANY, "Secure File Encryption with AES-256");
    subtitleLabel_->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    subtitleLabel_->SetForegroundColour(COLOR_TEXT_MUTED);
    mainSizer->Add(subtitleLabel_, 0, wxALIGN_CENTER_HORIZONTAL | wxTOP, 5);
    
    mainSizer->AddSpacer(30);

    // Create file picker section
    auto* fileLabel = new wxStaticText(mainPanel_, wxID_ANY, "Select File");
    fileLabel->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    fileLabel->SetForegroundColour(COLOR_TEXT);
    mainSizer->Add(fileLabel, 0, wxLEFT | wxBOTTOM, 15);

    filePicker_ = new wxFilePickerCtrl(mainPanel_, wxID_ANY,
        wxEmptyString, "Choose a file to encrypt/decrypt",
        wxEmptyString, wxDefaultPosition, wxSize(-1, 45),
        wxFLP_USE_TEXTCTRL | wxFLP_FILE_MUST_EXIST);
    filePicker_->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    mainSizer->Add(filePicker_, 0, wxEXPAND | wxLEFT | wxRIGHT, 30);

    mainSizer->AddSpacer(20);

    // Create key input section
    auto* keyLabel = new wxStaticText(mainPanel_, wxID_ANY, "Encryption Key");
    keyLabel->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    keyLabel->SetForegroundColour(COLOR_TEXT);
    mainSizer->Add(keyLabel, 0, wxLEFT | wxBOTTOM, 15);

    keyInput_ = new wxTextCtrl(mainPanel_, wxID_ANY, wxEmptyString,
        wxDefaultPosition, wxSize(-1, 45), wxTE_PASSWORD);
    keyInput_->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    mainSizer->Add(keyInput_, 0, wxEXPAND | wxLEFT | wxRIGHT, 30);

    // Key strength hint
    auto* keyHint = new wxStaticText(mainPanel_, wxID_ANY,
        "Use a strong key (minimum 8 characters recommended)");
    keyHint->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    keyHint->SetForegroundColour(COLOR_TEXT_MUTED);
    mainSizer->Add(keyHint, 0, wxLEFT | wxTOP, 35);
    
    mainSizer->AddSpacer(30);

    // Create button section
    auto* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    
    // Encrypt button
    encryptButton_ = new wxButton(mainPanel_, wxID_ANY, "Encrypt",
                                   wxDefaultPosition, wxSize(150, 45));
    encryptButton_->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    encryptButton_->SetBackgroundColour(COLOR_PRIMARY);
    encryptButton_->SetForegroundColour(*wxWHITE);
    encryptButton_->Bind(wxEVT_ENTER_WINDOW, &MainWindow::OnButtonEnter, this);
    encryptButton_->Bind(wxEVT_LEAVE_WINDOW, &MainWindow::OnButtonLeave, this);
    
    // Decrypt button
    decryptButton_ = new wxButton(mainPanel_, wxID_ANY, "Decrypt",
                                   wxDefaultPosition, wxSize(150, 45));
    decryptButton_->SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    decryptButton_->SetBackgroundColour(COLOR_SUCCESS);
    decryptButton_->SetForegroundColour(*wxWHITE);
    decryptButton_->Bind(wxEVT_ENTER_WINDOW, &MainWindow::OnButtonEnter, this);
    decryptButton_->Bind(wxEVT_LEAVE_WINDOW, &MainWindow::OnButtonLeave, this);
    
    buttonSizer->Add(encryptButton_, 1, wxLEFT | wxRIGHT, 25);
    buttonSizer->Add(decryptButton_, 1, wxLEFT | wxRIGHT, 25);
    
    mainSizer->Add(buttonSizer, 0, wxEXPAND);
    
    mainSizer->AddSpacer(30);

    // Create status section
    progressGauge_ = new wxGauge(mainPanel_, wxID_ANY, 100,
                                  wxDefaultPosition, wxSize(-1, 10),
                                  wxGA_HORIZONTAL | wxGA_SMOOTH);
    progressGauge_->SetValue(0);
    mainSizer->Add(progressGauge_, 0, wxEXPAND | wxLEFT | wxRIGHT, 50);
    
    mainSizer->AddSpacer(15);
    
    // Status text
    statusText_ = new wxStaticText(mainPanel_, wxID_ANY, "Ready");
    statusText_->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    statusText_->SetForegroundColour(COLOR_TEXT_MUTED);
    mainSizer->Add(statusText_, 0, wxALL | wxALIGN_CENTER, 10);
    
    mainSizer->AddSpacer(20);

    mainPanel_->SetSizer(mainSizer);
    
    // Center on screen
    Centre(wxBOTH);
    SetMinSize(wxSize(450, 450));
}

void MainWindow::SetupStyling() {
    // Additional styling can be added here if needed
}

void MainWindow::OnEncrypt(wxCommandEvent& WXUNUSED(evt)) {
    RunOperation("Encrypting", [this]() {
        wxString path = filePicker_->GetPath();
        wxString key = keyInput_->GetValue();

        AESKey aesKey(key.ToStdString());
        std::string resultPath = encryptFile(path.ToStdString(), aesKey);

        UpdateStatus("Encrypted: " + wxString(resultPath));
    });
}

void MainWindow::OnDecrypt(wxCommandEvent& WXUNUSED(evt)) {
    RunOperation("Decrypting", [this]() {
        wxString path = filePicker_->GetPath();
        wxString key = keyInput_->GetValue();

        AESKey aesKey(key.ToStdString());
        decryptFile(path.ToStdString(), aesKey);

        UpdateStatus("Decryption complete!");
    });
}

void MainWindow::OnButtonEnter(wxMouseEvent& evt) {
    wxButton* btn = dynamic_cast<wxButton*>(evt.GetEventObject());
    if (btn) {
        if (btn == encryptButton_) {
            btn->SetBackgroundColour(COLOR_PRIMARY_HOVER);
        } else if (btn == decryptButton_) {
            btn->SetBackgroundColour(COLOR_SUCCESS_HOVER);
        }
        btn->Refresh();
    }
    evt.Skip();
}

void MainWindow::OnButtonLeave(wxMouseEvent& evt) {
    wxButton* btn = dynamic_cast<wxButton*>(evt.GetEventObject());
    if (btn) {
        if (btn == encryptButton_) {
            btn->SetBackgroundColour(COLOR_PRIMARY);
        } else if (btn == decryptButton_) {
            btn->SetBackgroundColour(COLOR_SUCCESS);
        }
        btn->Refresh();
    }
    evt.Skip();
}

bool MainWindow::ValidateInputs() {
    if (keyInput_->GetValue().IsEmpty()) {
        wxMessageBox("Please enter an encryption key.", "Input Error", 
                     wxICON_ERROR | wxOK | wxCENTER);
        return false;
    }
    
    if (keyInput_->GetValue().Length() < 8) {
        int result = wxMessageBox(
            "Your key is less than 8 characters. This is not secure.\n"
            "Do you want to continue anyway?",
            "Weak Key Warning",
            wxYES_NO | wxICON_WARNING | wxCENTER
        );
        if (result != wxYES) {
            return false;
        }
    }
    
    if (filePicker_->GetPath().IsEmpty()) {
        wxMessageBox("Please select a file.", "Input Error", 
                     wxICON_ERROR | wxOK | wxCENTER);
        return false;
    }
    
    wxString path = filePicker_->GetPath();
    if (!wxFileExists(path)) {
        wxMessageBox("The selected file does not exist.", "File Error", 
                     wxICON_ERROR | wxOK | wxCENTER);
        return false;
    }
    
    return true;
}

void MainWindow::UpdateStatus(const wxString& message, bool isError) {
    statusText_->SetLabel(message);
    statusText_->SetForegroundColour(isError ? wxColour(220, 80, 80) : COLOR_TEXT_MUTED);
    statusText_->Refresh();
}

void MainWindow::OnFileChanged(wxFileDirPickerEvent& WXUNUSED(evt)) {
    wxString path = filePicker_->GetPath();
    
    // Check if it's an encrypted file
    if (path.EndsWith(".enc")) {
        UpdateStatus("Encrypted file ready for decryption");
    } else {
        UpdateStatus("File ready for encryption");
    }
}

void MainWindow::RunOperation(const wxString& operationName,
                               const std::function<void()>& operation) {
    if (!ValidateInputs()) {
        return;
    }

    try {
        UpdateStatus(operationName + "...");
        progressGauge_->SetValue(0);
        Refresh();
        Update();

        operation();

        progressGauge_->SetValue(100);
        UpdateStatus(operationName + " complete!");
    }
    catch (const std::exception& e) {
        wxMessageBox(e.what(), "Error", wxICON_ERROR | wxOK | wxCENTER);
        UpdateStatus(operationName + " failed!", true);
        progressGauge_->SetValue(0);
    }
}
