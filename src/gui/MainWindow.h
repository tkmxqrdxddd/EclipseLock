#pragma once
#include <wx/wx.h>
#include <wx/filepicker.h>
#include "../AESCrypt.h"

class MainWindow : public wxFrame {
public:
    MainWindow();

private:
    // GUI elements
    wxTextCtrl* keyInput;
    wxFilePickerCtrl* filePicker;
    wxButton* encryptButton;
    wxButton* decryptButton;
    wxGauge* progressGauge;
    wxStaticText* statusText;

    // Event handlers
    void OnEncrypt(wxCommandEvent& evt);
    void OnDecrypt(wxCommandEvent& evt);
    void OnFileChanged(wxFileDirPickerEvent& evt);
    void UpdateStatus(const wxString& message);

    wxDECLARE_EVENT_TABLE();

    bool ValidateInputs();
}; 