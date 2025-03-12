#include "server.h"
#include "interfaz.h"
#include <thread> // Incluye la biblioteca de hilos

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<System::String^>^ args)
{
    // Iniciar el servidor en un hilo separado
    std::thread serverThread([]() {
        int resultado = startServer();
        if (resultado != 0) {
            std::cerr << "El servidor no pudo iniciarse correctamente." << std::endl;
        }
        });

    // Iniciar la interfaz gráfica en el hilo principal
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    InterfazCLI::FormularioPrincipal formulario;
    Application::Run(% formulario);

    // Esperar a que el hilo del servidor termine (opcional)
    serverThread.join();

    return 0;
}