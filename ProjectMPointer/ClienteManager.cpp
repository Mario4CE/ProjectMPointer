#include "ClienteManager.h"
#include "Server.h"
#include "MemoryManager.h"
#include "interfaz.h"
#include <iostream>
#include <string>
#include "ErrorLogger.h"
#include "InfoLogger.h"

void handleClient(SOCKET clientSocket) {
    char buffer[1024]; // Buffer para recibir datos
    int bytesReceived; // N?mero de bytes recibidos

    while (true) {
        // Limpiar el buffer antes de recibir nuevos datos
        memset(buffer, 0, sizeof(buffer));

        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Cliente desconectado o error en la recepci�n.\n";
            std::string mensajeError = "Cliente desconectado o error en la recepci�n.";
            ErrorLogger::logError(mensajeError);
            break;
        }

        // Convertir el buffer recibido a un string
        std::string request(buffer, bytesReceived);
        std::cout << "Petici�n recibida: " << request << std::endl;
        std::string mensajeInfo = "Petici�n recibida: " + request;
        InfoLogger::logInfo(mensajeInfo);

        // Procesar la petici�n
        std::string response = MemoryManager::processRequest(request);

        // Enviar la respuesta al cliente
        int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        if (bytesSent == SOCKET_ERROR) {
            std::string mensajeError = "Error al enviar la respuesta al cliente.";
            ErrorLogger::logError(mensajeError);
            break;
        }

    }

    closesocket(clientSocket);
}