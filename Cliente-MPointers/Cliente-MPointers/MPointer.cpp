#include "MPointer.h"
#include "SocketUtils.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include <stdexcept>
#include <sstream>
#include <typeinfo>

// Definici�n de miembros est�ticos
template <typename T>
std::string MPointer<T>::serverAddress = "";

template <typename T>
int MPointer<T>::serverPort = 0;

// Implementaci�n de sendRequest
template <typename T>
std::string MPointer<T>::sendRequest(const std::string& request) {
    try {
        std::string response = SocketUtils::sendRequest(serverAddress, serverPort, request);
        if (response.empty()) {
            std::string mensajeError = "El servidor no devolvi� una respuesta v�lida.";
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

// Instanciaciones expl�citas
template class MPointer<int>;
template class MPointer<double>;