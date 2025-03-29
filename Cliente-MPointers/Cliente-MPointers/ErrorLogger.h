#ifndef ERRORLOGGER_H
#define ERRORLOGGER_H

#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <chrono>

/*
* Clase para registrar mensajes de error en un archivo log
*/
class ErrorLogger {
public:
    // Registrar un mensaje de error
    static void logError(const std::string& message);

private:
    // Obtener la fecha y hora actual formateada
    static std::string getCurrentTime();
};

#endif // ERRORLOGGER_H
