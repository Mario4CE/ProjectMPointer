#pragma once
#include "interfaz.h"
#include <string>

using namespace System;

namespace InterfazCLI {
    public ref class Respuestas {
    private:
        static InterfazCLI::FormularioPrincipal^ formulario;

    public:
        static void SetFormulario(InterfazCLI::FormularioPrincipal^ form) {
            formulario = form;
        }

        static void ActualizarLabelEnFormulario(const std::string& texto) {
            if (formulario != nullptr) {
                String^ textoCLI = gcnew String(texto.c_str());
                formulario->ActualizarRespuesta(textoCLI);
            }
        }
    };
}