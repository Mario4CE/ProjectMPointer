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
    int bytesReceived; // N�mero de bytes recibidos

    // Configurar el MemoryManager para enviar mensajes a este cliente
    MemoryManager::getInstance().setClientSocket(clientSocket);

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

        // Procesar la petici�n - ahora usar� sendToClient internamente si es necesario
        std::string response = MemoryManager::processRequest(request);

        // Enviar la respuesta final al cliente usando nuestra funci�n mejorada
        if (!sendToClient(clientSocket, response)) {
            std::string mensajeError = "Error al enviar la respuesta al cliente.";
            ErrorLogger::logError(mensajeError);
            break;
        }
    }

    // Limpiar el socket del cliente al desconectarse
    MemoryManager::getInstance().setClientSocket(INVALID_SOCKET);
    closesocket(clientSocket);
}
