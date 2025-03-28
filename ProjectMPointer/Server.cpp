#include "Server.h"
#include "ClienteManager.h"
#include "MemoryManager.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include "ActualizarRespuesta.h"

/*
* Start Winsock
*/
int startWinsock() {
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 0), &wsa);
}

/*
* Función para manejar un cliente
* Inicializa al server
*/
int startServer() {
    long rc = 0; // Initialize rc
    SOCKET acceptSocket = INVALID_SOCKET; // Initialize acceptSocket
    SOCKADDR_IN addr = {}; // Initialize addr

    rc = startWinsock();
    if (rc != 0) {
        std::cout << "Error: startWinsock, código de error: " << WSAGetLastError() << std::endl;
        std::string mensajeError = "Error: startWinsock, código de error: ";
        ErrorLogger::logError(mensajeError);
        return 1;
    }
    else {
        std::cout << "¡Winsock iniciado!\n";
        std::string mensajeInfo = "¡Winsock iniciado!";
        InfoLogger::logInfo(mensajeInfo);
        acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (acceptSocket == INVALID_SOCKET) {
            std::cout << "Error: No se pudo crear el socket, código de error: " << WSAGetLastError() << std::endl;
            std::string mensajeError = "Error: No se pudo crear el socket, código de error: ";
            ErrorLogger::logError(mensajeError);
            return 1;
        }
        else {
            std::cout << "¡Socket creado!\n";
            std::string mensajeInfo = "¡Socket creado!";
            InfoLogger::logInfo(mensajeInfo);
            memset(&addr, 0, sizeof(SOCKADDR_IN));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);
            addr.sin_addr.s_addr = INADDR_ANY;
            rc = bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
            if (rc == SOCKET_ERROR) {
                std::cout << "Error: bind, código de error: " << WSAGetLastError() << std::endl;
                std::string mensajeError = "Error: bind, codigo de error: ";
                ErrorLogger::logError(mensajeError);
                return 1;
            }
            else {
                std::cout << "Socket asociado al puerto " << PORT << "\n";

                std::string mensajeInfo = "Socket asociado al puerto " + std::to_string(PORT);
                InfoLogger::logInfo(mensajeInfo);
                rc = listen(acceptSocket, MAX_PENDING_CONNECTIONS);
                if (rc == SOCKET_ERROR) {
                    std::cout << "Error: listen, código de error: " << WSAGetLastError() << std::endl;
                    std::string mensajeError = "Error: listen, código de error: ";
                    ErrorLogger::logError(mensajeError);
                    return 1;
                }
                else {
                    std::cout << "acceptSocket está en modo de escucha....\n";
                    std::string mensajeInfo = "acceptSocket está en modo de escucha....";
                    InfoLogger::logInfo(mensajeInfo);
                    InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Esperando conexión...");
                    while (true) {
                        SOCKET connectedSocket = accept(acceptSocket, NULL, NULL);
                        if (connectedSocket == INVALID_SOCKET) {
                            std::cout << "Error: accept, código de error: " << WSAGetLastError() << std::endl;
                            std::string mensajeError = "Error: accept, código de error: ";
                            ErrorLogger::logError(mensajeError);
                            continue;
                        }
                        else {
                            std::cout << "¡Nueva conexión aceptada!\n";
                            std::string mensajeInfo = "¡Nueva conexión aceptada!";
                            InfoLogger::logInfo(mensajeInfo);
                            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Conexión establecida");
                            handleClient(connectedSocket);
                        }
                    }
                }
            }
        }
    }
    closesocket(acceptSocket);
    WSACleanup();
    return 0;
}

/*
* Función para manejar un cliente
* Se encarga de recibir los mensajes del cliente y procesarlos
* Se encarga de enviar mensajes al cliente
*/
bool sendToClient(SOCKET clientSocket, const std::string& message) {
    if (clientSocket == INVALID_SOCKET) {
        ErrorLogger::logError("Intento de enviar a socket inválido");
        return false;
    }

    // Verificar si el socket sigue conectado
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(clientSocket, &writefds);
    timeval timeout = { 1, 0 }; // Timeout de 1 segundo

    if (select(0, NULL, &writefds, NULL, &timeout) <= 0) {
        ErrorLogger::logError("Socket no está listo para escritura o error de conexión");
        return false;
    }

    // Resto de tu implementación de envío...
    uint32_t messageLength = htonl(static_cast<uint32_t>(message.size()));
    int rc = send(clientSocket, reinterpret_cast<const char*>(&messageLength), sizeof(messageLength), 0);

    if (rc == SOCKET_ERROR) {
        int error = WSAGetLastError();
        ErrorLogger::logError("Error al enviar longitud: " + std::to_string(error));
        return false;
    }

    rc = send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);
    if (rc == SOCKET_ERROR) {
        ErrorLogger::logError("Error al enviar mensaje: " + std::to_string(WSAGetLastError()));
        return false;
    }

    return true;
}
