

#pragma once

#include <msclr/gcroot.h>
#include "MPointer.h"
#include "SocketUtils.h" // Incluir SocketUtils
#include <iostream> // Incluir iostream para mensajes de depuraci?n

namespace ClienteMPointers {
    using namespace System;
    using namespace System::Windows::Forms;

    public ref class Cliente : public Form {
    private:
        MPointer<int>* mptrInt;
        MPointer<double>* mptrDouble;
        MPointer<float>* mptrFloat;
        MPointer<std::string>* mptrStr;
    private: System::Windows::Forms::Button^ BtnConectar;
           MPointer<char>* mptrChar;

           // Variable para rastrear el estado de la conexi?n
           bool conectado;

    public:
        Cliente(void) {
            InitializeComponent();

            // Inicializaci?n correcta
            mptrInt = new MPointer<int>();
            mptrDouble = new MPointer<double>();
            mptrFloat = new MPointer<float>();
            mptrStr = new MPointer<std::string>();

            // Inicialmente no conectado
            conectado = false;
            BtnConectar->Enabled = !conectado; // Habilitar solo si no est? conectado
        }

    protected:
        ~Cliente() {
            if (mptrInt != nullptr) {
                delete mptrInt;
                mptrInt = nullptr;
            }
            if (mptrDouble != nullptr) {
                delete mptrDouble;
                mptrDouble = nullptr;
            }
            if (mptrFloat != nullptr) {
                delete mptrFloat;
                mptrFloat = nullptr;
            }
            if (mptrStr != nullptr) {
                delete mptrStr;
                mptrStr = nullptr;
            }
            if (mptrChar != nullptr) {
                delete mptrChar;
                mptrChar = nullptr;
            }
        }

    private:
        // Controles de la interfaz gr?fica
        System::Windows::Forms::Button^ btnCliente;
        System::Windows::Forms::TextBox^ txtPeticion;
        System::Windows::Forms::Label^ lblRespuesta;
        System::Windows::Forms::Label^ lblTitle;

        // Contenedor de componentes
        System::ComponentModel::Container^ components;

        // Manejador de eventos para el clic del bot?n
        System::Void btnCliente_Click(System::Object^ sender, System::EventArgs^ e);

        // Declaraci?n de la funci?n EnviarPeticion
        //std::string EnviarPeticion(const std::string& peticion); // Declaraci?n correcta

#pragma region Windows Form Designer generated code
        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        void InitializeComponent(void) {
            this->btnCliente = (gcnew System::Windows::Forms::Button());
            this->txtPeticion = (gcnew System::Windows::Forms::TextBox());
            this->lblRespuesta = (gcnew System::Windows::Forms::Label());
            this->lblTitle = (gcnew System::Windows::Forms::Label());
            this->BtnConectar = (gcnew System::Windows::Forms::Button());
            this->SuspendLayout();
            //
            // btnCliente
            //
            this->btnCliente->BackColor = System::Drawing::SystemColors::InactiveCaption;
            this->btnCliente->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->btnCliente->Location = System::Drawing::Point(434, 151);
            this->btnCliente->Name = L"btnCliente";
            this->btnCliente->Size = System::Drawing::Size(224, 80);
            this->btnCliente->TabIndex = 0;
            this->btnCliente->Text = L"Enviar Petici?n";
            this->btnCliente->UseVisualStyleBackColor = false;
            this->btnCliente->Click += gcnew System::EventHandler(this, &Cliente::btnCliente_Click);
            //
            // txtPeticion
            //
            this->txtPeticion->Font = (gcnew System::Drawing::Font(L"Arial", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->txtPeticion->Location = System::Drawing::Point(44, 97);
            this->txtPeticion->Name = L"txtPeticion";
            this->txtPeticion->Size = System::Drawing::Size(290, 50);
            this->txtPeticion->TabIndex = 1;
            //
            // lblRespuesta
            //
            this->lblRespuesta->BackColor = System::Drawing::Color::LightGray;
            this->lblRespuesta->Font = (gcnew System::Drawing::Font(L"Arial Narrow", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lblRespuesta->Location = System::Drawing::Point(38, 172);
            this->lblRespuesta->Name = L"lblRespuesta";
            this->lblRespuesta->Size = System::Drawing::Size(308, 164);
            this->lblRespuesta->TabIndex = 2;
            this->lblRespuesta->Text = L"Respuesta del servidor aparecer? aqu?.";
            this->lblRespuesta->TextAlign = System::Drawing::ContentAlignment::TopCenter;
            //
            // lblTitle
            //
            this->lblTitle->AutoSize = true;
            this->lblTitle->BackColor = System::Drawing::SystemColors::InactiveCaption;
            this->lblTitle->Font = (gcnew System::Drawing::Font(L"Arial", 27.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->lblTitle->Location = System::Drawing::Point(199, 20);
            this->lblTitle->Name = L"lblTitle";
            this->lblTitle->Size = System::Drawing::Size(356, 42);
            this->lblTitle->TabIndex = 3;
            this->lblTitle->Text = L"Cliente de MPointers\r\n";
            this->lblTitle->Click += gcnew System::EventHandler(this, &Cliente::lblTitle_Click);
            //
            // BtnConectar
            //
            this->BtnConectar->BackColor = System::Drawing::SystemColors::WindowFrame;
            this->BtnConectar->Font = (gcnew System::Drawing::Font(L"Arial", 21.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
                static_cast<System::Byte>(0)));
            this->BtnConectar->Location = System::Drawing::Point(560, 292);
            this->BtnConectar->Name = L"BtnConectar";
            this->BtnConectar->Size = System::Drawing::Size(164, 70);
            this->BtnConectar->TabIndex = 4;
            this->BtnConectar->Text = L"Conectar";
            this->BtnConectar->UseVisualStyleBackColor = false;
            this->BtnConectar->Click += gcnew System::EventHandler(this, &Cliente::BtnConectar_Click); // Agregar manejador de eventos
            //
            // Cliente
            //
            this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
            this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
            this->ClientSize = System::Drawing::Size(771, 374);
            this->Controls->Add(this->BtnConectar);
            this->Controls->Add(this->lblTitle);
            this->Controls->Add(this->btnCliente);
            this->Controls->Add(this->txtPeticion);
            this->Controls->Add(this->lblRespuesta);
            this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
            this->MaximumSize = this->Size;
            this->MinimumSize = this->Size;
            this->Name = L"Cliente";
            this->Text = L"Cliente";
            this->ResumeLayout(false);
            this->PerformLayout();
        }
#pragma endregion
    private: System::Void lblTitle_Click(System::Object^ sender, System::EventArgs^ e) {
    }

    private: System::Void BtnConectar_Click(System::Object^ sender, System::EventArgs^ e) {
        if (!conectado) {
            try {
                // Intentar conectar al servidor
                std::string respuesta = SocketUtils::sendRequest("127.0.0.1", 12345, "Conectar"); // Enviar petici?n de conexi?n
                conectado = true;
                BtnConectar->Enabled = false; // Deshabilitar el bot?n despu?s de conectar
                lblRespuesta->Text = gcnew System::String(respuesta.c_str()); // Mostrar respuesta
                lblRespuesta->ForeColor = System::Drawing::Color::Black; // Color de texto normal
                std::cout << "Conexi?n exitosa: " << respuesta << std::endl; // Mensaje de depuraci?n
            }
            catch (const std::exception& ex) {
                // Manejar errores de conexi?n
                lblRespuesta->Text = gcnew System::String(("Error de conexi?n: " + std::string(ex.what())).c_str());
                lblRespuesta->ForeColor = System::Drawing::Color::Red; // Color de texto rojo para errores
                std::cerr << "Error de conexi?n: " << ex.what() << std::endl; // Mensaje de depuraci?n
            }
        }
    }
    };
}
