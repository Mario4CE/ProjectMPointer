#include "MemoryManager.h"
#include "interfaz.h"
#include "ErrorLogger.h"
#include "InfoLogger.h"
#include <iostream>
#include <sstream>
#include "ActualizarRespuesta.h"

// Inicializar el estado de la memoria
std::unordered_map<int, MemoryManager::MemoryBlock> MemoryManager::memoryBlocks;
int MemoryManager::nextId = 1;

void MemoryManager::initializeMemory() {
    // Limpiar el estado de la memoria
    memoryBlocks.clear();
    nextId = 1;
    InfoLogger::logInfo("Memoria inicializada con " + std::to_string(SIZE_MB) + " MB.");
}

std::vector<std::string> MemoryManager::getMemoryState() {
    std::vector<std::string> state;
    for (const auto& block : memoryBlocks) {
        std::stringstream ss;
        ss << "ID: " << block.second.id
            << ", Tamaño: " << block.second.size
            << ", Tipo: " << block.second.type
            << ", Valor: " << block.second.value
            << ", RefCount: " << block.second.refCount;
        state.push_back(ss.str());
    }
    return state;
}

std::string MemoryManager::processRequest(const std::string& request) {
    // Inicializar la memoria si no está inicializada
    if (memoryBlocks.empty()) {
        initializeMemory();
    }

    // Registrar la petición en el log de información
    InfoLogger::logInfo("Petición recibida: " + request);

    // Procesar la petición
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    if (command == "Create") {
        std::string size, type;
        iss >> size >> type;
        return handleCreate(size, type);
    }
    else if (command == "Set") {
        int id;
        std::string value;
        iss >> id >> value;
        return handleSet(id, value);
    }
    else if (command == "Get") {
        int id;
        iss >> id;
        return handleGet(id);
    }
    else if (command == "IncreaseRefCount") {
        int id;
        iss >> id;
        return handleIncreaseRefCount(id);
    }
    else if (command == "DecreaseRefCount") {
        int id;
        iss >> id;
        return handleDecreaseRefCount(id);
    }
    else {
        ErrorLogger::logError("Comando no reconocido: " + command);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: No se pudo procesar la solicitud.");
        return "Error: Comando no reconocido";
    }
}

std::string MemoryManager::handleCreate(const std::string& size, const std::string& type) {
    MemoryBlock block;
    block.id = nextId++;
    block.size = std::stoul(size);
    block.type = type;
    block.value = "";
    block.refCount = 1;

    memoryBlocks[block.id] = block;
    InfoLogger::logInfo("Creado bloque ID: " + std::to_string(block.id));
    return "Creado bloque ID: " + std::to_string(block.id);
}

std::string MemoryManager::handleSet(int id, const std::string& value) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        ErrorLogger::logError("ID no encontrado: " + std::to_string(id));
        return "Error: ID no encontrado";
    }
    memoryBlocks[id].value = value;
    InfoLogger::logInfo("Valor asignado al bloque ID: " + std::to_string(id));
    return "Valor asignado al bloque ID: " + std::to_string(id);
}

std::string MemoryManager::handleGet(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        ErrorLogger::logError("ID no encontrado: " + std::to_string(id));
        return "Error: ID no encontrado";
    }
    InfoLogger::logInfo("Valor obtenido del bloque ID: " + std::to_string(id));
    return "Valor en bloque ID " + std::to_string(id) + ": " + memoryBlocks[id].value;
}

std::string MemoryManager::handleIncreaseRefCount(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        ErrorLogger::logError("ID no encontrado: " + std::to_string(id));
        return "Error: ID no encontrado";
    }
    memoryBlocks[id].refCount++;
    InfoLogger::logInfo("RefCount incrementado para bloque ID: " + std::to_string(id));
    return "RefCount incrementado para bloque ID: " + std::to_string(id);
}

std::string MemoryManager::handleDecreaseRefCount(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        ErrorLogger::logError("ID no encontrado: " + std::to_string(id));
        return "Error: ID no encontrado";
    }
    if (--memoryBlocks[id].refCount == 0) {
        memoryBlocks.erase(id);
        InfoLogger::logInfo("Bloque ID " + std::to_string(id) + " liberado (refCount = 0)");
        return "Bloque ID " + std::to_string(id) + " liberado (refCount = 0)";
    }
    InfoLogger::logInfo("RefCount decrementado para bloque ID: " + std::to_string(id));
    return "RefCount decrementado para bloque ID: " + std::to_string(id);
}