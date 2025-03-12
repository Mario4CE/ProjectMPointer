#include "Server.h" // Incluye el archivo de encabezado "Server.h"

int startWinsock(void) {
    WSADATA wsa; // Estructura para almacenar información de inicialización de Winsock
    return WSAStartup(MAKEWORD(2, 0), &wsa); // Inicializa Winsock versión 2.0 y devuelve el resultado
}

int startServer() {
    long rc; // Variable para almacenar códigos de retorno
    SOCKET acceptSocket; // Socket para aceptar conexiones entrantes
    SOCKADDR_IN addr; // Estructura para almacenar la dirección del servidor

    rc = startWinsock(); // Inicializa Winsock
    if (rc != 0) {
        std::cout << "Error: startWinsock, código de error: " << GetLastError() << std::endl; // Imprime un mensaje de error si falla la inicialización de Winsock
        return 1; // Retorna 1 para indicar un error
    }
    else {
        std::cout << "¡Winsock iniciado!\n"; // Imprime un mensaje de éxito si Winsock se inicializa correctamente
        acceptSocket = socket(AF_INET, SOCK_STREAM, 0); // Crea un socket para conexiones TCP/IP
        if (acceptSocket == INVALID_SOCKET) {
            std::cout << "Error: No se pudo crear el socket, código de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la creación del socket
            return 1; // Retorna 1 para indicar un error
        }
        else {
            std::cout << "¡Socket creado!\n"; // Imprime un mensaje de éxito si el socket se crea correctamente
            memset(&addr, 0, sizeof(SOCKADDR_IN)); // Inicializa la estructura de dirección con ceros
            addr.sin_family = AF_INET; // Establece la familia de direcciones a IPv4
            addr.sin_port = htons(12345); // Establece el puerto a 12345 (convertido a orden de bytes de red)
            addr.sin_addr.s_addr = ADDR_ANY; // Establece la dirección IP para aceptar conexiones en cualquier interfaz
            rc = bind(acceptSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN)); // Asocia el socket a la dirección y puerto especificados
            if (rc == SOCKET_ERROR) {
                std::cout << "Error: bind, código de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la asociación del socket
            }
            else {
                std::cout << "Socket asociado al puerto 12345\n"; // Imprime un mensaje de éxito si el socket se asocia correctamente
                rc = listen(acceptSocket, 10); // Pone el socket en modo de escucha, permitiendo hasta 10 conexiones pendientes
                if (rc == SOCKET_ERROR) {
                    std::cout << "Error: listen, código de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la puesta en modo de escucha
                    return 1; // Retorna 1 para indicar un error
                }
                else {
                    std::cout << "acceptSocket está en modo de escucha....\n"; // Imprime un mensaje de éxito indicando que el socket está escuchando
                    SOCKET connectedSocket; // Socket para la conexión aceptada
                    connectedSocket = accept(acceptSocket, NULL, NULL); // Acepta una conexión entrante
                    if (connectedSocket == INVALID_SOCKET) {
                        std::cout << "Error: accept, código de error: " << WSAGetLastError() << std::endl; // Imprime un mensaje de error si falla la aceptación de la conexión
                        return 1; // Retorna 1 para indicar un error
                    }
                    else {
                        std::cout << "¡Nueva conexión aceptada!\n"; // Imprime un mensaje de éxito si la conexión se acepta correctamente
                    }
                }
            }
        }
    }
    return 0; // Retorna 0 para indicar éxito
}