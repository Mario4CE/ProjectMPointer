#include "Cliente.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <msclr/marshal_cppstd.h> // Para convertir System::String^ a std::string

#pragma comment(lib, "ws2_32.lib")

namespace ClienteMPointers {

    // Manejador de eventos para el clic del botón
    System::Void Cliente::btnCliente_Click(System::Object^ sender, System::EventArgs^ e) {
        // Obtener la petición del TextBox
        System::String^ peticion = this->txtPeticion->Text;

        // Convertir System::String^ a std::string
        std::string peticionStr = msclr::interop::marshal_as<std::string>(peticion);

        // Enviar la petición al servidor
        std::string respuesta = EnviarPeticion(peticionStr);

        // Mostrar la respuesta en el Label
        this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
    }

    // Función para enviar una petición al servidor
    std::string Cliente::EnviarPeticion(const std::string& peticion) {
        // Inicializar Winsock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return "Error al inicializar Winsock.";
        }

        // Crear el socket del cliente
        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            WSACleanup();
            return "Error al crear el socket del cliente.";
        }

        // Configurar la dirección del servidor
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(12345); // Puerto del servidor
        inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr); // Dirección IP del servidor

        // Conectar al servidor
        if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
            closesocket(clientSocket);
            WSACleanup();
            return "Error al conectar con el servidor.";
        }

        // Enviar la petición al servidor
        if (send(clientSocket, peticion.c_str(), peticion.size(), 0) == SOCKET_ERROR) {
            closesocket(clientSocket);
            WSACleanup();
            return "Error al enviar la petición al servidor.";
        }

        // Recibir la respuesta del servidor
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            closesocket(clientSocket);
            WSACleanup();
            return "Error al recibir la respuesta del servidor.";
        }

        // Cerrar el socket y limpiar Winsock
        closesocket(clientSocket);
        WSACleanup();

        // Devolver la respuesta del servidor
        return std::string(buffer, bytesReceived);
    }
}