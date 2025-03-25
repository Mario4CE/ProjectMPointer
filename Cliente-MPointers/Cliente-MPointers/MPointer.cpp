#include "MPointer.h"
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include <stdexcept>
#include <sstream>
#include <typeinfo>

// Definición de miembros estáticos
template <typename T>
std::string MPointer<T>::serverAddress = "";

template <typename T>
int MPointer<T>::serverPort = 0;

// Implementación de sendRequest
template <typename T>
std::string MPointer<T>::sendRequest(const std::string& request) {
    try {
        std::string response = SocketUtils::sendRequest(serverAddress, serverPort, request);
        if (response.empty()) {
            std::string mensajeError = "El servidor no devolvió una respuesta válida.";
            ErrorLogger::logError(mensajeError);
            throw std::runtime_error(mensajeError);
        }
        return response;
    }
    catch (const std::exception& e) {
        std::string mensajeError = "Error en sendRequest: " + std::string(e.what());
        ErrorLogger::logError(mensajeError);
        throw;
    }
}

// Instanciaciones explícitas
template class MPointer<int>;
template class MPointer<double>;