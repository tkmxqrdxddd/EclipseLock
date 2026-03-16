#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include "AESCrypt.h"

/**
 * @brief Print usage information for CLI mode
 */
void printUsage(const char* programName) {
    std::cout << "EclipseLock - File Encryption Tool\n\n"
              << "Usage:\n"
              << "  GUI Mode: " << programName << "\n"
              << "  CLI Mode: " << programName << " <command> [options]\n\n"
              << "Commands:\n"
              << "  encrypt <file> <key>   Encrypt a file\n"
              << "  decrypt <file> <key>   Decrypt a file\n"
              << "  help                   Show this help message\n\n"
              << "Examples:\n"
              << "  " << programName << " encrypt document.pdf mypassword123\n"
              << "  " << programName << " decrypt document.pdf.enc mypassword123\n";
}

/**
 * @brief Handle command-line interface mode (non-GUI)
 * @return 0 if CLI was handled, -1 if should proceed to GUI
 */
int handleCLI(int argc, char* argv[]) {
    if (argc < 2) {
        return -1; // No arguments, proceed to GUI
    }
    
    std::string command = argv[1];
    
    if (command == "help" || command == "--help" || command == "-h") {
        printUsage(argv[0]);
        return 0;
    }
    
    if (command == "encrypt" || command == "decrypt") {
        if (argc < 4) {
            std::cerr << "Error: Missing arguments\n\n";
            printUsage(argv[0]);
            return 1;
        }
        
        std::string filename = argv[2];
        std::string key = argv[3];
        
        try {
            AESKey aesKey(key);
            
            if (command == "encrypt") {
                std::string resultPath = encryptFile(filename, aesKey);
                std::cout << "Encrypted: " << resultPath << std::endl;
            } else {
                decryptFile(filename, aesKey);
                std::cout << "Decrypted successfully" << std::endl;
            }
            return 0;
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    }
    
    std::cerr << "Error: Unknown command '" << command << "'\n\n";
    printUsage(argv[0]);
    return 1;
}

// GUI mode with wxWidgets
#include <wx/wx.h>
#include "gui/MainWindow.h"

/**
 * @brief Main application class
 */
class EclipseLockApp : public wxApp {
public:
    bool OnInit() override {
        MainWindow* window = new MainWindow();
        window->Show();
        return true;
    }
    
    int OnRun() override {
        // Check if we should run GUI (no CLI args processed)
        if (argc <= 1) {
            return wxApp::OnRun();
        }
        return 0;
    }
};

// Declare the app
wxDECLARE_APP(EclipseLockApp);

// Main entry point
int main(int argc, char* argv[]) {
    // First check for CLI commands
    int cliResult = handleCLI(argc, argv);
    if (cliResult != -1) {
        return cliResult; // CLI handled
    }
    
    // Run the wxWidgets app
    wxApp::SetInstance(new EclipseLockApp());
    return wxEntry(argc, argv);
}
