
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring> // Para memcpy

class MemoryManager {
public:
    // Tama�o de la memoria en MB
    static const int SIZE_MB = 1024; // 1 GB de memoria (puedes cambiarlo)

    // Estructura de un bloque de memoria
    struct MemoryBlock {
        int id;                // Identificador �nico del bloque
        size_t size;           // Tama�o del bloque en bytes
        std::string type;      // Tipo de dato almacenado en el bloque
        int refCount;          // Contador de referencias
        std::vector<char> data; // Espacio de memoria reservado (simulado con un vector de bytes)
    };

    // Obtener el estado actual de la memoria
    static std::vector<std::string> getMemoryState();

    // Procesar una petici�n (comando)
    static std::string processRequest(const std::string& request);

private:
    // Estado de la memoria (simulaci�n)
    static std::unordered_map<int, MemoryBlock> memoryBlocks;

    // Contador para generar IDs �nicos
    static int nextId;

    // Inicializar la memoria
    static void initializeMemory();

    // Funciones para manejar peticiones
    static std::string handleCreate(const std::string& size, const std::string& type);
    static std::string handleSet(int id, const std::string& value);
    static std::string handleGet(int id);
    static std::string handleIncreaseRefCount(int id);
    static std::string handleDecreaseRefCount(int id);

    // Validar que el tama�o sea congruente con el tipo de dato
    static bool validateSizeForType(const std::string& type, size_t size);
};

#endif // MEMORYMANAGER_H
