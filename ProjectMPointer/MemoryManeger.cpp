#include "MemoryManager.h"
#include "MemoryLogger.h"
#include <iostream>
#include <sstream>

// Inicializar el estado de la memoria
std::unordered_map<int, MemoryManager::MemoryBlock> MemoryManager::memoryBlocks;
int MemoryManager::nextId = 1;

void MemoryManager::initializeMemory() {
    // Limpiar el estado de la memoria
    memoryBlocks.clear();
    nextId = 1;
    std::cout << "Memoria inicializada con " << SIZE_MB << " MB." << std::endl;
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

    // Guardar la petición en el log
    MemoryLogger::logRequest(request);

    // Procesar la petición
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    if (command == "Create") {  //Metodo Create(Sujeto a cambios)
        std::string size, type;
        iss >> size >> type;
        return handleCreate(size, type);
    }
    else if (command == "Set") { //Metodo Set(Sujeto a cambios)
        int id;
        std::string value;
        iss >> id >> value;
        return handleSet(id, value);
    }
    else if (command == "Get") { //Metodo Set(Sujeto a cambios)
        int id;
        iss >> id;
        return handleGet(id);
    }
    else if (command == "IncreaseRefCount") { //Metodo Incrementar count(Sujeto a cambios)
        int id;
        iss >> id;
        return handleIncreaseRefCount(id);
    }
    else if (command == "DecreaseRefCount") { //Metodo Reducir count(Sujeto a cambios)
        int id;
        iss >> id;
        return handleDecreaseRefCount(id);
    }
    else {
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
    return "Creado bloque ID: " + std::to_string(block.id);
}

std::string MemoryManager::handleSet(int id, const std::string& value) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        return "Error: ID no encontrado";
    }
    memoryBlocks[id].value = value;
    return "Valor asignado al bloque ID: " + std::to_string(id);
}

std::string MemoryManager::handleGet(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        return "Error: ID no encontrado";
    }
    return "Valor en bloque ID " + std::to_string(id) + ": " + memoryBlocks[id].value;
}

std::string MemoryManager::handleIncreaseRefCount(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        return "Error: ID no encontrado";
    }
    memoryBlocks[id].refCount++;
    return "RefCount incrementado para bloque ID: " + std::to_string(id);
}

std::string MemoryManager::handleDecreaseRefCount(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        return "Error: ID no encontrado";
    }
    if (--memoryBlocks[id].refCount == 0) {
        memoryBlocks.erase(id);
        return "Bloque ID " + std::to_string(id) + " liberado (refCount = 0)";
    }
    return "RefCount decrementado para bloque ID: " + std::to_string(id);
}