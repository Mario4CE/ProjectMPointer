#ifndef SOCKETUTILS_H
#define SOCKETUTILS_H

#include <string>
#include <stdexcept>

class SocketUtils {
public:
    static std::string sendRequest(const std::string& address, int port, const std::string& request);
};

#endif // SOCKETUTILS_H
