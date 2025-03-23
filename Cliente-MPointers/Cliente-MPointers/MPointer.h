#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <typeinfo>

template <typename T>
class MPointer {
public:
    // Constructor y destructor
    MPointer();
    MPointer(int id);
    ~MPointer();

    // Funciones de gesti�n de memoria
    static void Init(const std::string& address, int port);
    static MPointer<T> New();

    // Operadores
    T operator*();
    MPointer<T>& operator=(const MPointer<T>& other); // Declaraci�n correcta
    void operator=(const T& value); // Sobrecarga para asignaci�n de valores
    int operator&();

private:
    int id; // ID del bloque de memoria
    static std::string serverAddress; // Direcci�n del servidor
    static int serverPort; // Puerto del servidor
    std::string sendRequest(const std::string& request); // Funci�n de utilidad
};

#endif // MPOINTER_H
