#include "ErrorLogger.h"
#include <filesystem>
#include <iostream> 
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

void ErrorLogger::logError(const std::string& message) {
    // Crear la carpeta "logs/errors" si no existe
    if (!fs::exists("logs/errors")) {
        std::cout << "Creando carpeta 'logs/errors'..." << std::endl;
        if (!fs::create_directories("logs/errors")) {
            std::cerr << "Error: No se pudo crear la carpeta 'logs/errors'." << std::endl;
            return;
        }
        std::cout << "Carpeta 'logs/errors' creada correctamente." << std::endl;
    }

    // Abrir el archivo de errores en modo append
    std::ofstream logFile("logs/errors/errors.log", std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" << getCurrentTime() << "] ERROR: " << message << std::endl;
        logFile.close();
    }
    else {
        std::cerr << "Error: No se pudo abrir el archivo de errores." << std::endl; // Usa std::cerr
    }
}

std::string ErrorLogger::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << now_ms.count();
    return oss.str();
}