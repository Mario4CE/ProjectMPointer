#pragma once
#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <thread>
#include <string>

#pragma comment(lib, "ws2_32.lib")

class Servidor {
public:
    Servidor(int port);
    ~Servidor();
    void iniciar();
    void detener();

private:
    int port;
    SOCKET serverSocket;
    bool running;
    void manejarCliente(SOCKET clientSocket);
};

#endif // SERVER_H