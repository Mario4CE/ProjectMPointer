#include "Cliente.h"
#include "MPointer.h"
#include "SocketUtils.h" // Incluir la clase SocketUtils
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

    // Intentar conectar al servidor y enviar una petición de prueba
    try {
        std::string response = SocketUtils::sendRequest("127.0.0.1", 12345, "Petición de prueba"); // Reemplaza con la IP y el puerto de tu servidor
        std::cout << "Respuesta del servidor: " << response << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error de conexión: " << e.what() << std::endl;
    }

    Application::Run(% form);

    return 0;
}