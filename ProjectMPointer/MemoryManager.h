#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <string>
#include <unordered_map>

class MemoryManager {
public:
    // Tamaño de la memoria en MB
    static const int SIZE_MB = 1024; // 1 GB de memoria (puedes cambiarlo)

    // Estructura de un bloque de memoria
    struct MemoryBlock {
        int id;
        size_t size;
        std::string type;
        std::string value;
        int refCount;
    };

    // Simulación del estado de la memoria
    static std::vector<std::string> getMemoryState();

    // Procesar una petición
    static std::string processRequest(const std::string& request);

private:
    // Estado de la memoria (simulación)
    static std::unordered_map<int, MemoryBlock> memoryBlocks;

    // Contador para generar IDs únicos
    static int nextId;

    // Inicializar la memoria
    static void initializeMemory();

    // Funciones para manejar peticiones
    static std::string handleCreate(const std::string& size, const std::string& type);
    static std::string handleSet(int id, const std::string& value);
    static std::string handleGet(int id);
    static std::string handleIncreaseRefCount(int id);
    static std::string handleDecreaseRefCount(int id);
};

#endif // MEMORYMANAGER_H
