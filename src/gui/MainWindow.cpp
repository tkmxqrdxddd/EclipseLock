#include "MainWindow.h"

wxBEGIN_EVENT_TABLE(MainWindow, wxFrame)
    EVT_BUTTON(wxID_ANY, MainWindow::OnEncrypt)
    EVT_BUTTON(wxID_ANY, MainWindow::OnDecrypt)
    EVT_FILEPICKER_CHANGED(wxID_ANY, MainWindow::OnFileChanged)
wxEND_EVENT_TABLE()

MainWindow::MainWindow() 
    : wxFrame(nullptr, wxID_ANY, "EclipseLock", 
              wxDefaultPosition, wxSize(500, 300))
{
    // Create main panel with vertical layout
    auto* mainPanel = new wxPanel(this);
    auto* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Add file picker
    filePicker = new wxFilePickerCtrl(mainPanel, wxID_ANY, 
        wxEmptyString, "Choose a file");
    mainSizer->Add(filePicker, 0, wxEXPAND | wxALL, 5);

    // Add key input field
    auto* keyLabel = new wxStaticText(mainPanel, wxID_ANY, "Encryption Key:");
    mainSizer->Add(keyLabel, 0, wxALL, 5);
    
    keyInput = new wxTextCtrl(mainPanel, wxID_ANY, "", 
        wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
    mainSizer->Add(keyInput, 0, wxEXPAND | wxALL, 5);

    // Add buttons
    auto* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    encryptButton = new wxButton(mainPanel, wxID_ANY, "Encrypt");
    decryptButton = new wxButton(mainPanel, wxID_ANY, "Decrypt");
    buttonSizer->Add(encryptButton, 1, wxALL, 5);
    buttonSizer->Add(decryptButton, 1, wxALL, 5);
    mainSizer->Add(buttonSizer, 0, wxEXPAND);

    // Add progress bar
    progressGauge = new wxGauge(mainPanel, wxID_ANY, 100);
    mainSizer->Add(progressGauge, 0, wxEXPAND | wxALL, 5);

    // Add status text
    statusText = new wxStaticText(mainPanel, wxID_ANY, "Ready");
    mainSizer->Add(statusText, 0, wxALL, 5);

    mainPanel->SetSizer(mainSizer);
    Center();
}

void MainWindow::OnEncrypt(wxCommandEvent& evt) {
    if (!ValidateInputs()) return;

    try {
        wxString path = filePicker->GetPath();
        wxString key = keyInput->GetValue();
        
        UpdateStatus("Encrypting...");
        progressGauge->SetValue(0);

        // Use your existing encryption code
        AESKey aesKey(key.ToStdString());
        encrypt(path.ToStdString(), aesKey);

        progressGauge->SetValue(100);
        UpdateStatus("Encryption complete!");
    }
    catch (const std::exception& e) {
        wxMessageBox(e.what(), "Error", wxICON_ERROR);
        UpdateStatus("Encryption failed!");
    }
}

bool MainWindow::ValidateInputs() {
    // Example validation logic
    if (keyInput->GetValue().IsEmpty()) {
        wxMessageBox("Please enter an encryption key.", "Input Error", wxICON_ERROR);
        return false;
    }
    if (filePicker->GetPath().IsEmpty()) {
        wxMessageBox("Please select a file.", "Input Error", wxICON_ERROR);
        return false;
    }
    return true;
}

void MainWindow::UpdateStatus(const wxString& message) {
    statusText->SetLabel(message);
}

void MainWindow::OnDecrypt(wxCommandEvent& evt) {
    // Implement decryption logic here
    if (!ValidateInputs()) return;

    try {
        wxString path = filePicker->GetPath();
        wxString key = keyInput->GetValue();

        UpdateStatus("Decrypting...");
        progressGauge->SetValue(0);

        // Use your existing decryption code
        AESKey aesKey(key.ToStdString());
        decrypt(path.ToStdString(), aesKey); // Ensure this function is implemented

        progressGauge->SetValue(100);
        UpdateStatus("Decryption complete!");
    }
    catch (const std::exception& e) {
        wxMessageBox(e.what(), "Error", wxICON_ERROR);
        UpdateStatus("Decryption failed!");
    }
}

void MainWindow::OnFileChanged(wxFileDirPickerEvent& evt) {
    // Handle file change event here
    wxString path = filePicker->GetPath();
    UpdateStatus("Selected file: " + path);
}
  