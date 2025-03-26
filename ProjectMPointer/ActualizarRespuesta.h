#pragma once
#include "interfaz.h"
#include <string>
#include <msclr/marshal_cppstd.h>
#include <msclr/lock.h> 

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Threading;
using namespace System::Text;  // ¡Esto es lo que faltaba para Encoding!

namespace InterfazCLI {
    public ref class Respuestas {
    private:
        static InterfazCLI::FormularioPrincipal^ formulario;
        static System::Object^ lockObject = gcnew System::Object();
        static TcpClient^ clientSocket;

    public:
        static void SetFormulario(InterfazCLI::FormularioPrincipal^ form) {
            formulario = form;
        }

        static void SetClientSocket(TcpClient^ client) {
            msclr::lock l(lockObject);
            clientSocket = client;
        }

        static void ActualizarLabelEnFormulario(const std::string& texto) {
            if (formulario != nullptr) {
                String^ textoCLI = gcnew String(texto.c_str());
                if (formulario->InvokeRequired) {
                    formulario->Invoke(gcnew Action<String^>(formulario, &InterfazCLI::FormularioPrincipal::ActualizarRespuesta), textoCLI);
                }
                else {
                    formulario->ActualizarRespuesta(textoCLI);
                }
            }
        }

        static void SendMessage(const std::string& mensaje) {
            msclr::lock l(lockObject);

            if (clientSocket != nullptr && clientSocket->Connected) {
                try {
                    array<unsigned char>^ buffer = Encoding::ASCII->GetBytes(gcnew String(mensaje.c_str()));

                    NetworkStream^ stream = clientSocket->GetStream();
                    stream->Write(buffer, 0, buffer->Length);
                }
                catch (Exception^ ex) {
                    System::Diagnostics::Debug::WriteLine("Error enviando mensaje: " + ex->Message);
                }
            }
            else {
                System::Diagnostics::Debug::WriteLine("Error: Socket no conectado para enviar mensaje");
            }
        }
    };
}