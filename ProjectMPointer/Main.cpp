#include "Server.h"
#include "Interfaz.h"
#include "ClienteManager.h"
#include "MemoryManager.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include <thread>
#include <filesystem>
#include <iostream>
#include "ActualizarRespuesta.h"

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<System::String^>^ args)
{
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    InterfazCLI::FormularioPrincipal formulario;

    InterfazCLI::Respuestas::SetFormulario(% formulario);

    // Iniciar el servidor en un hilo separado
    std::thread serverThread([]() {
        std::cout << "Iniciando el servidor..." << std::endl;
        int resultado = startServer();
        if (resultado != 0) {
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: El servidor no pudo iniciarse correctamente.");
            std::cerr << "El servidor no pudo iniciarse correctamente." << std:: endl;
        }
        });

    // Desacoplar el hilo del servidor para que no bloquee la ejecución de la interfaz gráfica
    serverThread.detach();

    // Iniciar la interfaz gráfica en el hilo principal
    Application::Run(% formulario);

    return 0;
}