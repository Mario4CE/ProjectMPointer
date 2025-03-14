#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <string>

class MemoryManager {
public:
    // Tama�o de la memoria en MB
    static const int SIZE_MB = 1024; // 1 GB de memoria (puedes cambiarlo)

    // Simulaci�n del estado de la memoria
    static std::vector<std::string> getMemoryState();

    // Simulaci�n de una petici�n que modifica la memoria
    static void processRequest(const std::string& request);

private:
    // Estado de la memoria (simulaci�n)
    static std::vector<std::string> memoryBlocks;

    // Inicializar la memoria
    static void initializeMemory();
};

#endif // MEMORYMANAGER_H
