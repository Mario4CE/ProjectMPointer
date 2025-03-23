#include "Cliente.h"
#include "MPointer.h"
#include <winsock2.h>
#include <iostream>

using namespace System;
using namespace System::Windows::Forms;

[STAThread]
int main(array<System::String^>^ args) {

    // Inicializar la interfaz gráfica
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);

    // Crear y ejecutar la ventana del cliente
    ClienteMPointers::Cliente form;
    Application::Run(% form);

    return 0;
}