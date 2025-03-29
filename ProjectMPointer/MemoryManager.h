
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
        size_t offset;
        int refCount;
        std::string type;

        MemoryBlock() : id(0), size(0), offset(0), refCount(0), type("") {}
        MemoryBlock(int i, size_t s, size_t o, const std::string& t)
            : id(i), size(s), offset(o), refCount(1), type(t) {
        }
    };

    static inline constexpr size_t TOTAL_MEMORY = 1024 * 1024 * 1024; // 1GB

    static MemoryManager& getInstance() {
        static MemoryManager instance;
        return instance;
    }

    MemoryManager(const MemoryManager&) = delete;
    void operator=(const MemoryManager&) = delete;

    void setClientSocket(SOCKET socket) { clientSocket = socket; }
    SOCKET getClientSocket() const { return clientSocket; }
    static std::string processRequest(const std::string& request);
    static void initialize();
    static std::vector<std::string> getMemoryState();

public:
    SOCKET clientSocket;

    static char* memoryPool;
    static std::unordered_map<int, MemoryBlock> memoryBlocks;
    static std::vector<std::pair<size_t, size_t>> freeBlocks;
    static int nextId;
    static size_t nextFree;

    MemoryManager() : clientSocket(INVALID_SOCKET) {}

private:
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