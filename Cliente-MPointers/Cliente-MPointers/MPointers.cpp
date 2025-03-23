#include "MPointer.h"
#include "SocketUtils.h"
#include <stdexcept>
#include <sstream>
#include <typeinfo>

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

    // Convertir la respuesta a un ID (entero)
    int newId;
    try {
        newId = std::stoi(response);
    }
    catch (const std::invalid_argument& e) {
        throw std::runtime_error("Respuesta inválida del servidor al crear un nuevo bloque: " + response);
    }

    return MPointer<T>(newId); // Devolver un nuevo MPointer con el ID asignado
}

template <typename T>
T MPointer<T>::operator*() {
    // Solicitar el valor almacenado en el bloque de memoria
    std::string response = sendRequest("Get " + std::to_string(id));

    // Convertir la respuesta a un valor de tipo T
    std::istringstream converter(response);
    T value;
    if (!(converter >> value)) {
        throw std::runtime_error("Error al convertir la respuesta del servidor a tipo " + std::string(typeid(T).name()));
    }

    return value; // Devolver el valor convertido
}

template <typename T>
MPointer<T>& MPointer<T>::operator=(const MPointer<T>& other) {
    if (this != &other) { // Evitar auto-asignación
        // Notificar al Memory Manager que se destruyó la referencia actual
        if (id != -1) {
            sendRequest("DecreaseRefCount " + std::to_string(id));
        }

        // Copiar el ID del otro MPointer
        id = other.id;

        // Notificar al Memory Manager que se incrementó la referencia
        sendRequest("IncreaseRefCount " + std::to_string(id));
    }

    return *this; // Devolver una referencia a este objeto
}

template <typename T>
void MPointer<T>::operator=(const T& value) {
    // Convertir el valor a cadena
    std::ostringstream oss;
    oss << value;

    // Enviar la solicitud al servidor para almacenar el valor
    sendRequest("Set " + std::to_string(id) + " " + oss.str());
}

template <typename T>
int MPointer<T>::operator&() {
    return id; // Devolver el ID del bloque de memoria
}

template <typename T>
std::string MPointer<T>::sendRequest(const std::string& request) {
    try {
        // Enviar la solicitud al servidor y recibir la respuesta
        std::string response = SocketUtils::sendRequest(serverAddress, serverPort, request);

        // Verificar si la respuesta está vacía o es inválida
        if (response.empty()) {
            throw std::runtime_error("El servidor no devolvió una respuesta válida.");
        }

        return response; // Devolver la respuesta del servidor
    }
    catch (const std::exception& e) {
        // Capturar y relanzar excepciones con un mensaje más descriptivo
        throw std::runtime_error("Error en sendRequest: " + std::string(e.what()));
    }
}