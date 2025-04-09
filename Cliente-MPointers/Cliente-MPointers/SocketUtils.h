#ifndef SOCKETUTILS_H
#define SOCKETUTILS_H

#include <string>
#include <stdexcept>

// Clase de utilidades para sockets
class SocketUtils {
public:
    static std::string sendRequest(const std::string& address, int port, const std::string& request);
};

// Declare the function without default arguments here
std::string receiveWelcomeMessage(SOCKET clientSocket, int timeoutMs, int maxRetries);


#endif // SOCKETUTILS_H

