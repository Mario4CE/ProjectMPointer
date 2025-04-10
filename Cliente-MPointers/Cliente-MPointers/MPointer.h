
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

    T convertResponse(const std::string& response) {
        std::istringstream iss(response);
        T value;

        if constexpr (std::is_same_v<T, int>) {
            iss >> value;
        }
        else if constexpr (std::is_same_v<T, double>) {
            iss >> value;
        }
        else if constexpr (std::is_same_v<T, char>) {
            if (!response.empty()) {
                value = response[0];
            }
        }
        else if constexpr (std::is_same_v<T, float>) {
            iss >> value;
        }
        else if constexpr (std::is_same_v<T, std::string>) {
            return response;
        }

        return value;
    }

public:

    // Constructor por defecto
    MPointer() : id(1) {} // Inicializa el ID

    // Constructor con ID espec?fico
    MPointer(int id) : id(id) {}

    // Destructor
    ~MPointer() {
        if (id != -1) {
            // Notificar al Memory Manager que se destruy? una referencia
            this->sendRequest("DecreaseRefCount " + std::to_string(id));
        }
    }

    // Inicializacion estatica
    static void Init(const std::string& address, int port) {
        serverAddress = address;
        serverPort = port;
    }

    // Creacion de nuevo MPointer
    static MPointer<T> New(int valor, int timeoutMs = 5000) {
        MPointer<T> temp;
        std::string response;
        const int maxRetries = 4; // Número máximo de reintentos
        int attempt = 0;
        bool success = false;

        while (attempt < maxRetries && !success) {
            attempt++;
            try {
                InfoLogger::logInfo("Intento #" + std::to_string(attempt) +
                    ": Solicitando nuevo bloque para tipo " +
                    std::string(typeid(T).name()));

                // Incluye el valor en la petición al servidor
                response = temp.sendRequest("Create " + std::string(std::to_string(valor) + " " + typeid(T).name()), timeoutMs);

                InfoLogger::logInfo("Respuesta recibida en New (intento #" + std::to_string(attempt) + "): >" + response + "<");

                if (response.empty() || response == "Error") {
                    throw std::runtime_error("Respuesta inválida del servidor: " + response);
                }

                // Si llegamos aquí, la respuesta fue válida
                success = true;
            }
            catch (const std::exception& e) {
                ErrorLogger::logError("Excepción en intento #" + std::to_string(attempt) +
                    " al crear MPointer: " + std::string(e.what()));

                if (attempt >= maxRetries) {
                    // Si es el último intento, relanzamos la excepción
                    throw std::runtime_error("Fallo después de " + std::to_string(maxRetries) +
                        " intentos: " + std::string(e.what()));
                }

                // Pequeña espera antes del reintento
                std::this_thread::sleep_for(std::chrono::milliseconds(300));
            }
        }

        // Limpiar: conservar solo los dígitos
        std::string cleanResponse;
        for (char c : response) {
            if (std::isdigit(static_cast<unsigned char>(c))) {
                cleanResponse += c;
            }
        }

        if (cleanResponse.empty()) {
            throw std::runtime_error("La respuesta del servidor no contiene un número válido: " + response);
        }

        // Convertir respuesta a ID
        int newId;
        try {
            newId = std::stoi(cleanResponse);
        }
        catch (const std::exception& e) {
            ErrorLogger::logError("Excepción al convertir respuesta a ID: " + std::string(e.what()));
            throw std::runtime_error("Respuesta inválida del servidor al crear un nuevo bloque: " + response);
        }

        InfoLogger::logInfo("Nuevo bloque creado con ID: " + std::to_string(newId));
        temp.id = newId;
        return temp;
    }

    // Operador de dereferencia
    T operator*() {
        std::string response = sendRequest("Get " + std::to_string(id));

        if (response.find("Error:") != std::string::npos) {
            throw std::runtime_error(response);
        }

        return convertResponse(response);
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

    // Operador de asignacion (valor)
    void operator=(const T& value) {
        std::ostringstream oss;
        oss << value;
        std::string comando = "Set " + std::to_string(id) + " " + oss.str();
        InfoLogger::logInfo("Enviando comando: " + comando);
        this->sendRequest(comando);
    }


    // Operador de direcci?n (obtener ID)
    int operator&() {
        return id; // Devolver el ID del bloque de memoria
    }

    std::string sendRequest(const std::string& request, int timeoutMs = 9000, int maxRetries = 3) {
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

                    InfoLogger::logInfo("Respuesta del Server:" + response);
                    // Convertir la respuesta a un valor de tipo T
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

    //Metodo que le envia al server el comando para que cierre el server, osea que cierre la ventana
    static bool closeServer() {
        try {
            // Creamos una instancia temporal para usar sendRequest
            MPointer<T> temp;
            std::string response = temp.sendRequest("Cerrar");

            if (response == "OK") {
                InfoLogger::logInfo("Servidor cerrado correctamente.");
                return true;
            }
            else {
                ErrorLogger::logError("Error al cerrar el servidor: " + response);
                return false;
            }
        }
        catch (const std::exception& e) {
            ErrorLogger::logError("Excepción al cerrar el servidor: " + std::string(e.what()));
            return false;
        }
    }

    // Método para incrementar el valor almacenado en el MPointer
    void increase() {
        std::string response = sendRequest("Increase " + std::to_string(id));

        if (response.find("Error:") != std::string::npos) {
            throw std::runtime_error(response);
        }

        InfoLogger::logInfo("Valor incrementado para el bloque " + std::to_string(id));
    }

    // Método para decrementar el valor almacenado en el MPointer
    void decrease() {
        std::string response = sendRequest("Decrease " + std::to_string(id));

        if (response.find("Error:") != std::string::npos) {
            throw std::runtime_error(response);
        }

        InfoLogger::logInfo("Valor decrementado para el bloque " + std::to_string(id));
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