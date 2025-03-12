#include "Server.h" // Incluye el archivo de encabezado "Server.h"

int startWinsock(void) {
    WSADATA wsa; // Estructura para almacenar informaci�n de inicializaci�n de Winsock
    return WSAStartup(MAKEWORD(2, 0), &wsa); // Inicializa Winsock versi�n 2.0 y devuelve el resultado
}

int startServer() {
    long rc; // Variable para almacenar c�digos de retorno
    SOCKET acceptSocket; // Socket para aceptar conexiones entrantes
    SOCKADDR_IN addr; // Estructura para almacenar la direcci�n del servidor

    rc = startWinsock(); // Inicializa Winsock
    if (rc != 0) {
        std::cout << "Error: startWinsock, c�digo de error: " << GetLastError() << std::endl; // Imprime un mensaje de error si falla la inicializaci�n de Winsock
        return 1; // Retorna 1 para indicar un error
    }
    else {
        std::cout << "�Winsock iniciado!\n"; // Imprime un mensaje de �xito si Winsock se inicializa correctamente
        acceptSocket = socket(AF_INET, SOCK_STREAM, 0); // Crea un socket para conexiones TCP/IP
        if (acceptSocket == INVALID_SOCKET) {
            std::cout << "Error: No se pudo crear el socket, c�digo de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la creaci�n del socket
            return 1; // Retorna 1 para indicar un error
        }
        else {
            std::cout << "�Socket creado!\n"; // Imprime un mensaje de �xito si el socket se crea correctamente
            memset(&addr, 0, sizeof(SOCKADDR_IN)); // Inicializa la estructura de direcci�n con ceros
            addr.sin_family = AF_INET; // Establece la familia de direcciones a IPv4
            addr.sin_port = htons(12345); // Establece el puerto a 12345 (convertido a orden de bytes de red)
            addr.sin_addr.s_addr = ADDR_ANY; // Establece la direcci�n IP para aceptar conexiones en cualquier interfaz
            rc = bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)); // Asocia el socket a la direcci�n y puerto especificados
            if (rc == SOCKET_ERROR) {
                std::cout << "Error: bind, c�digo de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la asociaci�n del socket
            }
            else {
                std::cout << "Socket asociado al puerto 12345\n"; // Imprime un mensaje de �xito si el socket se asocia correctamente
                rc = listen(acceptSocket, 10); // Pone el socket en modo de escucha, permitiendo hasta 10 conexiones pendientes
                if (rc == SOCKET_ERROR) {
                    std::cout << "Error: listen, c�digo de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la puesta en modo de escucha
                    return 1; // Retorna 1 para indicar un error
                }
                else {
                    std::cout << "acceptSocket est� en modo de escucha....\n"; // Imprime un mensaje de �xito indicando que el socket est� escuchando
                    SOCKET connectedSocket; // Socket para la conexi�n aceptada
                    connectedSocket = accept(acceptSocket, NULL, NULL); // Acepta una conexi�n entrante
                    if (connectedSocket == INVALID_SOCKET) {
                        std::cout << "Error: accept, c�digo de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la aceptaci�n de la conexi�n
                        return 1; // Retorna 1 para indicar un error
                    }
                    else {
                        std::cout << "�Nueva conexi�n aceptada!\n"; // Imprime un mensaje de �xito si la conexi�n se acepta correctamente
                    }
                }
            }
        }
    }
    return 0; // Retorna 0 para indicar �xito
}