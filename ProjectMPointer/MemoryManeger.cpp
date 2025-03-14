#include "MemoryManager.h"
#include "MemoryLogger.h"
#include <iostream>

// Inicializar el estado de la memoria
std::vector<std::string> MemoryManager::memoryBlocks;

void MemoryManager::initializeMemory() {
    // Crear bloques de memoria (cada bloque representa 1 MB)
    memoryBlocks.clear();
    for (int i = 0; i < SIZE_MB; i++) {
        memoryBlocks.push_back("Bloque " + std::to_string(i + 1) + ": Libre");
    }
    std::cout << "Memoria inicializada con " << SIZE_MB << " MB." << std::endl;
}

std::vector<std::string> MemoryManager::getMemoryState() {
    // Devolver el estado actual de la memoria
    return memoryBlocks;
}

void MemoryManager::processRequest(const std::string& request) {
    // Inicializar la memoria si no está inicializada
    if (memoryBlocks.empty()) {
        initializeMemory();
    }

    // Simulación de procesamiento de una petición
    std::cout << "Procesando petición: " << request << std::endl;

    // Guardar la petición en el log
    MemoryLogger::logRequest(request);

    // Simular la modificación de la memoria
    if (request.find("Asignar") != std::string::npos) {
        // Asignar un bloque libre
        for (auto& block : memoryBlocks) {
            if (block.find("Libre") != std::string::npos) {
                block = "Bloque " + block.substr(7, block.find(":") - 7) + ": Ocupado por " + request.substr(8);
                break;
            }
        }
    }
    else if (request.find("Liberar") != std::string::npos) {
        // Liberar un bloque ocupado
        for (auto& block : memoryBlocks) {
            if (block.find(request.substr(8)) != std::string::npos) {
                block = "Bloque " + block.substr(7, block.find(":") - 7) + ": Libre";
                break;
            }
        }
    }

    // Guardar el estado de la memoria en un archivo
    MemoryLogger::logMemoryState(memoryBlocks);
}