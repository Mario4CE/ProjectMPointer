
#include "Interfaz.h"
#include <Windows.h>
#include <string>

using namespace System;

// clase que se encarga de actualizar la respuesta en el formulario

namespace InterfazCLI {
    public ref class Respuestas {
    private:
        static InterfazCLI::FormularioPrincipal^ formulario;
        static Object^ lockObject = gcnew Object();

    public:
        static void SetFormulario(InterfazCLI::FormularioPrincipal^ form) {
            formulario = form;
        }

        static void ActualizarLabelEnFormulario(const std::string& texto) { //Metodo que cambia el texto del label en el formulario
            if (formulario != nullptr) {
                String^ textoCLI = gcnew String(texto.c_str());
                formulario->ActualizarRespuesta(textoCLI);
            }
        }
        static void CerrarVentana() { //Metodo que cierra la ventana
            if (formulario != nullptr) {
                formulario->CerrarVentana();
            }
        }
    };
}
