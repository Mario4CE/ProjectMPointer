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


    size_t totalMemoryMB = static_cast<size_t>(TOTAL_MEMORY) / static_cast<size_t>(1024 * 1024);

    std::cout << "Memoria inicializada con " << totalMemoryMB << " MB." << std::endl;

    startCleanupTask();
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
            << ", Dirección: " << static_cast<void*>(memoryPool + block.second.offset);

        // Siempre leer como string, porque puede haber múltiples valores separados por coma
        const char* dataStart = memoryPool + block.second.offset;
        size_t len = strnlen(dataStart, block.second.size);
        std::string dataStr(dataStart, len);

        // Solo los char van entre comillas simples, los string entre comillas dobles
        if (block.second.type == "char") {
            ss << ", Dato: '" << dataStr << "'";
        }
        else if (block.second.type == "string" || block.second.type == "str") {
            ss << ", Dato: \"" << dataStr << "\"";
        }
        else {
            ss << ", Dato: " << dataStr;
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
            InfoLogger::logInfo("Creando bloque de memoria con tamaño " + size + " y tipo " + type);
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
        else if (command == "Increase") {
            int id;
            iss >> id;
            return handleIncreaseRefCount(id);
        }
        else if (command == "Decrease") {
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
    std::to_string(newBlock.id);
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
        return MemoryBlock(-1, 0, 0, "");
    }

    // Inicializar memoria a 0 y loggear dirección
    std::memset(memoryPool + newBlock.offset, 0, newBlock.size);
    InfoLogger::logInfo("Bloque creado en dirección: " +
        std::to_string(reinterpret_cast<uintptr_t>(memoryPool + newBlock.offset)));

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
    timeval timeout = { 1, 0 }; //segundos

    std::string mensaje = std::to_string(newBlock.id);

    int result = select(static_cast<int>(socket_fd), nullptr, &writefds, nullptr, &timeout);
    if (result > 0 && FD_ISSET(socket_fd, &writefds)) {
        if (enviarComando(mensaje)) {
            InfoLogger::logInfo("Mensaje enviado correctamente.");
        }
        else {
            ErrorLogger::logError("Error al enviar mensaje al cliente.");
        }
    }
    else {
        ErrorLogger::logError("Timeout o error en select() al esperar escritura en el socket ;(.");
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
        std::string errorMsg = "Error: ID no encontrado: " + std::to_string(id);
        ErrorLogger::logError(errorMsg);
        return errorMsg;
    }

    try {
        if (blockPtr->refCount > 0) {
            // Tratar cualquier tipo como string cuando refCount > 0
            const char* currentData = memoryPool + blockPtr->offset;
            std::string currentValue(currentData);

            std::string newValue = currentValue.empty() ? value : (currentValue + "," + value);

            if (newValue.size() >= blockPtr->size) {
                throw std::invalid_argument("El nuevo valor excede el tamaño del bloque.");
            }

            std::memset(memoryPool + blockPtr->offset, 0, blockPtr->size);
            std::strncpy(memoryPool + blockPtr->offset, newValue.c_str(), blockPtr->size - 1);

        }
        else {
            // refCount == 0: comportamiento normal por tipo
            std::memset(memoryPool + blockPtr->offset, 0, blockPtr->size);

            if (blockPtr->type == "int") {
                int intValue = std::stoi(value);
                std::memcpy(memoryPool + blockPtr->offset, &intValue, sizeof(int));
            }
            else if (blockPtr->type == "double") {
                double doubleValue = std::stod(value);
                std::memcpy(memoryPool + blockPtr->offset, &doubleValue, sizeof(double));
            }
            else if (blockPtr->type == "char") {
                if (value.size() != 1) {
                    throw std::invalid_argument("El valor para char debe ser un solo carácter.");
                }
                char charValue = value[0];
                std::memcpy(memoryPool + blockPtr->offset, &charValue, sizeof(char));
            }
            else if (blockPtr->type == "float") {
                float floatValue = std::stof(value);
                std::memcpy(memoryPool + blockPtr->offset, &floatValue, sizeof(float));
            }
            else if (blockPtr->type == "string" || blockPtr->type == "str") {
                size_t copySize = (std::min)(value.size(), blockPtr->size - 1);
                std::memcpy(memoryPool + blockPtr->offset, value.c_str(), copySize);
                *(memoryPool + blockPtr->offset + copySize) = '\0';
            }
            else {
                throw std::invalid_argument("Tipo de dato no soportado: " + blockPtr->type);
            }
        }

        memoryBlocks[id] = *blockPtr;
        MemoryLogger::logMemoryState(getMemoryState());

        std::string successMsg = "Valor actualizado correctamente en ID: " + std::to_string(id);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario(successMsg);

        return successMsg;
    }
    catch (const std::exception& e) {
        std::string errorMsg = "Error al asignar valor: " + std::string(e.what());
        ErrorLogger::logError(errorMsg);
        return errorMsg;
    }
}




/*
 * Obtiene el valor de un bloque de memoria.
 * - Lee el valor del bloque y lo devuelve como string.
 */
std::string MemoryManager::handleGet(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        std::string errorMsg = "Error: ID no encontrado: " + std::to_string(id);
        ErrorLogger::logError(errorMsg);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario(errorMsg);
        return errorMsg;
    }

    try {
        // Leer el dato como cadena (incluyendo cualquier tipo)
        const char* dataStart = memoryPool + blockPtr->offset;
        size_t len = strnlen(dataStart, blockPtr->size);
        std::string valueStr(dataStart, len);

        // Dividir en partes usando comas
        std::vector<std::string> parts;
        std::istringstream ss(valueStr);
        std::string part;
        while (std::getline(ss, part, ',')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }

        int currentRefCount = blockPtr->refCount;

        // Validar que el refCount esté dentro del rango
        if (currentRefCount >= 0 && currentRefCount < parts.size()) {
            std::string selectedValue = parts[currentRefCount];

            // Actualizar UI
            std::string uiMsg = "Valor obtenido (" + blockPtr->type + ") en ID "
                + std::to_string(id) + ": " + selectedValue;
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario(uiMsg);
            return selectedValue;
        }
        else {
            std::string errorMsg = "Error: RefCount " + std::to_string(currentRefCount)
                + " fuera de rango. Máximo: " + std::to_string(parts.size() - 1);
            ErrorLogger::logError(errorMsg);
            return errorMsg;
        }
    }
    catch (const std::exception& e) {
        std::string errorMsg = "Error al leer bloque " + std::to_string(id) + ": " + e.what();
        ErrorLogger::logError(errorMsg);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario(errorMsg);
        return errorMsg;
    }
}

std::string MemoryManager::handleIncreaseRefCount(int id) {
    auto blockPtr = findBlock(id);  // Buscar el bloque con el id indicado
    if (!blockPtr) {
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        return "Error: ID no encontrado";
    }

    // Incrementar el contador de referencias
    blockPtr->refCount++;

    // Para el tipo string (o str) la idea es que al incrementar se añada al valor almacenado una coma y el nuevo dato.
    if (blockPtr->type == "string" || blockPtr->type == "str") {
        // Leer el contenido actual
        const char* currentContent = memoryPool + blockPtr->offset;
        std::string newContent(currentContent);
        // Si ya existe contenido, se concatena una coma y un espacio, sino, se deja tal cual.
        if (!newContent.empty()) {
            newContent += ", ";
        }
        newContent += "Valor " + std::to_string(blockPtr->refCount);
        // Verificar que el nuevo contenido quepa en el bloque
        if (newContent.size() >= blockPtr->size)
            throw std::invalid_argument("El contenido resultante es demasiado grande para el bloque asignado.");
        std::memset(memoryPool + blockPtr->offset, 0, blockPtr->size);
        std::memcpy(memoryPool + blockPtr->offset, newContent.c_str(), newContent.size());
    }
    else {
        // Para otros tipos, si se desea hacer algo similar se podría extender la lógica.
        // En este ejemplo solo modificamos los bloques de tipo string.
    }

    // Actualizar el estado en el mapa y en el log
    memoryBlocks[id] = *blockPtr;
    MemoryLogger::logMemoryState(getMemoryState());

    // Construir mensaje de log (para el usuario, se muestra lo que hay en el bloque)
    std::string logMessage = "Increase: refCount incrementado para bloque ID " + std::to_string(id);
    logMessage += " - Dato actualizado: " + std::string(memoryPool + blockPtr->offset);
    InfoLogger::logInfo(logMessage);
    return logMessage;
}


// Decrementar el contador de referencias sin eliminar las referencias, solo restando el refCount
std::string MemoryManager::handleDecreaseRefCount(int id) {
    auto blockPtr = findBlock(id);  // Buscar el bloque de memoria con el id dado
    if (!blockPtr) {
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    // Decrementar el contador de referencias
    blockPtr->refCount--;  // Decrementamos en 1 el refCount del bloque

    // Si el contador de referencias llega a cero, liberar el bloque
    if (blockPtr->refCount < 0) {
        releaseMemory(id);  // Liberar el bloque de memoria
        std::cout << "Bloque ID " << id << " liberado (refCount = 0)" << std::endl;
        return "Bloque ID " + std::to_string(id) + " liberado (refCount = 0)";
    }

    // Mostrar las referencias actuales
    std::string logMessage = "RefCount decrementado para bloque ID: " + std::to_string(id);
    logMessage += " - Referencias: ";
    for (const auto& ref : blockPtr->references) {
        logMessage += ref + ", ";
    }
    if (!blockPtr->references.empty()) {
        logMessage = logMessage.substr(0, logMessage.length() - 2);  // Eliminar la última coma
    }

    std::cout << logMessage << std::endl;
    InfoLogger::logInfo(logMessage);

    MemoryLogger::logMemoryState(getMemoryState());

    std::string successMsg = "RefCount decrementado para bloque ID: " + std::to_string(id) + " - Referencias: " + logMessage;
    InterfazCLI::Respuestas::ActualizarLabelEnFormulario(successMsg);

    return "RefCount decrementado para bloque ID: " + std::to_string(id) + " - Referencias: " + logMessage;
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

void MemoryManager::cleanup() {
    std::vector<int> blocksToRemove;
    // Recorrer todos los bloques y buscar los que cumplen con la condición.
    for (const auto& pair : memoryBlocks) {
        const MemoryBlock& block = pair.second;
        // Revisar si el bloque tiene refCount 0.
        if (block.refCount == 0) {
            // Verificar si no hay ningún dato guardado (todo el bloque en 0).
            bool empty = true;
            char* dataStart = memoryPool + block.offset;
            for (size_t i = 0; i < block.size; ++i) {
                if (dataStart[i] != 0) {
                    empty = false;
                    break;
                }
            }
            if (empty) {
                blocksToRemove.push_back(pair.first);
            }
        }
    }

    // Eliminar los bloques que cumplen con la condición.
    for (int id : blocksToRemove) {
        releaseMemory(id);
        InfoLogger::logInfo("Cleanup: Bloque ID " + std::to_string(id) + " liberado por falta de referencias y sin datos.");
    }

    // Actualizar el estado de la memoria luego de la limpieza.
    MemoryLogger::logMemoryState(getMemoryState());
}

void MemoryManager::startCleanupTask() {
    std::thread([]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(45));
            MemoryManager::cleanup();
        }
        }).detach();
}




