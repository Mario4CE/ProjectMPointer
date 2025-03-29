#ifndef MEMORYLOGGER_H
#define MEMORYLOGGER_H

#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iostream>
#include <chrono>

/*
* Clase que se encarga de guardar información en un archivo de logs
* Informacion aparte de la de la info y error logger
*/
class MemoryLogger {
public:
    // Guarda una petición en un archivo de logs
    static void logRequest(const std::string& request);

    // Guarda el estado de la memoria en un archivo con fecha y hora
    static void logMemoryState(const std::vector<std::string>& memoryState);
};

#endif // MEMORYLOGGER_H
