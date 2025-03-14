#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <string>

class MemoryManager {
public:
    // Tamaño de la memoria en MB
    static const int SIZE_MB = 1024; // 1 GB de memoria (puedes cambiarlo)

    // Simulación del estado de la memoria
    static std::vector<std::string> getMemoryState();

    // Simulación de una petición que modifica la memoria
    static void processRequest(const std::string& request);

private:
    // Estado de la memoria (simulación)
    static std::vector<std::string> memoryBlocks;

    // Inicializar la memoria
    static void initializeMemory();
};

#endif // MEMORYMANAGER_H
