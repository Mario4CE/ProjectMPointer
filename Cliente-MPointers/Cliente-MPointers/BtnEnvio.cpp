
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
            std::string entero;
            iss >> entero;
            std::string tipo;
            iss >> tipo;

            if (tipo == "int") {
                *mptrInt = MPointer<int>::New();
                respuesta = "Nuevo MPointer<double> creado ";
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

            }

            else {
                respuesta = "Tipo no soportado: " + tipo;
                this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
            }
        }

        //Comando set. Se pone set y un id que se le envia al server para asignarle un valor al id seleccionado
        else if (comando == "Set") {
            std::string id;
            iss >> id;
            std::string valor;
            iss >> valor;
            // Aquí se puede agregar la lógica para enviar el comando al servidor
            respuesta = "Set " + id + " a " + valor;
            this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
        }

        //Comando get. Se pone get y el id para saber el valor que tiene ese dato
        else if (comando == "Get") {
            std::string id;
            iss >> id;
            // Aquí se puede agregar la lógica para enviar el comando al servidor
            respuesta = "Get " + id; // Aquí se puede agregar la lógica para obtener el valor del servidor
            this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
        }

        //Comando Increase
        else if (comando == "Increase") {
            std::string id;
            iss >> id;
            // Aquí se puede agregar la lógica para enviar el comando al servidor
            respuesta = "Increase " + id; // Aquí se puede agregar la lógica para aumentar el valor en el servidor
            this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
        }

        //Comando decrease
        else if (comando == "Decrease") {
            std::string id;
            iss >> id;
            // Aquí se puede agregar la lógica para enviar el comando al servidor
            respuesta = "Decrease " + id; // Aquí se puede agregar la lógica para disminuir el valor en el servidor
            this->lblRespuesta->ForeColor = System::Drawing::Color::Blue;
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
            respuesta = "Error: Comando no reconocido.";
            this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
        }
    }

    catch (const std::exception& e) {
        respuesta = "Error al recibir: " + std::string(e.what());
        this->lblRespuesta->ForeColor = System::Drawing::Color::Yellow;
    }

    this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
}