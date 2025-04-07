
//Bibliotecas necesarias y definiciones de funciones
#include "MemoryManager.h"
#include "ActualizarRespuesta.h"
#include "ErrorLogger.h"
#include "MemoryLogger.h"
#include "InfoLogger.h"
#include "Server.h"
#include "Interfaz.h"
#include "ClienteManager.h"
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cstring>
#include <algorithm>
#include <thread>
#include <winsock2.h>
#include <stdexcept>
#include <fcntl.h>

// --- Inicialización de Variables Estáticas ---
char* MemoryManager::memoryPool = nullptr;
std::unordered_map<int, MemoryManager::MemoryBlock> MemoryManager::memoryBlocks;
std::vector<std::pair<size_t, size_t>> MemoryManager::freeBlocks;
int MemoryManager::nextId = 1;
size_t MemoryManager::nextFree = 0;


// --- Funciones de Inicialización y Estado ---

/*
 * Inicializa el pool de memoria.
 * - Asigna 1GB de memoria.
 * - Limpia los bloques de memoria y los bloques libres.
 * - Resetea el ID del siguiente bloque.
 */
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
 * Obtiene el estado actual de la memoria.
 * - Recorre todos los bloques de memoria y crea una representación en string.
 * - Incluye ID, tamaño, tipo, refCount, offset y el valor almacenado.
 */
std::vector<std::string> MemoryManager::getMemoryState() {
    std::vector<std::string> state;
    for (const auto& block : memoryBlocks) {
        std::stringstream ss;
        ss << "ID: " << block.second.id
            << ", Tamaño: " << block.second.size
            << ", Tipo: " << block.second.type
            << ", RefCount: " << block.second.refCount
            << ", Offset: " << block.second.offset;

        // Leer el valor almacenado en el bloque de memoria
        if (block.second.type == "int") {
            int value;
            std::memcpy(&value, memoryPool + block.second.offset, sizeof(int));
            ss << ", Dato: " << value;
        }
        else if (block.second.type == "double") {
            double value;
            std::memcpy(&value, memoryPool + block.second.offset, sizeof(double));
            ss << ", Dato: " << value;
        }
        else if (block.second.type == "char") {
            char value;
            std::memcpy(&value, memoryPool + block.second.offset, sizeof(char));
            ss << ", Dato: " << value;
        }
        else if (block.second.type == "float") {
            float value;
            std::memcpy(&value, memoryPool + block.second.offset, sizeof(float));
            ss << ", Dato: " << value;
        }
        else if (block.second.type == "string" || block.second.type == "str") {
            std::string value(memoryPool + block.second.offset, block.second.size);
            ss << ", Dato: " << value;
        }

        state.push_back(ss.str());
    }
    return state;
}

// --- Funciones de Procesamiento de Peticiones ---

/*
 * Procesa las peticiones del cliente.
 * - Inicializa la memoria si es necesario.
 * - Analiza el comando y llama a la función correspondiente.
 * - Maneja errores y registra logs.
 */
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
            InfoLogger:: logInfo("Creando bloque de memoria con tamaño " + size + " y tipo " + type);
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
        else if (command == "Estado") {
            std::vector<std::string> state = getMemoryState();
            std::string response = "Estado de la memoria:\n";
            for (const auto& s : state) {
                response += s + "\n";
            }
            return response;
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


// --- Funciones de Manejo de Bloques de Memoria ---

/*
 * Valida el tamaño del bloque según el tipo de dato.
 * - Asegura que el tamaño sea coherente con el tipo para ahorrar memoria.
 */
bool MemoryManager::validateSizeForType(const std::string& type, size_t size) {
    std::string lowerType = type;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower);

    static const std::unordered_map<std::string, size_t> typeSizes = {
        {"int", sizeof(int)},
        {"double", sizeof(double)},
        {"char", sizeof(char)},
        {"float", sizeof(float)}
    };

    auto it = typeSizes.find(lowerType);
    if (it != typeSizes.end()) {
        return size != 0 && size % it->second == 0;
    }

    if (lowerType == "string" || lowerType == "str") {
        return true;
    }

    throw std::invalid_argument("Tipo de dato no soportado: " + type);
    
}

/*
 * Crea un nuevo bloque de memoria.
 * - Valida el tamaño del bloque.
 * - Asigna memoria y actualiza el estado.
 * - Envía un mensaje al cliente.
 */
std::string MemoryManager::handleCreate(const std::string& size, const std::string& type) {
    size_t blockSize;

    // 1️⃣ Validar tamaño
    std::string validationError = validateBlockSize(size, type, blockSize);
    if (!validationError.empty()) return validationError;

    // 2️⃣ Crear y almacenar bloque
    MemoryBlock newBlock = createMemoryBlock(blockSize, type);
    if (newBlock.id == -1) return "Error: No hay espacio suficiente";

    // 3️⃣ Actualizar UI y log
    updateUIWithBlockInfo(newBlock);

    // 4️⃣ Enviar mensaje de confirmación al cliente
    //sendBlockCreationMessage(newBlock);

    //Lo que se envia al cliente es el id del bloque
    return std::to_string(newBlock.id);
}

/**
 * 🔍 Valida el tamaño y convierte la entrada a un número.
 */
std::string MemoryManager::validateBlockSize(const std::string& size, const std::string& type, size_t& blockSize) {
    try {
        blockSize = std::stoul(size);
    }
    catch (const std::invalid_argument& e) {
        return logAndReturnError("Error: Tamaño no válido", e.what());
    }
    catch (const std::out_of_range& e) {
        return logAndReturnError("Error: Tamaño fuera de rango", e.what());
    }

    if (!validateSizeForType(type, blockSize)) {
        return logAndReturnError("Error: Tamaño incongruente con el tipo");
    }

    return ""; // Sin errores
}

/**
 * 📦 Crea un nuevo bloque de memoria y lo almacena.
 */
MemoryManager::MemoryBlock MemoryManager::createMemoryBlock(size_t blockSize, const std::string& type) {
    MemoryBlock newBlock(nextId++, blockSize, 0, type);

    if (!allocateMemory(newBlock.size, newBlock)) {
        logAndReturnError("Error: No hay espacio suficiente");
        return MemoryManager::MemoryBlock(-1, 0, 0, ""); // Retorna un bloque inválido
    }

    memoryBlocks[newBlock.id] = newBlock;
    return newBlock;
}


/**
 * 🖥️ Actualiza la UI y los logs con la información del nuevo bloque.
 */
void MemoryManager::updateUIWithBlockInfo(const MemoryBlock& newBlock) {
    std::ostringstream ss;
    ss << "Bloque ID: " << newBlock.id
        << ", Dirección: " << static_cast<void*>(memoryPool + newBlock.offset)
        << ", Tamaño: " << newBlock.size << " bytes";

    std::cout << ss.str() << std::endl;
    InterfazCLI::Respuestas::ActualizarLabelEnFormulario(ss.str());

    std::vector<std::string> memoryState = getMemoryState();
    MemoryLogger::logMemoryState(memoryState);
}

/**
 * 📤 Envía un mensaje al cliente con la información del bloque creado.
 */
void MemoryManager::sendBlockCreationMessage(const MemoryBlock& newBlock) {
    SOCKET socket_fd = 12345;
    int intentosMaximos = 2;
    int intentos = 0;
    if (socket_fd == INVALID_SOCKET) {
        ErrorLogger::logError("Socket inválido, no se pudo enviar mensaje.");
        return;
    }

    // Configurar select() para verificar si el socket está listo para escribir
    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(socket_fd, &writefds);
    timeval timeout = { 5, 0 }; // 10 segundos

    std::string mensaje =std::to_string(newBlock.id);

    int result = select(socket_fd , nullptr, &writefds, nullptr, &timeout);
    if (result > 0 && FD_ISSET(socket_fd, &writefds)) {
        if (enviarComando(mensaje)) {
            InfoLogger::logInfo("Mensaje enviado correctamente.");
        }
        else {
            ErrorLogger::logError("❌ Error al enviar mensaje al cliente.");
        }
    }
    else {
        ErrorLogger::logError("❌ Timeout o error en select() al esperar escritura en el socket ;(.");
    }
}

/**
 * 🛠️ Registra un error en el log y devuelve el mensaje de error.
 */
std::string MemoryManager::logAndReturnError(const std::string& errorMsg, const std::string& details) { // No default argument here
    InterfazCLI::Respuestas::ActualizarLabelEnFormulario(errorMsg);
    if (!details.empty()) {
        ErrorLogger::logError(errorMsg + ": " + details);
    }
    else {
        ErrorLogger::logError(errorMsg);
    }
    return errorMsg;
}

/*
 * Asigna un valor a un bloque de memoria.
 * - Valida el tipo de dato y el tamaño del valor.
 * - Actualiza el bloque de memoria.
 */
std::string MemoryManager::handleSet(int id, const std::string& value) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        std::string mensajeError = "Error: ID no encontrado: " + std::to_string(id);
        ErrorLogger::logError(mensajeError);
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    if (blockPtr->type == "int") {
        if (!validateDataType(blockPtr->type, value, sizeof(int))) {
            return "Error: Tamaño de valor incorrecto para el tipo int.";
        }
        int intValue = std::stoi(value);
        std::memcpy(memoryPool + blockPtr->offset, &intValue, sizeof(int));
    }
    else if (blockPtr->type == "double") {
        if (!validateDataType(blockPtr->type, value, sizeof(double))) {
            return "Error: Tamaño de valor incorrecto para el tipo double.";
        }
        double doubleValue = std::stod(value);
        std::memcpy(memoryPool + blockPtr->offset, &doubleValue, sizeof(double));
    }
    else if (blockPtr->type == "char") {
        if (!validateDataType(blockPtr->type, value, sizeof(char))) {
            return "Error: Tamaño de valor incorrecto para el tipo char.";
        }
        char charValue = value[0];
        std::memcpy(memoryPool + blockPtr->offset, &charValue, sizeof(char));
    }
    else if (blockPtr->type == "float") {
        if (!validateDataType(blockPtr->type, value, sizeof(float))) {
            return "Error: Tamaño de valor incorrecto para el tipo float.";
        }
        float floatValue = std::stof(value);
        std::memcpy(memoryPool + blockPtr->offset, &floatValue, sizeof(float));
    }
    else if (blockPtr->type == "string" || blockPtr->type == "str") {
        if (value.size() > blockPtr->size) {
            return "Error: El valor es demasiado grande para el bloque asignado.";
        }
        std::memcpy(memoryPool + blockPtr->offset, value.c_str(), value.size());
    }
    else {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tipo de dato no soportado: " + blockPtr->type);
        ErrorLogger::logError("Error: Tipo de dato no soportado: " + blockPtr->type);
        std::cerr << "Error: Tipo de dato no soportado: " << blockPtr->type << std::endl;
        return "Error: Tipo de dato no soportado";
    }

    return "Valor asignado al bloque ID: " + std::to_string(id);
}

/*
 * Obtiene el valor de un bloque de memoria.
 * - Lee el valor del bloque y lo devuelve como string.
 */
std::string MemoryManager::handleGet(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: ID no encontrado: " + std::to_string(id));
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    if (blockPtr->type == "int") {
        int value;
        std::memcpy(&value, memoryPool + blockPtr->offset, sizeof(int));
        return std::to_string(value);
    }
    else if (blockPtr->type == "double") {
        double value;
        std::memcpy(&value, memoryPool + blockPtr->offset, sizeof(double));
        return std::to_string(value);
    }
    else if (blockPtr->type == "char") {
        char value;
        std::memcpy(&value, memoryPool + blockPtr->offset, sizeof(char));
        return std::string(1, value);
    }
    else if (blockPtr->type == "float") {
        float value;
        std::memcpy(&value, memoryPool + blockPtr->offset, sizeof(float));
        return std::to_string(value);
    }
    else if (blockPtr->type == "string" || blockPtr->type == "str") {
        return std::string(memoryPool + blockPtr->offset, blockPtr->size);
    }
    else {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tipo de dato no soportado: " + blockPtr->type);
        ErrorLogger::logError("Error: Tipo de dato no soportado: " + blockPtr->type);
        std::cerr << "Error: Tipo de dato no soportado: " << blockPtr->type << std::endl;
        return "Error: Tipo de dato no soportado";
    }
}

/*
 * Incrementa el contador de referencias de un bloque.
 */
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

/*
 * Decrementa el contador de referencias de un bloque.
 * - Libera el bloque si el contador llega a cero.
 */
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

// --- Funciones de Gestión de Memoria ---

/*
 * Asigna memoria a un bloque.
 * - Busca un bloque libre con suficiente espacio.
 * - Actualiza la lista de bloques libres.
 */
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

/*
 * Libera la memoria de un bloque.
 * - Agrega el bloque liberado a la lista de bloques libres.
 * - Elimina el bloque del mapa de bloques de memoria.
 */
void MemoryManager::releaseMemory(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        return;
    }

    freeBlocks.push_back({ blockPtr->offset, blockPtr->size });
    memoryBlocks.erase(id);
}

// --- Funciones de Validación y Búsqueda ---

/*
 * Valida el tipo de dato de un valor.
 * - Asegura que el tamaño del valor sea correcto para el tipo de bloque.
 */
bool MemoryManager::validateDataType(const std::string& blockType, const std::string& value, size_t expectedSize) {
    if (blockType == "int" || blockType == "double" || blockType == "char" || blockType == "float" || blockType == "string") {
        if (value.size() != expectedSize) {
            return false;
        }
    }
    return true;
}

/*
 * Encuentra un bloque de memoria por su ID.
 * - Devuelve un puntero al bloque si se encuentra, o nullptr si no.
 */
MemoryManager::MemoryBlock* MemoryManager::findBlock(int id) {
    auto it = memoryBlocks.find(id);
    if (it == memoryBlocks.end()) {
        return nullptr;
    }
    return &it->second;
}
