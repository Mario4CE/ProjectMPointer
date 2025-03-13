#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <iostream>
#include <string>

// Constantes
const int PORT = 12345;
const int MAX_PENDING_CONNECTIONS = 10;

// Declaraciones de funciones
int startWinsock();
int startServer();
void handleClient(SOCKET clientSocket);

#endif // SERVER_H
