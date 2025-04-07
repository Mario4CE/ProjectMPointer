
#include "SocketUtils.h"
#include "ErrorLogger.h"
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

            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                throw std::runtime_error("Error al iniciar Winsock");
            }

            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket == INVALID_SOCKET) {
                throw std::runtime_error("Error al crear socket");
            }

            sockaddr_in serverAddr = {};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);

            if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) {
                throw std::runtime_error("Dirección IP inválida");
            }

            if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                throw std::runtime_error("Error al conectar con el servidor");
            }

            // Esperamos bienvenida
            char welcomeBuffer[1024];
            int welcomeBytesReceived = recv(clientSocket, welcomeBuffer, sizeof(welcomeBuffer), 0);
            if (welcomeBytesReceived > 0) {
                std::string welcomeMessage(welcomeBuffer, welcomeBytesReceived);
                std::cout << "Mensaje de bienvenida recibido: " << welcomeMessage << std::endl;
                return "Conexion establecida: " + welcomeMessage;
            }

            return "Conexion establecida.";
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
            struct timeval timeout = { 5, 0 }; // 5 segundos

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
            else if (result == 0) {
                ErrorLogger::logError("Tiempo de espera agotado al recibir respuesta");
                return "Error: timeout";
            }
            else {
                ErrorLogger::logError("Error en select()");
                return "Error";
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
