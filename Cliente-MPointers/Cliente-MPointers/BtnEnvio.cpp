#include "Cliente.h"
#include "SocketUtils.h"
#include <msclr/marshal_cppstd.h>

namespace ClienteMPointers {

    System::Void Cliente::btnCliente_Click(System::Object^ sender, System::EventArgs^ e) {
        // Obtener la petición del TextBox
        System::String^ peticion = this->txtPeticion->Text;

        // Convertir System::String^ a std::string
        std::string peticionStr = msclr::interop::marshal_as<std::string>(peticion);

        // Inicializar la variable respuesta
        std::string respuesta = ""; // Inicializar con un valor predeterminado

        try {
            // Enviar la petición al servidor
            respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, peticionStr);

            // Si no hay error, establecer el color del texto en negro
            this->lblRespuesta->ForeColor = System::Drawing::Color::Black;
        }
        catch (const std::runtime_error& e) {
            // Capturar el mensaje de error
            respuesta = e.what();

            // Si hay un error, establecer el color del texto en rojo
            this->lblRespuesta->ForeColor = System::Drawing::Color::Red;
        }

        // Mostrar la respuesta en el Label
        this->lblRespuesta->Text = gcnew System::String(respuesta.c_str());
    }
}