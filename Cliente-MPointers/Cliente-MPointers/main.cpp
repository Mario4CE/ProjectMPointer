#include "Cliente.h"
#include <winsock2.h>

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<System::String^>^ args) {
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    // Crear y ejecutar la ventana del cliente
    ClienteMPointers::Cliente form;
    Application::Run(% form);
    return 0;
}

