
#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <vector>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cstring>
#include <utility>

#ifdef _WIN32
#include <winsock2.h>
#else
typedef int SOCKET;
const int INVALID_SOCKET = -1;
#endif

class MemoryManager {
public:
    struct MemoryBlock {
        int id;
        size_t size;
        std::string type;
        int refCount;
        std::vector<char> data;

        MemoryBlock() : id(0), size(0), type(""), refCount(0) {}
        MemoryBlock(int i, size_t s, const std::string& t)
            : id(i), size(s), type(t), refCount(1), data(s) {
        }
    };

    // Constante como variable estática inline (C++17)
    static inline constexpr size_t TOTAL_MEMORY = 1024 * 1024 * 1024; // 1GB

    // Método Singleton
    static MemoryManager& getInstance() {
        static MemoryManager instance;  // Variable estática local (thread-safe en C++11)
        return instance;
    }

    // Eliminar operaciones de copia
    MemoryManager(const MemoryManager&) = delete;
    void operator=(const MemoryManager&) = delete;

    // Métodos públicos
    void setClientSocket(SOCKET socket) { clientSocket = socket; }
    SOCKET getClientSocket() const { return clientSocket; }
    static std::string processRequest(const std::string& request);
    static void initialize();
    static std::vector<std::string> getMemoryState();

public:
    // Miembros de instancia
    SOCKET clientSocket;

    // Miembros estáticos (solo declaración, no definición)
    static std::vector<char> memoryPool;
    static std::unordered_map<int, MemoryBlock> memoryBlocks;
    static std::vector<std::pair<size_t, size_t>> freeBlocks;
    static int nextId;

    // Constructor privado
    MemoryManager() : clientSocket(INVALID_SOCKET) {}

    // Métodos privados
    static std::string handleCreate(const std::string& size, const std::string& type);
    static std::string handleSet(int id, const std::string& value);
    static std::string handleGet(int id);
    static std::string handleIncreaseRefCount(int id);
    static std::string handleDecreaseRefCount(int id);
    static bool validateSizeForType(const std::string& type, size_t size);
    static bool allocateMemory(size_t size, MemoryBlock& block);
    static void releaseMemory(int id);
    static MemoryBlock* findBlock(int id);
};
#endif // MEMORYMANAGER_H