#include "ordenes.h"
#include "Server.h"
#include "MemoryManager.h"
#include <iostream>
#include <string>
#include "ErrorLogger.h"
#include "InfoLogger.h"

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Cliente desconectado o error en la recepción.\n";
            std::string mensajeError = "Cliente desconectado o error en la recepción.";
            break;
        }

        // Convertir el buffer recibido a un string
        std::string request(buffer, bytesReceived);
        std::cout << "Petición recibida: " << request << std::endl;
        std::string mensajeInfo = "Petición recibida: " + request;
        InfoLogger::logInfo(mensajeInfo);

        // Procesar la petición
        std::string response = MemoryManager::processRequest(request);

        // Enviar la respuesta al cliente
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    closesocket(clientSocket);
}