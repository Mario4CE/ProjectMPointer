

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

    try {
        std::istringstream iss(peticionStr);
        std::string comando;
        iss >> comando;

        if (comando == "New") {
            std::string tipo;
            iss >> tipo;

            if (tipo == "int") {
                *mptrInt = MPointer<int>::New();
                respuesta = "Nuevo MPointer<int> creado ";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
            }
            else if (tipo == "double") {
                *mptrDouble = MPointer<double>::New();
                respuesta = "Nuevo MPointer<double> creado ";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
            }
            else if (tipo == "char") {
                *mptrChar = MPointer<char>::New();
                respuesta = "Nuevo MPointer<char> creado ";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
            }
            else if (tipo == "string") {
                *mptrStr = MPointer<std::string>::New();
                respuesta = "Nuevo MPointer<std::string> creado ";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
            }
            else if (tipo == "float") {
                *mptrFloat = MPointer<float>::New();
                respuesta = "Nuevo MPointer<float> creado ";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
            }
            else {
                respuesta = "Tipo no soportado: " + tipo;
                this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
            }
        }

        //comando de Cerrar, cierra el cliente y el server
        else if (comando == "Cerrar") {
            respuesta = "Cerrando cliente";
            this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
            this->Close();
        }

        //Comando Ayuda
        else if (comando == "Ayuda") {
            respuesta = "Comandos disponibles:\n";
            respuesta += "New int\n";
            respuesta += "New double\n";
            respuesta += "New char\n";
            respuesta += "New string\n";
            respuesta += "New float\n";
            this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
        }

        //Comando Estado
        else if (comando == "Estado") {
            respuesta = "Estado"; //Pregunta por el estado del server
            this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
        }

        //Si el comando no es reconocido
        else {
            if (conectado) {
                respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
                this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
            }
            else {
                respuesta = "Error: No hay conexión establecida. Intente conectar primero.";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
            }
        }
    }

    catch (const std::exception& e) {
        respuesta = "Error: " + std::string(e.what());
        this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
    }

    this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
}