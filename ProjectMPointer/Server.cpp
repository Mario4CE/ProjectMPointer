#include "Server.h"
#include "ClienteManager.h"
#include "MemoryManager.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include "ActualizarRespuesta.h"

int startWinsock() {
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 0), &wsa);
}

int startServer() {
    long rc;
    SOCKET acceptSocket;
    SOCKADDR_IN addr;

    rc = startWinsock();
    if (rc != 0) {
        std::cout << "Error: startWinsock, c�digo de error: " << WSAGetLastError() << std::endl;
        std::string mensajeError = "Error: startWinsock, c�digo de error: ";
        ErrorLogger::logError(mensajeError);
        return 1;
    }
    else {
        std::cout << "�Winsock iniciado!\n";
        std::string mensajeInfo = "�Winsock iniciado!";
        InfoLogger::logInfo(mensajeInfo);
        acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (acceptSocket == INVALID_SOCKET) {
            std::cout << "Error: No se pudo crear el socket, c�digo de error: " << WSAGetLastError() << std::endl;
            std::string mensajeError = "Error: No se pudo crear el socket, c�digo de error: ";
            ErrorLogger::logError(mensajeError);
            return 1;
        }
        else {
            std::cout << "�Socket creado!\n";
            std::string mensajeInfo = "�Socket creado!";
            InfoLogger::logInfo(mensajeInfo);
            memset(&addr, 0, sizeof(SOCKADDR_IN));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);
            addr.sin_addr.s_addr = INADDR_ANY;
            rc = bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
            if (rc == SOCKET_ERROR) {
                std::cout << "Error: bind, c�digo de error: " << WSAGetLastError() << std::endl;
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
                    std::cout << "Error: listen, c�digo de error: " << WSAGetLastError() << std::endl;
                    std::string mensajeError = "Error: listen, c�digo de error: ";
                    ErrorLogger::logError(mensajeError);
                    return 1;
                }
                else {
                    std::cout << "acceptSocket est� en modo de escucha....\n";
                    std::string mensajeInfo = "acceptSocket est� en modo de escucha....";
                    InfoLogger::logInfo(mensajeInfo);
                    InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Esperando conexi�n...");
                    while (true) {
                        SOCKET connectedSocket = accept(acceptSocket, NULL, NULL);
                        if (connectedSocket == INVALID_SOCKET) {
                            std::cout << "Error: accept, c�digo de error: " << WSAGetLastError() << std::endl;
                            std::string mensajeError = "Error: accept, c�digo de error: ";
                            ErrorLogger::logError(mensajeError);
                            continue;
                        }
                        else {
                            std::cout << "�Nueva conexi�n aceptada!\n";
                            std::string mensajeInfo = "�Nueva conexi�n aceptada!";
                            InfoLogger::logInfo(mensajeInfo);
                            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Conexi�n establecida");
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

bool sendToClient(SOCKET clientSocket, const std::string& message) {
    if (clientSocket == INVALID_SOCKET) {
        std::string errorMsg = "Error: Intento de enviar a socket inv�lido";
        ErrorLogger::logError(errorMsg);
        return false;
    }

    // Primero enviamos la longitud del mensaje
    uint32_t messageLength = htonl(static_cast<uint32_t>(message.size()));
    int rc = send(clientSocket, reinterpret_cast<const char*>(&messageLength), sizeof(messageLength), 0);

    if (rc == SOCKET_ERROR) {
        std::string errorMsg = "Error al enviar longitud de mensaje, c�digo: " + std::to_string(WSAGetLastError());
        ErrorLogger::logError(errorMsg);
        return false;
    }

    // Luego enviamos el mensaje mismo
    rc = send(clientSocket, message.c_str(), static_cast<int>(message.size()), 0);

    if (rc == SOCKET_ERROR) {
        std::string errorMsg = "Error al enviar mensaje al cliente, c�digo: " + std::to_string(WSAGetLastError());
        ErrorLogger::logError(errorMsg);
        return false;
    }

    std::string infoMsg = "Mensaje enviado al cliente: " + message;
    InfoLogger::logInfo(infoMsg);
    return true;
}