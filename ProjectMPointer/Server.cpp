#include "Server.h"

int startWinsock(void) {
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 0), &wsa);
}

int startServer() {
    long rc;
    SOCKET acceptSocket;
    SOCKADDR_IN addr;

    rc = startWinsock();
    if (rc != 0) {
        std::cout << "Fehler: startWinsock, Fehlercode: " << GetLastError() << std::endl;
        return 1;
    }
    else {
        std::cout << "Winsock gestartet!\n";
        acceptSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (acceptSocket == INVALID_SOCKET) {
            std::cout << "Fehler: Der socket konnte nicht erstellt werden, Fehlercode: " << WSAGetLastError() << std::endl;
            return 1;
        }
        else {
            std::cout << "Socket erstellt!\n";
            memset(&addr, 0, sizeof(SOCKADDR_IN));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(12345);
            addr.sin_addr.s_addr = ADDR_ANY;
            rc = bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
            if (rc == SOCKET_ERROR) {
                std::cout << "Fehler: bind, Fehlercode: " << WSAGetLastError() << std::endl;
            }
            else {
                std::cout << "Socket an Port 12345 gebunden\n";
                rc = listen(acceptSocket, 10); // Maximal 10 Verbindungen dürfen ausstehen
                if (rc == SOCKET_ERROR) {
                    std::cout << "Fehler: listen, Fehlercode: " << WSAGetLastError() << std::endl;
                    return 1;
                }
                else {
                    std::cout << "acceptSocket ist im listen Modus....\n";
                    SOCKET connectedSocket;
                    connectedSocket = accept(acceptSocket, NULL, NULL);
                    if (connectedSocket == INVALID_SOCKET) {
                        std::cout << "Fehler: accept, Fehlercode: " << WSAGetLastError() << std::endl;
                        return 1;
                    }
                    else {
                        std::cout << "Neue Verbindung wurde akzeptiert!\n";
                    }
                }
            }
        }
    }
    return 0;
}