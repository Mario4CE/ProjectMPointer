
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

        // ----------- COMANDO NEW (TIPO DINÁMICO) -----------
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
        // ----------- OTROS COMANDOS BÁSICOS -----------
        else if (comando == "Set") {
            std::string id, valor;
            iss >> id >> valor;
            respuesta = "Set " + id + " a " + valor;
        }

        else if (comando == "Get") {
            std::string id;
            iss >> id;
            respuesta = "Get " + id;
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
            respuesta = "Cerrando cliente";
            colorRespuesta = System::Drawing::Color::Red;
            this->Close();
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
