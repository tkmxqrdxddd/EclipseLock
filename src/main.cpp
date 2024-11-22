#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <getopt.h>
#include <wx/wx.h>
#include "gui/MainWindow.h"

class EclipseLockApp : public wxApp {
public:
    bool OnInit() {
        MainWindow* window = new MainWindow();
        window->Show();
        return true;
    }
};

wxIMPLEMENT_APP(EclipseLockApp);