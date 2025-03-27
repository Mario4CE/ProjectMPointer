#include "ClienteManager.h"
#include "Server.h"
#include "MemoryManager.h"
#include "interfaz.h"
#include <iostream>
#include <string>
#include "ErrorLogger.h"
#include "InfoLogger.h"

void handleClient(SOCKET clientSocket) {
    char buffer[1024];

    try {
        while (true) {
            memset(buffer, 0, sizeof(buffer));
            int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

            if (bytesReceived <= 0) {
                int error = WSAGetLastError();
                if (error == WSAECONNRESET || error == WSAECONNABORTED) {
                    InfoLogger::logInfo("Cliente cerró la conexión abruptamente");
                }
                break;
            }

            std::string request(buffer, bytesReceived);
            std::string response = MemoryManager::processRequest(request);

            if (!sendToClient(clientSocket, response)) {
                break; // Si falla el envío, salir del bucle
            }
        }
    }
    catch (...) {
        ErrorLogger::logError("Excepción en handleClient");
    }

    // Cierre limpio
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
}