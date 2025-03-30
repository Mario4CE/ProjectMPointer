
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
#include <cstring> 
#include <algorithm>


// Inicializar variables estáticas
char* MemoryManager::memoryPool = nullptr;
std::unordered_map<int, MemoryManager::MemoryBlock> MemoryManager::memoryBlocks;
std::vector<std::pair<size_t, size_t>> MemoryManager::freeBlocks;
int MemoryManager::nextId = 1;
size_t MemoryManager::nextFree = 0;

/*
* Metodo para inicializar la memoria
* Inicializa la memoria con 1GB
* Reinicia los bloques de memoria
* Reinicia los bloques libres
* Reinicia el ID
* Pone todo en orden para empezar a trabajar
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
* Esto es lo que se guarda cuando se ejecuta una orden
* Es lo que nos mantiene el registro de las peticiones
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

/*
* Metodo para procesar una peticion
* Este metodo resive la peticion del cliente y la procesa para lleamar al metodo correspondiente
* y mostrar errores si es necesario
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
            throw std::invalid_argument("Comando no reconocido, su puta madre: " + command);
        }
    }
    catch (const std::exception& e) {
        std::string errorMsg = "Error: " + std::string(e.what());
        ErrorLogger::logError(errorMsg);
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario(errorMsg);
        return errorMsg;
    }
}

/*
* Validacion de tamaño de bloque, tiene que ser coherente con el tipo, esto para ahorrar memoria
*/

bool MemoryManager::validateSizeForType(const std::string& type, size_t size) {
    std::string lowerType = type;
    std::transform(lowerType.begin(), lowerType.end(), lowerType.begin(), ::tolower); // Convertir a minúsculas

    static const std::unordered_map<std::string, size_t> typeSizes = {
        {"int", sizeof(int)},
        {"double", sizeof(double)},
        {"char", sizeof(char)},
        {"float", sizeof(float)}
    };

    auto it = typeSizes.find(lowerType);
    if (it != typeSizes.end()) {
        return size != 0 && size % it->second == 0; // Corrección: Verificar si es múltiplo y no cero
    }

    if (lowerType == "string" || lowerType == "str") {
        return true; // Para string, solo validamos que el tipo sea correcto.
        // Opcional: Puedes agregar validación basada en la longitud máxima de la cadena si es necesario.
    }

    throw std::invalid_argument("Tipo de dato no soportado: " + type);
}

/*
* Metodo para crear un bloque de memoria
* Es uno de los 5 metodos que resive del cliente
*/

std::string MemoryManager::handleCreate(const std::string& size, const std::string& type) {
    size_t blockSize = std::stoul(size);

    // Validar el tamaño del bloque
    if (!validateSizeForType(type, blockSize)) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: Tamaño incongruente con el tipo");
        ErrorLogger::logError("Error: Tamaño incongruente con el tipo");
        return "Error: Tamaño incongruente con el tipo";
    }

    MemoryBlock newBlock(nextId++, blockSize, 0, type); // Offset inicial es 0

    // Asignar memoria con el tamaño proporcionado por el cliente
    if (!allocateMemory(newBlock.size, newBlock)) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: No hay espacio suficiente");
        return "Error: No hay espacio suficiente";
    }

    memoryBlocks[newBlock.id] = newBlock;

    // Imprimir la dirección de memoria del bloque creado
    std::cout << "Bloque ID: " << newBlock.id << ", Dirección: " << (void*)(memoryPool + newBlock.offset) << ", Tamaño: " << newBlock.size << " bytes" << std::endl;
    
    //Se actuliza ellabell del formulario mostrando el Bloque con el Id creado y abajo de eso el estado de la memoria
    std::stringstream ss;
    ss << "Bloque ID: " << newBlock.id
        << ", Dirección: " << (void*)(memoryPool + newBlock.offset)
        << ", Tamaño: " << newBlock.size << " bytes";
    InterfazCLI::Respuestas::ActualizarLabelEnFormulario(ss.str());
    std::vector<std::string> memoryState = getMemoryState();
    MemoryLogger::logMemoryState(memoryState);

    getMemoryState();

    return "Creado bloque ID: " + std::to_string(newBlock.id);
}

/*
* Metodo para asignar un valor a un bloque de memoria
* Es uno de los 5 metodos que resive del cliente
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
* Metodo para obtener el valor de un bloque de memoria
* Es uno de los 5 metodos que resive del cliente
*/

std::string MemoryManager::handleGet(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        InterfazCLI::Respuestas::ActualizarLabelEnFormulario("Error: ID no encontrado: " + std::to_string(id));
        ErrorLogger::logError("Error: ID no encontrado: " + std::to_string(id));
        std::cerr << "Error: ID no encontrado: " << id << std::endl;
        return "Error: ID no encontrado";
    }

    // Leer el valor almacenado en el bloque de memoria
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
* Metodo para incrementar el contador de referencias de un bloque de memoria
* Es uno de los 5 metodos que resive del cliente
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
* Metodo para decrementar el contador de referencias de un bloque de memoria
* Es uno de los 5 metodos que resive del cliente
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

/*
* Metodo para asignar memoria
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
* Metodo para liberar memoria
*/

void MemoryManager::releaseMemory(int id) {
    auto blockPtr = findBlock(id);
    if (!blockPtr) {
        return;
    }

    freeBlocks.push_back({ blockPtr->offset, blockPtr->size });
    memoryBlocks.erase(id);
}

/*
Validacion que manda a hacer el metodo de Set
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
* Metodo para encontrar un bloque
*/

MemoryManager::MemoryBlock* MemoryManager::findBlock(int id) {
    auto it = memoryBlocks.find(id);
    if (it == memoryBlocks.end()) {
        return nullptr;
    }
    return &it->second;
}