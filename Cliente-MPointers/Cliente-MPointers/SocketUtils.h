#ifndef SOCKETUTILS_H
#define SOCKETUTILS_H

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

class SocketUtils {
public:
    // Enviar una petición al servidor y recibir la respuesta
    static std::string sendRequest(const std::string& address, int port, const std::string& request);
};

#endif // SOCKETUTILS_H
