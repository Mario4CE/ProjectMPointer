
#ifndef CLIENTEMANAGER_H
#define CLIENTEMANAGER_H

#include <winsock2.h>
#include <string>

class ClienteManager  // Clase para manejar la conexi�n con el cliente
{
public:
    static void handleClient(SOCKET clientSocket); // Funci�n para manejar un cliente
};
#endif  // MANEJADOR_CLIENTE_H
