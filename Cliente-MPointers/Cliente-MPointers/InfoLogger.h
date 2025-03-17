#ifndef INFOLOGGER_H
#define INFOLOGGER_H

#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <chrono>

class InfoLogger {
public:
    // Registrar un mensaje de información
    static void logInfo(const std::string& message);

private:
    // Obtener la fecha y hora actual formateada
    static std::string getCurrentTime();
};

#endif // INFOLOGGER_H
