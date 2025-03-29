
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

std::string SocketUtils::sendRequest(const std::string& address, int port, const std::string& request) {
    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::string mensajeError = "Error al inicializar Winsock. Código de error: " + std::to_string(WSAGetLastError());
        ErrorLogger::logError(mensajeError);
        throw std::runtime_error("Error al inicializar Winsock.");
    }

    // Crear el socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        WSACleanup();
        std::string mensajeError = "Error al crear el socket. Código de error: " + std::to_string(WSAGetLastError());
        ErrorLogger::logError(mensajeError);
        throw std::runtime_error("Error al crear el socket.");
    }

    // Configurar un timeout de 5 segundos para recv
    struct timeval timeout = { 0 }; // Initialize the struct
    timeout.tv_sec = 5; // 5 segundos
    timeout.tv_usec = 0;
    setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));

    // Configurar la dirección del servidor
    sockaddr_in serverAddr = {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) {
        closesocket(clientSocket);
        WSACleanup();
        std::string mensajeError = "Dirección IP inválida: " + address;
        ErrorLogger::logError(mensajeError);
        throw std::runtime_error("Dirección IP inválida.");
    }

    // Conectar al servidor
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        WSACleanup();
        std::string mensajeError = "Error al conectar con el servidor. Código de error: " + std::to_string(error);
        ErrorLogger::logError(mensajeError);
        throw std::runtime_error("Error al conectar con el servidor. Código de error: " + std::to_string(error));
    }

    // Enviar la petición
    if (send(clientSocket, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        WSACleanup();
        std::string mensajeError = "Error al enviar la petición. Código de error: " + std::to_string(error);
        ErrorLogger::logError(mensajeError);
        throw std::runtime_error("Error al enviar la petición. Código de error: " + std::to_string(error));
    }

    // Recibir la respuesta
    std::string response;
    char buffer[1024];
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        response.append(buffer, bytesReceived);
    }

    if (bytesReceived == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        WSACleanup();
        std::string mensajeError = "Error al recibir la respuesta. Código de error: " + std::to_string(error);
        ErrorLogger::logError(mensajeError);
        throw std::runtime_error("Error al recibir la respuesta. Código de error: " + std::to_string(error));
    }

    // Cerrar el socket y limpiar Winsock
    closesocket(clientSocket);
    WSACleanup();

    // Devolver la respuesta
    return response;
}
