#include "InfoLogger.h"
#include "ErrorLogger.h"
#include <filesystem> 
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <fstream>

namespace fs = std::filesystem;
// Funci�n para loggear informaci�n

void InfoLogger::logInfo(const std::string& message) {
    // Mostrar el mensaje en la consola
    std::cout << "INFO: " << message << std::endl;

    try {
        // Crear la carpeta "logs/info" si no existe
        if (!fs::exists("logs/info")) {
            if (!fs::create_directories("logs/info")) {
                std::cerr << "Error: No se pudo crear la carpeta 'logs/info'." << std::endl;
                std::string mensajeError = "Error: No se pudo crear la carpeta 'logs/info'.";
                ErrorLogger::logError(mensajeError);
                return;
            }
        }

        // Abrir el archivo de informaci�n en modo append
        std::ofstream logFile("logs/info/info.log", std::ios::app);
        if (logFile.is_open()) {
            logFile << "[" << getCurrentTime() << "] INFO: " << message << std::endl;
            logFile.close();
        }
        else {
            std::cerr << "Error: No se pudo abrir el archivo de informaci�n." << std::endl;
            std::string mensajeError = "Error: No se pudo abrir el archivo de informaci�n.";
            ErrorLogger::logError(mensajeError);
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error del sistema de archivos: " << e.what() << std::endl;
        std::string mensajeError = "Error del sistema de archivos: " + std::string(e.what());
        ErrorLogger::logError(mensajeError);
    }
    catch (const std::exception& e) {
        std::cerr << "Error inesperado: " << e.what() << std::endl;
        std::string mensajeError = "Error inesperado: " + std::string(e.what());
        ErrorLogger::logError(mensajeError);
    }
}

std::string InfoLogger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << now_ms.count();
    return oss.str();
}