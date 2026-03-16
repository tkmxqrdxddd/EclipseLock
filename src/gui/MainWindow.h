#pragma once

#include <wx/wx.h>
#include <wx/filepicker.h>
#include "AESCrypt.h"

/**
 * @brief Main application window for EclipseLock
 * 
 * Provides a modern-looking GUI for file encryption and decryption operations
 */
class MainWindow : public wxFrame {
public:
    MainWindow();
    ~MainWindow() override = default;

private:
    // Main container
    wxPanel* mainPanel_;
    
    // Header section
    wxStaticText* titleLabel_;
    wxStaticText* subtitleLabel_;
    
    // GUI elements
    wxFilePickerCtrl* filePicker_;
    wxTextCtrl* keyInput_;
    wxButton* encryptButton_;
    wxButton* decryptButton_;
    wxGauge* progressGauge_;
    wxStaticText* statusText_;
    
    // Event handlers
    void OnEncrypt(wxCommandEvent& evt);
    void OnDecrypt(wxCommandEvent& evt);
    void OnFileChanged(wxFileDirPickerEvent& evt);
    void OnButtonEnter(wxMouseEvent& evt);
    void OnButtonLeave(wxMouseEvent& evt);
    
    // Helper methods
    void UpdateStatus(const wxString& message, bool isError = false);
    bool ValidateInputs();
    void RunOperation(const wxString& operationName, 
                      const std::function<void()>& operation);
    void SetupStyling();
    
    wxDECLARE_EVENT_TABLE();
};
