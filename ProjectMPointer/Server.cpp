
#include "Server.h"
#include "ClienteManager.h"
#include "MemoryManager.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include "ActualizarRespuesta.h"
#include <thread> // Para manejar clientes en hilos separados
#include <fcntl.h>

/*
* Start Winsock
*/

int startWinsock() {
    WSADATA wsa;
    int result = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (result != 0) {
        ErrorLogger::logError("Error: startWinsock, código de error: " + std::to_string(result));
        return 1;
    }
    else {
        InfoLogger::logInfo("¡Winsock iniciado!");
        return 0;
    }
}

/*
* Función para manejar un cliente
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
        ErrorLogger::logError("Error: No se pudo crear el socket, código de error: " + std::to_string(error));
        WSACleanup();
        return 1;
    }
    else {
        InfoLogger::logInfo("¡Socket creado!");
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // Asociar el socket al puerto

    if (bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        ErrorLogger::logError("Error: bind, código de error: " + std::to_string(error));
        closesocket(acceptSocket);
        WSACleanup();
        return 1;
    }
    else {
        InfoLogger::logInfo("Socket asociado al puerto " + std::to_string(PORT));
    }

    // Poner el socket en modo de escucha

    if (listen(acceptSocket, MAX_PENDING_CONNECTIONS) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        ErrorLogger::logError("Error: listen, código de error: " + std::to_string(error));
        closesocket(acceptSocket);
        WSACleanup();
        return 1;
    }
    else {
        InfoLogger::logInfo("acceptSocket está en modo de escucha....");
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Esperando conexión...");
    }

    // Aceptar conexiones entrantes
    while (true) {
        SOCKET connectedSocket = accept(acceptSocket, NULL, NULL);
        if (connectedSocket == INVALID_SOCKET) {
            int error = WSAGetLastError();
            ErrorLogger::logError("Error: accept, código de error: " + std::to_string(error));
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error al aceptar conexión");
            continue; // Continúa escuchando nuevas conexiones
        }
        else {
            InfoLogger::logInfo("¡Nueva conexión aceptada!");
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Conexión establecida");

            // Enviar mensaje de bienvenida al cliente
            std::string mensajeBienvenida = "¡Bienvenido al servidor!";
            if (!sendToClient(connectedSocket, mensajeBienvenida)) {
                ErrorLogger::logError("Error al enviar mensaje de bienvenida. Cerrando conexión.");
                closesocket(connectedSocket); // Cerrar el socket si falla el envío
                continue; // Continúa escuchando nuevas conexiones
            }

            // Iniciar el hilo para manejar el cliente
            std::thread clientThread(ClienteManager::handleClient, connectedSocket);
            clientThread.detach();
        }
    }

    closesocket(acceptSocket);
    WSACleanup();
    return 0;
}

/*
* Metodo que llama a sendToClientel mensaje para enviarle
* Este metodo se va a usar desde otras clases
*/

bool  enviarComando(const std::string& message) {
    SOCKET clientSocket = 12345;
    std::cout << "Valor de clientSocket: " << clientSocket << std::endl;
    if (clientSocket == INVALID_SOCKET) {
        ErrorLogger::logError("Intento de enviar a socket inválido en enviar comando");
        return false;
    }
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(clientSocket, &writefds);
    timeval timeout = { 5, 0 };
    return sendToClient(clientSocket, message);
}

/*
* Función para manejar un cliente
* Se encarga de recibir los mensajes del cliente y procesarlos
* Se encarga de enviar mensajes al cliente
*/

bool sendToClient(SOCKET clientSocket, const std::string& message) {
    if (clientSocket == INVALID_SOCKET) {
        ErrorLogger::logError("❌ Intento de enviar a un socket inválido.");
        return false;
    }

    // Comprobar si el socket sigue conectado antes de enviar datos
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(clientSocket, &writefds);
    timeval timeout = { 10, 0 }; // Esperar hasta 10 segundos

    int result = select(0, NULL, &writefds, NULL, &timeout);
    if (result <= 0) { // Error o timeout
        int error = WSAGetLastError();
        ErrorLogger::logError("❌ Socket no está listo para escritura... Código de error: " + std::to_string(error));
        closesocket(clientSocket); // Cerrar el socket para evitar errores futuros
        return false;
    }

    // Enviar longitud del mensaje
    uint32_t messageLength = htonl(static_cast<uint32_t>(message.size()));
    if (send(clientSocket, reinterpret_cast<const char*>(&messageLength), sizeof(messageLength), 0) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        ErrorLogger::logError("❌ Error al enviar longitud del mensaje. Código: " + std::to_string(error));
        closesocket(clientSocket);
        return false;
    }

    // Enviar mensaje completo
    if (send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0) == SOCKET_ERROR) {
        int error = WSAGetLastError();
        ErrorLogger::logError("❌ Error al enviar mensaje. Código: " + std::to_string(error));
        closesocket(clientSocket);
        return false;
    }

    InfoLogger::logInfo("✅ Mensaje enviado correctamente.");
    return true;
}


int obtenerSocket() {
    return MemoryManager::getInstance().getClientSocket();
}
