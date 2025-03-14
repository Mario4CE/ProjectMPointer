#include "Server.h"
#include "interfaz.h"
#include "ordenes.h"
#include "MemoryManager.h" 
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

    // Desacoplar el hilo del servidor para que no bloquee la ejecución de la interfaz gráfica
    serverThread.detach();

    // Simulación de peticiones que modifican la memoria (esto puede moverse al manejador de clientes)
    std::thread memoryThread([]() {
        // Simular algunas peticiones
        MemoryManager::processRequest("Asignar bloque 4 al proceso C");
        MemoryManager::processRequest("Liberar bloque 2");
        MemoryManager::processRequest("Asignar bloque 1 al proceso D");
        std::cout << "Peticiones simuladas. Verifica la carpeta 'logs' para ver los archivos generados." << std::endl;
        });

    // Desacoplar el hilo de manejo de memoria
    memoryThread.detach();


    // Iniciar la interfaz gráfica en el hilo principal
    Application::EnableVisualStyles();
    Application::SetCompatibleTextRenderingDefault(false);
    InterfazCLI::FormularioPrincipal formulario;
    Application::Run(% formulario);

    return 0;
}