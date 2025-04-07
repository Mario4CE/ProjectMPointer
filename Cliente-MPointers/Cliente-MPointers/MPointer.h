
#ifndef MPOINTER_H
#define MPOINTER_H

#include <string>
#include <stdexcept>
#include <sstream>
#include <typeinfo>
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include <future>
#include <chrono>
#include <thread>

template <typename T>
/*
* Clase MPointer
* Representa un puntero inteligente que almacena un ID de un bloque de memoria en el servidor
* y permite acceder a los valores almacenados en dicho bloque.
*/
class MPointer {

private:
    int id; // ID del bloque de memoria
    static std::string serverAddress; // Dirección del servidor
    static int serverPort; // Puerto del servidor

public:

    // Constructor por defecto
    MPointer() : id(-1) {} // Inicializa el ID a -1 (inv?lido)

    // Constructor con ID espec?fico
    MPointer(int id) : id(id) {}

    // Destructor
    ~MPointer() {
        if (id != -1) {
            // Notificar al Memory Manager que se destruy? una referencia
            this->sendRequest("DecreaseRefCount " + std::to_string(id));
        }
    }

    // Inicializaci?n est?tica
    static void Init(const std::string& address, int port) {
        serverAddress = address;
        serverPort = port;
    }

    // Creaci?n de nuevo MPointer
    template <typename T>
    static MPointer<T> New(int maxRetries = 5, int timeoutMs = 5000) {
        MPointer<T> temp;
        std::string response;
        int attempt = 0;

        while (attempt < maxRetries) {
            try {
                attempt++;
                InfoLogger::logInfo("Intento #" + std::to_string(attempt) + ": solicitando nuevo bloque para tipo " + typeid(T).name());

                // Llamamos a sendRequest de forma asíncrona
                auto future = std::async(std::launch::async, [&]() {
                    return temp.sendRequest("Create " + std::to_string(sizeof(T)) + " " + typeid(T).name());
                    });

                // Esperamos la respuesta con timeout
                if (future.wait_for(std::chrono::milliseconds(timeoutMs)) == std::future_status::ready) {
                    response = future.get();

                    if (!response.empty() && response != "Error") {
                        break; // Éxito, salimos del bucle
                    }
                    else {
                        ErrorLogger::logError("Respuesta inválida o vacía en intento #" + std::to_string(attempt) + ": " + response);
                    }
                }
                else {
                    ErrorLogger::logError("Timeout esperando respuesta del servidor en intento #" + std::to_string(attempt));
                }
            }
            catch (const std::exception& e) {
                ErrorLogger::logError("Excepción en intento #" + std::to_string(attempt) + ": " + e.what());
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(300));  // Espera entre intentos
        }

        // Validar respuesta final después de los intentos
        if (response.empty() || response == "Error") {
            std::string mensajeError = "Fallo al obtener una respuesta válida del servidor después de " + std::to_string(maxRetries) + " intentos.";
            ErrorLogger::logError(mensajeError);
            throw std::runtime_error(mensajeError);
        }

        // Convertir respuesta a ID
        int newId;
        try {
            newId = std::stoi(response);
        }
        catch (const std::invalid_argument& e) {
            std::string mensajeError = "Error al convertir la respuesta del servidor a entero: " + std::string(e.what());
           ErrorLogger::logError(mensajeError);
            throw std::runtime_error("Respuesta inválida del servidor al crear un nuevo bloque: " + response);
        }

        InfoLogger::logInfo("Nuevo bloque creado con ID: " + std::to_string(newId));
        temp.id = newId;
        return temp;
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

    // Operador de asignaci?n (copia de MPointer)
    MPointer<T>& operator=(const MPointer<T>& other) {
        if (this != &other) { // Evitar auto-asignaci?n
            // Notificar al Memory Manager que se destruy? la referencia actual
            if (id != -1) {
                this->sendRequest("DecreaseRefCount " + std::to_string(id));
            }

            // Copiar el ID del otro MPointer
            id = other.id;

            // Notificar al Memory Manager que se increment? la referencia
            this->sendRequest("IncreaseRefCount " + std::to_string(id));
        }

        return *this; // Devolver una referencia a este objeto
    }

    // Operador de asignaci?n (valor)
    void operator=(const T& value) {
        // Convertir el valor a cadena
        std::ostringstream oss;
        oss << value;

        // Enviar la solicitud al servidor para almacenar el valor
        this->sendRequest("Set " + std::to_string(id) + " " + oss.str());
    }

    // Operador de direcci?n (obtener ID)
    int operator&() {
        return id; // Devolver el ID del bloque de memoria
    }

    std::string sendRequest(const std::string& request, int timeoutMs = 5000, int maxRetries = 3) {
        int attempt = 0;
        std::string response;

        while (attempt < maxRetries) {
            try {
                attempt++;
                InfoLogger::logInfo("Intento #" + std::to_string(attempt) + ": Enviando solicitud al servidor...");

                // Lanzamos la solicitud en un hilo asincrónico
                auto future = std::async(std::launch::async, [&]() {
                    return SocketUtils::sendRequest(serverAddress, serverPort, request);
                    });

                // Esperamos con timeout
                if (future.wait_for(std::chrono::milliseconds(timeoutMs)) == std::future_status::ready) {
                    response = future.get();  // Obtener la respuesta

                    // Validación de la respuesta
                    if (response.empty()) {
                        throw std::runtime_error("El servidor no devolvió una respuesta válida.");
                    }
                    else if (response == "Error") {
                        throw std::runtime_error("Error en la solicitud al servidor.");
                    }

                    InfoLogger::logInfo("Respuesta del servidor: " + response);
                    return response;  // Éxito
                }
                else {
                    // Timeout alcanzado
                    throw std::runtime_error("Timeout al esperar respuesta del servidor.");
                }
            }
            catch (const std::exception& e) {
                std::string mensajeError = "Error en intento #" + std::to_string(attempt) + " - " + e.what();
                ErrorLogger::logError(mensajeError);

                if (attempt >= maxRetries) {
                    throw std::runtime_error("Fallo al enviar solicitud después de " + std::to_string(maxRetries) + " intentos: " + e.what());
                }

                // Pequeña espera antes del reintento
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
        }

        throw std::runtime_error("Error desconocido en sendRequest.");
    }

};

// Inicializacion de miembros estaticos
template <typename T>
std::string MPointer<T>::serverAddress = "";

template <typename T>
int MPointer<T>::serverPort = 0;

// Instanciaciones explicitas
template class MPointer<int>;
template class MPointer<double>;
template class MPointer<char>;
template class MPointer<std::string>;
template class MPointer<float>;

#endif // MPOINTER_H
// Fin del archivo MPointer.h