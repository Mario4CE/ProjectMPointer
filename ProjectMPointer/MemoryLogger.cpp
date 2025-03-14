#include "MemoryLogger.h"
#include <filesystem> // Para manejo de directorios

namespace fs = std::filesystem;

void MemoryLogger::logRequest(const std::string& request) {
    // Crear la carpeta "logs" si no existe
    if (!fs::exists("logs")) {
        fs::create_directory("logs");
    }

    // Abrir el archivo de logs en modo append
    std::ofstream logFile("logs/requests.log", std::ios::app);
    if (logFile.is_open()) {
        // Obtener la fecha y hora actual
        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(now);
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        // Escribir la petición en el archivo
        logFile << "[" << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << now_ms.count() << "] "
            << request << std::endl;
        logFile.close();
    }
    else {
        std::cerr << "Error: No se pudo abrir el archivo de logs." << std::endl;
    }
}

void MemoryLogger::logMemoryState(const std::vector<std::string>& memoryState) {
    // Crear la carpeta "logs" si no existe
    if (!fs::exists("logs")) {
        fs::create_directory("logs");
    }

    // Obtener la fecha y hora actual para el nombre del archivo
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::ostringstream filename;
    filename << "logs/memory_state_"
        << std::put_time(std::localtime(&now_time_t), "%Y%m%d_%H%M%S")
        << "_" << std::setfill('0') << std::setw(3) << now_ms.count() << ".log";

    // Abrir el archivo para guardar el estado de la memoria
    std::ofstream memoryFile(filename.str());
    if (memoryFile.is_open()) {
        memoryFile << "Estado de la memoria:\n";
        for (const auto& entry : memoryState) {
            memoryFile << entry << "\n";
        }
        memoryFile.close();
    }
    else {
        std::cerr << "Error: No se pudo crear el archivo de estado de memoria." << std::endl;
    }
}