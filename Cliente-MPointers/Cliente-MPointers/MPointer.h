
#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"

template <typename T>
class MPointer {
public:
    // Constructor por defecto
    MPointer() : id(-1) {} // Inicializa el ID a -1 (inválido)

    // Constructor con ID específico
    MPointer(int id) : id(id) {}

    // Destructor
    ~MPointer() {
        if (id != -1) {
            // Notificar al Memory Manager que se destruyó una referencia
            this->sendRequest("DecreaseRefCount " + std::to_string(id));
        }
    }

    // Inicialización estática
    static void Init(const std::string& address, int port) {
        serverAddress = address;
        serverPort = port;
    }

    // Creación de nuevo MPointer
    static MPointer<T> New() {
        // Necesitamos una instancia temporal para llamar a sendRequest
        MPointer<T> temp;
        std::string response = temp.sendRequest("Create " + std::to_string(sizeof(T)) + " " + typeid(T).name());

        // Convertir la respuesta a un ID (entero)
        int newId;
        try {
            newId = std::stoi(response);
        }
        catch (const std::invalid_argument& e) {
            std::string mensajeError = "Error al convertir la respuesta del servidor a entero: " + std::string(e.what());
            ErrorLogger::logError(mensajeError);
            throw std::runtime_error("Respuesta inválida del servidor al crear un nuevo bloque: " + response);
        }

        return MPointer<T>(newId); // Devolver un nuevo MPointer con el ID asignado
    }

    // Operador de dereferencia
    T operator*() {
        // Solicitar el valor almacenado en el bloque de memoria
        std::string response = this->sendRequest("Get " + std::to_string(id));

        // Convertir la respuesta a un valor de tipo T
        std::istringstream converter(response);
        T value;
        if (!(converter >> value)) {
            std::string mensajeError = "Error al convertir la respuesta del servidor a tipo " + std::string(typeid(T).name());
            ErrorLogger::logError(mensajeError);
            throw std::runtime_error("Error al convertir la respuesta del servidor a tipo " + std::string(typeid(T).name()));
        }

        return value; // Devolver el valor convertido
    }

    // Operador de asignación (copia de MPointer)
    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) { // Evitar auto-asignación
            // Notificar al Memory Manager que se destruyó la referencia actual
            if (id != -1) {
                this->sendRequest("DecreaseRefCount " + std::to_string(id));
            }

            // Copiar el ID del otro MPointer
            id = other.id;

            // Notificar al Memory Manager que se incrementó la referencia
            this->sendRequest("IncreaseRefCount " + std::to_string(id));
        }

        return *this; // Devolver una referencia a este objeto
    }

    // Operador de asignación (valor)
    void operator=(const T& value) {
        // Convertir el valor a cadena
        std::ostringstream oss;
        oss << value;

        // Enviar la solicitud al servidor para almacenar el valor
        this->sendRequest("Set " + std::to_string(id) + " " + oss.str());
    }

    // Operador de dirección (obtener ID)
    int operator&() {
        return id; // Devolver el ID del bloque de memoria
    }

private:
    int id; // ID del bloque de memoria
    static std::string serverAddress; // Dirección del servidor
    static int serverPort; // Puerto del servidor

    // Función para enviar solicitudes al servidor
    std::string sendRequest(const std::string& request) {
        try {
            // Enviar la solicitud al servidor y recibir la respuesta
            std::string response = SocketUtils::sendRequest(serverAddress, serverPort, request);

            // Verificar si la respuesta está vacía o es inválida
            if (response.empty()) {
                std::string mensajeError = "El servidor no devolvió una respuesta válida.";
                ErrorLogger::logError(mensajeError);
                throw std::runtime_error("El servidor no devolvió una respuesta válida.");
            }

            return response; // Devolver la respuesta del servidor
        }
        catch (const std::exception& e) {
            // Capturar y relanzar excepciones con un mensaje más descriptivo
            std::string mensajeError = "Error en sendRequest: " + std::string(e.what());
            ErrorLogger::logError(mensajeError);
            throw std::runtime_error("Error en sendRequest: " + std::string(e.what()));
        }
    }
};

// Inicialización de miembros estáticos
template <typename T>
std::string MPointer<T>::serverAddress = "";

template <typename T>
int MPointer<T>::serverPort = 0;

// Instanciaciones explícitas para los tipos que usarás
template class MPointer<int>;
template class MPointer<double>;

#endif // MPOINTER_H