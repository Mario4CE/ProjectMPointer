
#include "Cliente.h"
#include "SocketUtils.h"
#include "MPointer.h"
#include <msclr/marshal_cppstd.h>
#include <vector>
#include <sstream>

System::Void ClienteMPointers::Cliente::btnCliente_Click(System::Object^ sender, System::EventArgs^ e) {
    // Inicializar MPointer si no se ha hecho
    static bool initialized = false;
    if (!initialized) {

        MPointer<int>::Init("127.0.0.1", 12345);
        MPointer<double>::Init("127.0.0.1", 12345);
        MPointer<float>::Init("127.0.0.1", 12345);
        MPointer<std::string>::Init("127.0.0.1", 12345);

        initialized = true;
    }

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
                respuesta = "Nuevo MPointer<int> creado con ID: ";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
            }
            else if (tipo == "double") {
                *mptrDouble = MPointer<double>::New();
                respuesta = "Nuevo MPointer<double> creado ";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
            }
            else if (tipo == "float") {
                *mptrFloat = MPointer<float>::New();
                respuesta = "Nuevo MPointer<float> creado";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
            }
            else if (tipo == "string") {
                *mptrStr = MPointer<std::string>::New();
                respuesta = "Nuevo MPointer<std::string> creado";
                this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
            }
            else {
                respuesta = "Tipo no soportado: " + tipo;
                this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
            }
        }
        else if (comando == "Get") {
            respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
        }
        else if (comando == "Set") {
            respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
        }
        else if (comando == "IncreaseRefCount") {
            respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
        }
        else if (comando == "DecreaseDefcount") {
            respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
            //se limpia el contenido de la respuesta
            this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
        }
        else if (comando == "Cerrar") {
            respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
            this->Close();
        }
        else if (comando == "Ayuda") {
            respuesta = "Comandos soportados: New, Get , Set, IncreaseRefCount , DecreaseDefcount, Cerrar";
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
        }
        else {
            respuesta = "Comando no soportado: " + comando;
            this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
        }
    }
    catch (const std::exception& e) {
        respuesta = "Error: " + std::string(e.what());
        this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
    }

    this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
}