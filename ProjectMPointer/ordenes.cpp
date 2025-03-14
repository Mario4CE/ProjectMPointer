#include "ordenes.h"
#include "Server.h"
#include "MemoryManager.h"
#include <iostream>
#include <string>

void handleClient(SOCKET clientSocket) {
    char buffer[1024];
    int bytesReceived;

    while (true) {
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) {
            std::cout << "Cliente desconectado o error en la recepción.\n";
            break;
        }

        // Convertir el buffer recibido a un string
        std::string request(buffer, bytesReceived);
        std::cout << "Petición recibida: " << request << std::endl;

        // Procesar la petición
        std::string response = MemoryManager::processRequest(request);

        // Enviar la respuesta al cliente
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    closesocket(clientSocket);
}