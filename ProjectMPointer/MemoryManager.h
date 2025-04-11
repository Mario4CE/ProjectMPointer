

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

/*
* Clase MemoryManager
* Esta clase se encarga de manejar la memoria
* Tiene metodos para crear, asignar, liberar y obtener bloques de memoria
* Es una de nuestras clases mas importantes
* Es la que usaremos para manejar los MPointers y si la ocupamos llamar desde otro lado
*/
class MemoryManager {
public:
    /*
    * Son las estructuras que usaremos para manejar la memoria y las que guardaremos en los dum_folder
    */
    struct MemoryBlock {
        int id;
        size_t size;
        size_t offset;
        std::string type;
        int refCount;
        std::vector<std::string> references; // Para almacenar varias referencias

        // Constructor predeterminado
        MemoryBlock() : id(0), size(0), offset(0), type(""), refCount(0) {}

        // Constructor con parámetros
        MemoryBlock(int id, size_t size, size_t offset, const std::string& type)
            : id(id), size(size), offset(offset), type(type), refCount(0) {
        }

        // Método para agregar una referencia
        void addReference(const std::string& ref) {
            references.push_back(ref); // Agregar la referencia al vector
            refCount++; // Actualizar el contador de referencias
        }

        // Método para actualizar una referencia existente (si es necesario)
        void updateReference(int index, const std::string& newRef) {
            if (index >= 0 && index < references.size()) {
                references[index] = newRef; // Actualiza la referencia en el índice especificado
            }
        }
    };



    static inline constexpr size_t TOTAL_MEMORY = 1024 * 1024 * 1024; // 1GB total de memoria para el pool

    static MemoryManager& getInstance() { //Metodo para obtener la instancia
        static MemoryManager instance;
        return instance;
    }

    MemoryManager(const MemoryManager&) = delete; //Metodo para borrar la instancia
    void operator=(const MemoryManager&) = delete;

    void setClientSocket(SOCKET socket) { clientSocket = socket; } //Metodo para asignar el socket
    SOCKET getClientSocket() const { return clientSocket; } //Metodo para obtener el socket
    static std::string processRequest(const std::string& request); //Metodo para procesar la peticion
    static void initialize(); //Metodo para inicializar la memoria
    static std::vector<std::string> getMemoryState(); //Metodo para obtener el estado de la memoria

public:
    SOCKET clientSocket;

    static char* memoryPool; //Pool de memoria
    static std::unordered_map<int, MemoryBlock> memoryBlocks; //Mapa de bloques de memoria
    static std::vector<std::pair<size_t, size_t>> freeBlocks; //Vector de bloques libres
    static int nextId; //ID del siguiente bloque
    static size_t nextFree; //Siguiente bloque libre

    MemoryManager() : clientSocket(INVALID_SOCKET) {}

private:
    static std::string handleCreate(const std::string& size, const std::string& type); //Metodo Create
    static std::string handleSet(int id, const std::string& value); //Metodo Set
    static std::string handleGet(int id); //Metodo Get
    static std::string handleIncreaseRefCount(int id); //Metodo IncreaseRefCount
    static std::string handleDecreaseRefCount(int id); //Metodo DecreaseRefCount
    static bool validateSizeForType(const std::string& type, size_t size); //Metodo para validar el tama�o del tipo
    static bool allocateMemory(size_t size, MemoryBlock& block); //Metodo para asignar memoria
    static void releaseMemory(int id); //Metodo para liberar memoria
    static MemoryBlock* findBlock(int id); //Metodo para encontrar un bloque
    static bool validateDataType(const std::string& blockType, const std::string& value, size_t expectedSize);
    //Metodo para validar el tipo de dato
    static std::string validateBlockSize(const std::string& size, const std::string& type, size_t& blockSize);
    //Metodo para validar el tama�o del bloque
    static void updateUIWithBlockInfo(const MemoryBlock& newBlock); //Metodo para actualizar la UI con la informacion del bloque
    static void sendBlockCreationMessage(const MemoryBlock& newBlock); //Metodo para enviar un mensaje al cliente con la informacion del bloque
    static MemoryBlock createMemoryBlock(size_t blockSize, const std::string& type); //Metodo para crear un bloque de memoria
    static std::string logAndReturnError(const std::string& message, const std::string& error = ""); //Metodo para loggear y retornar un error
};

#endif // MEMORYMANAGER_H
