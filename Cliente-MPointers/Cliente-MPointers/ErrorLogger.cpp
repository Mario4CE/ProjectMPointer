#include "ErrorLogger.h"
#include <filesystem>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

// Método para loggear un error

void ErrorLogger::logError(const std::string& message) {
    // Mostrar el mensaje en la consola
    std::cerr << "ERROR: " << message << std::endl;

    // Crear la carpeta "logs/errors" si no existe
    if (!fs::exists("logs/errors")) {
        if (!fs::create_directories("logs/errors")) {
            std::cerr << "Error: No se pudo crear la carpeta 'logs/errors'." << std::endl;
            std::string mensajeError = "Error: No se pudo crear la carpeta 'logs/info'.";
            ErrorLogger::logError(mensajeError);
            return;
        }
    }

    // Abrir el archivo de errores en modo append
    std::ofstream logFile("logs/errors/errors.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << getCurrentTime() << "] ERROR: " << message << std::endl;
        logFile.close();
    }
    else {
        std::cerr << "Error: No se pudo abrir el archivo de errores." << std::endl;
        std::string mensajeError = "Error: No se pudo crear la carpeta 'logs/info'.";
        ErrorLogger::logError(mensajeError);
    }
}

std::string ErrorLogger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()
    ) % 1000;

    std::ostringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "[%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count() << "] ";

    return ss.str();
}