

#include "Cliente.h"
#include "SocketUtils.h"
#include "MPointer.h"
#include <msclr/marshal_cppstd.h>
#include <vector>
#include <sstream>

//Metodo principal para enviar la peticion al servidor

System::Void ClienteMPointers::Cliente::btnCliente_Click(System::Object^ sender, System::EventArgs^ e) {
    // Inicializar MPointer si no se ha hecho
    static bool initialized = false;
    if (!initialized) {
        MPointer<int>::Init("127.0.0.1", 12345);
        MPointer<double>::Init("127.0.0.1", 12345);
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
            }
            else if (tipo == "double") {
                *mptrDouble = MPointer<double>::New();
                respuesta = "Nuevo MPointer<double> creado ";
            }
            else {
                respuesta = "Tipo no soportado: " + tipo;
                this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
            }
        }
        else {
            respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
        }
    }
    catch (const std::exception& e) {
        respuesta = "Error: " + std::string(e.what());
        this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
    }

    this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
}