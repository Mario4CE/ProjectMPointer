
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>
#include <sstream> // Para construir la respuesta

#pragma comment(lib, "ws2_32.lib")

/*
* Enviar una petici�n a un servidor remoto y recibir la respuesta.
* address: Direcci�n IP del servidor.
* port: Puerto del servidor.
*/
std::string SocketUtils::sendRequest(const std::string& address, int port, const std::string& request) {
    // Variable para rastrear el estado de la conexi�n
    static SOCKET clientSocket = INVALID_SOCKET;

    try {
        if (request == "Conectar") {
            // Inicializar Winsock si es la primera conexi�n
            WSADATA wsaData;
            if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
                std::string mensajeError = "Error al inicializar Winsock. C�digo de error: " + std::to_string(WSAGetLastError());
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("Error al inicializar Winsock.");
            }

            // Crear el socket
            clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket == INVALID_SOCKET) {
                std::string mensajeError = "Error al crear el socket. C�digo de error: " + std::to_string(WSAGetLastError());
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("Error al crear el socket.");
            }

            // Configurar un timeout de 10 segundos para recv
            struct timeval timeout = { 10, 0 }; // 10 segundos
            if (setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
                std::string mensajeError = "Error al configurar el timeout del socket. C�digo de error: " + std::to_string(WSAGetLastError());
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("Error al configurar el timeout del socket.");
            }

            // Configurar la direcci�n del servidor
            sockaddr_in serverAddr = {};
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port);
            if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) {
                std::string mensajeError = "Direcci�n IP inv�lida: " + address;
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("Direcci�n IP inv�lida.");
            }

            // Conectar al servidor
            if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                std::string mensajeError = "Error al conectar con el servidor. C�digo de error: " + std::to_string(error);
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("Error al conectar con el servidor. C�digo de error: " + std::to_string(error));
            }

            return "Conexi�n establecida.";
        }
        else {
            // Verificar si hay una conexi�n establecida
            if (clientSocket == INVALID_SOCKET) {
                return "Error: No hay conexi�n establecida.";
            }

            // Enviar la petici�n
            if (send(clientSocket, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR) {
                int error = WSAGetLastError();
                std::string mensajeError = "Error al enviar la petici�n. C�digo de error: " + std::to_string(error);
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("Error al enviar la petici�n. C�digo de error: " + std::to_string(error));
            }

            // Recibir la respuesta
            std::stringstream responseStream; // Usar stringstream para construir la respuesta
            char buffer[1024];
            int bytesReceived;
            while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
                responseStream.write(buffer, bytesReceived); // Escribir en el stringstream
            }

            if (bytesReceived == SOCKET_ERROR) {
                int error = WSAGetLastError();
                std::string mensajeError = "Error al recibir la respuesta. C�digo de error: " + std::to_string(error);
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("Error al recibir la respuesta. C�digo de error: " + std::to_string(error));
            }

            // Devolver la respuesta
            return responseStream.str(); // Convertir el stringstream a string
        }
    }
    catch (...) {
        if (clientSocket != INVALID_SOCKET) {
            closesocket(clientSocket);
            clientSocket = INVALID_SOCKET; // Resetear el socket en caso de error
        }
        WSACleanup();
        throw; // Re-lanzar la excepci�n
    }
}