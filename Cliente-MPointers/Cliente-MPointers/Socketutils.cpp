
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <thread> // Para hilos
#include <chrono> // Para timeouts

#pragma comment(lib, "ws2_32.lib")

// Función para recibir mensajes del servidor en un hilo separado
void receiveMessages(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        std::string message(buffer, bytesReceived);
        std::cout << "Mensaje del servidor: " << message << std::endl;
    }
    if (bytesReceived == SOCKET_ERROR) {
        int error = WSAGetLastError();
        std::string errorMessage = "Error al recibir mensaje del servidor. Código de error: " + std::to_string(error);
        ErrorLogger::logError(errorMessage);
    }
}

std::string SocketUtils::sendRequest(const std::string& address, int port, const std::string& request) {
    static SOCKET clientSocket = INVALID_SOCKET;

    try {
        if (request == "Conectar") {
            // Inicialización de Winsock y creación del socket (como antes)
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { /* ... */ }
            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket == INVALID_SOCKET) { /* ... */ }

            // Configuración de la dirección del servidor y conexión (como antes)
            sockaddr_in serverAddr = {};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) { /* ... */ }
            if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) { /* ... */ }

            // Esperar 10 segundos por el mensaje de bienvenida
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(clientSocket, &readfds);
            struct timeval timeout = { 10, 0 }; // 10 segundos
            int result = select(0, &readfds, nullptr, nullptr, &timeout);
            if (result > 0) {
                // Mensaje de bienvenida recibido
                char welcomeBuffer[1024];
                int welcomeBytesReceived = recv(clientSocket, welcomeBuffer, sizeof(welcomeBuffer), 0);
                if (welcomeBytesReceived > 0) {
                    std::string welcomeMessage(welcomeBuffer, welcomeBytesReceived);
                    std::cout << "Mensaje de bienvenida recibido: " << welcomeMessage << std::endl;
                }
            }
            else if (result == 0) {
                std::cout << "No se recibió mensaje de bienvenida en 10 segundos." << std::endl;
            }
            else {
                int error = WSAGetLastError();
                std::string errorMessage = "Error al recibir mensaje de bienvenida. Código de error: " + std::to_string(error);
                ErrorLogger::logError(errorMessage);
                throw std::runtime_error("Error al recibir mensaje de bienvenida.");
            }

            // Iniciar hilo para recibir mensajes del servidor
            std::thread receiveThread(receiveMessages, clientSocket);
            receiveThread.detach();

            return "Conexión establecida.";
        }
        else {
            // Envío de la petición y recepción de la respuesta (como antes)
            if (clientSocket == INVALID_SOCKET) { /* ... */ }
            if (send(clientSocket, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR) { /* ... */ }

            std::stringstream responseStream;
            char buffer[1024];
            int bytesReceived;
            while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
                responseStream.write(buffer, bytesReceived);
            }
            if (bytesReceived == SOCKET_ERROR) { /* ... */ }

            return responseStream.str();
        }
    }
    catch (...) {
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET;
        }
        WSACleanup();
        throw;
    }
}