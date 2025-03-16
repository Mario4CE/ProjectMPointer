#include "Server.h"
#include "interfaz.h"
#include "ordenes.h"
#include "MemoryManager.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include <thread>
#include <filesystem>
#include <iostream>

using namespace System;
using namespace System::Windows::Forms;

[STAThreadAttribute]
int main(array<System::String^>^ args)

{
    // Probar los loggers antes de iniciar el servidor y la interfaz
    std::cout << "Probando los loggers..." << std::endl;

    // Mensajes de prueba
    std::string mensajeError = "Este es un mensaje de error de prueba desde el main.";
    std::string mensajeInfo = "Este es un mensaje de información de prueba desde el main.";

    // Registrar los mensajes en los loggers
    ErrorLogger::logError(mensajeError);
    InfoLogger::logInfo(mensajeInfo);

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