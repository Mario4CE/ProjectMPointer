
#include "Server.h"
#include "ClienteManager.h"
#include "MemoryManager.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include "ActualizarRespuesta.h"
#include <thread> // Para manejar clientes en hilos separados

/*
* Start Winsock
*/

int startWinsock() {
    WSADATA wsa;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (result != 0) {
        std::cout << "Error: startWinsock, c�digo de error: " << result << std::endl;
        ErrorLogger::logError("Error: startWinsock, c�digo de error: " + std::to_string(result));
        return 1;
    }
    else {
        std::cout << "�Winsock iniciado!\n";
        InfoLogger::logInfo("�Winsock iniciado!");
        return 0;
    }
}

/*
* Funci�n para manejar un cliente
* Inicializa al server
*/
int startServer() {
    SOCKET acceptSocket = INVALID_SOCKET;
    SOCKADDR_IN addr = {};

    if (startWinsock() != 0) {
        return 1;
    }
    // Crear el socket
    acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (acceptSocket == INVALID_SOCKET) {
        int error = WSAGetLastError();
        std::cout << "Error: No se pudo crear el socket, c�digo de error: " << error << std::endl;
        ErrorLogger::logError("Error: No se pudo crear el socket, c�digo de error: " + std::to_string(error));
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "�Socket creado!\n";
        InfoLogger::logInfo("�Socket creado!");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Asociar el socket al puerto

    if (bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        std::cout << "Error: bind, c�digo de error: " << error << std::endl;
        ErrorLogger::logError("Error: bind, c�digo de error: " + std::to_string(error));
        closesocket(acceptSocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "Socket asociado al puerto " << PORT << "\n";
        InfoLogger::logInfo("Socket asociado al puerto " + std::to_string(PORT));
    }

    // Poner el socket en modo de escucha

    if (listen(acceptSocket, MAX_PENDING_CONNECTIONS) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        std::cout << "Error: listen, c�digo de error: " << error << std::endl;
        ErrorLogger::logError("Error: listen, c�digo de error: " + std::to_string(error));
        closesocket(acceptSocket);
        WSACleanup();
        return 1;
    }
    else {
        std::cout << "acceptSocket est� en modo de escucha....\n";
        InfoLogger::logInfo("acceptSocket est� en modo de escucha....");
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Esperando conexi�n...");
    }

    // Aceptar conexiones entrantes

    // Aceptar conexiones entrantes
    while (true) {
        SOCKET connectedSocket = accept(acceptSocket, NULL, NULL);
        if (connectedSocket == INVALID_SOCKET) {
            int error = WSAGetLastError();
            std::cout << "Error: accept, c�digo de error: " << error << std::endl;
            ErrorLogger::logError("Error: accept, c�digo de error: " + std::to_string(error));
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error al aceptar conexi�n");
            continue; // Contin�a escuchando nuevas conexiones
        }
        else {
            std::cout << "�Nueva conexi�n aceptada!!!!!!\n";
            InfoLogger::logInfo("�Nueva conexi�n aceptada!");
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Conexi�n establecida");

            // Enviar mensaje al cliente
            std::string mensajeBienvenida = "�Bienvenido al servidor!";
            if (!sendToClient(connectedSocket, mensajeBienvenida)) {
                std::cout << "Error al enviar mensaje de bienvenida. Cerrando conexi�n.\n";
                ErrorLogger::logError("Error al enviar mensaje de bienvenida. Cerrando conexi�n.");
                closesocket(connectedSocket); // Cerrar el socket si falla el env�o
                continue; // Contin�a escuchando nuevas conexiones
            }
            else {
                std::cout << "Mensaje de bienvenida enviado.\n";
                InfoLogger::logInfo("Mensaje de bienvenida enviado.");
            }

            std::thread clientThread(handleClient, connectedSocket);
            clientThread.detach();
        }
    }

    closesocket(acceptSocket);
    WSACleanup();
    return 0;
}

/*
* Funci�n para manejar un cliente
* Se encarga de recibir los mensajes del cliente y procesarlos
* Se encarga de enviar mensajes al cliente
*/
bool sendToClient(SOCKET clientSocket, const std::string& message) {
    if (clientSocket == INVALID_SOCKET) {
        ErrorLogger::logError("Intento de enviar a socket inv�lido");
        return false;
    }

    // Verificar si el socket sigue conectado
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(clientSocket, &writefds);
    timeval timeout = { 1, 0 }; // Timeout de 1 segundo

    if (select(0, NULL, &writefds, NULL, &timeout) <= 0) {
        ErrorLogger::logError("Socket no est� listo para escritura o error de conexi�n");
        closesocket(clientSocket); // Cerrar el socket si la conexi�n est� rota
        return false;
    }

    // Resto de tu implementaci�n de env�o...
    uint32_t messageLength = htonl(static_cast<uint32_t>(message.size()));
    int rc = send(clientSocket, reinterpret_cast<const char*>(&messageLength), sizeof(messageLength), 0);

    if (rc == SOCKET_ERROR) {
        int error = WSAGetLastError();
        ErrorLogger::logError("Error al enviar longitud: " + std::to_string(error));
        closesocket(clientSocket); // Cerrar el socket si hay un error
        return false;
    }

    rc = send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);
    if (rc == SOCKET_ERROR) {
        int error = WSAGetLastError();
        ErrorLogger::logError("Error al enviar mensaje: " + std::to_string(error));
        closesocket(clientSocket); // Cerrar el socket si hay un error
        return false;
    }

    return true;
}
