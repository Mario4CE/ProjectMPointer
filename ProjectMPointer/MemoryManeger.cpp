
#include "MemoryManager.h"
#include "ActualizarRespuesta.h"
#include "ErrorLogger.h"
#include "interfaz.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>


// Inicializar el estado de la memoria
std::unordered_map<int, MemoryManager::MemoryBlock> MemoryManager::memoryBlocks;
int MemoryManager::nextId = 1;

// Inicializar la memoria
void MemoryManager::initializeMemory() {
    memoryBlocks.clear(); // Limpiar el estado de la memoria
    nextId = 1; // Reiniciar el contador de IDs
    std::cout << "Memoria inicializada con " << SIZE_MB << " MB." << std::endl;
}

// Obtener el estado de la memoria
std::vector<std::string> MemoryManager::getMemoryState() {
    std::vector<std::string> state;
    for (const auto& block : memoryBlocks) {
        std::stringstream ss;
        ss << "ID: " << block.second.id
            << ", Tamaño: " << block.second.size
            << ", Tipo: " << block.second.type
            << ", RefCount: " << block.second.refCount;
        state.push_back(ss.str());
    }
    return state;
}

// Procesar una petición
std::string MemoryManager::processRequest(const std::string& request) {
    // Inicializar la memoria si no está inicializada
    if (memoryBlocks.empty()) {
        initializeMemory();
    }

    // Registrar la petición en el log de información
    std::cout << "Petición recibida: " << request << std::endl;

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
        std::string mensajeError = "Error: Comando no reconocido: " + command;
        ErrorLogger::logError(mensajeError);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Comando no reconocido: " + command);
        std::cerr << "Error: Comando no reconocido: " << command << std::endl;
        return "Error: Comando no reconocido";
    }
}

// Validar que el tamaño sea congruente con el tipo de dato
bool MemoryManager::validateSizeForType(const std::string& type, size_t size) {
    if (type == "int") {
        return size % sizeof(int) == 0; // El tamaño debe ser un múltiplo de sizeof(int)
    }
    else if (type == "double") {
        return size % sizeof(double) == 0; // El tamaño debe ser un múltiplo de sizeof(double)
    }
    else if (type == "char") {
        return size % sizeof(char) == 0; // El tamaño debe ser un múltiplo de sizeof(char)
    }
    else if (type == "float") {
        return size % sizeof(float) == 0; // El tamaño debe ser un múltiplo de sizeof(float)
    }
    else {
        // Tipo no soportado
        ErrorLogger::logError("Error: Tipo de dato no soportado: " + type);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tipo de dato no soportado: " + type);
        std::cerr << "Error: Tipo de dato no soportado: " << type << std::endl;
        return "Error: Tipo de dato no soportado";
        return false;
    }
}

// Manejar la creación de un bloque de memoria
std::string MemoryManager::handleCreate(const std::string& size, const std::string& type) {
    size_t blockSize = std::stoul(size);

    // Validar que el tamaño sea congruente con el tipo de dato
    if (!validateSizeForType(type, blockSize)) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tamaño incongruente con el tipo de dato: " + type);
        ErrorLogger::logError("Error: Tamaño incongruente con el tipo de dato: " + type);
        std::cerr << "Error: Tamaño incongruente con el tipo de dato: " << type << std::endl;
        return "Error: Tamaño incongruente con el tipo de dato";
    }

    MemoryBlock block;
    block.id = nextId++;
    block.size = blockSize;
    block.type = type;
    block.refCount = 1;

    // Reservar espacio en memoria (simulado con un vector de bytes)
    block.data.resize(blockSize); // data es un std::vector<char>

    memoryBlocks[block.id] = block;
    std::cout << "Creado bloque ID: " << block.id << std::endl;
    return "Creado bloque ID: " + std::to_string(block.id);
}

// Manejar la asignación de un valor a un bloque de memoria
std::string MemoryManager::handleSet(int id, const std::string& value) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        std::string mensajeError = "Error: ID no encontrado: " + std::to_string(id);
        ErrorLogger::logError(mensajeError);
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    MemoryBlock& block = memoryBlocks[id];

    // Convertir el valor a bytes y almacenarlo en el bloque de memoria
    if (block.type == "int") {
        int intValue = std::stoi(value);
        std::memcpy(block.data.data(), &intValue, sizeof(int));
    }
    else if (block.type == "double") {
        double doubleValue = std::stod(value);
        std::memcpy(block.data.data(), &doubleValue, sizeof(double));
    }
    else if (block.type == "char") {
        char charValue = value[0]; // Tomar el primer carácter
        std::memcpy(block.data.data(), &charValue, sizeof(char));
    }
    else if (block.type == "float") {
        float floatValue = std::stof(value);
        std::memcpy(block.data.data(), &floatValue, sizeof(float));
    }
    else {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tipo de dato no soportado: " + block.type);
        ErrorLogger::logError("Error: Tipo de dato no soportado: " + block.type);
        std::cerr << "Error: Tipo de dato no soportado: " << block.type << std::endl;
        return "Error: Tipo de dato no soportado";
    }

    std::cout << "Valor asignado al bloque ID: " << id << std::endl;
    return "Valor asignado al bloque ID: " + std::to_string(id);
}

// Manejar la obtención de un valor de un bloque de memoria
std::string MemoryManager::handleGet(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: ID no encontrado: " + std::to_string(id));
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    MemoryBlock& block = memoryBlocks[id];

    // Convertir los bytes almacenados en el valor correspondiente
    std::stringstream ss;
    if (block.type == "int") {
        int intValue;
        std::memcpy(&intValue, block.data.data(), sizeof(int));
        ss << intValue;
    }
    else if (block.type == "double") {
        double doubleValue;
        std::memcpy(&doubleValue, block.data.data(), sizeof(double));
        ss << doubleValue;
    }
    else if (block.type == "char") {
        char charValue;
        std::memcpy(&charValue, block.data.data(), sizeof(char));
        ss << charValue;
    }
    else if (block.type == "float") {
        float floatValue;
        std::memcpy(&floatValue, block.data.data(), sizeof(float));
        ss << floatValue;
    }
    else {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tipo de dato no soportado: " + block.type);
        ErrorLogger::logError("Error: Tipo de dato no soportado: " + block.type);
        std::cerr << "Error: Tipo de dato no soportado: " << block.type << std::endl;
        return "Error: Tipo de dato no soportado";
    }

    std::cout << "Valor obtenido del bloque ID: " << id << std::endl;
    return "Valor en bloque ID " + std::to_string(id) + ": " + ss.str();
}

// Manejar el incremento del contador de referencias
std::string MemoryManager::handleIncreaseRefCount(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }
    memoryBlocks[id].refCount++;
    std::cout << "RefCount incrementado para bloque ID: " << id << std::endl;
    return "RefCount incrementado para bloque ID: " + std::to_string(id);
}

// Manejar el decremento del contador de referencias
std::string MemoryManager::handleDecreaseRefCount(int id) {
    if (memoryBlocks.find(id) == memoryBlocks.end()) {
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }
    if (--memoryBlocks[id].refCount == 0) {
        memoryBlocks.erase(id);
        std::cout << "Bloque ID " << id << " liberado (refCount = 0)" << std::endl;
        return "Bloque ID " + std::to_string(id) + " liberado (refCount = 0)";
    }
    std::cout << "RefCount decrementado para bloque ID: " << id << std::endl;
    return "RefCount decrementado para bloque ID: " + std::to_string(id);
}