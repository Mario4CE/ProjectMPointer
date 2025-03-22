#pragma once
#include "interfaz.h"
#include <string>

using namespace System;

// Clase que se encarga de llamar a la funcion de ActualizarRespuesta en el formulario principal

namespace InterfazCLI {
    public ref class Respuestas {
    private:
        static InterfazCLI::FormularioPrincipal^ formulario;

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
    };
}