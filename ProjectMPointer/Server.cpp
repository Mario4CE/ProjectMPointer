#include "Server.h"

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
        return 1;
    }
    else {
        std::cout << "�Winsock iniciado!\n";
        acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (acceptSocket == INVALID_SOCKET) {
            std::cout << "Error: No se pudo crear el socket, c�digo de error: " << WSAGetLastError() << std::endl;
            return 1;
        }
        else {
            std::cout << "�Socket creado!\n";
            memset(&addr, 0, sizeof(SOCKADDR_IN));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(PORT);
            addr.sin_addr.s_addr = INADDR_ANY;
            rc = bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
            if (rc == SOCKET_ERROR) {
                std::cout << "Error: bind, c�digo de error: " << WSAGetLastError() << std::endl;
                return 1;
            }
            else {
                std::cout << "Socket asociado al puerto " << PORT << "\n";
                rc = listen(acceptSocket, MAX_PENDING_CONNECTIONS);
                if (rc == SOCKET_ERROR) {
                    std::cout << "Error: listen, c�digo de error: " << WSAGetLastError() << std::endl;
                    return 1;
                }
                else {
                    std::cout << "acceptSocket est� en modo de escucha....\n";
                    while (true) {
                        SOCKET connectedSocket = accept(acceptSocket, NULL, NULL);
                        if (connectedSocket == INVALID_SOCKET) {
                            std::cout << "Error: accept, c�digo de error: " << WSAGetLastError() << std::endl;
                            continue;
                        }
                        else {
                            std::cout << "�Nueva conexi�n aceptada!\n";
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