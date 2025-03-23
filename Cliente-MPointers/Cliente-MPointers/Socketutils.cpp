
#include "SocketUtils.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <stdexcept>

#pragma comment(lib, "ws2_32.lib")

std::string SocketUtils::sendRequest(const std::string& address, int port, const std::string& request) {
    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("Error al inicializar Winsock.");
    }

    // Crear el socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        WSACleanup();
        throw std::runtime_error("Error al crear el socket.");
    }

    // Configurar la direcci�n del servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    if (inet_pton(AF_INET, address.c_str(), &serverAddr.sin_addr) <= 0) {
        closesocket(clientSocket);
        WSACleanup();
        throw std::runtime_error("Direcci�n IP inv�lida.");
    }

    // Conectar al servidor
    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        WSACleanup();
        throw std::runtime_error("Error al conectar con el servidor. C�digo de error: " + std::to_string(error));
    }

    // Enviar la petici�n
    if (send(clientSocket, request.c_str(), static_cast<int>(request.size()), 0) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        WSACleanup();
        throw std::runtime_error("Error al enviar la petici�n. C�digo de error: " + std::to_string(error));
    }

    // Recibir la respuesta
    char buffer[1024] = { 0 }; // Inicializar el buffer con ceros
    std::string response;
    int bytesReceived;
    while ((bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
        response.append(buffer, bytesReceived);
    }
    if (bytesReceived == SOCKET_ERROR) {
        int error = WSAGetLastError();
        closesocket(clientSocket);
        WSACleanup();
        throw std::runtime_error("Error al recibir la respuesta. C�digo de error: " + std::to_string(error));
    }

    // Cerrar el socket y limpiar Winsock
    closesocket(clientSocket);
    WSACleanup();

    // Devolver la respuesta
    return response;
}