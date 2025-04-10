
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

std::string SocketUtils::sendRequest(const std::string& address, int port, const std::string& request) {
    static SOCKET clientSocket = INVALID_SOCKET;

    try {
        if (request == "Conectar") {
            // Inicializamos Winsock
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                throw std::runtime_error("Error al inicializar Winsock");
            }
            // Creamos el socket
            clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (clientSocket == INVALID_SOCKET) {
                throw std::runtime_error("Error al crear el socket");
            }
            // Configuramos la dirección del servidor
            sockaddr_in serverAddr = {};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            serverAddr.sin_addr.s_addr = inet_addr(address.c_str());
            if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) {
                throw std::runtime_error("Dirección IP inválida");
            }


            // Conectamos al servidor
            if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                ErrorLogger::logError("Error al conectar al servidor. Código: " + std::to_string(error));
                return "Error";
            }
            InfoLogger::logInfo("Conectado al servidor " + address + ":" + std::to_string(port));
            // Enviamos la solicitud de conexión
            std::string welcomeMessage = receiveWelcomeMessage(clientSocket, 5000, 3);
            std::cout << "Mensaje de bienvenida recibido: " << welcomeMessage << std::endl;
            return "Conexion establecida: " + welcomeMessage;
        }

        else {
            if (clientSocket == INVALID_SOCKET) {
                throw std::runtime_error("Socket no conectado");
            }

            if (send(clientSocket, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                ErrorLogger::logError("Error al enviar la petición. Código: " + std::to_string(error));
                return "Error";
            }

            // Esperamos datos para leer
            fd_set readfds;
            FD_ZERO(&readfds);
            FD_SET(clientSocket, &readfds);
            struct timeval timeout = { 10, 0 }; // 5 segundos

            int result = select(0, &readfds, nullptr, nullptr, &timeout);
            if (result > 0 && FD_ISSET(clientSocket, &readfds)) {
                char buffer[1024];
                int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
                if (bytesReceived > 0) {
                    return std::string(buffer, bytesReceived);
                }
                else {
                    return "Error: no se recibió respuesta";
                }
            }
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


// Updated definition in SocketUtils.cpp
static std::string receiveWelcomeMessage(SOCKET clientSocket, int timeoutMs, int maxRetries) {
    int attempt = 0;
    std::string welcomeMessage;

    while (attempt < maxRetries) {
        try {
            attempt++;
            InfoLogger::logInfo("Intento #" + std::to_string(attempt) + ": Esperando mensaje de bienvenida...");

            // Buffer para almacenar el mensaje
            char welcomeBuffer[1024];

            // Usamos select() para esperar datos con timeout (evita bloqueo infinito)
            fd_set readSet;
            FD_ZERO(&readSet);
            FD_SET(clientSocket, &readSet);

            struct timeval timeout;
            timeout.tv_sec = timeoutMs / 1000;
            timeout.tv_usec = (timeoutMs % 1000) * 1000;

            // No need to cast clientSocket to int anymore
            int selectResult = select(0, &readSet, nullptr, nullptr, &timeout);

            if (selectResult == -1) {
                throw std::runtime_error("Error en select(): " + std::string(strerror(errno)));
            }
            else if (selectResult == 0) {
                throw std::runtime_error("Timeout esperando mensaje de bienvenida.");
            }

            // Si hay datos disponibles, los recibimos
            int welcomeBytesReceived = recv(clientSocket, welcomeBuffer, sizeof(welcomeBuffer), 0);

            if (welcomeBytesReceived > 0) {
                welcomeMessage.assign(welcomeBuffer, welcomeBytesReceived);
                InfoLogger::logInfo("Mensaje de bienvenida recibido:" + welcomeMessage);
                return welcomeMessage;
            }
            else if (welcomeBytesReceived == 0) {
                throw std::runtime_error("El servidor cerró la conexión.");
            }
            else {
                throw std::runtime_error("Error en recv(): " + std::string(strerror(errno)));
            }
        }
        catch (const std::exception& e) {
            std::string mensajeError = "Error en intento #" + std::to_string(attempt) + " - " + e.what();
            ErrorLogger::logError(mensajeError);

            if (attempt >= maxRetries) {
                throw std::runtime_error("Fallo al recibir mensaje de bienvenida después de " +
                    std::to_string(maxRetries) + " intentos: " + e.what());
            }

            // Pequeña espera antes del reintento
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

    throw std::runtime_error("Error desconocido en receiveWelcomeMessage.");
}

