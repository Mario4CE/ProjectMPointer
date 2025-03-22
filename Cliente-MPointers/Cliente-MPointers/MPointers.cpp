#include "MPointer.h"
#include "SocketUtils.h"
#include <stdexcept>

template <typename T>
MPointer<T>::MPointer() : id(-1) {} // Inicializa el ID a -1 (inválido)

template <typename T>
MPointer<T>::MPointer(int id) : id(id) {}

template <typename T>
MPointer<T>::~MPointer() {
    if (id != -1) {
        // Notificar al Memory Manager que se destruyó una referencia
        sendRequest("DecreaseRefCount " + std::to_string(id));
    }
}

template <typename T>
void MPointer<T>::Init(const std::string& address, int port) {
    serverAddress = address;
    serverPort = port;
}

template <typename T>
MPointer<T> MPointer<T>::New() {
    // Solicitar un nuevo bloque de memoria al Memory Manager
    std::string response = sendRequest("Create " + std::to_string(sizeof(T)) + " " + typeid(T).name());
    int newId = std::stoi(response); // El Memory Manager devuelve el ID del nuevo bloque
    return MPointer<T>(newId);
}

template <typename T>
T MPointer<T>::operator*() {
    // Solicitar el valor almacenado en el bloque de memoria
    std::string response = sendRequest("Get " + std::to_string(id));
    T value;
    // Convertir la respuesta a un valor de tipo T (esto es un ejemplo simplificado)
    // Aquí deberías implementar la lógica para convertir la respuesta en un valor de tipo T
    return value;
}

template <typename T>
MPointer<T>& MPointer<T>::operator=(const MPointer<T>& other) {
    if (this != &other) {
        // Notificar al Memory Manager que se destruyó la referencia actual
        if (id != -1) {
            sendRequest("DecreaseRefCount " + std::to_string(id));
        }
        // Copiar el ID del otro MPointer
        id = other.id;
        // Notificar al Memory Manager que se incrementó la referencia
        sendRequest("IncreaseRefCount " + std::to_string(id));
    }
    return *this;
}

template <typename T>
int MPointer<T>::operator&() {
    return id; // Devolver el ID del bloque de memoria
}

template <typename T>
std::string MPointer<T>::sendRequest(const std::string& request) {
    // Usar las funciones de utilidad para sockets
    return SocketUtils::sendRequest(serverAddress, serverPort, request);
}