
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

// Inicializar variables estáticas
std::vector<char> MemoryManager::memoryPool;
std::unordered_map<int, MemoryManager::MemoryBlock> MemoryManager::memoryBlocks;
std::vector<std::pair<size_t, size_t>> MemoryManager::freeBlocks;
int MemoryManager::nextId = 1;

void MemoryManager::initialize() {
    memoryPool.resize(TOTAL_MEMORY);
    freeBlocks.clear();
    freeBlocks.emplace_back(0, TOTAL_MEMORY);
    memoryBlocks.clear();
    nextId = 1;

    // Use a larger type for the calculation to avoid overflow
    size_t totalMemoryMB = TOTAL_MEMORY / (1024 * 1024);
    std::cout << "Memoria inicializada con " << totalMemoryMB << " MB." << std::endl;
}


/*
* Metodo que obtiene el estado de la memoria
* Se obtiene el estado de la memoria en un vector de strings
* Se recorre el mapa de bloques de memoria y se obtiene la informacion de cada bloque
*/
std::vector<std::string> MemoryManager::getMemoryState() {
    std::vector<std::string> state;
    for (const auto& block : memoryBlocks) {
        std::stringstream ss;
        ss << "ID: " << block.second.id
            << ", Tamaño: " << block.second.size
            << ", Tipo: " << block.second.type
            << ", RefCount: " << block.second.refCount
            << ", Dirección: " << static_cast<const void*>(block.second.data.data());

        state.push_back(ss.str());
    }
    return state;
}

/*
* Procesar comandos (Create, Set, Get, etc.)
* Revisa si el comando es valido pata de esa manera procesarlo
* Si el comando es correcto procede a realizar la accion solicitada
* Si el comando no es correcto se lanza una excepcion, detalle importante es que el cliente hace una verificacion antes
* pero se agrega una verificacion adicional en el servidor para evitar errores
*/

std::string MemoryManager::processRequest(const std::string& request) {
    if (memoryPool.empty()) {
        initialize();
    }

    MemoryLogger::logRequest("CMD: " + request);
    std::istringstream iss(request);
    std::string command;
    iss >> command;

    try {//Se agrega el comando Create
        if (command == "Create") {
            std::string size, type;
            iss >> size >> type;
            return handleCreate(size, type);
        }//Se agrega el comando Set
        else if (command == "Set") {
            int id;
            std::string value;
            iss >> id >> value;
            return handleSet(id, value);
        }//Se agrega el comando Get
        else if (command == "Get") {
            int id;
            iss >> id;
            return handleGet(id);
        }//Se agrega el comando IncreaseRefCount
        else if (command == "IncreaseRefCount") {
            int id;
            iss >> id;
            return handleIncreaseRefCount(id);
        }//Se agrega el comando DecreaseRefCount
        else if (command == "DecreaseRefCount") {
            int id;
            iss >> id;
            return handleDecreaseRefCount(id);
        }//Se agrega el comando Cerrar
        else if (command == "Cerrar") {
            InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Cerrando servidor...");
            //Se cierra la ventana del server
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

// Validar que el tamaño sea congruente con el tipo de dato
bool MemoryManager::validateSizeForType(const std::string& type, size_t size) {
    if (type == "int") return size % sizeof(int) == 0;
    if (type == "double") return size % sizeof(double) == 0;
    if (type == "char") return size % sizeof(char) == 0;
    if (type == "float") return size % sizeof(float) == 0;
    if (type == "string" || type == "str") return true;

    throw std::invalid_argument("Tipo de dato no soportado: " + type);
}

/*
* Metodo que crea un bloque de memoria
* Se crea un bloque de memoria con un tamaño y tipo de dato especifico
* Se valida que el tamaño sea congruente con el tipo de dato
*/

std::string MemoryManager::handleCreate(const std::string& size, const std::string& type) {
    size_t blockSize = std::stoul(size);
    if (!validateSizeForType(type, blockSize)) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tamaño incongruente con el tipo");
        ErrorLogger::logError("Error: Tamaño incongruente con el tipo");
        //InterfazCLI::Respuestas::SendMessage("Error: Tamaño incongruente con el tipo");
        return "Error: Tamaño incongruente con el tipo";
    }

    // Creación directa con constructor mejorado
    MemoryBlock newBlock(nextId++, blockSize, type);

    // Asignar memoria
    if (!allocateMemory(newBlock.size, newBlock)) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: No hay espacio suficiente");
        return "Error: No hay espacio suficiente";
    }

    // Insertar en el mapa
    memoryBlocks[newBlock.id] = newBlock;

    InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Creado bloque ID: " + std::to_string(newBlock.id));

    return "Creado bloque ID: " + std::to_string(newBlock.id);
}

MemoryManager::MemoryBlock* MemoryManager::findBlock(int id) {
    auto it = memoryBlocks.find(id);
    return (it != memoryBlocks.end()) ? &it->second : nullptr;
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
    else if (block.type == "string" || block.type == "str") {
        // Para strings, almacenamos el contenido completo
        if (block.data.size() < value.size()) {
            block.data.resize(value.size());
        }
        std::memcpy(block.data.data(), value.data(), value.size());
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
    else if (block.type == "string" || block.type == "str") {
        // Para strings, leemos directamente los datos almacenados
        ss << std::string(block.data.begin(), block.data.end());
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

bool MemoryManager::allocateMemory(size_t size, MemoryBlock& block) {
    // 1. Buscar un bloque libre adecuado
    for (auto it = freeBlocks.begin(); it != freeBlocks.end(); ++it) {
        auto& [start, blockSize] = *it;

        if (blockSize >= size) {
            // 2. Asignar memoria del pool
            block.data.resize(size);

            // 3. Actualizar lista de bloques libres
            if (blockSize == size) {
                freeBlocks.erase(it);
            }
            else {
                it->first += size;
                it->second -= size;
            }

            return true;
        }
    }

    // 4. Si no hay espacio suficiente
    return false;
}

void MemoryManager::releaseMemory(int id) {
    auto it = memoryBlocks.find(id);
    if (it != memoryBlocks.end()) {
        // Decrease the reference count
        it->second.refCount--;
        if (it->second.refCount <= 0) {
            // If reference count is zero, release the memory
            freeBlocks.push_back({ it->second.size, it->second.id });
            memoryBlocks.erase(it);
        }
    }
    else {
        throw std::runtime_error("Memory block not found");
    }
}