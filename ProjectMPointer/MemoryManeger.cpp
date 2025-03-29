
#include "MemoryManager.h"
#include "ActualizarRespuesta.h"
#include "ErrorLogger.h"
#include "MemoryLogger.h"
#include "Server.h"
#include "Interfaz.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <variant>
#include <string>
#include <memory>
#include <cstring> // Para memcpy

// Inicializar variables estáticas
char* MemoryManager::memoryPool = nullptr;
std::unordered_map<int, MemoryManager::MemoryBlock> MemoryManager::memoryBlocks;
std::vector<std::pair<size_t, size_t>> MemoryManager::freeBlocks;
int MemoryManager::nextId = 1;
size_t MemoryManager::nextFree = 0;

void MemoryManager::initialize() {
    memoryPool = new char[TOTAL_MEMORY];
    freeBlocks.clear();
    freeBlocks.emplace_back(0, TOTAL_MEMORY);
    memoryBlocks.clear();
    nextId = 1;
    nextFree = 0;

    size_t totalMemoryMB = static_cast<size_t>(TOTAL_MEMORY) / (1024 * 1024);
    std::cout << "Memoria inicializada con " << totalMemoryMB << " MB." << std::endl;
}
/*
* Esto es lo que se guarda cuando se ejecuta una orden
*/
std::vector<std::string> MemoryManager::getMemoryState() {
    std::vector<std::string> state;
    for (const auto& block : memoryBlocks) {
        std::stringstream ss;
        ss << "ID: " << block.second.id
            << ", Tamaño: " << block.second.size
            << ", Tipo: " << block.second.type
            << ", RefCount: " << block.second.refCount
            << ", Offset: " << block.second.offset; // Usamos offset en lugar de dirección de datos
        /*
        * Offsets es la dirección de memoria donde comienza el bloque de memoria.
        */

        state.push_back(ss.str());
    }
    return state;
}

std::string MemoryManager::processRequest(const std::string& request) {
    if (memoryPool == nullptr) {
        initialize();
    }

    MemoryLogger::logRequest("CMD: " + request);
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    try {
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
        else if (command == "Cerrar") {
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Cerrando servidor...");
            InterfazCLI::Respuestas::CerrarVentana();
            return "Cerrando servidor...";
        }
        else {
            throw std::invalid_argument("Comando no reconocido: " + command);
        }
    }
    catch (const std::exception& e) {
        std::string errorMsg = "Error: " + std::string(e.what());
        ErrorLogger::logError(errorMsg);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario(errorMsg);
        return errorMsg;
    }
}

bool MemoryManager::validateSizeForType(const std::string& type, size_t size) {
    if (type == "int") return size % sizeof(int) == 0;
    if (type == "double") return size % sizeof(double) == 0;
    if (type == "char") return size % sizeof(char) == 0;
    if (type == "float") return size % sizeof(float) == 0;
    if (type == "string" || type == "str") return true;

    throw std::invalid_argument("Tipo de dato no soportado: " + type);
}

std::string MemoryManager::handleCreate(const std::string& size, const std::string& type) {
    size_t blockSize = std::stoul(size);
    if (!validateSizeForType(type, blockSize)) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tamaño incongruente con el tipo");
        ErrorLogger::logError("Error: Tamaño incongruente con el tipo");
        return "Error: Tamaño incongruente con el tipo";
    }

    MemoryBlock newBlock(nextId++, blockSize, 0, type); // Offset inicial es 0

    if (!allocateMemory(newBlock.size, newBlock)) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: No hay espacio suficiente");
        return "Error: No hay espacio suficiente";
    }

    memoryBlocks[newBlock.id] = newBlock;

    // Imprimir la dirección de memoria del bloque creado
    std::cout << "Bloque ID: " << newBlock.id << ", Dirección: " << (void*)(memoryPool + newBlock.offset) << ", Tamaño: " << newBlock.size << std::endl;

    InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Creado bloque ID: " + std::to_string(newBlock.id));

    return "Creado bloque ID: " + std::to_string(newBlock.id);
}

MemoryManager::MemoryBlock* MemoryManager::findBlock(int id) {
    auto it = memoryBlocks.find(id);
    return (it != memoryBlocks.end()) ? &it->second : nullptr;
}

std::string MemoryManager::handleSet(int id, const std::string& value) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        std::string mensajeError = "Error: ID no encontrado: " + std::to_string(id);
        ErrorLogger::logError(mensajeError);
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    if (value.size() > blockPtr->size) {
        return "Error: El valor es demasiado grande para el bloque asignado.";
    }

    std::memcpy(memoryPool + blockPtr->offset, value.c_str(), value.size());
    return "Valor asignado al bloque ID: " + std::to_string(id);
}

std::string MemoryManager::handleGet(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: ID no encontrado: " + std::to_string(id));
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    return std::string(memoryPool + blockPtr->offset, blockPtr->size);
}

std::string MemoryManager::handleIncreaseRefCount(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }
    blockPtr->refCount++;
    std::cout << "RefCount incrementado para bloque ID: " << id << std::endl;
    return "RefCount incrementado para bloque ID: " + std::to_string(id);
}

std::string MemoryManager::handleDecreaseRefCount(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }
    blockPtr->refCount--;
    if (blockPtr->refCount == 0) {
        releaseMemory(id);
        std::cout << "Bloque ID " << id << " liberado (refCount = 0)" << std::endl;
        return "Bloque ID " + std::to_string(id) + " liberado (refCount = 0)";
    }
    std::cout << "RefCount decrementado para bloque ID: " << id << std::endl;
    return "RefCount decrementado para bloque ID: " + std::to_string(id);
}

bool MemoryManager::allocateMemory(size_t size, MemoryBlock& block) {
    for (auto it = freeBlocks.begin(); it != freeBlocks.end(); ++it) {
        if (it->second >= size) {
            block.offset = it->first;
            block.size = size;

            if (it->second == size) {
                freeBlocks.erase(it);
            }
            else {
                it->first += size;
                it->second -= size;
            }
            return true;
        }
    }
    return false;
}

void MemoryManager::releaseMemory(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        return;
    }

    freeBlocks.push_back({ blockPtr->offset, blockPtr->size });
    memoryBlocks.erase(id);
}