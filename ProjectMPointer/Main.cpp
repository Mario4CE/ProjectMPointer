#include "Server.h"
#include "interfaz.h"
#include "ordenes.h"
#include <thread> 
#include <iostream>

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<System::String^>^ args)
{
    // Iniciar el servidor en un hilo separado
    std::thread serverThread([]() {
        std::cout << "Iniciando el servidor..." << std::endl;
        int resultado = startServer();
        if (resultado != 0) {
            std::cerr << "El servidor no pudo iniciarse correctamente." << std::endl;
        }
        });

    // Desacoplar el hilo del servidor para que no bloquee la ejecuci�n de la interfaz gr�fica
    serverThread.detach();

    // Iniciar la interfaz gr�fica en el hilo principal
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    InterfazCLI::FormularioPrincipal formulario;
    Application::Run(% formulario);

    return 0;
}