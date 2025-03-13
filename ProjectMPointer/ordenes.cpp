#include "ordenes.h"
#include "Server.h"
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

        std::string receivedData(buffer, bytesReceived);
        std::cout << "Datos recibidos: " << receivedData << std::endl;

        std::string response = "Servidor recibió: " + receivedData;
        send(clientSocket, response.c_str(), response.length(), 0);
    }

    closesocket(clientSocket);
}