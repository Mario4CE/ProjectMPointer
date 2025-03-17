#include "Server.h"
#include "ordenes.h"
#include "MemoryManager.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"

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