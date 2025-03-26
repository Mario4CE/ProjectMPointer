
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring> // Para memcpy
#include <utility> // Para std::pair

class MemoryManager {
public:
    // Tamaño total de la memoria (1GB)
    static constexpr size_t TOTAL_MEMORY = 1024 * 1024 * 1024; // 1 GB en bytes

    // Estructura de un bloque de memoria
    struct MemoryBlock {
        int id;
        size_t size;
        std::string type;
        int refCount;
        std::vector<char> data;

        // Constructor por defecto necesario para std::unordered_map
        MemoryBlock() : id(0), size(0), type(""), refCount(0) {}

        // Constructor con parámetros
        MemoryBlock(int i, size_t s, const std::string& t)
            : id(i), size(s), type(t), refCount(1), data(s) {
        }

        // Constructor de copia
        MemoryBlock(const MemoryBlock&) = default;

        // Operador de asignación
        MemoryBlock& operator=(const MemoryBlock&) = default;
    };

    // Inicializar el sistema de memoria
    static void initialize();

    // Obtener estado actual de la memoria
    static std::vector<std::string> getMemoryState();

    // Procesar comandos (Create, Set, Get, etc.)
    static std::string processRequest(const std::string& request);

private:
    // Pool de memoria principal
    static std::vector<char> memoryPool;

    // Bloques asignados (mapeados por ID)
    static std::unordered_map<int, MemoryBlock> memoryBlocks;

    // Bloques libres (lista de pares <offset, tamaño>)
    static std::vector<std::pair<size_t, size_t>> freeBlocks;

    // Contador para IDs
    static int nextId;

    // Funciones de manejo de comandos
    static std::string handleCreate(const std::string& size, const std::string& type);
    static std::string handleSet(int id, const std::string& value);
    static std::string handleGet(int id);
    static std::string handleIncreaseRefCount(int id);
    static std::string handleDecreaseRefCount(int id);

    // Validación de tipos
    static bool validateSizeForType(const std::string& type, size_t size);

    // Asignación de memoria
    static bool allocateMemory(size_t size, MemoryBlock& block);

    // Liberación de memoria
    static void releaseMemory(int id);

    // Helper para encontrar bloque por ID
    static MemoryBlock* findBlock(int id);
};

#endif // MEMORYMANAGER_H