#pragma once
#ifndef MYFORM_H
#define MYFORM_H

#include <Windows.h>
#include <thread>
#include "Server.h"

namespace ProjectMPointer {

    using namespace System;
    using namespace System::ComponentModel;
    using namespace System::Collections;
    using namespace System::Windows::Forms;
    using namespace System::Data;
    using namespace System::Drawing;

    public ref class MyForm : public System::Windows::Forms::Form {
    public:
        MyForm(void) {
            InitializeComponent();
            // Iniciar el servidor de sockets en un hilo separado
            servidor = new Servidor(8080);
            serverThread = gcnew System::Threading::Thread(gcnew System::Threading::ThreadStart(this, &MyForm::iniciarServidor));
            serverThread->Start();
        }

    protected:
        ~MyForm() {
            if (components) {
                delete components;
            }
            // Detener el servidor al cerrar la aplicación
            if (servidor != nullptr) {
                servidor->detener();
                delete servidor;
            }
        }

    private:
        System::Windows::Forms::Label^ lbltitulo;
        System::Windows::Forms::TextBox^ txtMensajes;
        System::ComponentModel::Container^ components;
        Servidor* servidor;
        System::Threading::Thread^ serverThread;

        void iniciarServidor() {
            servidor->iniciar();
        }

        void mostrarMensaje(String^ mensaje) {
            // Verificar si se necesita invocar en el hilo de la interfaz gráfica
            if (this->InvokeRequired) {
                this->Invoke(gcnew Action<String^>(this, &MyForm::mostrarMensaje), mensaje);
            }
            else {
                txtMensajes->AppendText(mensaje + "\r\n");
            }
        }

#pragma region Windows Form Designer generated code
        void InitializeComponent(void) {
            this->lbltitulo = (gcnew System::Windows::Forms::Label());
            this->txtMensajes = (gcnew System::Windows::Forms::TextBox());
            this->SuspendLayout();
            // 
            // lbltitulo
            // 
            this->lbltitulo->AutoSize = true;
            this->lbltitulo->Font = (gcnew System::Drawing::Font(L"Arial Black", 24, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lbltitulo->ForeColor = System::Drawing::SystemColors::ActiveCaption;
            this->lbltitulo->Location = System::Drawing::Point(350, 28);
            this->lbltitulo->Name = L"lbltitulo";
            this->lbltitulo->Size = System::Drawing::Size(174, 45);
            this->lbltitulo->TabIndex = 0;
            this->lbltitulo->Text = L"MPointer";
            // 
            // txtMensajes
            // 
            this->txtMensajes->Location = System::Drawing::Point(50, 100);
            this->txtMensajes->Multiline = true;
            this->txtMensajes->Name = L"txtMensajes";
            this->txtMensajes->Size = System::Drawing::Size(785, 300);
            this->txtMensajes->TabIndex = 1;
            // 
            // MyForm
            // 
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(885, 430);
            this->Controls->Add(this->txtMensajes);
            this->Controls->Add(this->lbltitulo);
            this->Name = L"MyForm";
            this->Text = L"Memory Manager";
            this->ResumeLayout(false);
            this->PerformLayout();
        }
#pragma endregion
    };
}
#endif  
