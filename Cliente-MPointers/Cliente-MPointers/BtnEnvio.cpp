

#include "Cliente.h"
#include "SocketUtils.h"
#include "MPointer.h"
#include <msclr/marshal_cppstd.h>
#include <vector>
#include <sstream>
#include <map>
#include <variant>
#include <stdexcept>
#include <thread>
#include <future>
#include <mutex>

System::Void ClienteMPointers::Cliente::btnCliente_Click(System::Object^ sender, System::EventArgs^ e) {
    System::String^ peticion = this->txtPeticion->Text;
    std::string peticionStr = msclr::interop::marshal_as<std::string>(peticion);
    std::promise<std::string> respuestaPromise;
    std::future<std::string> respuestaFuture = respuestaPromise.get_future();

    std::thread([this, peticionStr, &respuestaPromise]() {
        std::string respuesta = "";
        try {
            std::istringstream iss(peticionStr);
            std::string comando;
            iss >> comando;

            if (comando == "New") {
                // ... (tu lógica para crear MPointer)
            }
            else if (comando == "Cerrar") {
                // ... (tu lógica para cerrar la aplicación)
            }
            else if (comando == "Ayuda") {
                // ... (tu lógica para mostrar la ayuda)
            }
            else if (comando == "Estado") {
                // ... (tu lógica para obtener el estado)
            }
            else {
                if (conectado) {
                    respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);
                }
                else {
                    respuesta = "Error: No hay conexión establecida. Intente conectar primero.";
                }
            }
        }
        catch (const std::exception& e) {
            respuesta = "Error: " + std::string(e.what());
        }
        respuestaPromise.set_value(respuesta);
        }).detach();

    std::string respuesta = respuestaFuture.get();
    this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
    if (respuesta.find("Error:") != std::string::npos) {
        this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
    }
    else {
        this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
    }
}