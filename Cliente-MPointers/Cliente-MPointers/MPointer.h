#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include "SocketUtils.h" // Para usar las funciones de sockets

template <typename T>
class MPointer {
private:
    int id; // ID del bloque de memoria en el Memory Manager
    static std::string serverAddress; // Dirección del servidor (Memory Manager)
    static int serverPort; // Puerto del servidor

    // Método para enviar una petición al Memory Manager
    std::string sendRequest(const std::string& request);

public:
    // Constructor por defecto
    MPointer();

    // Constructor con ID
    MPointer(int id);

    // Destructor
    ~MPointer();

    // Método estático para inicializar la conexión con el Memory Manager
    static void Init(const std::string& address, int port);

    // Método estático para crear un nuevo MPointer
    static MPointer<T> New();

    // Sobrecarga del operador *
    T operator*();

    // Sobrecarga del operador =
    MPointer<T>& operator=(const MPointer<T>& other);

    // Sobrecarga del operador &
    int operator&();
};

// Inicialización de variables estáticas
template <typename T>
std::string MPointer<T>::serverAddress = "";

template <typename T>
int MPointer<T>::serverPort = 0;

#endif // MPOINTER_H
