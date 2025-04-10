
#include "Cliente.h"
#include "SocketUtils.h"
#include "MPointer.h"
#include <msclr/marshal_cppstd.h>
#include <vector>
#include <sstream>
#include <map>
#include <variant>
#include <stdexcept> 

System::Void ClienteMPointers::Cliente::btnCliente_Click(System::Object^ sender, System::EventArgs^ e) {

    System::String^ peticion = this->txtPeticion->Text;
    std::string peticionStr = msclr::interop::marshal_as<std::string>(peticion);
    std::string respuesta = "";
    System::Drawing::Color colorRespuesta = System::Drawing::Color::Blue;

    try {
        std::istringstream iss(peticionStr);
        std::string comando;
        iss >> comando;

        // ----------- COMANDO NEW -----------
        if (comando == "New") {
            std::string valorStr, tipo;
            iss >> valorStr >> tipo;

            std::cout << "DEBUG - tipo: " << tipo << ", valorStr: '" << valorStr << "'" << std::endl;

            // Validar que valorStr no esté vacío
            if (valorStr.empty()) {
                respuesta = "Error: Falta el valor. Uso correcto: New <tipo> <valor>";
                colorRespuesta = System::Drawing::Color::Red;
                return;
            }

            int valor;
            try {
                valor = std::stoi(valorStr);
            }
            catch (const std::exception& e) {
                respuesta = "Error: El valor proporcionado no es un número válido.";
                colorRespuesta = System::Drawing::Color::Red;
                return;
            }

            // Ahora sí, crear el MPointer según el tipo
            if (tipo == "int") {
                mptrInt = new MPointer<int>(MPointer<int>::New(valor));
                respuesta = "Nuevo MPointer<int> creado con valor " + std::to_string(valor);
            }
            else if (tipo == "double") {
                mptrDouble = new MPointer<double>(MPointer<double>::New(valor));
                *mptrDouble = static_cast<double>(valor);
                respuesta = "Nuevo MPointer<double> creado con valor " + std::to_string(valor);
            }
            else if (tipo == "char") {
                mptrChar = new MPointer<char>(MPointer<char>::New(valor));
                *mptrChar = static_cast<char>(valor);
                respuesta = std::string("Nuevo MPointer<char> creado con valor '") + static_cast<char>(valor) + "'";
            }
            else if (tipo == "string") {
                mptrStr = new MPointer<std::string>(MPointer<std::string>::New(valor));
                *mptrStr = std::to_string(valor);
                respuesta = "Nuevo MPointer<string> creado con valor \"" + std::to_string(valor) + "\"";
            }
            else if (tipo == "float") {
                mptrFloat = new MPointer<float>(MPointer<float>::New(valor));
                *mptrFloat = static_cast<float>(valor);
                respuesta = "Nuevo MPointer<float> creado con valor " + std::to_string(valor);
            }
            else {
                respuesta = "Tipo no soportado: " + tipo;
                colorRespuesta = System::Drawing::Color::Red;
            }
        }

        // ----------- COMANDO SET -----------
        else if (comando == "Set") {
            std::string idStr, valorStr;
            iss >> idStr >> valorStr;

            // Validar ID
            if (idStr.empty() || !std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {
                respuesta = "ID inválido: " + idStr;
                colorRespuesta = System::Drawing::Color::Red;
                this->lblRespuesta->ForeColor = colorRespuesta;
                this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
                return;
            }

            try {
                // Crear un MPointer temporal solo para usar sendRequest
                MPointer<int> temp;
                std::string comando = "Set " + idStr + " " + valorStr;
                std::string response = temp.sendRequest(comando);

                if (response.find("Error:") == std::string::npos) {
                    respuesta = "Operación exitosa! Respuesta del servidor con exito" + response;
                    colorRespuesta = System::Drawing::Color::Green; // Color verde para éxito
                }
                else {
                    respuesta = "Error al asignar valor: " + response;
                    colorRespuesta = System::Drawing::Color::Red;
                }
            }
            catch (const std::exception& e) {
                respuesta = "Error al comunicarse con el servidor: " + std::string(e.what());
                colorRespuesta = System::Drawing::Color::Red;
            }
        }

        // ----------- COMANDO GET -----------
        else if (comando == "Get") {
            std::string idStr;
            iss >> idStr;

            // Validar ID
            if (idStr.empty() || !std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {
                respuesta = "ID inválido: " + idStr;
                colorRespuesta = System::Drawing::Color::Red;
            }
            else {
                try {
                    // Crear un MPointer temporal para usar sendRequest
                    MPointer<int> temp;
                    std::string comandoGet = "Get " + idStr;
                    std::string response = temp.sendRequest(comandoGet);

                    // Analizar la respuesta del servidor
                    if (response.find("Error:") != std::string::npos) {
                        respuesta = response;
                        colorRespuesta = System::Drawing::Color::Red;
                    }
                    else {
                        respuesta = "Valor del bloque " + idStr + ": " + response;
                        colorRespuesta = System::Drawing::Color::Green;
                    }
                }
                catch (const std::exception& e) {
                    respuesta = "Error al comunicarse con el servidor: " + std::string(e.what());
                    colorRespuesta = System::Drawing::Color::Red;
                }
            }

            // Actualizar la interfaz
            this->lblRespuesta->ForeColor = colorRespuesta;
            this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
        }

        // ----------- COMANDO INCREASE -----------
        else if (comando == "Increase") {
            std::string idStr;
            iss >> idStr;

            // Validar ID
            if (idStr.empty() || !std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {
                respuesta = "ID inválido: " + idStr;
                colorRespuesta = System::Drawing::Color::Red;
            }
            else {
                try {
                    // Crear un MPointer temporal para usar sendRequest
                    MPointer<int> temp(std::stoi(idStr));
                    temp.increase();

                    respuesta = "Valor incrementado para el bloque " + idStr;
                    colorRespuesta = System::Drawing::Color::Green;
                }
                catch (const std::exception& e) {
                    respuesta = "Error al incrementar valor: " + std::string(e.what());
                    colorRespuesta = System::Drawing::Color::Red;
                }
            }
            }

            // ----------- COMANDO DECREASE -----------
        else if (comando == "Decrease") {
                std::string idStr;
                iss >> idStr;

                // Validar ID
                if (idStr.empty() || !std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {
                    respuesta = "ID inválido: " + idStr;
                    colorRespuesta = System::Drawing::Color::Red;
                }
                else {
                    try {
                        // Crear un MPointer temporal para usar sendRequest
                        MPointer<int> temp(std::stoi(idStr));
                        temp.decrease();

                        respuesta = "Valor decrementado para el bloque " + idStr;
                        colorRespuesta = System::Drawing::Color::Green;
                    }
                    catch (const std::exception& e) {
                        respuesta = "Error al decrementar valor: " + std::string(e.what());
                        colorRespuesta = System::Drawing::Color::Red;
                    }
                }
                }

        // ----------- COMANDO CERRAR -----------
        else if (comando == "Cerrar") {
            try {
                // Usamos MPointer<int> como tipo genérico para llamar a la función estática
                if (MPointer<int>::closeServer()) {
                    respuesta = "Servidor cerrado correctamente";
                    // Opcional: Cerrar la aplicación cliente también
                    this->Close();
                }
                else {
                    respuesta = "El servidor no pudo cerrarse correctamente";
                    colorRespuesta = System::Drawing::Color::Red;
                }
            }
            catch (const std::exception& e) {
                respuesta = "Error al cerrar el servidor: " + std::string(e.what());
                colorRespuesta = System::Drawing::Color::Red;
            }
            }

            // ----------- COMANDO AYUDA -----------
        else if (comando == "Ayuda") {
            respuesta =
                "Comandos disponibles:\n"
                "New <tipo> (int, double, char, string, float)\n"
                "Set <id> <valor>\n"
                "Get <id>\n"
                "Increase <id>\n"
                "Decrease <id>\n"
                "Cerrar\n"
                "Estado";
        }

        // ----------- COMANDO ESTADO -----------
        else if (comando == "Estado") {
            respuesta = "Estado en desarrollo";
        }

        else {
            respuesta = "Error: Comando no reconocido.";
            colorRespuesta = System::Drawing::Color::Red;
        }
    }
    catch (const std::exception& e) {
        respuesta = "Error al recibir: " + std::string(e.what());
        colorRespuesta = System::Drawing::Color::Red;
    }

    // Mostrar respuesta
    this->lblRespuesta->ForeColor = colorRespuesta;
    this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
}
