#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <iostream>
#include <string>

// Constantes
const int PORT = 12345; // Puerto de escucha
const int MAX_PENDING_CONNECTIONS = 10; // Número máximo de conexiones pendientes

// Declaraciones de funciones
int startWinsock(); // Inicializar Winsock 

int startServer(); // Iniciar el servidor

void handleClient(SOCKET clientSocket); // Función para manejar un cliente

bool sendToClient(SOCKET clientSocket, const std::string& message); // Enviar un mensaje a un cliente

#endif // SERVER_H
