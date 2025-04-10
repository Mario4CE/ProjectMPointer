
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
            std::string dummy, tipo;
            iss >> dummy >> tipo;

            if (tipo == "int") {
                mptrInt = new MPointer<int>(MPointer<int>::New());
                respuesta = "Nuevo MPointer<int> creado";
            }
            else if (tipo == "double") {
                mptrDouble = new MPointer<double>(MPointer<double>::New());
                respuesta = "Nuevo MPointer<double> creado";
            }
            else if (tipo == "char") {
                mptrChar = new MPointer<char>(MPointer<char>::New());
                respuesta = "Nuevo MPointer<char> creado";
            }
            else if (tipo == "string") {
                mptrStr = new MPointer<std::string>(MPointer<std::string>::New());
                respuesta = "Nuevo MPointer<string> creado";
            }
            else if (tipo == "float") {
                mptrFloat = new MPointer<float>(MPointer<float>::New());
                respuesta = "Nuevo MPointer<float> creado";
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

            // Validar si el ID es un número
            if (idStr.empty() || !std::all_of(idStr.begin(), idStr.end(), ::isdigit)) {
                respuesta = "ID inválido: " + idStr;
                colorRespuesta = System::Drawing::Color::Red;
            }
            else {
                int id = std::stoi(idStr);
                bool encontrado = false;

                // Buscar el MPointer correspondiente y obtener su valor
                if (mptrInt && &(*mptrInt) == id) {
                    try {
                        int valor = **mptrInt;  // Llama a operator* que envía "Get" al servidor
                        respuesta = "Valor (int): " + std::to_string(valor);
                        encontrado = true;
                    }
                    catch (const std::exception& e) {
                        respuesta = "Error al obtener valor: " + std::string(e.what());
                        colorRespuesta = System::Drawing::Color::Red;
                    }
                }
                else if (mptrDouble && &(*mptrDouble) == id) {
                    try {
                        double valor = **mptrDouble;
                        respuesta = "Valor (double): " + std::to_string(valor);
                        encontrado = true;
                    }
                    catch (...) {
                        respuesta = "Error al obtener valor double";
                        colorRespuesta = System::Drawing::Color::Red;
                    }
                }
                else if (mptrChar && &(*mptrChar) == id) {
                    try {
                        char valor = **mptrChar;
                        respuesta = "Valor (char): " + std::string(1, valor);
                        encontrado = true;
                    }
                    catch (...) {
                        respuesta = "Error al obtener valor char";
                        colorRespuesta = System::Drawing::Color::Red;
                    }
                }
                else if (mptrStr && &(*mptrStr) == id) {
                    try {
                        std::string valor = **mptrStr;
                        respuesta = "Valor (string): " + valor;
                        encontrado = true;
                    }
                    catch (...) {
                        respuesta = "Error al obtener valor string";
                        colorRespuesta = System::Drawing::Color::Red;
                    }
                }
                else if (mptrFloat && &(*mptrFloat) == id) {
                    try {
                        float valor = **mptrFloat;
                        respuesta = "Valor (float): " + std::to_string(valor);
                        encontrado = true;
                    }
                    catch (...) {
                        respuesta = "Error al obtener valor float";
                        colorRespuesta = System::Drawing::Color::Red;
                    }
                }

                if (!encontrado) {
                    respuesta = "No se encontró un MPointer con ID " + idStr;
                    colorRespuesta = System::Drawing::Color::Red;
                }
            }
        }

        else if (comando == "Increase") {
            std::string id;
            iss >> id;
            respuesta = "Increase " + id;
        }

        else if (comando == "Decrease") {
            std::string id;
            iss >> id;
            respuesta = "Decrease " + id;
        }

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
